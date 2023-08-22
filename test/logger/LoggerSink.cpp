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
    @file   LoggerSink.cpp
    @date   16, February 2023

    ===============================================================
 */
 
#include <jaut_logger/jaut_AbstractLogger.cpp>
#include <jaut_logger/jaut_BasicLogger.h>
#include <jaut_logger/format/jaut_LogFormatCallback.cpp>
#include <jaut_logger/format/jaut_LogFormatPattern.cpp>
#include <jaut_logger/rotation/jaut_LogRotationManager.cpp>
#include <jaut_logger/rotation/policies/jaut_PolicyDaily.cpp>
#include <jaut_logger/rotation/policies/jaut_PolicySizeLimit.cpp>
#include <jaut_logger/rotation/strategies/jaut_StrategyPattern.cpp>
#include <jaut_logger/sink/jaut_LogSinkRotatingFile.cpp>
#include <jaut_logger/sink/jaut_LogSinkFile.cpp>
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
    void cleanLogs(const juce::File &logFile)
    {
        (void) logFile.deleteFile();
        
        const juce::RangedDirectoryIterator rdi(logFile.getParentDirectory(), false,
                                                ("rotated/" + logFile.getFileName() + "_*.zip"));
        
        for (const juce::DirectoryEntry &entry : rdi)
        {
            (void) entry.getFile().deleteFile();
        }
    }
    
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
    template<class Logger, class ...Sinks>
    std::unique_ptr<jaut::AbstractLogger> prepareLogger(jaut::LogLevel::Value level,
                                                        const juce::String    &name,
                                                        Sinks                 ...sinks)
    {
        typename Logger::Options options;
        options.logLevel          = level;
        options.onUnexpectedThrow = onThrow;
        
        auto logger = std::make_unique<Logger>(name, std::move(options), std::move(sinks)...);
        
        if constexpr (std::is_same_v<Logger, jaut::LoggerAsync>)
        {
            const std::thread::id tid = std::this_thread::get_id();
            
            // test if sorting by time
            logger->log({ {}, std::nullopt, "VERBOSE TEST", name, "", tid, juce::Time(2), jaut::LogLevel::Verbose });
            logger->log({ {}, std::nullopt, "FATAL TEST",   name, "", tid, juce::Time(6), jaut::LogLevel::Fatal   });
            logger->log({ {}, std::nullopt, "DEBUG TEST",   name, "", tid, juce::Time(1), jaut::LogLevel::Debug   });
            logger->log({ {}, std::nullopt, "ERROR TEST",   name, "", tid, juce::Time(5), jaut::LogLevel::Error   });
            logger->log({ {}, std::nullopt, "TRACE TEST",   name, "", tid, juce::Time(0), jaut::LogLevel::Trace   });
            
            // test if insertion order by same timestamp
            logger->log({ {}, std::nullopt, "INFO TEST", name, "", tid, juce::Time(4), jaut::LogLevel::Info });
            logger->log({ {}, std::nullopt, "WARN TEST", name, "", tid, juce::Time(4), jaut::LogLevel::Warn });
        }
        else
        {
            logger->trace  ("TRACE TEST");
            logger->debug  ("DEBUG TEST");
            logger->verbose("VERBOSE TEST");
            logger->info   ("INFO TEST");
            logger->warn   ("WARN TEST");
            logger->error  ("ERROR TEST");
            logger->fatal  ("FATAL TEST");
        }
        
        return logger;
    }
    
    juce::String makeStreamLevelTest(jaut::LogLevel::Value level, const juce::String &name, bool async)
    {
        using SinkSync  = jaut::LogSinkOstream<>;
        using SinkAsync = jaut::LogSinkOstream<juce::CriticalSection>;
        
        std::stringstream ss;
        auto              formatter = std::make_unique<jaut::LogFormatPattern>("{msg}");
        
        if (async)
        {
            auto logger = prepareLogger<jaut::LoggerAsync>(level, name, std::make_unique<SinkAsync>(ss, std::move(formatter)));
            juce::Thread::sleep(3000);
            
            const auto &sink = static_cast<const SinkAsync&>(*logger->getSinks()[0]); // NOLINT
            juce::CriticalSection::ScopedLockType locker(sink.getStreamLock());
            
            return ss.str();
        }
        else
        {
            (void) prepareLogger<jaut::LoggerSimple>(level, name, std::make_unique<SinkSync>(ss, std::move(formatter)));
        }
        
        return ss.str();
    }
    
    juce::String makeFileLevelTest(jaut::LogLevel::Value level, const juce::String &name, bool async)
    {
        using Sink = jaut::LogSinkFile;
        
        const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                           .getParentDirectory()
                                           .getChildFile(name + ".log");
        (void) file.deleteFile();
        
        jaut::LogSinkFile::Options options;
        options.formatter = std::make_unique<jaut::LogFormatPattern>("{msg}");
        
        if (async)
        {
            (void) prepareLogger<jaut::LoggerAsync>(level, name, std::make_unique<Sink>(file, std::move(options)));
            juce::Thread::sleep(3000);
        }
        else
        {
            (void) prepareLogger<jaut::LoggerSimple>(level, name, std::make_unique<Sink>(file, std::move(options)));
        }
        
        return file.loadFileAsString();
    }
    
    void basicLevelTest(juce::String(*func)(jaut::LogLevel::Value, const juce::String&, bool), bool async)
    {
        juce::String test_string;
        
        test_string = func(jaut::LogLevel::Warn, "WARN-TEST", async);
        EXPECT_EQ(prepString(test_string), "WARN TEST\nERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Warn test";
        
        test_string = func(jaut::LogLevel::Fatal, "FATAL-TEST", async);
        EXPECT_EQ(prepString(test_string), "FATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Fatal test";
        
        test_string = func(jaut::LogLevel::Off, "OFF-TEST", async);
        EXPECT_EQ(prepString(test_string), "")
            << "Async: " << (async ? "True" : "False") << "; Test: Off test";
        
        test_string = func(jaut::LogLevel::Error, "ERROR-TEST", async);
        EXPECT_EQ(prepString(test_string), "ERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Error test";
        
        test_string = func(jaut::LogLevel::Info, "INFO-TEST", async);
        EXPECT_EQ(prepString(test_string), "INFO TEST\nWARN TEST\nERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Info test";
        
        test_string = func(jaut::LogLevel::Verbose, "VERBOSE-TEST", async);
        EXPECT_EQ(prepString(test_string), "VERBOSE TEST\nINFO TEST\nWARN TEST\nERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Verbose test";
        
        test_string = func(jaut::LogLevel::Debug, "DEBUG-TEST", async);
        EXPECT_EQ(prepString(test_string), "DEBUG TEST\nVERBOSE TEST\nINFO TEST\nWARN TEST\nERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Debug test";
        
        test_string = func(jaut::LogLevel::Trace, "TRACE-TEST", async);
        EXPECT_EQ(prepString(test_string),
                  "TRACE TEST\nDEBUG TEST\nVERBOSE TEST\nINFO TEST\nWARN TEST\nERROR TEST\nFATAL TEST\n")
            << "Async: " << (async ? "True" : "False") << "; Test: Trace test";
    }
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(LoggerSinkTest, TestOstreamSink)
{
    // Sync test
    basicLevelTest(&::makeStreamLevelTest, false);
    
    // Async test
    basicLevelTest(&::makeStreamLevelTest, true);
}

TEST(LoggerSinkTest, TestFileSink)
{
    // Sync test
    basicLevelTest(&::makeFileLevelTest, false);
    
    // Async test
    basicLevelTest(&::makeFileLevelTest, true);
    
    // Append test
    {
        const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                           .getParentDirectory().getChildFile("non-appending-file.log");
        
        jaut::LoggerSimple::Options options;
        options.onUnexpectedThrow = ::onThrow;
        
        jaut::LogSinkFile::Options sf_options;
        sf_options.append    = false;
        sf_options.formatter = std::make_unique<jaut::LogFormatCallback>([](const jaut::LogMessage &msg)
                                                                         {
                                                                             return msg.message + '\n';
                                                                         });
        
        jaut::LoggerSimple logger("APPENDER", std::move(options),
                                  std::make_unique<jaut::LogSinkFile>(file, std::move(sf_options)));
        
        logger << jaut::LogLevel::Info << "Test 1";
        EXPECT_EQ(prepString(file.loadFileAsString()), "Test 1\n");
        
        logger << jaut::LogLevel::Info << "Test 6";
        EXPECT_EQ(prepString(file.loadFileAsString()), "Test 6\n");
        
        logger << jaut::LogLevel::Info << "Test 2";
        EXPECT_EQ(prepString(file.loadFileAsString()), "Test 2\n");
        
        logger << jaut::LogLevel::Info << "Test 4";
        EXPECT_EQ(prepString(file.loadFileAsString()), "Test 4\n");
        
        logger << jaut::LogLevel::Info << "Test 0";
        EXPECT_EQ(prepString(file.loadFileAsString()), "Test 0\n");
    }
}

TEST(LoggerSinkTest, TestRotatingFileSink)
{
    if (juce::Thread *const thread = juce::Thread::getCurrentThread())
    {
        thread->setCurrentThreadName("MAIN");
    }
    
    const juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
                                       .getParentDirectory().getChildFile("sized.rotator.log");
    
    {
        ::cleanLogs(file);
        
        std::stringstream memory_log;
        
        jaut::LoggerSimple::Options options;
        options.logLevel = jaut::LogLevel::Trace;
        options.onUnexpectedThrow = ::onThrow;
        
        jaut::LogSinkRotatingFile::Options sf_options;
        sf_options.rotationPolicy   = jaut::PolicySizeLimit(45);
        sf_options.rotationStrategy = jaut::StrategyPattern("rotated/sized.rotator.log_%Y-%m-%d_%H-%M-%S.zip");
        sf_options.formatter        = std::make_unique<jaut::LogFormatCallback>([](const jaut::LogMessage &msg)
                                                                                {
                                                                                    return msg.message + '\n';
                                                                                });
        
        // We store all our rotated log archives for automated content test
        std::vector<juce::File> archives;
        archives.reserve(3);
        
        auto log_sink_file = std::make_unique<jaut::LogSinkRotatingFile>(file, std::move(sf_options));
        log_sink_file->eventLogRotated += jaut::makeHandler([&archives](const juce::File &archive)
                                                            {
                                                                archives.emplace_back(archive);
                                                            });
        
        jaut::LoggerSimple logger("sized", std::move(options), std::make_unique<jaut::LogSinkOstream<>>(memory_log),
                                  std::move(log_sink_file));
        
        using namespace std::string_view_literals;
        static constexpr std::array messages {
            "Debug string oh yeah"sv,
            "Some minor inconvenience"sv,
            "Whoops, that's not good major tom"sv,
            "Here take this, it's dangerous to go alone"sv,
            "Hmm, we might have done something stupid"sv,
            "Sec, let me just real quick... clutter this for you"sv,
            "Some new message..."sv
        };
        
        int i = 0;
        
        // Print our messages with increasing log level
        // We put in a delay, so the rotation doesn't happen at the same time,
        // and we can distinguish them by their datetime
        for (const std::string_view msg: messages)
        {
            logger << static_cast<jaut::LogLevel::Value>(i++) << msg;
            juce::Thread::sleep(1000);
        }
        
        // Sorting our log archives from oldest to newest
        std::sort(archives.begin(), archives.end(), [](auto &&l, auto &&r)
        {
            return l.getCreationTime() < r.getCreationTime();
        });
        
        i = 0;
        
        for (const juce::File &archive: archives)
        {
            juce::ZipFile zip(archive);
            std::unique_ptr<juce::InputStream> data(zip.createStreamForEntry(0));
            
            EXPECT_TRUE(data);
            
            const juce::String text = data->readEntireStreamAsString();
            const juce::String test = juce::String(messages[i].data()) + '\n' + messages[i + 1].data() + '\n';
            
            EXPECT_EQ(prepString(text), test);
            i += 2;
        }
        
        EXPECT_EQ(prepString(file.loadFileAsString()), (juce::String(messages[i].data()) + '\n'));
    }
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
