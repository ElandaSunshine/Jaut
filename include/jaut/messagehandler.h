/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   message.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/imessage.h>
#include <jaut/impl/atomicringbuffer.h>

namespace jaut
{
struct IMessageHandler {};

template<int BufferSize = 64, class MessageBuffer = AtomicRingBuffer<BufferSize>>
class JAUT_API MessageHandler : public IMessageHandler, private Timer
{
public:
    struct JAUT_API Options final
    {
        int maxMessagesPerLoop       {  100 };
        int garbageCollectorInterval {  100 };
        bool enableGarbageCollecting { true };
        bool strictThreadDistinction { true };
    };

    //==================================================================================================================
    explicit MessageHandler(const Options &options = Options())
        : options(options), cancelUpdates(false),
          cancelCount(0)
    {
        jassert((std::is_base_of_v<IMessageBuffer, MessageBuffer>));

        if(ensureMessageThread() && options.enableGarbageCollecting)
        {
            startTimer(jmax(options.garbageCollectorInterval, 1));
        }
    }

    //==================================================================================================================
    void send(const std::shared_ptr<IMessage> &message)
    {
        if(!ensureMessageThread())
        {
            return;
        }

        if(!message)
        {
            /**
             *  A message can't be null!
             */
            jassertfalse;
            return;
        }

        if(MessageManager::getInstance()->isThisTheMessageThread())
        {
            if(!isTimerRunning() && options.enableGarbageCollecting)
            {
                startTimer(jmax(options.garbageCollectorInterval, 1));
            }
        }

        messageBuffer.push(message);
    }

    template<class MessageType, class... Args>
    void send(Args&&... ctorArgs)
    {
        /**
         *  MessageType must be of type IMessage.
         */
        jassert((std::is_base_of_v<IMessage, MessageType>));

        send(std::make_shared<MessageType>(std::forward<Args>(ctorArgs)...));
    }

    void send(std::initializer_list<std::shared_ptr<IMessage>> messages)
    {
        for(auto message : messages)
        {
            send(message);
        }
    }

    //==================================================================================================================
    void sendToAll(std::shared_ptr<IMessage> message)
    {
        if(!ensureMessageThread())
        {
            return;
        }

        /**
         *  A message can't be null!
         */
        jassert(message != nullptr);

        if(message)
        {
            handleMessage(message, MessageThread);
            send(message);
        }
    }

    template<class MessageType, class... Args>
    void sendToAll(Args&&... ctorArgs)
    {
        /**
         *  MessageType must be of type IMessage.
         */
        jassert((std::is_base_of_v<IMessage, MessageType>));

        sendToAll(std::make_shared<MessageType>(std::forward<Args>(ctorArgs)...));
    }

    void sendToAll(std::initializer_list<std::shared_ptr<IMessage>> messages)
    {
        for(auto message : messages)
        {
            sendToAll(message);
        }
    }

    //==================================================================================================================
    bool hasPendingMessages() const noexcept
    {
        return !messageBuffer.isEmpty();
    };

protected:
    virtual void handleMessage(IMessage *message, MessageDirection direction)
    {
        message->handleMessage(*this, direction);
    }

    //==================================================================================================================
    void processNextMessage()
    {
        if(!ensureNotMessageThread())
        {
            return;
        }

        const bool should_collect = !MessageManager::getInstance()->isThisTheMessageThread();

        if(should_collect && cancelUpdates.load(std::memory_order_acquire))
        {
            deleteUnusedMessages();
            cancelUpdates.store(0, std::memory_order_relaxed);
        }

        if(auto message = messageBuffer->pop())
        {
            handleMessage(message.get());

            if(should_collect)
            {
                garbageCollector.push(message);
            }
        }
    }

    void processAllMessages()
    {
        bool is_not_message_thread = ensureNotMessageThread();

        if(!is_not_message_thread)
        {
            return;
        }

        const bool should_collect = !MessageManager::getInstance()->isThisTheMessageThread();

        if(should_collect)
        {
            deleteUnusedMessages();
        }

        int  counter = options.maxMessagesPerLoop;
        auto message = messageBuffer.pop();

        while(message && (counter == -1 || (--counter) > 0))
        {
            handleMessage(message.get());

            if(should_collect)
            {
                garbageCollector.push(message);
            }

            message = messageBuffer.pop();
        }
    }

    //==================================================================================================================
    void cancelPendingMessages()
    {
        if(!hasPendingMessages())
        {
            return;
        }

        if(MessageManager::getInstance()->isThisTheMessageThread())
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
    bool deleteUnusedMessages()
    {
        bool should_delete = cancelUpdates.load(std::memory_order_acquire);

        if(should_delete)
        {
            int counter = (cancelCount > 0 ? std::exchange(cancelCount, 0) : messageBuffer.size());
            IMessageBuffer::Message message = messageBuffer.pop();

            while(message != nullptr && (--counter) > 0)
            {
                garbageCollector.push(message);
                message = messageBuffer.pop();
            }

            cancelUpdates.store(false, std::memory_order_relaxed);
        }
    }

    //==================================================================================================================
    inline bool ensureNotMessageThread() const
    {
        if(options.strictThreadDistinction)
        {
            const bool is_not_message_thread = !MessageManager::getInstance()->isThisTheMessageThread();
            jassert(is_not_message_thread);
            return is_not_message_thread;
        }

        return true;
    }

    inline bool ensureMessageThread() const
    {
        if(options.strictThreadDistinction)
        {
            const bool is_message_thread = MessageManager::getInstance()->isThisTheMessageThread();
            jassert(is_message_thread);
            return is_message_thread;
        }

        return true;
    }
};
}