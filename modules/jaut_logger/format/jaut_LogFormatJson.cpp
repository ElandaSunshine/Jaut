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
    @file   jaut_LogFormatJson.cpp
    @date   24, January 2023

    ===============================================================
 */

#include <jaut_logger/format/jaut_LogFormatJson.h>

#include <jaut_logger/jaut_LogMessage.h>


//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    juce::var createJsonObject(const jaut::LogMessage &logMessage)
    {
        auto root_obj = std::make_unique<juce::DynamicObject>();
        root_obj->setProperty("name",        logMessage.name);
        root_obj->setProperty("message",     logMessage.message);
        root_obj->setProperty("thread_name", logMessage.threadName);
        root_obj->setProperty("thread_id",   jaut::toString(logMessage.threadId));
        root_obj->setProperty("timestamp",   logMessage.timestamp.toMilliseconds());
        
        auto level_obj = std::make_unique<juce::DynamicObject>();
        level_obj->setProperty("id",   static_cast<int>(logMessage.level));
        level_obj->setProperty("name", jaut::LogLevel::names[static_cast<std::size_t>(logMessage.level)].data());
        root_obj->setProperty("level", level_obj.release());
        
        if (logMessage.exception.has_value())
        {
            auto ex_obj = std::make_unique<juce::DynamicObject>();
            ex_obj->setProperty("name",    logMessage.exception.value().name);
            ex_obj->setProperty("message", logMessage.exception.value().message);
            root_obj->setProperty("exception", ex_obj.release());
        }
        else
        {
            root_obj->setProperty("exception", {});
        }
    
        {
            juce::Array<juce::var> fields;
            fields.ensureStorageAllocated(static_cast<int>(logMessage.fields.size()));
        
            for (const jaut::LogMessage::Field &field: logMessage.fields)
            {
                auto field_obj = std::make_unique<juce::DynamicObject>();
                field_obj->setProperty("name", field.name);
                field_obj->setProperty("value", field.value);
                fields.add(field_obj.release());
            }
        
            root_obj->setProperty("fields", fields);
        }
        
        return { root_obj.release() };
    }
    
    juce::var prepareObject(const juce::String &content, juce::String *failedContent, bool prettyPrint)
    {
        juce::var result = juce::JSON::parse(content);
                
        if (!result.isArray())
        {
            if (failedContent && content.contains("---BEGIN VALID---"))
            {
                const juce::String valid_content = content.fromLastOccurrenceOf("---BEGIN VALID---", false, false);
                result                           = juce::JSON::parse(valid_content);
                
                if (result.isArray())
                {
                    *failedContent = content.upToLastOccurrenceOf("---BEGIN VALID---", true, false);
                }
                else
                {
                    result         = juce::Array<juce::var>{};
                    *failedContent = content + (!prettyPrint ? juce::newLine.getDefault() : "")
                                             + "---BEGIN VALID---" + juce::newLine;
                }
            }
            else
            {
                result = juce::Array<juce::var>{};
                
                if (failedContent)
                {
                    *failedContent = content + (!prettyPrint ? juce::newLine.getDefault() : "")
                                             + "---BEGIN VALID---" + juce::newLine;
                }
            }
        }
        
        return result;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LogFormatJson
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LogFormatJson::LogFormatJson(Options parOptions)
        : options(parOptions)
    {}
    
    //==================================================================================================================
    juce::String LogFormatJson::format(const LogMessage &parLogMessage) const
    {
        const juce::var output = ::createJsonObject(parLogMessage);
        return juce::JSON::toString(output, !options.prettyPrint) + ','
                                                + (options.prettyPrint ? juce::newLine.getDefault() : "");
    }
    
    juce::String LogFormatJson::formatReplace(const jaut::LogMessage &parLogMessage, const juce::String &parContent)
    {
        juce::String output;
        juce::var    new_object = ::createJsonObject(parLogMessage);
        
        if (!options.shouldCache)
        {
            const juce::var temp = ::prepareObject(parContent, nullptr, options.prettyPrint);
            temp.getArray()->add(std::move(new_object));
            output = juce::JSON::toString(temp, !options.prettyPrint);
        }
        else
        {
            if (json.isVoid())
            {
                json = ::prepareObject(parContent, (options.tryPreserveInvalidLog ? &failedJson : nullptr),
                                       options.prettyPrint);
            }
            
            if (parContent.trim().isEmpty())
            {
                json.getArray()->clear();
                failedJson = "";
            }
            
            json.getArray()->add(std::move(new_object));
            output = juce::JSON::toString(json, !options.prettyPrint);
        }
        
        return failedJson + output + (options.prettyPrint ? juce::newLine.getDefault() : "");
    }
    
    //==================================================================================================================
    bool LogFormatJson::supportsReplacingFormatter() const noexcept
    {
        return options.preferReplacingFormatter;
    }
}
//======================================================================================================================
// endregion LogFormatJson
//**********************************************************************************************************************
