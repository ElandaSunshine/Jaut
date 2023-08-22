/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   jaut_MessageHandler.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/util/jaut_TypeTraits.h>
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_message/thread/buffer/jaut_AtomicRingBuffer.h>
#include <jaut_message/thread/message/jaut_IMessage.h>
#include <jaut_message/thread/exception/jaut_QueueSpaceExceededException.h>
#include <jaut_message/thread/message/inbuilt/jaut_MessageCallback.h>

#include <juce_events/juce_events.h>



namespace jaut
{
    struct JAUT_API IMessageHandler {};
    
    /**
     *  A simple implementation of an thread synchronisation manager.
     *  The MessageHandler class helps sending messages from the message-thread to any other (real-time) thread.
     *
     *  To get started with this class, simply inherit from this class and, if needed, 
     *  override handleMessage(IMessage*, MessageDirection) if message receiving needs to be handled manually.
     *  
     *  The back-buffer serves in two ways, one is as garbage collector and another is sending messages to the message
     *  thread.
     *  
     *  @tparam BufferSize     The size of the message buffer and the garbage collector
     *  @tparam BackBufferSize The size of the message buffer that goes from the target to the message thread
     */
    template<int BufferSize = 16, int BackBufferSize = static_cast<int>(BufferSize * 1.5)>
    class JAUT_API MessageHandler : public IMessageHandler, private juce::Timer
    {
    public:
        using BufferType     = AtomicRingBuffer<BufferSize>;
        using MessagePointer = typename BufferType::Message;
        
        /** The deferred message init policies callback type. */
        using DeferredMessageInitHandler = std::function<bool(IMessage*)>;
        
        //==============================================================================================================
        static constexpr int bufferSize     = BufferSize;
        static constexpr int backBufferSize = BackBufferSize;
        
        //==============================================================================================================
        /** Declares a few options for the MessageHandler class. */
        struct Options final
        {
            /** Determines the maximum amount of messages to handle when processAllMessages was called. */
            int maxMessagesPerLoop = std::numeric_limits<int>::max();
            
            /** 
             *  Defines the time (in milliseconds) in what interval deferred and garbage collected messages should
             *  be processed.
             */
            int deferredAndGCInterval = 100;
            
            /**
             *  Whether messages on the target thread should be garbage collected and transferred to the messages thread
             *  or if they should be destroyed on the thread they were handled on. (might be problematic for rt-threads)
             */
            bool enableGarbageCollecting = true;
        };
    
        //==============================================================================================================
        /**
         *  Defines the callback for initialising deferred messages on the audio thread to be sent back to the
         *  message thread and handled there.
         *  
         *  If this callback returns true, then the message will be enqueued for the message thread, returning false
         *  however cancels the message and it will be returned to be garbage collected.
         */
        DeferredMessageInitHandler deferredMessageInitHandler = [](IMessage*){ return true; };
        
        //==============================================================================================================
        /**
         *  Constructs a new MessageHandler instance.
         *  
         *  @param options The options for this MessageHandler instance
         *  @throws std::logic_error If the policies is not owned by the message thread
         */
        explicit MessageHandler(Options options = Options());
        
        //==============================================================================================================
        /**
         *  Schedules a new message for the target-thread and pushes it into the message buffer.
         *  If the message buffer is full, it will return false.
         *  <br><br>
         *  This will start the garbage collector if it wasn't scheduled already.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         *  
         *  @param message The message to send to the target-thread
         *  @throws jaut::QueueSpaceExceededException If the queue was full
         */
        void send(NonNull<MessagePointer> message);
        
        /**
         *  Schedules a new message for the target-thread and pushes it into the message buffer.
         *  If the message buffer is full, this won't do anything.
         *  <br><br>
         *  This will start the garbage collector if it wasn't scheduled already.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         *  
         *  @param message The message to send to the target-thread
         *  @throws jaut::QueueSpaceExceededException If the queue was full
         */
        void send(NonNull<message::MessageCallback::CallbackHandler> callback);
        
        /**
         *  Schedules a new message for the target-thread and pushes it into the message buffer.
         *  If the message buffer is full, this won't do anything.
         *  <br><br>
         *  This will start the garbage collector if it wasn't scheduled already.
         *  <br><br>
         *  Other than send(std::shared_ptr<IMessage>), this constructs a new message from its constructor's
         *  parameters and pushes it into the buffer.
         *  You should always prefer this function over the other.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         *  
         *  @tparam MessageType The message class
         *  @param args The arguments to pass to the message's constructor
         *  
         *  @throws jaut::QueueSpaceExceededException If the queue was full
         */
        template<class MessageType, class... Args>
        void send(Args &&...args);
        
        /**
         *  Schedules a list of messages to send to the target-thread and pushes them into the buffer.
         *  All the messages that don't fit into the buffer anymore, will be ignored and destroyed.
         *  <br><br>
         *  This will start the garbage collector if it wasn't scheduled already.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         *  
         *  @param messages The list of messages to schedule
         *  @throws jaut::QueueSpaceExceededException If the queue was full
         */
        void sendAll(std::vector<NonNull<MessagePointer>> messages);
        
        /**
         *  Schedules a list of messages to send to the target-thread and pushes them into the buffer.
         *  All the messages that don't fit into the buffer anymore, will be ignored and destroyed.
         *  <br><br>
         *  This will start the garbage collector if it wasn't scheduled already.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         *  
         *  @param messages The list of messages to schedule
         *  @throws jaut::QueueSpaceExceededException If the queue was full
         */
        void sendAll(std::vector<NonNull<message::MessageCallback::CallbackHandler>> messages);
        
        //==============================================================================================================
        /**
         *  Returns whether there are any pending messages in the message to target buffer.
         *  @return True if there are messages in the buffer, false if not
         */
        JAUT_NODISCARD
        bool hasPendingMessages() const noexcept;
        
        //==============================================================================================================
        /**
         *  Processes the next message in the buffer to the target-thread and calls handleMessage().
         *  After the message was handled, it will, if and only if the garbage collector is enabled, push it into
         *  the garbage collector and schedule it for disposal.
         */
        void processNextMessage();
    
        /**
         *  Processes all messages in the buffer to the target-thread and calls handleMessage().
         *  After the message was handled, it will, if and only if the garbage collector is enabled, push it into
         *  the garbage collector and schedule it for disposal.
         */
        void processAllMessages();
    
        //==============================================================================================================
        /**
         *  Cancels all pending messages in the buffer to the target currently scheduled.
         *  If there are no messages, this will do nothing.
         *  <br><br>
         *  Note: You shouldn't call this on a real-time thread as it locks to get the thread it is on.
         */
        void cancelPendingMessages();
        
    protected:
        /**
         *  Handles the message.
         *  This can be override, if needed for cases where messages need to be intercepted.
         *  However, only do that if you know what you are doing.
         *
         *  @param message   The message to handle
         *  @param direction The thread the message was handled on
         */
        virtual void handleMessage(IMessage *message, MessageDirection direction);
        
    private:
        struct WaybackMessage
        {
            //==========================================================================================================
            bool           handle  {};
            MessagePointer message {};
    
            //==========================================================================================================
            operator bool() const noexcept { return message; } // NOLINT
        };
        
        //==============================================================================================================
        AtomicRingBuffer<BackBufferSize, WaybackMessage> backBuffer;
        
        Options           options;
        BufferType        messageBuffer;
        std::atomic<bool> cancelUpdates { false };
        
        //==============================================================================================================
        void timerCallback() override;
        void deleteUnusedMessages();
        void sendMessage(MessagePointer message);
    
        //==============================================================================================================
        void processMessages(int count);
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageHandler)
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<int N, int M>
    inline MessageHandler<N, M>::MessageHandler(Options parOptions)
        : options(parOptions)
    {
        if (!juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            jassertfalse;
            throw std::logic_error("The message policies can only be declared on the message thread");
        }
        
        startTimer(std::max(options.deferredAndGCInterval, 1));
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::send(NonNull<MessagePointer> parMessage)
    {
        sendMessage(std::move(parMessage.get()));
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::send(NonNull<message::MessageCallback::CallbackHandler> parCallback)
    {
        send(std::make_unique<message::MessageCallback>(std::move(parCallback.get())));
    }
    
    template<int N, int M>
    template<class MessageType, class... Args>
    inline void MessageHandler<N, M>::send(Args &&...parArgs)
    {
        static_assert(std::is_base_of_v<IMessage, MessageType>);
        send(std::make_unique<MessageType>(std::forward<Args>(parArgs)...));
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::sendAll(std::vector<NonNull<MessagePointer>> parMessages)
    {
        for (auto &message : parMessages)
        {
            send(std::move(message.get()));
        }
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::sendAll(std::vector<NonNull<message::MessageCallback::CallbackHandler>>
                                                  parMessages)
    {
        for (auto &message : parMessages)
        {
            send(std::move(message.get()));
        }
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline bool MessageHandler<N, M>::hasPendingMessages() const noexcept
    {
        return !messageBuffer.isEmpty();
    }
    
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::processNextMessage()
    {
        processMessages(1);
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::processAllMessages()
    {
        processMessages(options.maxMessagesPerLoop);
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::cancelPendingMessages()
    {
        if (!hasPendingMessages())
        {
            return;
        }
        
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            cancelUpdates.store(true);
        }
        else
        {
            deleteUnusedMessages();
        }
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::handleMessage(IMessage *parMessage, MessageDirection parDirection)
    {
        parMessage->handleMessage(this, parDirection);
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::timerCallback()
    {
        WaybackMessage message = backBuffer.pop();
        
        while (message.message)
        {
            if (message.handle)
            {
                handleMessage(message.message.get(), MessageDirection::MessageThread);
            }
            
            message = backBuffer.pop();
        }
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::deleteUnusedMessages()
    {
        while (MessagePointer message = messageBuffer.pop())
        {
            backBuffer.push({false, std::move(message)});
        }
    
        cancelUpdates.store(false);
    }
    
    template<int N, int M>
    inline void MessageHandler<N, M>::sendMessage(MessagePointer parMessage)
    {
        // This check will lock, so you wouldn't call this method on a real-time thread
        // Use deferred messages instead
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            if (messageBuffer.push(std::move(parMessage)) < 0)
            {
                throw QueueSpaceExceededException("Could not enqueue message for the target thread, queue was full");
            }
        }
        else
        {
            if (backBuffer.push({ true, std::move(parMessage) }) < 0)
            {
                throw QueueSpaceExceededException("Could not enqueue message for the message thread, queue was full");
            }
        }
    }
    
    //==================================================================================================================
    template<int N, int M>
    inline void MessageHandler<N, M>::processMessages(int parCount)
    {
        // Do not handle messages on the message thread
        jassert(!juce::MessageManager::getInstance()->isThisTheMessageThread());
    
        if (cancelUpdates.load())
        {
            deleteUnusedMessages();
            return;
        }
        
        while ((parCount--) > 0)
        {
            MessagePointer message = messageBuffer.pop();
            
            if (!message)
            {
                break;
            }
            
            const bool is_deferred = (message->getDeferId() >= 0);
            bool       defer       = false;
            
            if (!is_deferred)
            {
                handleMessage(message.get(), MessageDirection::TargetThread);
            }
            else
            {
                defer = deferredMessageInitHandler(message.get());
            }
            
            if (options.enableGarbageCollecting || defer)
            {
                backBuffer.push({ defer, std::move(message) });
            }
        }
    }
}
