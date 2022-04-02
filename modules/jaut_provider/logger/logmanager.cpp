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
    @file   logmanager.cpp
    @date   28, March 2022

    ===============================================================
 */

//**********************************************************************************************************************
// region LoggerRegistry
//======================================================================================================================
namespace jaut::detail
{
    //==================================================================================================================
    LoggerRegistry& LoggerRegistry::getInstance()
    {
        static LoggerRegistry registry;
        return registry;
    }
    
    //==================================================================================================================
    LoggerRegistry::LogPtr LoggerRegistry::get(const juce::String &name)
    {
        const juce::ScopedLock lock(criticalSection);
        
        auto it = loggers.find(name);
        return (it != loggers.end() ? it->second : nullptr);
    }
    
    //==================================================================================================================
    void LoggerRegistry::addLogger(LogPtr logger)
    {
        const juce::ScopedLock lock(criticalSection);
        const juce::String &name = logger->getName();
        
        if (hasLogger(name))
        {
            throw LoggerExistsException(name);
        }
        
        loggers[name] = std::move(logger);
    }
    
    LoggerRegistry::LogPtr LoggerRegistry::createLogger(const juce::String &name, std::unique_ptr<ILogSink> sink)
    {
        const juce::ScopedLock lock(criticalSection);
        
        if (hasLogger(name))
        {
            throw LoggerExistsException(name);
        }
        
        auto logger   = std::make_shared<Logger>(name, std::move(sink));
        loggers[name] = logger;
        
        return logger;
    }
    
    //==================================================================================================================
    bool LoggerRegistry::contains(const juce::String &name) const
    {
        const juce::ScopedLock lock(criticalSection);
        return hasLogger(name);
    }
    
    void LoggerRegistry::clear()
    {
        loggers.clear();
    }
    
    //==================================================================================================================
    bool LoggerRegistry::hasLogger(const juce::String &name) const
    {
        return loggers.find(name) != loggers.end();
    }
}
//======================================================================================================================
// endregion LoggerRegistry
//**********************************************************************************************************************
// region namespace::log
//======================================================================================================================
namespace jaut::log
{
    std::shared_ptr<Logger> get(const juce::String &name)
    {
        return detail::LoggerRegistry::getInstance().get(name);
    }
    
    bool exists(const juce::String &name)
    {
        return detail::LoggerRegistry::getInstance().contains(name);
    }
    
    void add(std::shared_ptr<Logger> logger)
    {
        detail::LoggerRegistry::getInstance().addLogger(std::move(logger));
    }
    
    std::shared_ptr<Logger> createLogger(const juce::String &name, std::unique_ptr<ILogSink> sinkPtr)
    {
        return detail::LoggerRegistry::getInstance().createLogger(name, std::move(sinkPtr));
    }
}
//======================================================================================================================
// endregion namespace::log
//**********************************************************************************************************************