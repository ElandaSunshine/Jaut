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
    @file   jaut_AbstractLogger.cpp
    @date   17, October 2022

    ===============================================================
 */

#include <jaut_logger/jaut_AbstractLogger.h>

#include <jaut_logger/sink/jaut_ILogSink.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    JAUT_NODISCARD
    juce::String createExceptionString(const juce::String &message, const std::exception &ex)
    {
        juce::String output;
        
        if (!message.isEmpty())
        {
            output << message << juce::newLine;
        }
        
        output << "An exception occurred '" << jaut::getActualTypeName(&ex)
               << "': " << ex.what();
        
        return output;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Logger::LogBuilder
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    AbstractLogger::LogBuilder::LogBuilder(AbstractLogger &parLogger, LogLevel::Value parLevel)
        : logger(parLogger),
          logMessage(parLogger.makeLog((parLogger.shouldLog(parLevel) ? parLevel : LogLevel::Off), ""))
    {}
    
    AbstractLogger::LogBuilder::~LogBuilder()
    {
        if (logMessage.level == LogLevel::Off)
        {
            return;
        }
        
        try
        {
            if (!logMessage.message.isEmpty() || logMessage.exception.has_value() || !logMessage.fields.empty())
            {
                logger.log(std::move(logMessage));
            }
        }
        catch (const std::exception &ex)
        {
            logger.handleException(ex);
        }
    }
}
//======================================================================================================================
// endregion Logger::LogBuilder
//**********************************************************************************************************************
// region Logger
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    AbstractLogger::LogBuilder AbstractLogger::operator<<(Level parLevel)
    {
        return { *this, parLevel };
    }
    
    //==================================================================================================================
    void AbstractLogger::trace  (const juce::String &parMessage) { log(makeLog(Level::Trace,   parMessage)); }
    void AbstractLogger::debug  (const juce::String &parMessage) { log(makeLog(Level::Debug,   parMessage)); }
    void AbstractLogger::verbose(const juce::String &parMessage) { log(makeLog(Level::Verbose, parMessage)); }
    void AbstractLogger::info   (const juce::String &parMessage) { log(makeLog(Level::Info,    parMessage)); }
    void AbstractLogger::warn   (const juce::String &parMessage) { log(makeLog(Level::Warn,    parMessage)); }
    void AbstractLogger::error  (const juce::String &parMessage) { log(makeLog(Level::Error,   parMessage)); }
    void AbstractLogger::fatal  (const juce::String &parMessage) { log(makeLog(Level::Fatal,   parMessage)); }
    
    void AbstractLogger::trace(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Trace, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::debug(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Debug, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::verbose(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Verbose, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::info(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Info, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::warn(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Warn, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::error(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Error, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    void AbstractLogger::fatal(const juce::String &parMessage, const std::exception &parException)
    {
        log(makeLog(Level::Fatal, parMessage, {}, LogMessage::ExceptionSpec::fromException(parException)));
    }
    
    //==================================================================================================================
    void AbstractLogger::setLogLevel(Level parLogLevel) noexcept
    {
        logLevel.store(parLogLevel);
    }
    
    //==================================================================================================================
    AbstractLogger::Level AbstractLogger::getLogLevel() const noexcept
    {
        return logLevel.load();
    }
    
    const juce::String& AbstractLogger::getName() const noexcept
    {
        return name;
    }
    
    const std::vector<AbstractLogger::SinkPtr>& AbstractLogger::getSinks() const noexcept
    {
        return sinks;
    }
    
    bool AbstractLogger::shouldLog(Level parLogLevel) const noexcept
    {
        return !(parLogLevel == Level::Off || parLogLevel < getLogLevel());
    }
    
    //==================================================================================================================
    jaut::LogMessage AbstractLogger::makeLog(jaut::LogLevel::Value                    level,
                                             juce::String                             message,
                                             std::vector<jaut::LogMessage::Field>     fields,
                                             std::optional<LogMessage::ExceptionSpec> ex)
    {
        jaut::LogMessage log_message;
        log_message.name      = getName();
        log_message.timestamp = juce::Time::getCurrentTime();
        log_message.level     = level;
        log_message.threadId  = std::this_thread::get_id();

        if (const juce::Thread *c_thread = juce::Thread::getCurrentThread())
        {
            log_message.threadName = c_thread->getThreadName();
        }
        
        std::swap(log_message.fields,  fields);
        std::swap(log_message.message, message);
        
        if (ex.has_value())
        {
            std::swap(log_message.exception, ex);
        }
        
        return log_message;
    }
}
//======================================================================================================================
// endregion Logger
//**********************************************************************************************************************
