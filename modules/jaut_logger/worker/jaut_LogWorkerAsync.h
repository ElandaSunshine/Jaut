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
    @file   jaut_LogWorkerAsync.h
    @date   03, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_logger_define.h>
#include <jaut_logger/jaut_LogLevel.h>
#include <jaut_logger/sink/jaut_ILogSink.h>
#include <jaut_logger/worker/jaut_ILogWorker.h>

#include <jaut_core/define/jaut_Define.h>

#include <jaut_message/thread/buffer/jaut_AtomicRingBuffer.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The async log worker, which works asynchronously to the threads the logger is used by.<br>
     *  This will introduce a new thread to the logger that will handle consumption of messages when it feels the
     *  need to.
     *  <br><br>
     *  Do note that there are locks involved on the producer site, to make sure the buffer is not pushing on
     *  several threads simultaneously.<br>
     *  However, this is disabled by default, assuming the logger will log only on one thread.<br>
     *  The consumer side is entirely lock-free as long as the implementation allows atomic ints to be lock-free.
     *  
     *  @tparam BufferSize The size of the message queue
     */
    template<int BufferSize = 512, class CriticalSection = juce::DummyCriticalSection>
    class JAUT_API LogWorkerAsync : public ILogWorker, private juce::Thread
    {
    public:
        LogWorkerAsync();
        
        //==============================================================================================================
        void setup(AbstractLogger &logger, const FlushPolicy::Settings &flushPolicy) override;
        void finalise() override;
        
        //==============================================================================================================
        bool enqueue(LogMessage message) override;
        
        //==============================================================================================================
        JAUT_NODISCARD bool isEmpty()  const override;
        JAUT_NODISCARD bool isFull()   const override;
        JAUT_NODISCARD int  size()     const override;
        JAUT_NODISCARD int  capacity() const override;
        
        //==============================================================================================================
        bool               flush()                                 override;
        FlushAttemptResult tryFlush(const LogMessage &lastMessage) override;
        
    private:
        using Clock     = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        using Guard     = typename CriticalSection::ScopedLockType;
        
        //==============================================================================================================
        CriticalSection lock;
        
        FlushPolicy::Settings                    flushBehaviour;
        AtomicRingBuffer<BufferSize, LogMessage> buffer;
        TimePoint                                lastTime;
        AbstractLogger                           *logger { nullptr };
        
        std::atomic<bool> dirty   { false };
        std::atomic<bool> running {  true };
        
        //==============================================================================================================
        void run() override;
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogWorkerAsync)
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<int N, class L>
    inline LogWorkerAsync<N, L>::LogWorkerAsync()
        : juce::Thread("Logger")
    {}
    
    //==================================================================================================================
    template<int N, class L>
    inline void LogWorkerAsync<N, L>::setup(AbstractLogger &parLogger, const FlushPolicy::Settings &parFlushPolicy)
    {
        logger = &parLogger;
        
        using std::swap;
        FlushPolicy::Settings temp_policy(parFlushPolicy);
        swap(flushBehaviour, temp_policy);
        
        for (const AbstractLogger::SinkPtr &sink_ptr : parLogger.getSinks())
        {
            sink_ptr->onOpen();
        }
        
        lastTime = Clock::now();
        
        startThread();
    }
    
    template<int N, class L>
    void LogWorkerAsync<N, L>::finalise()
    {
        if (flushBehaviour.flushOnFinalisation)
        {
            dirty = true;
            while (!isEmpty());
            
            running = false;
            stopThread(5000);
        }
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline bool LogWorkerAsync<N, L>::enqueue(LogMessage parMessage)
    {
        jdscoped Guard(lock);
        
        const int result = buffer.push(std::move(parMessage));
        return (result > -1);
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline bool LogWorkerAsync<N, L>::isEmpty() const
    {
        jdscoped Guard(lock);
        return buffer.isEmpty();
    }
    
    template<int N, class L>
    inline bool LogWorkerAsync<N, L>::isFull() const
    {
        jdscoped Guard(lock);
        return buffer.isFull();
    }
    
    template<int N, class L>
    inline int LogWorkerAsync<N, L>::size() const
    {
        jdscoped Guard(lock);
        return buffer.size();
    }
    
    template<int N, class L>
    inline int LogWorkerAsync<N, L>::capacity() const
    {
        jdscoped Guard(lock);
        return buffer.capacity();
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline bool LogWorkerAsync<N, L>::flush()
    {
        if (!logger)
        {
            return false;
        }
        
        dirty = true;
        return true;
    }
    
    template<int N, class L>
    inline ILogWorker::FlushAttemptResult LogWorkerAsync<N, L>::tryFlush(const LogMessage &parLastMessage)
    {
        if (!logger)
        {
            return ILogWorker::FlushAttemptResult::NotReady;
        }
        
        if (dirty)
        {
            return ILogWorker::FlushAttemptResult::Async;
        }
        
        if (    flushBehaviour.policies.test(FlushPolicy::Instant)
            || (flushBehaviour.policies.test(FlushPolicy::Filled)   && isFull())
            || (flushBehaviour.policies.test(FlushPolicy::Levelled) && parLastMessage.level >= flushBehaviour.level)
            || (flushBehaviour.policies.test(FlushPolicy::Custom)   && flushBehaviour.customPolicy(parLastMessage)))
        {
            flush();
            
            if (!dirty)
            {
                return ILogWorker::FlushAttemptResult::Successful;
            }
        }
        
        return ILogWorker::FlushAttemptResult::Async;
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline void LogWorkerAsync<N, L>::run()
    {
        while (running)
        {
            if (flushBehaviour.policies.test(FlushPolicy::Timed))
            {
                const TimePoint now = Clock::now();
                const int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(now - lastTime)
                                                                 .count());
                
                if (elapsed >= flushBehaviour.interval)
                {
                    dirty    = true;
                    lastTime = Clock::now();
                }
            }
            
            if (dirty.exchange(false))
            {
                if (buffer.isEmpty())
                {
                    continue;
                }
                
                std::vector<LogMessage> messages;
                messages.reserve(buffer.size() + 10);
                
                while (!buffer.isEmpty())
                {
                    messages.emplace_back(buffer.pop());
                }
                
                std::stable_sort(messages.begin(), messages.end(),
                                 [](auto &&left, auto &&right) { return (left.timestamp < right.timestamp);});
                
                for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
                {
                    sink_ptr->prepare(static_cast<int>(messages.size()));
                    
                    for (LogMessage &message : messages)
                    {
                        sink_ptr->print(message);
                    }
                    
                    sink_ptr->flush();
                }
            }
            
            #if JAUT_PROVIDER_LOGGER_ASYNC_SLEEP > -1
                // Cut some other threads some slack,
                // the logger thread is not that important that it has to consume every cycle it gets
                // we don't really care if the delay is a tad bit more than specified
                sleep(JAUT_LOGGER_ASYNC_SLEEP);
            #endif
        }
        
        for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
        {
            sink_ptr->onClose();
        }
    }
}
