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
    @file   jaut_LogFormatJson.h
    @date   24, January 2023

    ===============================================================
 */

#pragma once

#include <jaut_logger/format/jaut_ILogFormat.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    struct LogMessage;
    
    //==================================================================================================================
    /**
     *  Provides a json formatter that will log all events as json objects.
     *  <br><br>
     *  The appending formatter will just output json objects individually, that means, with several objects outputted
     *  the log file will not be valid json anymore.
     *  <br><br>
     *  The replacing formatter will output proper valid json depending on the content of the current log file.
     */
    class JAUT_API LogFormatJson : public ILogFormat
    {
    public:
        static constexpr bool default_preferReplacingFormatter = true;
        static constexpr bool default_prettify                 = true;
        static constexpr bool default_tryPreserveInvalidLog    = true;
        static constexpr bool default_shouldCache              = true;
        
        //==============================================================================================================
        struct Options
        {
            /**
             *  Determines whether the replacing instead of the appending formatter should be used.<br>
             *  For non supporting sinks, this will be ignored.
             */
            bool preferReplacingFormatter = default_preferReplacingFormatter;
            
            /**
             *  Determines whether the json output should be pretty printed with indentation and new lines,
             *  or whether it should be put into one line for compactness reasons.
             */
            bool prettyPrint = default_prettify;
            
            /**
             *  Determines whether invalid logs should be preserved or overwritten.
             *  <br><br>
             *  If this is true and the log file contains invalid json that cannot be parsed for some reason,
             *  this formatter will separate the invalid text with "---BEGIN VALID---"
             *  and let the valid json be followed after that.<br>
             *  This helps when, despite being invalid json, any previous logs need to be preserved,
             *  otherwise it will just delete the invalid content and replace it with new incoming log events.
             *  <br><br>
             *  This is only important when the replacing formatter is used and has no use for the appending version.
             */
            bool tryPreserveInvalidLog = default_tryPreserveInvalidLog;
            
            /**
             *  Determines whether the log file should be internally cached,
             *  so as to that the log file doesn't need to be read everytime again.
             *  <br><br>
             *  This is only important when the replacing formatter is used and has no use for the appending version.
             */
            bool shouldCache = default_shouldCache;
        };
        
        //==============================================================================================================
        /**
         *  Creates a new instance of this formatter.
         *  @param options The options for the formatter
         */
        explicit LogFormatJson(Options options);
        
        /** Creates a new instance of this formatter. */
        LogFormatJson() = default;
        
        //==============================================================================================================
        JAUT_NODISCARD
        juce::String format(const LogMessage &logMessage) const override;
        
        JAUT_NODISCARD
        juce::String formatReplace(const jaut::LogMessage &logMessage, const juce::String &content) override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool supportsReplacingFormatter() const noexcept override;
        
    private:
        Options options;
        
        juce::var    json;
        juce::String failedJson;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogFormatJson)
    };
}
