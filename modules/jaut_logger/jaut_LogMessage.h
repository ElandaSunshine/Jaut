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
    @file   jaut_LogMessage.h
    @date   03, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogLevel.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_Stringable.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /** The message container a single log-entry will be composed off. */
    struct JAUT_API LogMessage
    {
        //==============================================================================================================
        /** A tag denoting some sort of information for the entry. */
        struct Field
        {
            /** The name of the tag. */
            juce::String name;
            
            /** The content of the tag. */
            juce::var value;
        };
        
        struct ExceptionSpec
        {
            //==========================================================================================================
            JAUT_NODISCARD
            static ExceptionSpec fromException(const std::exception &ex);
            
            //==========================================================================================================
            juce::String name;
            juce::String message;
        };
        
        //==============================================================================================================
        /** The list of additional fields. */
        std::vector<Field> fields;
        
        /** The exception if one was thrown. */
        std::optional<ExceptionSpec> exception;
        
        /** The actual log message. */
        juce::String message;
        
        /** The logger name. */
        juce::String name;
        
        /** The name of the thread or empty if no name was set. */
        juce::String threadName;
        
        /** The id of the thread. */
        std::thread::id threadId;
        
        /** The timestamp of the message of when it has been posted. */
        juce::Time timestamp;
        
        /** The priority of the message. */
        LogLevel::Value level;
    };
    
    //==================================================================================================================
    namespace detail
    {
        template<class Field>
        void emplaceField(std::vector<Field> &fieldList, Field &&field)
        {
            LogMessage::Field temp_field = std::forward<Field>(field);
            
            auto it = std::find_if(fieldList.begin(), fieldList.end(),
                                [&name = temp_field.name](const LogMessage::Field &f)
                                {
                                    return (f.name == name);
                                });
            
            if (it != fieldList.end())
            {
                it->value = temp_field.value;
            }
            else
            {
                fieldList.emplace_back(std::move(temp_field));
            }
        }
    }
    
    //==================================================================================================================
    template<>
    struct JAUT_API Stringable<LogMessage::Field>
    {
        JAUT_NODISCARD
        static juce::String toString(const LogMessage::Field &obj)
        {
            return "{name=" + obj.name + ", value=" + jaut::toString(obj.value) + '}';
        }
        
        JAUT_NODISCARD
        static juce::String toName()
        {
            return "jaut::LogMessage::Field";
        }
    };
    
    template<>
    struct JAUT_API Stringable<LogMessage::ExceptionSpec>
    {
        JAUT_NODISCARD
        static juce::String toString(const LogMessage::ExceptionSpec &obj)
        {
            return "{name=" + obj.name + ", message=" + obj.message + '}';
        }
        
        JAUT_NODISCARD
        static juce::String toName()
        {
            return "jaut::LogMessage::ExceptionSpec";
        }
    };
    
    template<>
    struct JAUT_API Stringable<LogMessage>
    {
        JAUT_NODISCARD
        static juce::String toString(const LogMessage &obj)
        {
            const juce::String exception_string = [&ex = obj.exception]()
            {
                return (ex.has_value() ? jaut::toString(ex.value()) : "N/A");
            }();
            
            std::stringstream thread_id;
            thread_id << obj.threadId;
            
            return "{fields="     + jaut::toString(obj.fields)
                + ", exception="  + exception_string
                + ", message="    + obj.message
                + ", name="       + obj.name
                + ", threadName=" + obj.threadName
                + ", threadId="   + thread_id.str()
                + ", timestamp="  + obj.timestamp.toISO8601(true) + " ("
                                  + jaut::toString(obj.timestamp.toMilliseconds()) + ")"
                  ", level="      + jaut::toString(LogLevel::names[obj.level]) + " (Priority: "
                                  + jaut::toString(obj.level) + ")}";
        }
    
        JAUT_NODISCARD
        static juce::String toName()
        {
            return "jaut::LogMessage";
        }
    };
    
    //==================================================================================================================
    /**
     *  Creates a logging field which should be added to the logging message.
     *  
     *  @param key   The field key
     *  @param value The field value
     *  @return The new field
     */
    JAUT_NODISCARD
    JAUT_API inline LogMessage::Field mfield(const juce::String &key, juce::var value)
    {
        return { ("fd_" + key), std::move(value) };
    }
    
    //==================================================================================================================
    // IMPLEMENTATION LogMessage::ExceptionSpec
    LogMessage::ExceptionSpec LogMessage::ExceptionSpec::fromException(const std::exception &ex)
    {
        return { getActualTypeName(&ex), juce::String(ex.what()) };
    }
}
