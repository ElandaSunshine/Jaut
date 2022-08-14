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

    @author Elanda (elanda@elandasunshine.com)
    @file   logger.cpp
    @date   24, March 2022

    ===============================================================
 */
 
#include "logger.h"

#if JAUT_OPTLIB_FMT
    #include "fmt/core.h"
    #include "fmt/color.h"
#endif

#ifdef __GNUG__
    #include "cxxabi.h"
#endif

//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    #ifdef __GNUG__
    juce::String getExceptionName(const std::exception *exception)
    {
        int status {};
        std::unique_ptr<char, void(*)(void*)> name {
            abi::__cxa_demangle(typeid(*exception).name(), nullptr, nullptr, &status),
            std::free
        };
        
        return (!status ? name.get() : "std::exception");
    }
    #else
    juce::String getExceptionName(const std::exception *exception)
    {
        return typeid(*exception).name();
    }
    #endif
    
    juce::String formatString(const juce::String &pattern, const juce::String &input, const juce::String &loggerName,
                              const juce::String &logLevel)
    {
        const juce::String time_patterns = "aAbBcdHIjmMpSUwWxXyYZ%";
        
        const juce::Time now = juce::Time::getCurrentTime();
        juce::String buffer;
        buffer.preallocateBytes(1024);
        juce::String::CharPointerType it = pattern.getCharPointer();
        
        if (*it == '\0')
        {
            return buffer;
        }
        
        for (;;)
        {
            const juce::juce_wchar c = it.getAndAdvance();
            juce::String value;
            
            if (c == '\0')
            {
                break;
            }
            
            if (c == '%')
            {
                const juce::juce_wchar n         = *it;
                bool                   break_out = false;
                
                if (time_patterns.containsChar(n))
                {
                    value << '%' << n;
                }
                else
                {
                    switch (n)
                    {
                        case '\0':
                        {
                            break_out = true;
                            break;
                        }
        
                        case 't':
                        {
                            const juce::Thread *thread = juce::Thread::getCurrentThread();
            
                            if (thread)
                            {
                                const juce::String name = thread->getThreadName();
                                value = (name.isEmpty() ? "MAIN" : name);
                            }
                            else
                            {
                                value = "MAIN";
                            }
            
                            break;
                        }
        
                        case 'n':
                        {
                            value = loggerName;
                            break;
                        }
        
                        case 'q':
                        {
                            value = input;
                            break;
                        }
        
                        case 'l':
                        {
                            value = logLevel;
                            break;
                        }
        
                        case 'N':
                        {
                            value = juce::String(now.getMilliseconds());
                            break;
                        }
        
                        default:;
                    }
    
                    if (break_out)
                    {
                        break;
                    }
                }
                
                ++it;
            }
            else
            {
                buffer << c;
            }
            
            if (!value.isEmpty())
            {
                buffer << value;
            }
        }
        
        return now.formatted(buffer);
    }
    
    constexpr int getBitPosition(jaut::Logger::LogLevel level)
    {
        return static_cast<int>(std::log2(static_cast<double>(level)));
    }
    
    std::string_view getLogLevelString(jaut::Logger::LogLevel level)
    {
        static constexpr std::array level_names {
            "",
            "INFO",
            "WARN",
            "ERROR",
            "DEBUG",
            "FATAL"
        };
        
        const int pos = getBitPosition(level);
        
        // Uhhh seems like I forgot to add something :)
        jassert(pos < level_names.size());
        return level_names[static_cast<std::size_t>(pos)];
    }
    
    juce::String createExceptionString(const juce::String &message, const std::exception &ex)
    {
        juce::String output;
        output << message << "\nAn exception occurred '" << ::getExceptionName(&ex) << "': " << ex.what();
        return output;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Logger
//======================================================================================================================
//======================================================================================================================
namespace jaut
{
    //******************************************************************************************************************
    // region LogBuilder
    //==================================================================================================================
    Logger::LogBuilder::LogBuilder(Logger &logger, LogLevel level, const juce::var &object)
        : logger(logger), message(object.toString()), logLevel(level)
    {}
    
    Logger::LogBuilder::~LogBuilder()
    {
        flush();
    }

    //==================================================================================================================
    Logger::LogBuilder& Logger::LogBuilder::operator<<(const juce::var &object)
    {
        this->message << object.toString();
        return *this;
    }
    
    Logger::LogBuilder& Logger::LogBuilder::operator<<(const ILoggable &loggable)
    {
        this->message << loggable.toLog();
        return *this;
    }
    
    //==================================================================================================================
    void Logger::LogBuilder::flush()
    {
        if (!message.isEmpty())
        {
            logger.log(logLevel, message);
            message.clear();
        }
    }
    //==================================================================================================================
    // endregion LogBuilder
    //******************************************************************************************************************
    // region Logger
    //==================================================================================================================
    Logger::Logger(juce::String name, SinkPtr sink)
        : name(std::move(name)),
          sink(std::move(sink))
    {}
    
    //==================================================================================================================
    Logger::LogBuilder Logger::operator<<(LogLevel level)
    {
        return { *this, level };
    }
    
    Logger::LogBuilder Logger::operator<<(const juce::var &object)
    {
        return { *this, options.defaultLogLevel, object };
    }
    
    Logger::LogBuilder Logger::operator<<(const jaut::ILoggable &loggable)
    {
        return { *this, options.defaultLogLevel, loggable.toLog() };
    }
    
    //==================================================================================================================
    void Logger::log(LogLevel level, const juce::String &message)
    {
        if (level == LogLevel::Debug && !options.outputDebugMessages)
        {
            return;
        }
        
        const juce::String level_name = ::getLogLevelString(level).data();
        juce::String       formatted  = (level == None ? message : ::formatString(format, message, name, level_name));
        
        if ((options.isolatedLevels & level) == level)
        {
            formatted = '\n' + formatted + '\n';
        }
        
        sink->print(formatted, level);
        sink->flush();
    }
    
    void Logger::unformatted(const juce::String &message) { log(LogLevel::None,  message); }
    void Logger::info       (const juce::String &message) { log(LogLevel::Info,  message); }
    void Logger::warn       (const juce::String &message) { log(LogLevel::Warn,  message); }
    void Logger::error      (const juce::String &message) { log(LogLevel::Error, message); }
    void Logger::debug      (const juce::String &message) { log(LogLevel::Debug, message); }
    void Logger::fatal      (const juce::String &message) { log(LogLevel::Fatal, message); }
    
    void Logger::log(LogLevel level, const juce::String &message, const std::exception &exception)
    {
        log(level, ::createExceptionString(message, exception));
    }
    
    void Logger::unformatted(const juce::String &message, const std::exception &exception)
    {
        unformatted(::createExceptionString(message, exception));
    }
    
    void Logger::info(const juce::String &message, const std::exception &exception)
    {
        info(::createExceptionString(message, exception));
    }
    
    void Logger::warn(const juce::String &message, const std::exception &exception)
    {
        warn(::createExceptionString(message, exception));
    }
    
    void Logger::error(const juce::String &message, const std::exception &exception)
    {
        error(::createExceptionString(message, exception));
    }
    
    void Logger::debug(const juce::String &message, const std::exception &exception)
    {
        debug(::createExceptionString(message, exception));
    }
    
    void Logger::fatal(const juce::String &message, const std::exception &exception)
    {
        fatal(::createExceptionString(message, exception));
    }
    
    //==================================================================================================================
    void Logger::setFormat(juce::String newFormat) noexcept
    {
        format = std::move(newFormat);
    }
    
    //==================================================================================================================
    const Logger::Options &Logger::getOptions() const noexcept
    {
        return options;
    }
    
    void Logger::setOptions(Options newOptions)
    {
        options = newOptions;
    }
    
    //==================================================================================================================
    const juce::String &Logger::getName() const noexcept
    {
        return name;
    }
    
    ILogSink &Logger::getSink() noexcept
    {
        return *sink;
    }
    
    const ILogSink &Logger::getSink() const noexcept
    {
        return *sink;
    }
    //==================================================================================================================
    // endregion Logger
    //******************************************************************************************************************
}
//======================================================================================================================
//======================================================================================================================
// endregion Logger
//**********************************************************************************************************************