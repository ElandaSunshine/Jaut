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
    @file   jaut_LogFormatCallback.cpp
    @date   07, November 2022

    ===============================================================
 */

#include <jaut_logger/format/jaut_LogFormatCallback.h>

#include <jaut_logger/jaut_LogMessage.h>



//**********************************************************************************************************************
// region LogFormatCallback
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LogFormatCallback::LogFormatCallback(NonNull<FormatAppenderType> parAppendCallback,
                                         FormatReplacerType          parReplaceCallback)
        : appendCallback (std::move(parAppendCallback.get())),
          replaceCallback(std::move(parReplaceCallback))
    {}
    
    //==================================================================================================================
    juce::String LogFormatCallback::format(const LogMessage &parLogMessage) const
    {
        return appendCallback(parLogMessage);
    }
    
    juce::String LogFormatCallback::formatReplace(const jaut::LogMessage &logMessage, const juce::String &content)
    {
        return (replaceCallback ? replaceCallback(logMessage, content) : "");
    }
    
    //==================================================================================================================
    bool LogFormatCallback::supportsReplacingFormatter() const
    {
        return (replaceCallback != nullptr);
    }
}
//======================================================================================================================
// endregion LogFormatCallback
//**********************************************************************************************************************
