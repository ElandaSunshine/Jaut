/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any internal version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   jaut_message.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

namespace jaut
{
struct IMessageHandler {};

/**
 *  A simple implementation of an one-way thread synchronisation manager.
 *  The MessageHandler class helps sending messages from the message-thread to any other thread.
 *
 *  To get started with this class, simply inherit from this class and, if needed, override handleMessage() if
 *  message receiving needs to be handled manually.
 *
 *  @tparam BufferSize    The size of the message buffer and the garbage collector
 *  @tparam MessageBuffer The type of buffer to use as the message buffer
 */
template<int BufferSize = 64, class MessageBuffer = AtomicRingBuffer<BufferSize>>
class JAUT_API MessageHandler : public IMessageHandler, private juce::Timer
{
public:
    /**
     *  Declares a few options for the MessageHandler class.
     */
    struct JAUT_API Options final
    {
        /**
         *  Determines the maximum amount of messages to handle when processAllMessages was called.
         */
        int maxMessagesPerLoop { 100 };
        
        /**
         *  Determines the intervall of when the garbage collector should kick in to collect old messages.
         */
        int garbageCollectorInterval { 100 };
        
        /**
         *  Determines whether garbage collection should be enabled, which will dispose old, unused messages on the
         *  message thread.
         */
        bool enableGarbageCollecting { true };
    };

    //==================================================================================================================
    /**
     *  Constructs a new MessageHandler instance.
     *  @param options The options for this MessageHandler instance
     */
    explicit MessageHandler(const Options &options = Options()) noexcept
        : options(options), cancelUpdates(false),
          cancelCount(0)
    {
        JAUT_ENSURE_MESSAGE_THREAD();
        
        static_assert(std::is_base_of_v<IMessageBuffer, MessageBuffer>, R"(Type-argument "MessageBuffer" must be )"
                                                                        R"(any type inheriting IMessageBuffer.)");

        if(options.enableGarbageCollecting)
        {
            startTimer(std::max(options.garbageCollectorInterval, 1));
        }
    }

    //==================================================================================================================
    /**
     *  Schedules a new message for the target-thread and pushes it into the message buffer.
     *  If the message buffer is full, this won't do anything.
     *
     *  This will start the garbage collector if it wasn't scheduled already.
     *
     *  @param message The message to send to the target-thread
     */
    void send(std::shared_ptr<IMessage> message)
    {
        JAUT_ENSURE_MESSAGE_THREAD();
        
        if(!message)
        {
            /**
             *  A jaut_message can't be null!
             */
            jassertfalse;
            return;
        }

        if(!isTimerRunning() && options.enableGarbageCollecting)
        {
            startTimer(std::max(options.garbageCollectorInterval, 1));
        }

        messageBuffer.push(message);
    }

    /**
     *  Schedules a new message for the target-thread and pushes it into the message buffer.
     *  If the message buffer is full, this won't do anything.
     *
     *  This will start the garbage collector if it wasn't scheduled already.
     *
     *  Other than send(std::shared_ptr<IMessage>), this constructs a new message from its constructor's
     *  parameters and pushes it into the buffer.
     *  You should always prefer this function over the other.
     *
     *  @tparam MessageType The message class
     *  @tparam Args The parameters of the message class' constructor
     *  @param args The arguments to pass to the message's constructor
     */
    template<class MessageType, class... Args>
    void send(Args&&... args)
    {
        /**
         *  MessageType must be of type IMessage.
         */
        jassert((std::is_base_of_v<IMessage, MessageType>));

        send(std::make_shared<MessageType>(std::forward<Args>(args)...));
    }

    /**
     *  Schedules a list of messages to send to the target-thread and pushes them into the buffer.
     *  All the messages that don't fit into the buffer anymore, will be ignored and destroyed.
     *
     *  This will start the garbage collector if it wasn't scheduled already.
     *
     *  @param messages The list of messages to schedule
     */
    void send(std::initializer_list<std::shared_ptr<IMessage>> messages)
    {
        for(auto message : messages)
        {
            send(message);
        }
    }

    //==================================================================================================================
    /**
     *  Schedules a new message for the target-thread and pushes it into the message buffer.
     *  If the message buffer is full, this won't do anything.
     *
     *  Other than send(std::shared_ptr<IMessage>), this will not only push the message but also handle the message
     *  right on the sender-thread.
     *
     *  @param message The message to push
     */
    void sendToAll(std::shared_ptr<IMessage> message)
    {
        /**
         *  A jaut_message can't be null!
         */
        jassert(message != nullptr);

        if(message)
        {
            handleMessage(message.get(), MessageThread);
            send(message);
        }
    }

    /**
     *  Schedules a new message for the target-thread and pushes it into the message buffer.
     *  If the message buffer is full, this won't do anything.
     *
     *  Other than send(Args&&... args), this will not only push the message but also handle the message
     *  right on the sender-thread.
     *
     *  @tparam MessageType The message class
     *  @tparam Args The parameters of the message class' constructor
     *  @param args The arguments to pass to the message's constructor
     */
    template<class MessageType, class... Args>
    void sendToAll(Args&&... args)
    {
        /**
         *  MessageType must be of type IMessage.
         */
        jassert((std::is_base_of_v<IMessage, MessageType>));
        sendToAll(std::make_shared<MessageType>(std::forward<Args>(args)...));
    }
    
    /**
     *  Schedules a list of messages to send to the target-thread and pushes them into the buffer.
     *  All the messages that don't fit into the buffer anymore, will be ignored and destroyed.
     *
     *  Other than send(std::initializer_list<std::shared_ptr<IMessage>>), this will not only push the message but
     *  also handle the message right on the sender-thread.
     *
     *  @param messages The list of messages to schedule
     */
    void sendToAll(std::initializer_list<std::shared_ptr<IMessage>> messages)
    {
        for(auto message : messages)
        {
            sendToAll(message);
        }
    }

    //==================================================================================================================
    /**
     *  Returns whether there are any pending messages in the buffer.
     *  @return True if there are messages in the buffer, false if not
     */
    bool hasPendingMessages() const noexcept
    {
        return !messageBuffer.isEmpty();
    };

protected:
    /**
     *  Handles the message.
     *
     *  @param message   The message to handle
     *  @param direction The thread the message was handled on
     */
    virtual void handleMessage(IMessage *message, MessageDirection direction)
    {
        message->handleMessage(this, direction);
    }

    //==================================================================================================================
    /**
     *  Processes the next message in the buffer on the target-thread and calls handleMessage().
     *  After the message was handled, it will, if and only if the garbage collector is enabled, push it into
     *  the garbage collector and schedule it for disposal.
     */
    void processNextMessage()
    {
        const bool should_collect = !juce::MessageManager::getInstance()->isThisTheMessageThread();
        const bool should_cancel  = cancelUpdates.load(std::memory_order_acquire);
        
        if (should_collect && should_cancel)
        {
            deleteUnusedMessages();
            cancelUpdates.store(false, std::memory_order_relaxed);
        }

        if (auto message = messageBuffer.pop())
        {
            handleMessage(message.get(), should_collect ? TargetThread : MessageThread);

            if(should_collect)
            {
                garbageCollector.push(message);
            }
        }
    }
    
    /**
     *  Processes all messages in the buffer on the target-thread and calls handleMessage().
     *  After the message was handled, it will, if and only if the garbage collector is enabled, push it into
     *  the garbage collector and schedule it for disposal.
     */
    void processAllMessages()
    {
        if(!juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            return;
        }

        const bool should_collect = !juce::MessageManager::getInstance()->isThisTheMessageThread();

        if(should_collect)
        {
            deleteUnusedMessages();
        }

        int counter  = options.maxMessagesPerLoop;
        auto message = messageBuffer.pop();

        while(message && (counter == -1 || --counter > 0))
        {
            handleMessage(message.get(), should_collect ? TargetThread : MessageThread);

            if(should_collect)
            {
                garbageCollector.push(message);
            }

            message = messageBuffer.pop();
        }
    }

    //==================================================================================================================
    /**
     *  Cancels all pending messages in the buffer currently scheduled.
     *  If there are no messages, this will do nothing.
     */
    void cancelPendingMessages()
    {
        if(!hasPendingMessages())
        {
            return;
        }

        if(juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            cancelCount = messageBuffer.size();
            cancelUpdates.store(true, std::memory_order_release);

            return;
        }

        deleteUnusedMessages();
    }

private:
    Options options;
    MessageBuffer messageBuffer;
    AtomicRingBuffer<static_cast<int>(BufferSize * 1.5)> garbageCollector;
    std::atomic<bool> cancelUpdates;
    int cancelCount;

    //==================================================================================================================
    void timerCallback() override
    {
        (void)(garbageCollector.pop());
    }

    //==================================================================================================================
    void deleteUnusedMessages()
    {
        const bool should_delete = cancelUpdates.load(std::memory_order_acquire);

        if(should_delete)
        {
            int counter = (cancelCount > 0 ? cancelCount : messageBuffer.size());
            IMessageBuffer::Message message = messageBuffer.pop();

            while(message && --counter > 0)
            {
                garbageCollector.push(message);
                message = messageBuffer.pop();
            }

            cancelCount = 0;
            cancelUpdates.store(false, std::memory_order_release);
        }
    }
};
}
