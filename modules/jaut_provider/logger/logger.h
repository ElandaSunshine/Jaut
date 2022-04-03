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

    @author Elanda (elanda@elandasunshine.com)
    @file   logger.h
    @date   24, March 2022

    ===============================================================
 */
 
#pragma once

#include <fstream>

namespace jaut
{
    class ILogSink;
    
    /**
     *  A simple and lightweight Logger implementation that aims at being a bit more advanced than the juce one.
     *  The implementation of this logger is heavily inspired by Apache's log4j and gabime's spdlog, so you will
     *  find some similarities here and there.
     */
    class Logger
    {
    public:
        /** The log level to produce. */
        enum LogLevel
        {
            /** For unformatted messages. */
            None = 1,
            
            /** For informative messages. */
            Info = 2,
            
            /** For warning messages that should prepare the user for possible conflicts in future. */
            Warn = 4,
            
            /** For problems that arise during runtime, but are still recoverable and usually don't force a shutdown. */
            Error = 8,
            
            /** For developer messages that are only output in debug mode. */
            Debug = 16,
            
            /** For problems that arise during runtime that usually end up in shutting down the program. */
            Fatal = 32
        };
        
        /**
         *  Builds a log message that should be outputted by the logger.
         *  This is majorly used by the loggers stream operator overloads (left shift operator),
         *  but nothing stops you from using it yourself. :)
         */
        class LogBuilder
        {
        public:
            /**
             *  Creates a new LogBuilder instance.
             *
             *  @param logger The logger that should output the built message
             *  @param level  The log-level of the output
             *  @param object Any object what the message should start with or nothing if empty
             */
            LogBuilder(Logger &logger, LogLevel level, const juce::var &object = juce::var());
            
            /** This will flush the message if it is not empty. */
            ~LogBuilder();
            
            //==========================================================================================================
            /**
             *  Appends a new object to the message.
             *
             *  @param object The object to append
             *  @return The LogBuilder instance
             */
            LogBuilder& operator<<(const juce::var &object);
            
            /**
             *  Appends a new ILoggable object to the message.
             *
             *  @param loggable The ILoggable object to append
             *  @return The LogBuilder instance
             */
            LogBuilder& operator<<(const jaut::ILoggable &loggable);
            
            //==========================================================================================================
            /** Forces the LogBuilder to manually flush, if the message is not empty. */
            void flush();
            
        private:
            Logger       &logger;
            juce::String message;
            LogLevel     logLevel;
        };
        
        struct Options
        {
            /** A bit mask of log levels that should stand out by adding a new line before and after the message. */
            std::uint32_t isolatedLevels {};
            
            /** The default log level to be used when trying to log with a method that does not take a log level. */
            LogLevel defaultLogLevel { LogLevel::Info };
            
            /** Whether the sink should process debug messages. */
            bool outputDebugMessages { JUCE_DEBUG };
        };
    
        //==============================================================================================================
        /** The type of the sink object the logger stores internally. */
        using SinkPtr = std::unique_ptr<ILogSink>;
        
        //==============================================================================================================
        /**
         *  Create a new logger object with the specified name.
         *  @param name The name of the logger
         */
        explicit Logger(juce::String name, SinkPtr sink);
        
        //==============================================================================================================
        /**
         *  Constructs a new message to log with the specified log level.
         *  @param level The level of the log message
         *  @return The LogBuilder object
         */
        [[nodiscard]]
        LogBuilder operator<<(LogLevel level);
        
        /**
         *  Creates a new message to log starting off with the specified object.
         *  The level of this method can be specified with jaut::Logger::Options::defaultLogLevel.
         *
         *  @param object The object to start the message with
         *  @return The LogBuilder object
         */
        LogBuilder operator<<(const juce::var &object);
        
        /**
         *  Creates a new message to log starting off with the specified ILoggable object.
         *  The level of this method can be specified with jaut::Logger::Options::defaultLogLevel.
         *
         *  @param loggable The ILoggable object to start the message with
         *  @return The LogBuilder object
         */
        LogBuilder operator<<(const jaut::ILoggable &loggable);
        
        //==============================================================================================================
        /**
         *  Logs a message with the specified level.
         *
         *  @param level   The logging level of the message
         *  @param message The message to be logged
         */
        void log(LogLevel level, const juce::String &message);
        
        /**
         *  Logs a message without any formatting.
         *  @param message The message to be logged
         */
        void unformatted(const juce::String &message);
        
        /**
         *  Logs a message with the info level.
         *  @param message The message to be logged
         */
        void info(const juce::String &message);
        
        /**
         *  Logs a message with the warn level.
         *  @param message The message to be logged
         */
        void warn(const juce::String &message);
        
        /**
         *  Logs a message with the error level.
         *  @param message The message to be logged
         */
        void error(const juce::String &message);
        
        /**
         *  Logs a message with the debug level.
         *  @param message The message to be logged
         */
        void debug(const juce::String &message);
        
        /**
         *  Logs a message with the fatal level.
         *  @param message The message to be logged
         */
        void fatal(const juce::String &message);
        
        /**
         *  Logs a message with the specified level and the exception message.
         *
         *  @param level     The logging level of the message
         *  @param message   The message to be logged
         *  @param exception The exception that should be added to the message
         */
        void log(LogLevel level, const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message without any formatting and appends the exception message.
         *  @param message The message to be logged
         */
        void unformatted(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the info level and appends the exception message.
         *  @param message The message to be logged
         */
        void info(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the warn level and appends the exception message.
         *  @param message The message to be logged
         */
        void warn(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the error level and appends the exception message.
         *  @param message The message to be logged
         */
        void error(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the debug level and appends the exception message.
         *  @param message The message to be logged
         */
        void debug(const juce::String &message, const std::exception &exception);
        
        /**
         *  Logs a message with the fatal level and appends the exception message.
         *  @param message The message to be logged
         */
        void fatal(const juce::String &message, const std::exception &exception);
        
        //==============================================================================================================
        /**
         *  Sets the pattern format a log message should be compromised of.
         *  The default pattern for this is: [%l/%t][%n][%Y-%m-%d %I:%M:%S.%N] %q
         *  <br>
         *  <br>
         *  <table>
         *      <caption>A list of format specifiers to be used</caption>
         *      <tr>
         *          <td><b>%t</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
         *          <td>The current thread name, or "MAIN" if the current thread couldn't be gathered</td>
         *      </tr>
         *      <tr>
         *          <td><b>%n</b></td>
         *          <td>The name of the logger itself</td>
         *      </tr>
         *      <tr>
         *          <td><b>%q</b></td>
         *          <td>The message string</td>
         *      </tr>
         *      <tr>
         *          <td><b>%l</b></td>
         *          <td>The textual representation of the message's log level</td>
         *      </tr>
         *      <tr>
         *          <td><b>%N</b></td>
         *          <td>The milliseconds of when the message was posted</td>
         *      </tr>
         *      <tr>
         *          <td><b>%a</b></td>
         *          <td>The locale's abbreviated weekday name</td>
         *      </tr>
         *      <tr>
         *          <td><b>%A</b></td>
         *          <td>The locale's full weekday name</td>
         *      </tr>
         *      <tr>
         *          <td><b>%b</b></td>
         *          <td>The locale's abbreviated month name</td>
         *      </tr>
         *      <tr>
         *          <td><b>%B</b></td>
         *          <td>The locale's full month name</td>
         *      </tr>
         *      <tr>
         *          <td><b>%c</b></td>
         *          <td>The locale's appropriate date and time representation</td>
         *      </tr>
         *      <tr>
         *          <td><b>%d</b></td>
         *          <td>The day of the month as a decimal number [01,31]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%H</b></td>
         *          <td>The hour (24-hour clock) as a decimal number [00,23]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%I</b></td>
         *          <td>The hour (12-hour clock) as a decimal number [01,12]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%j</b></td>
         *          <td>The day of the year as a decimal number [001,366]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%m</b></td>
         *          <td>The month as a decimal number [01,12]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%M</b></td>
         *          <td>The minute as a decimal number [00,59]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%p</b></td>
         *          <td>The locale's equivalent of either a.m. or p.m.</td>
         *      </tr>
         *      <tr>
         *          <td><b>%S</b></td>
         *          <td>The second as a decimal number [00,60]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%U</b></td>
         *          <td>
         *              The week number of the year (Sunday as the first day of the week) as a decimal number[00,53]
         *          </td>
         *      </tr>
         *      <tr>
         *          <td><b>%w</b></td>
         *          <td>The weekday as a decimal number [0,6], with 0 representing Sunday</td>
         *      </tr>
         *      <tr>
         *          <td><b>%W</b></td>
         *          <td>
         *              The week number of the year (Monday as the first day of the week) as a decimal number [00,53].
         *              All days in a new year preceding the first Monday are considered to be in week 0
         *          </td>
         *      </tr>
         *      <tr>
         *          <td><b>%x</b></td>
         *          <td>The locale's appropriate date representation</td>
         *      </tr>
         *      <tr>
         *          <td><b>%X</b></td>
         *          <td>The locale's appropriate time representation</td>
         *      </tr>
         *      <tr>
         *          <td><b>%y</b></td>
         *          <td>The year without century as a decimal number [00,99]</td>
         *      </tr>
         *      <tr>
         *          <td><b>%Y</b></td>
         *          <td>The year with century as a decimal number</td>
         *      </tr>
         *      <tr>
         *          <td><b>%Z</b></td>
         *          <td>The timezone name or abbreviation, or by no bytes if no timezone information exists</td>
         *      </tr>
         *      <tr>
         *          <td><b>%%</b></td>
         *          <td>The % character</td>
         *      </tr>
         *  </table>
         *
         *  @param format The format string to be used
         */
        void setFormat(juce::String format) noexcept;
        
        //==============================================================================================================
        /**
         *  Set the options for this logger.
         *  @param options The new Options object
         */
        void setOptions(Options options);
        
        /**
         *  Get the options of the logger.
         *  @return The Logger's options
         */
        const Options& getOptions() const noexcept;
        
        //==============================================================================================================
        const juce::String& getName() const noexcept;
        
        ILogSink& getSink() noexcept;
        const ILogSink& getSink() const noexcept;
        
    private:
        SinkPtr      sink;
        Options      options;
        juce::String name;
        juce::String format { "[%l/%t][%n][%Y-%m-%d %I:%M:%S.%N] %q" };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Logger)
        JAUT_CREATE_ANTILEAK_HANDLER(detail::LoggerRegistry::getInstance().clear();)
    };
}
