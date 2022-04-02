/**
    ===============================================================
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

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda (elanda@elandasunshine.com)
    @file   sinkfileimpl.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut::detail
{
    template<class CriticalSection, class RotationHandler>
    class LogSinkFileImpl : public ILogSink
    {
    public:
        struct Options
        {
            //==========================================================================================================
            /** Whether the sink should append an empty line to the end of the file on destruction. */
            bool concludeWithNewline { false };
            
            /**
             *  Whether messages should be buffered when no log file is yet open and then flushed as soon
             *  as a file handle has been created.
             */
            bool bufferNonDumpedMessages { false };
            
            /** Bitmask of log levels that should be excluded from the log file. */
            int excludedLevels { Logger::Debug };
            
            /** If log rotation is enabled, this determines the maximum amount of rotated log files. */
            int maxLogFiles { 5 };
            
            /** If log rotation is enabled, this determines whether old logs should be compressed with gzip. */
            bool compressLogFiles { true };
        };
    
        //==============================================================================================================
        using RotationManager = LogRotationManager<RotationHandler>;
        
        //==============================================================================================================
        explicit LogSinkFileImpl(juce::File logFile, Options options = {})
            : logFile(std::move(logFile)), options(options)
        {
            using Behaviour = typename RotationManager::FileManagementBehaviour;
            
            rotationManager = std::make_unique<RotationManager>(logFile, options.maxLogFiles,
                                                                static_cast<Behaviour>(!options.compressLogFiles));
            setLogFile(this->logFile);
            stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        }
        
        explicit LogSinkFileImpl(Options options = {})
            : options(options)
        {
            stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        }
        
        ~LogSinkFileImpl() override
        {
            if (options.concludeWithNewline)
            {
                if (stream.is_open())
                {
                    stream << std::endl;
                }
            }
        }
        
        //==============================================================================================================
        void print(const juce::String &message, Logger::LogLevel level) final
        {
            if ((options.excludedLevels & level) == level)
            {
                return;
            }
            
            const typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (stream.is_open())
            {
                rotationManager->tryRotateLogs(message, level);
                stream << message << '\n';
            }
            else if (options.bufferNonDumpedMessages)
            {
                buffer.add(message);
            }
        }
        
        void flush() final
        {
            const typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (stream.is_open())
            {
                std::flush(stream);
            }
        }
        
        //==============================================================================================================
        const juce::File& getLogFile() const noexcept { return logFile; }
        const Options&    getOptions() const noexcept { return options; }
        
        //==============================================================================================================
        juce::String getLastError() { return std::exchange(lastError, ""); }
        
        //==============================================================================================================
        bool setLogFile(const juce::File &newLogFile)
        {
            using Behaviour = typename RotationManager::FileManagementBehaviour;
            const typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (!logFile.exists())
            {
                (void) logFile.create();
            }
            
            try
            {
                stream.open(logFile.getFullPathName().toStdString(), std::ios::out | std::ios::app);
                logFile = newLogFile;
                rotationManager = std::make_unique<RotationManager>(logFile, options.maxLogFiles,
                                                                    static_cast<Behaviour>(!options.compressLogFiles));
                
                if (!buffer.isEmpty())
                {
                    for (const auto &message : buffer)
                    {
                        stream << message << '\n';
                    }
                    
                    stream.flush();
                    buffer.clear();
                }
            }
            catch (const std::exception &ex)
            {
                lastError = ex.what();
                return false;
            }
            
            return true;
        }
        
    private:
        CriticalSection                  criticalSection;
        std::unique_ptr<RotationManager> rotationManager;
        
        juce::StringArray buffer;
        std::ofstream     stream;
        juce::File        logFile;
        Options           options;
        juce::String      lastError;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkFileImpl)
    };
}