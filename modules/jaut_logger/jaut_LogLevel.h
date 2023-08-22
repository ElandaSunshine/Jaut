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
    @file   jaut_LogLevel.h
    @date   03, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_logger_define.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /** The log level to produce. */
    struct JAUT_API LogLevel
    {
        //==============================================================================================================
        /** The log-level priority list. */
        enum Value
        {
            /**
             *  For maximum fine-grained messages.<br>
             *  This is the thing you will want to use when you need to trace how your application works.
             */
            Trace,
            
            /** For developer messages or more fine-grained results. */
            Debug,
            
            /**
             *  For messages that should only be output when verbose mode is enabled.
             *  Verbose messages will also get the Verbose tag by default,
             *  this can be changed by JAUT_PROVIDER_LOGGER_VERBOSE_TAG however.
             */
            Verbose,
            
            /** For informative messages. */
            Info,
            
            /** For warning messages that should prepare the user for possible conflicts in future. */
            Warn,
            
            /** For problems that arise during runtime, but are still recoverable and usually don't force a shutdown. */
            Error,
            
            /** For problems that arise during runtime that usually end up in shutting down the process. */
            Fatal,
            
            /**
             *  No logs.
             *  Using this as level for logging a message will do absolutely nothing.
             */
            Off
        };
        
        //==============================================================================================================
        /** The list of names for each log level. */
        static constexpr std::array names {
            std::string_view{ "Trace" },
            std::string_view{ "Debug" },
            std::string_view{ JUCE_STRINGIFY(JAUT_LOGGER_VERBOSE_TAG) },
            std::string_view{ "Info"  },
            std::string_view{ "Warn"  },
            std::string_view{ "Error" },
            std::string_view{ "Fatal" }
        };
        
        static_assert(names.size() == LogLevel::Off);
    };
}
