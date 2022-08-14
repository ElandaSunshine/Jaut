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
    @file   logmanager.h
    @date   28, March 2022

    ===============================================================
 */
#pragma once

namespace jaut
{
    class Logger;
    class ILogSink;
    
    namespace detail
    {
        class LoggerRegistry
        {
        public:
            using LogPtr = std::shared_ptr<Logger>;
            
            //==========================================================================================================
            static LoggerRegistry& getInstance();
            
            //==========================================================================================================
            LogPtr get(const juce::String &name);
            
            //==========================================================================================================
            void addLogger(LogPtr logger);
            LogPtr createLogger(const juce::String &name, std::unique_ptr<ILogSink> sink);
            
            //==========================================================================================================
            bool contains(const juce::String &name) const;
            void clear();
            
        private:
            juce::CriticalSection                    criticalSection;
            std::unordered_map<juce::String, LogPtr> loggers;
            
            //==========================================================================================================
            bool hasLogger(const juce::String &name) const;
        };
    }
    
    namespace log
    {
        //==============================================================================================================
        using Ptr = std::shared_ptr<Logger>;
        
        //==============================================================================================================
        /**
         *  Gets a registered logger by its name.
         *
         *  @param name The name of the logger to get
         *  @return The logger that is registered by that name or a nullptr if none was found
         */
        std::shared_ptr<Logger> get(const juce::String &name);
        
        /**
         *  Checks whether a logger by that name has been registered.
         *
         *  @param name The name of the logger
         *  @return True if the logger exists, false otherwise
         */
        bool exists(const juce::String &name);
        
        /**
         *  Registers a logger into the global logger map.
         *  The name of the logger will be determined from the logger class' set name.
         *
         *  @param logger The logger to register
         *  @throws LoggerExistsException If the logger already exists
         */
        void add(std::shared_ptr<Logger> logger);
        
        /**
         *  Creates a logger and adds it to the global registry.
         *
         *  @param name    The name of the new logger
         *  @param sinkPtr The sink the logger should be using
         *  @return The new logger
         *
         *  @throws LoggerExistsException If a logger with that name was already registered
         */
        std::shared_ptr<Logger> createLogger(const juce::String &name, std::unique_ptr<ILogSink> sinkPtr);
    }
}


