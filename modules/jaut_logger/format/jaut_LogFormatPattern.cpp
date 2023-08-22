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
    @file   jaut_LogFormatPattern.cpp
    @date   07, November 2022

    ===============================================================
 */

#include <jaut_logger/format/jaut_LogFormatPattern.h>

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/detail/jaut_fmt.h>



//**********************************************************************************************************************
// region LogFormatPattern
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LogFormatPattern::LogFormatPattern(juce::String parPattern, juce::String parExceptionPattern)
        : pattern(std::move(parPattern)),
          exceptionPattern(std::move(parExceptionPattern))
    {}
    
    //==================================================================================================================
    juce::String LogFormatPattern::format(const LogMessage &parLogMessage) const
    {
        using namespace fmt::literals;
        
        const auto             time      = static_cast<std::time_t>(parLogMessage.timestamp.toMilliseconds() / 1000);
        const std::size_t      num_args  = (10 + parLogMessage.fields.size());
        const std::string_view l_pattern = (parLogMessage.exception.has_value()
                                                ? exceptionPattern
                                                : pattern).toRawUTF8();
        
        fmt::dynamic_format_arg_store<fmt::format_context> args;
        args.reserve(num_args, num_args);
        
        std::stringstream thread_id;
        thread_id << parLogMessage.threadId;
        
        args.push_back("name"_a   = parLogMessage.name);
        args.push_back("msg"_a    = parLogMessage.message);
        args.push_back("t_id"_a   = thread_id.str());
        args.push_back("t_name"_a = parLogMessage.threadName);
        args.push_back("t_ref"_a  = (!parLogMessage.threadName.isEmpty() ? parLogMessage.threadName : thread_id.str()));
        args.push_back("time_g"_a = fmt::gmtime   (time));
        args.push_back("time_l"_a = fmt::localtime(time));
        args.push_back("level"_a  = LogLevel::names[parLogMessage.level]);
        
        if (parLogMessage.exception.has_value())
        {
            args.push_back("ex_id"_a  = parLogMessage.exception->name);
            args.push_back("ex_msg"_a = parLogMessage.exception->message);
        }
        else
        {
            args.push_back("ex_id"_a  = "n/a");
            args.push_back("ex_msg"_a = "n/a");
        }
        
        for (const auto &[name, value] : parLogMessage.fields)
        {
            args.push_back(fmt::arg(name.toRawUTF8(), jaut::toString(value)));
        }
        
        return (fmt::vformat(l_pattern, args) + juce::newLine);
    }
    
    //==================================================================================================================
    bool LogFormatPattern::supportsReplacingFormatter() const
    {
        return false;
    }
}
//======================================================================================================================
// endregion LogFormatPattern
//**********************************************************************************************************************
