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
    @file   jaut_LogWorkerSimple.h
    @date   03, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogLevel.h>
#include <jaut_logger/sink/jaut_ILogSink.h>
#include <jaut_logger/worker/jaut_ILogWorker.h>

#include <jaut_core/define/jaut_Define.h>

#include <jaut_message/thread/buffer/jaut_SimpleRingBuffer.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The simple log worker which works synchronously to the thread the logger is owned by.<br>
     *  Do note that the logging thread will also be responsible for processing the messages.
     *  
     *  @tparam BufferSize      The size of the buffer for log messages
     *  @tparam CriticalSection A lock that allows multiple threads to log messages
     */
    template<int BufferSize = 512, class CriticalSection = juce::DummyCriticalSection>
    class JAUT_API LogWorkerSimple : public ILogWorker
    {
    public:
        LogWorkerSimple() = default;
        
        //==============================================================================================================
        bool enqueue(LogMessage message) override;
    
        //==============================================================================================================
        void setup(AbstractLogger &logger, const FlushPolicy::Settings &flushPolicy) override;
        void finalise() override;
        
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
        SimpleRingBuffer<BufferSize, LogMessage> buffer;
        TimePoint                                lastTime;
        AbstractLogger                           *logger { nullptr };
        
        //==============================================================================================================
        void flushInternal();
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogWorkerSimple)
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<int N, class L>
    inline bool LogWorkerSimple<N, L>::enqueue(LogMessage parMessage)
    {
        jdscoped Guard(lock);
        
        const int result = buffer.push(std::move(parMessage));
        return (result > -1);
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline void LogWorkerSimple<N, L>::setup(AbstractLogger &parLogger, const FlushPolicy::Settings &parFlushPolicy)
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
    }
    
    template<int N, class L>
    void LogWorkerSimple<N, L>::finalise()
    {
        if (flushBehaviour.flushOnFinalisation)
        {
            flushInternal();
        }
        
        for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
        {
            sink_ptr->onClose();
        }
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline bool LogWorkerSimple<N, L>::isEmpty() const
    {
        jdscoped Guard(lock);
        return buffer.isEmpty();
    }
    
    template<int N, class L>
    inline bool LogWorkerSimple<N, L>::isFull() const
    {
        jdscoped Guard(lock);
        return buffer.isFull();
    }
    
    template<int N, class L>
    inline int LogWorkerSimple<N, L>::size() const
    {
        jdscoped Guard(lock);
        return buffer.size();
    }
    
    template<int N, class L>
    inline int LogWorkerSimple<N, L>::capacity() const
    {
        jdscoped Guard(lock);
        return buffer.capacity();
    }
    
    //==================================================================================================================
    template<int N, class L>
    inline bool LogWorkerSimple<N, L>::flush()
    {
        if (!logger)
        {
            return false;
        }
        
        flushInternal();
        return true;
    }
    
    template<int N, class L>
    inline ILogWorker::FlushAttemptResult LogWorkerSimple<N, L>::tryFlush(const LogMessage &parLastMessage)
    {
        if (!logger)
        {
            return ILogWorker::FlushAttemptResult::NotReady;
        }
        
        const TimePoint now     = Clock::now();
        const int       elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(now - lastTime)
                                                               .count());
        
        if (    flushBehaviour.policies.test(FlushPolicy::Instant)
            || (flushBehaviour.policies.test(FlushPolicy::Timed)    && elapsed >= flushBehaviour.interval)
            || (flushBehaviour.policies.test(FlushPolicy::Filled)   && isFull())
            || (flushBehaviour.policies.test(FlushPolicy::Levelled) && parLastMessage.level >= flushBehaviour.level)
            || (flushBehaviour.policies.test(FlushPolicy::Custom)   && flushBehaviour.customPolicy(parLastMessage)))
        {
            flushInternal();
            lastTime = now;
            
            return ILogWorker::FlushAttemptResult::Successful;
        }
        
        return ILogWorker::FlushAttemptResult::Unsuccessful;
    }
    
    //==================================================================================================================
    template<int N, class L>
    void LogWorkerSimple<N, L>::flushInternal()
    {
        jdscoped Guard(lock);
        
        if (!isEmpty())
        {
            for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
            {
                sink_ptr->prepare(size());
            }
            
            do
            {
                const LogMessage message = buffer.pop();
                
                for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
                {
                    sink_ptr->print(message);
                }
            }
            while (!isEmpty());
            
            for (const AbstractLogger::SinkPtr &sink_ptr : logger->getSinks())
            {
                sink_ptr->flush();
            }
        }
    }
}
