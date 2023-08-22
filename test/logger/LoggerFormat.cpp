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
    @file   LoggerFormat.cpp
    @date   16, February 2023

    ===============================================================
 */

#include <jaut_logger/jaut_AbstractLogger.cpp>
#include <jaut_logger/jaut_BasicLogger.h>
#include <jaut_logger/format/jaut_LogFormatJson.cpp>
#include <jaut_logger/format/jaut_LogFormatPattern.cpp>
#include <jaut_logger/format/jaut_LogFormatXml.cpp>
#include <jaut_logger/sink/jaut_LogSinkFile.cpp>
#include <jaut_logger/sink/jaut_LogSinkOstream.h>

#include <jaut_core/util/jaut_CommonUtils.h>

#include <gtest/gtest.h>



//**********************************************************************************************************************
// region Preprocessor
//======================================================================================================================
//======================================================================================================================
// endregion Preprocessor
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    juce::String prepString(const juce::String &content)
    {
        return content.replace("\r\n", "\n");
    }
    
    //==================================================================================================================
    void onThrow(const std::exception &ex)
    {
        ASSERT_FALSE(true) << "Exception '" << jaut::getActualTypeName(&ex) << "' occurred: " << ex.what();
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    //******************************************************************************************************************
    // region Testing Facilities
    //==================================================================================================================
    
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(LoggerFormatTest, TestJsonAppendingFormatter)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    jaut::LogFormatJson::Options fj_options;
    fj_options.prettyPrint = true;
    
    std::stringstream stream;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("appender.json");
    (void) file.deleteFile();
    
    jaut::LoggerSimple logger("json-logger", std::move(options),
        std::make_unique<jaut::LogSinkOstream<>>(
            stream,
            std::make_unique<jaut::LogFormatJson>(fj_options)
        ),
        std::make_unique<jaut::LogSinkFile>(
            file,
            jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatJson>(fj_options) }
        )
    );
    
    logger << jaut::LogLevel::Info << "Test message to test json formatter";
    {
        const juce::var                  json = juce::JSON::parse(stream.str());
        const juce::DynamicObject *const root = json.getDynamicObject();
        
        ASSERT_TRUE(root != nullptr);
    
        EXPECT_EQ(root->getProperty("message").toString(), "Test message to test json formatter")
            << "Content: " << stream.str();
        stream.str("");
    }
    
    logger << jaut::LogLevel::Info << "Another test yippieee";
    {
        const juce::var                  json = juce::JSON::parse(stream.str());
        const juce::DynamicObject *const root = json.getDynamicObject();
        
        ASSERT_TRUE(root != nullptr);
    
        EXPECT_EQ(root->getProperty("message").toString(), "Another test yippieee")
            << "Content: " << stream.str();
        stream.str("");
    }
}

TEST(LoggerFormatTest, TestJsonReplacingFormatter)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    jaut::LogFormatJson::Options fj_options;
    fj_options.prettyPrint = true;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("replacing.json");
    (void) file.deleteFile();
    
    jaut::LoggerSimple logger("json-logger", std::move(options),
        std::make_unique<jaut::LogSinkFile>(
            file,
            jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatJson>(fj_options), 4096, true, false }
        )
    );
    
    logger << jaut::LogLevel::Info << "0";
    logger << jaut::LogLevel::Info << "1";
    logger << jaut::LogLevel::Info << "2";
    logger << jaut::LogLevel::Info << "3";
    
    {
        const juce::var                     json = juce::JSON::parse(file.loadFileAsString());
        const juce::Array<juce::var> *const root = json.getArray();
        ASSERT_TRUE(root != nullptr);
    
        for (int i = 0; i <= 3; ++i)
        {
            const juce::DynamicObject *const obj = root->getReference(i).getDynamicObject();
            ASSERT_TRUE(obj != nullptr);
            EXPECT_EQ(static_cast<int>(obj->getProperty("message")), i);
        }
    }
    
    logger << jaut::LogLevel::Info << "4";
    logger << jaut::LogLevel::Info << "5";
    logger << jaut::LogLevel::Info << "6";
    logger << jaut::LogLevel::Info << "7";
    
    {
        const juce::var                     json = juce::JSON::parse(file.loadFileAsString());
        const juce::Array<juce::var> *const root = json.getArray();
        ASSERT_TRUE(root != nullptr);
    
        for (int i = 0; i <= 7; ++i)
        {
            const juce::DynamicObject *const obj = root->getReference(i).getDynamicObject();
            ASSERT_TRUE(obj != nullptr);
            EXPECT_EQ(static_cast<int>(obj->getProperty("message")), i);
        }
    }
}

TEST(LoggerFormatTest, TestJsonReplacingFormatterWithInvalidJson)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    jaut::LogFormatJson::Options fj_options;
    fj_options.prettyPrint           = false;
    fj_options.tryPreserveInvalidLog = true;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("replacing-invalid-test.json");
    (void) file.deleteFile();
    
    for (int i = 0; i < 2; ++i)
    {
        if (i == 1)
        {
            // make the json invalid by cutting the content in half (probably the half)
            const juce::String text = file.loadFileAsString();
            (void) file.replaceWithText(text.substring(0, (text.length() / 2)));
        }
        
        jaut::LoggerSimple logger("json-logger", options,
            std::make_unique<jaut::LogSinkFile>(
                file,
                jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatJson>(fj_options), 4096, true, false }
            )
        );
        
        logger << jaut::LogLevel::Info << "0";
        logger << jaut::LogLevel::Info << "1";
        logger << jaut::LogLevel::Info << "2";
        logger << jaut::LogLevel::Info << "3";
        logger << jaut::LogLevel::Info << "4";
        logger << jaut::LogLevel::Info << "5";
        logger << jaut::LogLevel::Info << "6";
        logger << jaut::LogLevel::Info << "7";
    }
    
    const juce::String text = file.loadFileAsString().fromLastOccurrenceOf("---BEGIN VALID---", false, false);
    const juce::var    json = juce::JSON::parse(text);
    
    const juce::Array<juce::var> *const root = json.getArray();
    ASSERT_TRUE(root != nullptr);
    
    for (int j = 0; j <= 7; ++j)
    {
        const juce::DynamicObject *const obj = root->getReference(j).getDynamicObject();
        ASSERT_TRUE(obj != nullptr);
        EXPECT_EQ(static_cast<int>(obj->getProperty("message")), j);
    }
}

TEST(LoggerFormatTest, TestXmlAppendingFormatter)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    std::stringstream stream;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("appender.xml");
    (void) file.deleteFile();
    
    jaut::LoggerSimple logger("xml-logger", std::move(options),
        std::make_unique<jaut::LogSinkOstream<>>(
            stream,
            std::make_unique<jaut::LogFormatXml>()
        ),
        std::make_unique<jaut::LogSinkFile>(
            file,
            jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatXml>() }
        )
    );
    
    logger << jaut::LogLevel::Info << "Test message to test xml formatter";
    {
        const std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(stream.str());
        ASSERT_TRUE(xml != nullptr && xml->hasTagName("Event"));
        
        EXPECT_EQ(xml->getChildByName("Message")->getAllSubText(), "Test message to test xml formatter")
            << "Content: " << stream.str();
        
        stream.str("");
    }
    
    logger << jaut::LogLevel::Info << "Another test yippieee";
    {
        const std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(stream.str());
        ASSERT_TRUE(xml != nullptr && xml->hasTagName("Event"));
        
        EXPECT_EQ(xml->getChildByName("Message")->getAllSubText(), "Another test yippieee")
            << "Content: " << stream.str();
        
        stream.str("");
    }
}

TEST(LoggerFormatTest, TestXmlReplacingFormatter)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("replacing.xml");
    (void) file.deleteFile();
    
    jaut::LoggerSimple logger("xml-logger", std::move(options),
        std::make_unique<jaut::LogSinkFile>(
            file,
            jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatXml>(), 4096, true, false }
        )
    );
    
    logger << jaut::LogLevel::Info << "0";
    logger << jaut::LogLevel::Info << "1";
    logger << jaut::LogLevel::Info << "2";
    logger << jaut::LogLevel::Info << "3";
    
    {
        std::vector<int> ints { 0, 1, 2, 3 };
        
        const std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(file.loadFileAsString());
        ASSERT_TRUE(xml != nullptr && xml->hasTagName("Log"));
        
        for (const juce::XmlElement *const element : xml->getChildIterator())
        {
            ASSERT_TRUE(element->hasTagName("Event"));
            const auto it = std::find(ints.begin(), ints.end(), element->getChildByName("Message")
                                                                       ->getAllSubText().getIntValue());
            
            if (it != ints.end())
            {
                (void) ints.erase(it);
            }
        }
        
        EXPECT_TRUE(ints.empty());
    }
    
    logger << jaut::LogLevel::Info << "4";
    logger << jaut::LogLevel::Info << "5";
    logger << jaut::LogLevel::Info << "6";
    logger << jaut::LogLevel::Info << "7";
    
    {
        std::vector<int> ints { 0, 1, 2, 3, 4, 5, 6, 7 };
        
        const std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(file.loadFileAsString());
        ASSERT_TRUE(xml != nullptr && xml->hasTagName("Log"));
        
        for (const juce::XmlElement *const element : xml->getChildIterator())
        {
            ASSERT_TRUE(element->hasTagName("Event"));
            const auto it = std::find(ints.begin(), ints.end(), element->getChildByName("Message")
                                                                       ->getAllSubText().getIntValue());
            
            if (it != ints.end())
            {
                (void) ints.erase(it);
            }
        }
        
        EXPECT_TRUE(ints.empty());
    }
}

TEST(LoggerFormatTest, TestXmlReplacingFormatterWithInvalidXml)
{
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    jaut::LogFormatXml::Options fx_options;
    fx_options.tryPreserveInvalidLog = true;
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("replacing-invalid-test.xml");
    (void) file.deleteFile();
    
    for (int i = 0; i < 2; ++i)
    {
        if (i == 1)
        {
            // make the json invalid by cutting the content in half (probably the half)
            const juce::String text = file.loadFileAsString();
            (void) file.replaceWithText(text.substring(0, (text.length() / 2)));
        }
        
        jaut::LoggerSimple logger("xml-logger", options,
            std::make_unique<jaut::LogSinkFile>(
                file,
                jaut::LogSinkFile::Options{ std::make_unique<jaut::LogFormatXml>(fx_options), 4096, true, false }
            )
        );
        
        logger << jaut::LogLevel::Info << "0";
        logger << jaut::LogLevel::Info << "1";
        logger << jaut::LogLevel::Info << "2";
        logger << jaut::LogLevel::Info << "3";
        logger << jaut::LogLevel::Info << "4";
        logger << jaut::LogLevel::Info << "5";
        logger << jaut::LogLevel::Info << "6";
        logger << jaut::LogLevel::Info << "7";
    }
    
    std::vector<int> ints { 0, 1, 2, 3, 4, 5, 6, 7 };
    
    const juce::String text = file.loadFileAsString().fromLastOccurrenceOf("---BEGIN VALID---", false, false);
    const std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(text);
    ASSERT_TRUE(xml != nullptr && xml->hasTagName("Log"));
    
    for (const juce::XmlElement *const element : xml->getChildIterator())
    {
        ASSERT_TRUE(element->hasTagName("Event"));
        const auto it = std::find(ints.begin(), ints.end(), element->getChildByName("Message")
                                                                   ->getAllSubText().getIntValue());
        
        if (it != ints.end())
        {
            (void) ints.erase(it);
        }
    }
    
    EXPECT_TRUE(ints.empty());
}

//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
