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
    @file   jaut_ILogSink.h
    @date   28, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/format/jaut_LogFormatPattern.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /** Sinks are the actual output implementations, they will be there to map a logger to an output stream. */
    struct ILogSink
    {
        //==============================================================================================================
        /** The default formatting type used for inbuilt-sinks. */
        using DefaultFormatterType = LogFormatPattern;
        
        //==============================================================================================================
        virtual ~ILogSink() = default;
        
        //==============================================================================================================
        /**
         *  Prints the message.
         *  If the sink manages a buffer, this should add the message to the buffer instead of directly outputting it.
         *  <br><br>
         *  Note that, if there is a buffer for this sink,
         *  the buffer should not be managing mere references to the data; it is not said the objects will still exist
         *  at the point of flushing.
         *  
         *  @param message The message content
         */
        virtual void print(const LogMessage &message) = 0;
        
        //==============================================================================================================
        /**
         *  If there is an internal buffer for this sink, you can use this to pre-allocate it.
         *  @param preallocateBufferSize The likely needed amount of log-entries
         */
        virtual void prepare(int numLines) { jaut::ignore(numLines); }
        
        /** Flushes the stream if the sink manages a buffer. */
        virtual void flush() {}
        
        //==============================================================================================================
        /** An optional method that should be used to prepare a sink before it can be used.<br> */
        virtual void onOpen() {}
        
        /** An optional method that should be used to close off a sink before destruction.<br> */
        virtual void onClose() {}
        
        //==============================================================================================================
        /**
         *  Gets the formatter for this sink.
         *  If no formatter is found, the message will be as is or the way the sink formats it.
         *  
         *  @return The sink formatter or nullptr if no formatter was given
         */
        JAUT_NODISCARD
        virtual const ILogFormat* getFormatter() const = 0;
    };
}
