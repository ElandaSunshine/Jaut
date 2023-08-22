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
    @date   24, February 2023

    ===============================================================
 */

#include <jaut_logger/format/jaut_LogFormatXml.h>

#include <jaut_logger/jaut_LogMessage.h>


//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    std::unique_ptr<juce::XmlElement> createXmlObject(const jaut::LogMessage &logMessage)
    {
        auto root_obj = std::make_unique<juce::XmlElement>("Event");
        root_obj->createNewChildElement("Name")      ->addTextElement(logMessage.name);
        root_obj->createNewChildElement("Message")   ->addTextElement(logMessage.message);
        root_obj->createNewChildElement("ThreadName")->addTextElement(logMessage.threadName);
        root_obj->createNewChildElement("ThreadId")  ->addTextElement(jaut::toString(logMessage.threadId));
        root_obj->createNewChildElement("Timestamp") ->addTextElement(juce::String(logMessage.timestamp.toMilliseconds()));
        
        juce::XmlElement *const level_obj = root_obj->createNewChildElement("Level");
        level_obj->addTextElement(jaut::LogLevel::names[static_cast<std::size_t>(logMessage.level)].data());
        level_obj->setAttribute("id", static_cast<int>(logMessage.level));
        
        if (juce::XmlElement *const ex_obj = root_obj->createNewChildElement("Exception");
            logMessage.exception.has_value())
        {
            ex_obj->addTextElement(logMessage.exception.value().message);
            ex_obj->setAttribute("name", logMessage.exception.value().name);
        }
    
        juce::XmlElement *const fields_obj = root_obj->createNewChildElement("Fields");
    
        for (const jaut::LogMessage::Field &field: logMessage.fields)
        {
            juce::XmlElement *const field_obj = fields_obj->createNewChildElement("Field");
            field_obj->addTextElement(field.value);
            field_obj->setAttribute("name", field.name);
        }
        
        return root_obj;
    }
    
    std::unique_ptr<juce::XmlElement> prepareObject(const juce::String &content,
                                                    juce::String       *failedContent)
    {
        std::unique_ptr<juce::XmlElement> result = juce::XmlDocument::parse(content);
        
        if (!result || !result->hasTagName("Log"))
        {
            if (failedContent && content.contains("---BEGIN VALID---"))
            {
                const juce::String valid_content = content.fromLastOccurrenceOf("---BEGIN VALID---", false, false);
                result                           = juce::XmlDocument::parse(content);
                
                if (result && result->hasTagName("Log"))
                {
                    *failedContent = content.upToLastOccurrenceOf("---BEGIN VALID---", true, false);
                }
                else
                {
                    result         = std::make_unique<juce::XmlElement>("Log");
                    *failedContent = content + juce::newLine + "---BEGIN VALID---" + juce::newLine;
                }
            }
            else
            {
                result = std::make_unique<juce::XmlElement>("Log");
                
                if (failedContent)
                {
                    *failedContent = content + juce::newLine + "---BEGIN VALID---" + juce::newLine;
                }
            }
        }
        
        return result;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LogFormatXml
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LogFormatXml::LogFormatXml(Options parOptions)
        : options(parOptions)
    {}
    
    //==================================================================================================================
    juce::String LogFormatXml::format(const LogMessage &parLogMessage) const
    {
        const std::unique_ptr<juce::XmlElement> output = ::createXmlObject(parLogMessage);
        return (output->toString(juce::XmlElement::TextFormat{}.withoutHeader()) + juce::newLine);
    }
    
    juce::String LogFormatXml::formatReplace(const jaut::LogMessage &parLogMessage, const juce::String &parContent)
    {
        juce::String                      output;
        std::unique_ptr<juce::XmlElement> new_object = ::createXmlObject(parLogMessage);
        
        if (!options.shouldCache)
        {
            const std::unique_ptr<juce::XmlElement> temp = ::prepareObject(parContent, nullptr);
            temp->addChildElement(new_object.release());
            output = temp->toString();
        }
        else
        {
            if (!xml)
            {
                xml = ::prepareObject(parContent, (options.tryPreserveInvalidLog ? &failedXml : nullptr));
            }
            
            if (parContent.trim().isEmpty())
            {
                xml->deleteAllChildElements();
                failedXml = "";
            }
            
            xml->addChildElement(new_object.release());
            output = xml->toString();
        }
        
        return (failedXml + output + juce::newLine);
    }
    
    //==================================================================================================================
    bool LogFormatXml::supportsReplacingFormatter() const noexcept
    {
        return options.preferReplacingFormatter;
    }
}
//======================================================================================================================
// endregion LogFormatXml
//**********************************************************************************************************************
