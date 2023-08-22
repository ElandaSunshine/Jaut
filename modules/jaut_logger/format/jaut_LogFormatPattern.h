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
    @file   jaut_LogFormatPattern.h
    @date   07, November 2022

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
     *  Provides a simple and straightforward fmt pattern formatter.<br>
     *  Use this when all you want is to format a line with some of the log data.
     *  <br><br>
     *  Valid fmt named argument keys are:
     *  <ul>
     *      <li>name: The name of the logger that issued the log event</li>
     *      <li>msg: The message that was issued</li>
     *      <li>t_id: The thread id that issued the log event</li>
     *      <li>t_name: The thread name that issued the log event or empty if the thread has no name</li>
     *      <li>t_ref: The thread name if it is available, otherwise the thread id</li>
     *      <li>time_g: The timestamp of when the log event was issued in UTC</li>
     *      <li>time_l: The timestamp of when the log event was issued in local time</li>
     *      <li>level: The event's current log level</li>
     *      <li>ex_id: The name of the exception if one was thrown</li>
     *      <li>ex_msg: The message of the exception if one was thrown</li>
     *      <li>fd_{field name}: The name of a field that was attached to the log event</li>
     *      <li>Any other fmt included formatting key, found <a href="https://fmt.dev/dev/syntax.html">here</a></li>
     *  </ul>
     */
    class JAUT_API LogFormatPattern : public ILogFormat
    {
    public:
        inline static const juce::String default_pattern
            = "{time_g:%F %T} [{name}/{level}][{t_ref}]: {msg}";
        inline static const juce::String default_exceptionPattern
            = juce::String("{time_g:%F %T} [{name}/{level}][{t_ref}]: {msg}")
                + juce::NewLine::getDefault() + "    Encountered exception {ex_id}: {ex_msg}";
        
        //==============================================================================================================
        explicit LogFormatPattern(juce::String pattern          = default_pattern,
                                  juce::String exceptionPattern = default_exceptionPattern);
        
        //==============================================================================================================
        JAUT_NODISCARD
        juce::String format(const LogMessage &logMessage) const override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool supportsReplacingFormatter() const override;
        
    private:
        juce::String pattern;
        juce::String exceptionPattern;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogFormatPattern)
    };
}
