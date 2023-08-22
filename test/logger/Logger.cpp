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
    @file   Logger.cpp
    @date   02, October 2022

    ===============================================================
 */

#include <jaut_logger/jaut_AbstractLogger.cpp>
#include <jaut_logger/jaut_BasicLogger.h>
#include <jaut_logger/format/jaut_LogFormatCallback.cpp>
#include <jaut_logger/sink/jaut_LogSinkOstream.h>

#include <jaut_core/util/jaut_CommonUtils.h>

#include <gtest/gtest.h>



//**********************************************************************************************************************
// region Preprocessor
//======================================================================================================================
#define LOG_TYPE_TEST(MSG) { EXPECT_EQ(stream.str(), MSG); stream.str(""); }
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
TEST(LoggerTest, TestLogMethods)
{
    std::stringstream stream;
    
    jaut::LoggerSimple::Options options;
    options.onUnexpectedThrow = ::onThrow;
    
    jaut::LoggerSimple logger("METHODS", std::move(options),
                              std::make_unique<jaut::LogSinkOstream<>>(
                                  stream,
                                  std::make_unique<jaut::LogFormatCallback>([](const jaut::LogMessage &msg)
                                  {
                                      if (msg.exception.has_value())
                                      {
                                          return msg.message + ' ' + msg.exception->message;
                                      }
                                      
                                      if (!msg.fields.empty())
                                      {
                                          return msg.message + ' ' + msg.fields[0].value.toString();
                                      }
                                      
                                      return msg.message;
                                  })));
    
    // streaming operator
    {
        // normal
        logger << jaut::LogLevel::Info
               << "Test";
        LOG_TYPE_TEST("Test")
        
        // with field (last)
        logger << jaut::LogLevel::Info
               << "Testing "
               << 1
               << " field:"
               << jaut::mfield("test", "Hello world!");
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with field (first)
        logger << jaut::LogLevel::Info
               << jaut::mfield("test", "Hello world!")
               << "Testing "
               << 1
               << " field:";
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with field (middle)
        logger << jaut::LogLevel::Info
               << "Testing "
               << 1
               << jaut::mfield("test", "Hello world!")
               << " field:";
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with same field twice (should overwrite first appearance)
        logger << jaut::LogLevel::Info
               << "Testing "
               << 1
               << jaut::mfield("test", "Hello worldsad dadawd!")
               << " field:"
               << jaut::mfield("test", "Hello world!");
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with exception
        logger << jaut::LogLevel::Fatal
               << "Here is some error "
               << std::runtime_error("TEST EXCEPTION UH OH")
               << "message:";
        LOG_TYPE_TEST("Here is some error message: TEST EXCEPTION UH OH")
        
        // with same exception twice (should overwrite first appearance)
        logger << jaut::LogLevel::Fatal << "Here is some error "
               << std::runtime_error("TEST EXCEPTION UH OH HAHAH UUHHH")
               << "message:"
               << std::runtime_error("TEST EXCEPTION UH OH");
        LOG_TYPE_TEST("Here is some error message: TEST EXCEPTION UH OH")
    }
    
    // functional
    {
        // normal
        logger.info("Test");
        LOG_TYPE_TEST("Test")
        
        // with exception
        logger.fatal("Here is some error message:",
                     std::runtime_error("TEST EXCEPTION UH OH"));
        LOG_TYPE_TEST("Here is some error message: TEST EXCEPTION UH OH")
    }
    
    // printf-alike
    {
        // normal
        logger.info("Test {} nr {}",
                    "object",
                    1);
        LOG_TYPE_TEST("Test object nr 1")
        
        // with field (last argument)
        logger.info("Testing {} {}:",
                    1,
                    "field",
                    jaut::mfield("test", "Hello world!"));
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with field (first argument)
        logger.info("Testing {} {}:",
                    jaut::mfield("test", "Hello world!"),
                    1,
                    "field");
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with field (middle argument)
        logger.info("Testing {} {}:",
                    1,
                    jaut::mfield("test", "Hello world!"),
                    "field");
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with same field twice (should overwrite first appearance)
        logger.info("Testing {} {}:",
                    jaut::mfield("test", "Hello worldsad dadawd!"),
                    1,
                    jaut::mfield("test", "Hello world!"),
                    "field");
        LOG_TYPE_TEST("Testing 1 field: Hello world!")
        
        // with exception
        logger.fatal("Here is some {} message:",
                     "error",
                     std::runtime_error("TEST EXCEPTION UH OH"));
        LOG_TYPE_TEST("Here is some error message: TEST EXCEPTION UH OH")
        
        // with same exception twice (should overwrite first appearance)
        logger.fatal("Here is some {} message:",
                     std::runtime_error("TEST EXCEPTION UH OH HAHAH UUHHH"),
                     "error",
                     std::runtime_error("TEST EXCEPTION UH OH"));
        LOG_TYPE_TEST("Here is some error message: TEST EXCEPTION UH OH")
    }
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
