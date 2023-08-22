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
    @file   jaut_LogSinkFile.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/exception/jaut_LogIOException.h>
#include <jaut_logger/format/jaut_LogFormatPattern.h>
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
    class JAUT_API LogSinkFile : public ILogSink
    {
    public:
        static constexpr bool default_buffered = true;
        static constexpr int  default_bufferSize = 4096;
        static constexpr bool default_append = true;
        
        //==============================================================================================================
        struct Options
        {
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
        /**
         *  Creates a new file logging sink.
         *  @param logFile The file to log to
         *  
         *  @throw jaut::LogIOException If anything IO didn't work as expected
         */
        explicit LogSinkFile(juce::File logFile);
        
        /**
         *  Creates a new file logging sink.
         *  
         *  @param logFile The file to log to
         *  @param options The options for this file sink
         *  
         *  @throw jaut::LogIOException If anything IO didn't work as expected
         */
        LogSinkFile(juce::File logFile, Options options);
        
        ~LogSinkFile() override;
        
        //==============================================================================================================
        void print(const LogMessage &logMessage) override;
        void flush()                             override;
        
        //==============================================================================================================
        void onOpen()  override;
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
        std::vector<char> streamBuf;
        
        std::ofstream stream;
        Options       options;
        juce::File    logFile;
        juce::String  content;
        
        //==============================================================================================================
        void clearContents();
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkFile)
    };
}
