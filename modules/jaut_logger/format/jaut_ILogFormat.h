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
    @file   jaut_ILogFormat.h
    @date   16, October 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_logger/jaut_LogMessage.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /**
     *  Provides an interface for custom and inbuilt sink formatters.<br>
     *  A formatter will read the provided log event and try to output it to a conscious format.
     *  <br><br>
     *  A formatter can be an appending formatter, a replacing formatter or both.<br>
     *  Since not all sinks support replacing formatters, every formatter must be at least an appending formatter to
     *  guarantee support for all sinks.
     *  <br><br>
     *  A replacing formatter supporting sink will always pick the replacing version.
     *  (unless specified otherwise, if the formatter supports setting this option)
     */
    struct JAUT_API ILogFormat
    {
        //==============================================================================================================
        struct Util
        {
            template<class Fn>
            static void printHeader(const ILogFormat *const formatter, Fn &&printFunc)
            {
                if (juce::String header; formatter && !(header = formatter->getHeader()).isEmpty())
                {
                    std::forward<Fn>(printFunc)(header);
                }
            }
            
            template<class Fn>
            static void printFooter(const ILogFormat *const formatter, Fn &&printFunc)
            {
                if (juce::String footer; formatter && !(footer = formatter->getFooter()).isEmpty())
                {
                    std::forward<Fn>(printFunc)(footer);
                }
            }
        };
        
        //==============================================================================================================
        virtual ~ILogFormat() = default;
        
        //==============================================================================================================
        /**
         *  Formats the incoming message and appends it to the output.
         *  
         *  @param logMessage The log event
         *  @return The string to append
         */
        JAUT_NODISCARD
        virtual juce::String format(const LogMessage &logMessage) const = 0;
        
        /**
         *  Formats the incoming message together with all the last log events and formats the stream so that it
         *  can be replaced together with the latest log event.
         *  
         *  @param logMessage The log event
         *  @param content    The entire content of the current output, this means all previous log events
         *  @return The string to replace the output with
         */
        JAUT_NODISCARD
        virtual juce::String formatReplace(JAUT_MUNUSED const LogMessage   &logMessage,
                                           JAUT_MUNUSED const juce::String &content)
        {
            return {};
        }
        
        //==============================================================================================================
        /**
         *  Gets the header that will be printed on construction of the sink.<br>
         *  Note that this will always be outputted, even if the output is already filled from a previous session.
         *  @return The header string
         */
        JAUT_NODISCARD
        virtual juce::String getHeader() const { return ""; }
        
        /**
         *  Gets the footer that will be printed on destruction of the sink.<br>
         *  Note that this will always be outputted, even if the output is already filled from a previous session.
         *  @return The footer string
         */
        JAUT_NODISCARD
        virtual juce::String getFooter() const { return ""; }
        
        //==============================================================================================================
        /**
         *  Whether this formatter supports replace-formatting.<br>
         *  Note that you can also use this to determine that a formatter, that is in fact a replacing formatter,
         *  wants to disable replace formatting through a flag or similar means.
         *  
         *  @return True if replace-formatting is supported, false if not
         */
        JAUT_NODISCARD
        virtual bool supportsReplacingFormatter() const = 0;
    };
}
