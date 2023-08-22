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
    @file   jaut_LogFormatCallback.h
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
    /** A simple formatter that can be used via a passed in lambda function or other invocable. */
    class JAUT_API LogFormatCallback : public ILogFormat
    {
    public:
        /** The type of the appending format call. */
        using FormatAppenderType = std::function<juce::String(const LogMessage&)>;
        
        /** The type of the replacing format call. */
        using FormatReplacerType = std::function<juce::String(const LogMessage&, const juce::String&)>;
        
        //==============================================================================================================
        /**
         *  Creates a new instance of the callback formatter.
         *  
         *  @param appendCallback  The appending formatter callback
         *  @param replaceCallback The replacing formatter callback, leave it nullptr if you don't need it
         */
        explicit LogFormatCallback(NonNull<FormatAppenderType> appendCallback,
                                   FormatReplacerType          replaceCallback = nullptr);
        
        //==============================================================================================================
        JAUT_NODISCARD
        juce::String format(const LogMessage &logMessage) const override;
        
        JAUT_NODISCARD
        juce::String formatReplace(const jaut::LogMessage &logMessage, const juce::String &content) override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool supportsReplacingFormatter() const override;
        
    private:
        FormatAppenderType appendCallback;
        FormatReplacerType replaceCallback;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogFormatCallback)
    };
}
