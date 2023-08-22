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
    @file   jaut_AbstractLogger.h
    @date   17, October 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_logger/detail/jaut_fmt.h>
#include <jaut_logger/format/jaut_ILogFormat.h>

#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_core/util/jaut_Stringable.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    namespace detail
    {
        template<class ...Args>
        struct FmtEnableIfCheck : std::true_type {};
        
        template<>
        struct FmtEnableIfCheck<> : std::false_type {};
        
        template<class First>
        struct FmtEnableIfCheck<First>
        {
            static constexpr bool value = !std::is_base_of_v<std::exception, std::decay_t<First>>;
        };
        
        template<class ...Args>
        inline constexpr bool FmtEnableIfCheck_v = FmtEnableIfCheck<Args...>::value;
    }
    
    //==================================================================================================================
    struct ILogSink;
    
    //==================================================================================================================
    /**
     *  A simple and lightweight Logger implementation that aims at being a bit more advanced than the juce one.
     *  The implementation of this logger is heavily inspired by Apache's log4j and gabime's spdlog, so you will
     *  find some similarities here and there.
     *  <br><br>
     *  Do note that loggers are immutable, so any options you give it should only be changed through re-constructing
     *  the logger.
     */
    class JAUT_API AbstractLogger
    {
    public:
        /** A convenience typedef for log level values. */
        using Level = LogLevel::Value;
        
        /** The pointer type that wraps a dynamic instance to jaut::ILogSink objects. */
        using SinkPtr = std::unique_ptr<ILogSink>;
        
        //==============================================================================================================
        class LogBuilder
        {
        public:
            LogBuilder(AbstractLogger &logger, Level level);
            ~LogBuilder();
            
            //==========================================================================================================
            /**
             *  Appends a new object to the message.
             *  <br><br>
             *  Note that directly adding exceptions more than once will override the previous exception.
             *  
             *  @param object The object to append
             *  @return The LogBuilder instance
             */
            template<class T>
            LogBuilder& operator<<(T &&object);
            
        private:
            AbstractLogger &logger;
            LogMessage     logMessage;
        };
        
        //==============================================================================================================
        /**
         *  The default log level used, based on the configuration it was compiled with.<br>
         *  For DEBUG configurations this will be jaut::LogLevel::Debug, for others jaut::LogLevel::Info.
         */
        static constexpr Level defaultLevel = JAUT_DEBUGGING_OR(LogLevel::Debug, Level::Info);
        
        //==============================================================================================================
        /**
         *  @param name     The id of the logger
         *  @param sinks    The list of sinks
         *  @param logLevel The logging priority
         */
        template<class ...Sinks>
        AbstractLogger(juce::String name, Level logLevel, Sinks ...sinks);
        
        virtual ~AbstractLogger() = default;
        
        //==============================================================================================================
        /**
         *  Constructs a new message to log with the specified log level.
         *  
         *  @param level The level of the log message
         *  @return The LogBuilder object
         */
        JAUT_NODISCARD
        LogBuilder operator<<(Level level);
        
        //==============================================================================================================
        /**
         *  Logs a message with the trace level.
         *  @param message The message to be logged
         */
        virtual void trace(const juce::String &message);
        
        /**
         *  Logs a message with the debug level.
         *  @param message The message to be logged
         */
        virtual void debug(const juce::String &message);
        
        /**
         *  Logs a message with the verbose level.
         *  @param message The message to be logged
         */
        virtual void verbose(const juce::String &message);
        
        /**
         *  Logs a message with the info level.
         *  @param message The message to be logged
         */
        virtual void info(const juce::String &message);
        
        /**
         *  Logs a message with the warn level.
         *  @param message The message to be logged
         */
        virtual void warn(const juce::String &message);
        
        /**
         *  Logs a message with the error level.
         *  @param message The message to be logged
         */
        virtual void error(const juce::String &message);
        
        /**
         *  Logs a message with the fatal level.
         *  @param message The message to be logged
         */
        virtual void fatal(const juce::String &message);
        
        /**
         *  Logs a message with the trace level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void trace(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the debug level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void debug(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the verbose level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void verbose(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the info level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void info(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the warn level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void warn(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the error level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void error(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the fatal level and attaches an exception to the log compound.
         *  
         *  @param message   The message to be logged
         *  @param exception The exception that was caught
         */
        virtual void fatal(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the trace level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void trace(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the debug level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void debug(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the verbose level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void verbose(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the info level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void info(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the warn level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void warn(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the error level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void error(const juce::String &message, Args &&...args);
        
        /**
         *  Logs a message with the fatal level.<br>
         *  This will format the passed objects with the supplied message via fmt.
         *  
         *  @param message The message to be logged
         *  @param args    The objects to format the message with
         */
        template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>* = nullptr>
        void fatal(const juce::String &message, Args &&...args);
        
        //==============================================================================================================
        /**
         *  Sets the current log level to be used.
         *  @param logLevel The new log level
         */
        void setLogLevel(Level logLevel) noexcept;
        
        //==============================================================================================================
        /**
         *  Gets the priority level of the logger.<br>
         *  The higher the priority, the fewer logs will be printed.
         *  (this means, only high priority messages will be printed)
         *  
         *  @return The log level
         */
        JAUT_NODISCARD
        Level getLogLevel() const noexcept;
        
        /**
         *  Gets the name of the logger.
         *  @return The logger name
         */
        JAUT_NODISCARD
        const juce::String& getName() const noexcept;
        
        /**
         *  Gets the sink objects the logger has.
         *  @return The sink object list
         */
        JAUT_NODISCARD
        const std::vector<SinkPtr>& getSinks() const noexcept;
        
        /**
         *  Determines whether the logger should log based on the given level.
         *  
         *  @param logLevel The level to check
         *  @return True if should be logged
         */
        JAUT_NODISCARD
        bool shouldLog(Level logLevel) const noexcept;
        
        //==============================================================================================================
        /** Force the logger to flush instantly if it is not empty. */
        virtual void flush() = 0;
        
        /**
         *  Logs a message.
         *  @param message The message compound to be logged
         */
        virtual void log(LogMessage message) = 0;
        
    protected:
        /**
         *  Handles exceptions inside flushing destructors.<br>
         *  AbstractLogger does this for its Builder, if your logger implementation also flushes in its destructor,
         *  you would want to also use this there.
         *
         *  @param exception The exception that was thrown
         */
        virtual void handleException(const std::exception &exception) const = 0;
        
    private:
        std::vector<SinkPtr> sinks;
        juce::String         name;
        std::atomic<Level>   logLevel;
        
        //==============================================================================================================
        jaut::LogMessage makeLog(jaut::LogLevel::Value                    level,
                                 juce::String                             message,
                                 std::vector<jaut::LogMessage::Field>     fields = {},
                                 std::optional<LogMessage::ExceptionSpec> ex     = std::nullopt);
        
        template<class ...Args>
        void makeFormatCall(LogLevel::Value    level,
                            const juce::String &message,
                            Args               &&...args);
    };
    
    //==================================================================================================================
    // IMPLEMENTATION Logger::LogBuilder
    template<class ...Sinks>
    inline AbstractLogger::AbstractLogger(juce::String parName, Level parLogLevel, Sinks ...parSinks)
        : name(std::move(parName)),
          logLevel(parLogLevel)
    {
        static_assert(((sameTypeIgnoreTemplate_v<std::unique_ptr, Sinks>
                        && std::is_base_of_v<ILogSink, getTypeAt_t<Sinks, 0>>) && ...));
        
        sinks.reserve(sizeof...(Sinks));
        ((void) sinks.emplace_back(std::move(parSinks)), ...);
    }
    
    //==================================================================================================================
    template<class T>
    inline AbstractLogger::LogBuilder& AbstractLogger::LogBuilder::operator<<(T &&parObject)
    {
        using Type = std::decay_t<T>;
        
        if (logMessage.level != Level::Off)
        {
            if constexpr (std::is_same_v<Type, LogMessage::Field>)
            {
                detail::emplaceField(logMessage.fields, std::forward<T>(parObject));
            }
            else if constexpr (std::is_base_of_v<std::exception, Type>)
            {
                logMessage.exception = LogMessage::ExceptionSpec::fromException(std::forward<T>(parObject));
            }
            else
            {
                logMessage.message << jaut::toString(std::forward<T>(parObject));
            }
        }
        
        return *this;
    }
    
    //==================================================================================================================
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::trace(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Trace, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::debug(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Debug, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::verbose(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Verbose, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::info(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Info, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::warn(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Warn, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::error(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Error, parMessage, std::forward<Args>(parArgs)...);
    }
    
    template<class ...Args, std::enable_if_t<detail::FmtEnableIfCheck_v<Args...>>*>
    inline void AbstractLogger::fatal(const juce::String &parMessage, Args &&...parArgs)
    {
        makeFormatCall(Level::Fatal, parMessage, std::forward<Args>(parArgs)...);
    }
    
    //==================================================================================================================
    namespace detail
    {
        using FieldVec = std::vector<LogMessage::Field>;
        
        class FilterArgProcessor
        {
        public:
            struct Result
            {
                FieldVec                                 fields;
                std::optional<LogMessage::ExceptionSpec> exceptionSpec { std::nullopt };
            };
            
            //==========================================================================================================
            static void processArg(Result &result, LogMessage::Field field)
            {
                detail::emplaceField(result.fields, std::move(field));
            }
            
            static void processArg(Result &result, const std::exception &ex)
            {
                result.exceptionSpec = LogMessage::ExceptionSpec::fromException(ex);
            }
            
            //==========================================================================================================
            template<class ...Fields>
            Result operator()(Fields &&...fields)
            {
                Result result;
                result.fields.reserve(sizeof...(Fields));
                
                (processArg(result, std::forward<Fields>(fields)), ...);
                result.fields.shrink_to_fit();
                
                return result;
            }
        };
        
        class FormatArgProcessor
        {
        public:
            explicit FormatArgProcessor(const juce::String &parPattern) noexcept
                : pattern(parPattern.toStdString())
            {}
            
            //==========================================================================================================
            template<class ...Args>
            juce::String operator()(Args &&...parArgs)
            {
                return fmt::format(pattern, std::forward<Args>(parArgs)...);
            }
            
        private:
            std::string pattern;
        };
    }
    
    template<class ...Args>
    inline void AbstractLogger::makeFormatCall(LogLevel::Value level, const juce::String &message, Args &&...args)
    {
        using Filter = ArgFilter<
            PredicateOr<
                std::is_same<std::decay_t<PType<>>, LogMessage::Field>,
                std::is_base_of<std::exception, std::decay_t<PType<>>>
            >,
            Args...
        >;
        
        using Result = detail::FilterArgProcessor::Result;
        
        Result       result = Filter::invokeFiltered(detail::FilterArgProcessor{},        std::forward<Args>(args)...);
        juce::String text   = Filter::invokeExcluded(detail::FormatArgProcessor{message}, std::forward<Args>(args)...);
        
        log(makeLog(level, std::move(text), std::move(result.fields), std::move(result.exceptionSpec)));
    }
}
