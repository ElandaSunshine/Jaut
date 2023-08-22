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
    @file   jaut_Logger.h
    @date   24, March 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_logger/jaut_AbstractLogger.h>
#include <jaut_logger/jaut_FlushPolicy.h>
#include <jaut_logger/worker/jaut_LogWorkerAsync.h>
#include <jaut_logger/worker/jaut_LogWorkerSimple.h>

#include <jaut_core/define/jaut_DefUtils.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_core/util/jaut_Stringable.h>
#include <jaut_core/util/jaut_TypeTraits.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  A proper logging implementation that aims at being a bit more advanced than the juce native one.
     *  The implementation of this logger is heavily inspired by Apache's log4j and gabime's spdlog, so you will
     *  find some similarities here and there.
     *  <br><br>
     *  Do note that loggers are immutable, so any options you give it can only be changed when re-constructing the
     *  logger.
     */
    template<class Worker>
    class JAUT_API BasicLogger : public AbstractLogger
    {
    private:
        static_assert(std::is_base_of_v<ILogWorker, Worker>, JAUT_ASSERT_LOGGER_WORKER_NOT_A_WORKER);
        static_assert(!std::is_abstract_v<Worker>,           JAUT_ASSERT_LOGGER_WORKER_IS_ABSTRACT);
        
    public:
        /** The callback type for custom exception handlers. */
        using UnexpectedThrowCallback = std::function<void(const std::exception &exception)>;
        
        //==============================================================================================================
        /**
         *  A set of options to use for logger construction.<br>
         *  Options are immutable; once set, they are fixed.
         */
        struct Options
        {
            /** Determines the flushing behaviour of the logger.*/
            FlushPolicy::Settings flushPolicySettings;
            
            /**
             *  This event occurs when a vital exception was thrown inside a destructor.<br>
             *  This might happen in the log message builder or when the logger gets destructed and need to make a
             *  last flush to its sinks.
             *  <br><br>
             *  If no custom handler is used, the program will be terminated as if there was no exception handling.
             *  (with additional debug output in debug mode)
             *  <br><br>
             *  If you just want to swallow exceptions, you can supply an empty lambda doing nothing.
             */
            UnexpectedThrowCallback onUnexpectedThrow = nullptr;
            
            /**
             *  The maximum log level, the higher the level the more limited the output is.
             *  
             *  For instance, if the log-level is info, all log levels from info downwards will be logged.
             *  Or if the log-level is LogLevel::Fatal, this will be the only log that will be sent as it's the highest.
             */
            Level logLevel = JAUT_DEBUGGING_OR(Level::Debug, Level::Info);
            
            /**
             *  The amount of milliseconds a log message should be retried to be enqueued when the buffer is full.<br>
             *  This only makes sense when using an async worker or there is an interval specified.
             *  <br><br>
             *  If this is -1, it will block until it could enqueue the message<br>
             *  If this is 0, it will try it once<br>
             *  If this is > 0, it will try it that number of seconds before it gives up
             *  <br><br>
             *  Do note that this might stall the thread forever if it is set to block and any of the flush policies
             *  is set unfortunate enough to never flush so be careful with this.
             *  (Example: Only mode "Levelled" is enabled and the last message in the buffer happens to not be
             *  significant enough of a level to flush the buffer)
             */
            int bufferOverflowRetryTimeout = 0;
        };
        
        //==============================================================================================================
        /**
         *  Create a new logger object with the specified name.
         *  
         *  @param name  The name of the logger
         *  @param sinks The logging sinks
         */
        template<class ...Sinks>
        explicit BasicLogger(juce::String name, Sinks ...sinks);
        
        /**
         *  Create a new logger object with the specified name.
         *  
         *  @param name    The name of the logger
         *  @param sinks   The logging sinks
         *  @param options The options of the logger
         */
        template<class ...Sinks>
        BasicLogger(juce::String name, Options options, Sinks ...sinks);
        
        /**
         *  The destructor might throw internally when it makes a last flush.<br>
         *  For this cases there is the jaut::BasicLogger::Options::onUnexpectedThrow callback that can be used
         *  to handle them.
         */
        ~BasicLogger() override;
        
        //==============================================================================================================
        void log(LogMessage logMessage) override;
        
        //==============================================================================================================
        void flush() override;
        
        //==============================================================================================================
        /**
         *  Get the options of the logger.
         *  @return The Logger's options
         */
        JAUT_NODISCARD
        const Options& getOptions() const noexcept;
        
        /**
         *  Gets the worker this logger is using.
         *  @return The worker instance
         */
        JAUT_NODISCARD
        ILogWorker& getWorker() noexcept;
        
        /**
         *  Gets the worker this logger is using.
         *  @return The worker instance
         */
        JAUT_NODISCARD
        const ILogWorker& getWorker() const noexcept;
        
    private:
        Options options;
        Worker  worker;
        
        //==============================================================================================================
        void handleException(const std::exception &exception) const override;
        
        //==============================================================================================================
        void flushInternal();
        void handleExceptionInternal(const std::exception &exception) const;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicLogger)
    };
    
    //==================================================================================================================
    /**
     *  The default sync logger, this will make message processing happen on the same thread as the logging.<br>
     *  This variant will give you the default sync logger with a buffer size of 512 and no lock on the producer site.
     */
    using LoggerSimple = BasicLogger<LogWorkerSimple<>>;
    
    /** 
     *  The async logger, this will create a separate thread that will process the logging messages asynchronously.<br>
     *  This variant will give you the default async logger with a buffer size of 512 and no lock on the producer site.
     */
    using LoggerAsync = BasicLogger<LogWorkerAsync<>>;
    
    /**
     *  The default sync logger, this will make message processing happen on the same thread as the logging.<br>
     *  This variant will give you a sync logger with a custom buffer size and no lock on the producer site.
     *  
     *  @tparam BufferSize The size of the log worker message buffer
     */
    template<int BufferSize>
    using LoggerSimpleCS = BasicLogger<LogWorkerSimple<BufferSize>>;
    
    /**
     *  The async logger, this will create a separate thread that will process the logging messages asynchronously.<br>
     *  This variant will give you an async logger with a custom buffer size and no lock on the producer site.
     *  
     *  @tparam BufferSize The size of the log worker message buffer
     */
    template<int BufferSize>
    using LoggerAsyncCS = BasicLogger<LogWorkerAsync<BufferSize>>;
    
    /**
     *  The default sync logger, this will make message processing happen on the same thread as the logging.<br>
     *  This variant will give you a sync logger with a default buffer size of 512 and a lock on the producer site.
     */
    using LoggerSimpleMT = BasicLogger<LogWorkerSimple<512, juce::CriticalSection>>;
    
    /**
     *  The async logger, this will create a separate thread that will process the logging messages asynchronously.<br>
     *  This variant will give you an async logger with a default buffer size of 512 and a lock on the producer site.
     */
    using LoggerAsyncMT = BasicLogger<LogWorkerAsync<512, juce::CriticalSection>>;
    
    /**
     *  The default sync logger, this will make message processing happen on the same thread as the logging.<br>
     *  This variant will give you a sync logger with a custom buffer size and a lock on the producer site.
     *  
     *  @tparam BufferSize The size of the log worker message buffer
     */
    template<int BufferSize>
    using LoggerSimpleCSMT = BasicLogger<LogWorkerSimple<BufferSize, juce::CriticalSection>>;
    
    /**
     *  The async logger, this will create a separate thread that will process the logging messages asynchronously.<br>
     *  This variant will give you an async logger with a custom buffer size and a lock on the producer site.
     *  
     *  @tparam BufferSize The size of the log worker message buffer
     */
    template<int BufferSize>
    using LoggerAsyncCSMT = BasicLogger<LogWorkerAsync<BufferSize, juce::CriticalSection>>;
    
    //==================================================================================================================
    // IMPLEMENTATION BasicLogger
    template<class T>
    template<class ...Sinks>
    BasicLogger<T>::BasicLogger(juce::String parName, Sinks ...parSinks)
        : BasicLogger(std::move(parName), Options{}, std::move(parSinks)...)
    {}
    
    template<class T>
    template<class ...Sinks>
    BasicLogger<T>::BasicLogger(juce::String parName, Options parOptions, Sinks ...parSinks)
        : AbstractLogger(std::move(parName), parOptions.logLevel, std::move(parSinks)...),
          options(std::move(parOptions))
    {
        worker.setup(*this, parOptions.flushPolicySettings);
    }
    
    template<class T>
    BasicLogger<T>::~BasicLogger()
    {
        try
        {
            worker.finalise();
        }
        catch (const std::exception &ex)
        {
            handleExceptionInternal(ex);
        }
    }
    
    //==================================================================================================================
    template<class T>
    void BasicLogger<T>::log(LogMessage parLogMessage)
    {
        if (!shouldLog(parLogMessage.level))
        {
            return;
        }
        
        const bool                           queue_result = worker.enqueue (parLogMessage);
        const ILogWorker::FlushAttemptResult flush_result = worker.tryFlush(parLogMessage);
        
        if (!queue_result)
        {
            switch (flush_result)
            {
                case ILogWorker::FlushAttemptResult::Successful:
                {
                    (void) worker.enqueue(parLogMessage);
                    break;
                }
                
                case ILogWorker::FlushAttemptResult::Unsuccessful:
                {
                    if (options.flushPolicySettings.policies.test(FlushPolicy::Timed))
                    {
                        break;
                    }
                    
                    JAUT_FALLTHROUGH;
                }
                
                case ILogWorker::FlushAttemptResult::Async:
                {
                    const int timeout = options.bufferOverflowRetryTimeout;
                    
                    if (timeout < 0)
                    {
                        // Do note that this is a dangerous combination for the reasons explained in the docs
                        // for jaut::Logger::Options::bufferOverflowRetryTimeout
                        jassert(!options.flushPolicySettings.policies.test(FlushPolicy::Levelled));
                        
                        while (!worker.enqueue(parLogMessage))
                        {
                            if (flush_result != ILogWorker::FlushAttemptResult::Async)
                            {
                                (void) worker.tryFlush(parLogMessage);
                            }
                        }
                    }
                    else if (timeout > 0)
                    {
                        using Clock     = std::chrono::steady_clock;
                        using TimePoint = std::chrono::time_point<Clock>;
                        
                        using std::chrono::duration_cast;
    
                        const TimePoint start = Clock::now();
    
                        while (duration_cast<std::chrono::milliseconds>(Clock::now() - start).count() < timeout)
                        {
                            if (worker.enqueue(parLogMessage))
                            {
                                break;
                            }
        
                            if (flush_result != ILogWorker::FlushAttemptResult::Async)
                            {
                                (void) worker.tryFlush(parLogMessage);
                            }
                        }
                    }
                    else if (flush_result != ILogWorker::FlushAttemptResult::Unsuccessful)
                    {
                        (void) worker.enqueue(parLogMessage);
                    }
    
                    break;
                }
        
                    // I have no clue how you got here, honestly
                    // It should be pretty much impossible to get here except you did something really weird,
                    // or you implemented your own worker returning this despite having been initialised at this point
                case ILogWorker::FlushAttemptResult::NotReady: jassertfalse;
            }
        }
    }
    
    //==================================================================================================================
    template<class T>
    void BasicLogger<T>::flush()
    {
        flushInternal();
    }
    
    //==================================================================================================================
    template<class T>
    const typename BasicLogger<T>::Options &BasicLogger<T>::getOptions() const noexcept
    {
        return options;
    }
    
    template<class T>
    ILogWorker& BasicLogger<T>::getWorker() noexcept
    {
        return worker;
    }
    
    template<class T>
    const ILogWorker& BasicLogger<T>::getWorker() const noexcept
    {
        return worker;
    }
    
    //==================================================================================================================
    template<class T>
    void BasicLogger<T>::handleException(const std::exception &parException) const
    {
        handleExceptionInternal(parException);
    }
    
    //==================================================================================================================
    template<class T>
    void BasicLogger<T>::flushInternal()
    {
        worker.flush();
    }
    
    template<class T>
    void BasicLogger<T>::handleExceptionInternal(const std::exception &parException) const
    {
        if (options.onUnexpectedThrow)
        {
            options.onUnexpectedThrow(parException);
        }
        else
        {
            // Hey there, you've got a problem
            DBG(parException.what());
            jassertfalse;
            
            std::terminate();
        }
    }
}
