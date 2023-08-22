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
    @file   jaut_LogSinkRotatingFile.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/exception/jaut_LogIOException.h>
#include <jaut_logger/format/jaut_ILogFormat.h>
#include <jaut_logger/rotation/jaut_LogRotationManager.h>
#include <jaut_logger/rotation/policies/jaut_PolicyDaily.h>
#include <jaut_logger/rotation/strategies/jaut_StrategyPattern.h>
#include <jaut_logger/sink/jaut_ILogSink.h>

#include <jaut_core/define/jaut_Define.h>

#include <fstream>



namespace jaut
{
    /**
     *  A Logger output sink for log files.<br>
     *  This sink supports replacing formatters.
     */
    class JAUT_API LogSinkRotatingFile : public ILogSink
    {
    public:
        /**
         *  The default rotation policy for the rotating file sink.<br>
         *  By default this will be a daily file sink that rotates at midnight every day.
         */
        inline static RotationPolicy defaultRotationPolicy = PolicyDaily(0, 0);
        
        /**
         *  The default rotation strategy.<br>
         *  This will rotate all files and save them as zip archive with their date and time of the rotation as name.
         */
        inline static RotationStrategy defaultRotationStrategy = StrategyPattern("log_%Y-%m-%d_%H-%M-%S.zip");
        
        static constexpr bool default_buffered = true;
        static constexpr int  default_bufferSize = 4096;
        static constexpr bool default_append = true;
        
        //==============================================================================================================
        struct Options
        {
            /** 
             *  The rotation policy.<br>
             *  This determines the condition under which a log should be rotated.
             *  
             *  @see jaut::RotationPolicy
             */
            NonNull<RotationPolicy> rotationPolicy = defaultRotationPolicy;
            
            /**
             *  The rotation strategy.<br>
             *  This will allow you defining how logs are rotated and what should happen to old logs.
             *  
             *  @see jaut::RotationStrategy
             */
            NonNull<RotationStrategy> rotationStrategy = defaultRotationStrategy;
            
            /**
             *  The formatter to be used by this sink.
             *  @see jaut::LogFormat
             */
            NonNull<std::unique_ptr<ILogFormat>> formatter = std::make_unique<DefaultFormatterType>();
            
            /** If buffered is true, this will determine the size of the internal stream buffer. (in bytes) */
            int bufferSize = default_bufferSize;
            
            /** Should messages be buffered before they are written to the file. */
            bool buffered = default_buffered;
            
            /**
             *  Should new events be appended or should the file be overwritten entirely.<br>
             *  If supported and enabled, this will enable the replacing version of the formatter.
             */
            bool append = default_append;
        };
        
        //==============================================================================================================
        using LogRotatedHandler = LogRotationManager::AfterRotationHandler;
        
        //==============================================================================================================
        /**
         *  This event will be raised whenever the log file was rotated.<br>
         *  Note that, while adding events is thread-safe, the event being raised is not necessarily, as
         *  this will always be raised on the thread the log messages are processed on.
         *  <br><br>
         *  If Options::logRotationOptions::fileManagementBehaviour is set to
         *  LogRotationManager::FileManagementBehaviour::Delete, the event's parameter is an invalid file.
         *
         *  @param par1 The newly rotated log file/archive
         */
        Event<LogRotatedHandler, juce::CriticalSection> eventLogRotated;
        
        //==============================================================================================================
        /**
         *  Creates a new file logging sink.
         *  @param logFile The file to log to
         *  
         *  @throw jaut::LogIOException If anything IO didn't work as expected
         */
        explicit LogSinkRotatingFile(juce::File logFile);
        
        /**
         *  Creates a new file logging sink.
         *  
         *  @param logFile The file to log to
         *  @param options The options for this file sink
         *  
         *  @throw jaut::LogIOException If anything IO didn't work as expected
         */
        LogSinkRotatingFile(juce::File logFile, Options options);
        
        ~LogSinkRotatingFile() override;
        
        //==============================================================================================================
        void print(const LogMessage &logMessage) override;
        void flush()                             override;
        
        //==============================================================================================================
        void onOpen() override;
        void onClose() override;
        
        //==============================================================================================================
        /**
         *  Gets the file that has been set to be the log file.
         *  @return The current log file
         */
        JAUT_NODISCARD
        const juce::File& getLogFile() const noexcept;
        
        /**
         *  Gets the options of this file sink.
         *  @return The options
         */
        JAUT_NODISCARD
        const Options& getOptions() const noexcept;
        
        JAUT_NODISCARD
        const ILogFormat* getFormatter() const override;
        
    private:
        LogRotationManager rotationManager;
        
        std::vector<char> streamBuf;
        
        std::ofstream stream;
        Options       options;
        juce::File    logFile;
        juce::String  content;
        
        //==============================================================================================================
        void closeOnRotation(const juce::File&);
        void openOnRotation(const juce::File&);
        
        //==============================================================================================================
        void clearContents();
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkRotatingFile)
    };
}
