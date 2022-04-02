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
    @file   ilogsink.h
    @date   28, March 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  Defines a output sink for jaut::Logger objects.
     *  Sinks provide an interface that help in handling how loggers should output logging messages.
     */
    struct ILogSink
    {
        //==============================================================================================================
        virtual ~ILogSink() = default;
        
        //==============================================================================================================
        /**
         *  Print the message to the log.
         *
         *  @param message The message to print
         *  @param level   The log level
         */
        virtual void print(const juce::String &message, Logger::LogLevel level) = 0;
        
        /**
         *  Flushes the stream if a buffer is needed.
         *  Otherwise leave it as is.
         */
        virtual void flush() {}
    };
}