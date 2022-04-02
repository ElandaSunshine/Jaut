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
    @file   logrotationmanager.h
    @date   30, March 2022

    ===============================================================
 */
 
#pragma once

namespace jaut
{
    /**
     *  This class helps managing log files and how they should be rotated.
     *  It is not meant to be derived from but rather to be added as a type, but nothing stops you from doing so
     *  if you need to.
     *
     *  The passed in rotation-handler is a predicate that will evaluate whether it is necessary to rotate the logs
     *  now or not.
     *  The specification for such a handler is as follows:
     *
     *  @code
     *  struct MyRotationHandler
     *  {
     *      bool needsToBeRotated(const juce::File &fileToRotate) const
     *      {
     *          // mommy, do the logs need to be rotated now?
     *          return true;
     *      }
     *  }
     *  @endcode
     *
     *  @tparam RotationHandler The RotationHandler predicate
     */
    template<class RotationHandler>
    class LogRotationManager
    {
    public:
        /** Defines the behaviour of log files */
        enum class FileManagementBehaviour
        {
            /**
             *  Compress the log file.
             *  The name of the compressed log file will be something like: "logname.1.log.gz"
             */
            Compress,
            
            /**
             *  Move the current log file.
             *  The name of the compressed log file will be something like: "logname.1.log"
             */
            Move,
            
            /** Just delete the old log file and create and empty one. */
            Delete
        };
        
        //==============================================================================================================
        /**
         *  This even will be raised before the rotation happens.
         *  This is useful for closing the stream first if it is open.
         */
        jaut::Event<jaut::TrivialHandler> BeforeRotation;
        
        /**
         *  This even will be raised after the rotation happens.
         *  This is useful for opening the new log file.
         */
        jaut::Event<jaut::TrivialHandler> AfterRotation;
        
        //==============================================================================================================
        /**
         *  Creates a new instance of the LogRotationManager class.
         *
         *  @param logFile   The log file to manage
         *  @param maxFiles  The maximum amount of files before old log files will be deleted,
         *                   -1 if all should be kept
         *  @param behaviour The behaviour of the old log file when it hit its limits
         */
        LogRotationManager(const juce::File &logFile, int maxFiles, FileManagementBehaviour behaviour)
            : rotationHandler(constructIfPossible(logFile)), logFile(logFile), behaviour(behaviour),
              maxFiles(maxFiles)
        {}
        
        //==============================================================================================================
        /**
         *  Get the log file rotation behaviour when a log hit its limits.
         *  @return The behaviour
         */
        FileManagementBehaviour getBehaviour() const noexcept { return behaviour; }
        
        /**
         *  Gets how many log files should be kept for deleting the oldest one.
         *  If this is -1, all files will be kept.
         *
         *  @return The maximum amount of log files
         */
        int getMaxFiles() const noexcept { return maxFiles; }
        
        //==============================================================================================================
        /**
         *  Tries to rotate the log file if the RotationHandler evaluates to true, otherwise does nothing.
         *  If you want to force a rotation, you can use forceRotateLogs() instead
         *
         *  @param logMessage The message that should get logged
         *  @param logLevel   The log level of the current message
         */
        void tryRotateLogs(const juce::String &logMessage, Logger::LogLevel logLevel)
        {
            if (rotationHandler.needsToBeRotated(logMessage, logLevel))
            {
                forceRotateLogs();
                rotationHandler.reset();
            }
        }
        
        /** Forces an unchecked rotation of the current log file. */
        void forceRotateLogs()
        {
            BeforeRotation();
            rotateLogs(logFile.getParentDirectory(), logFile.getFileNameWithoutExtension());
            
            if (behaviour == FileManagementBehaviour::Compress)
            {
                compress(logFile, logFile.getFileNameWithoutExtension() + ".1.log.gz");
            }
            else if (behaviour == FileManagementBehaviour::Move)
            {
                move(logFile, logFile.getFileNameWithoutExtension() + ".1.log");
            }
            
            deleteAndCreate(logFile);
            AfterRotation();
        }
        
    private:
        constexpr RotationHandler constructIfPossible(const juce::File &file)
        {
            if constexpr(std::is_constructible_v<RotationHandler, juce::File>)
            {
                return RotationHandler(file);
            }
            else
            {
                return {};
            }
        }
        
        //==============================================================================================================
        static void deleteAndCreate(const juce::File &fileToDelete)
        {
            fileToDelete.deleteFile();
            const juce::Result result = fileToDelete.create();
            
            if (result.failed())
            {
                throw LogRotationException(result.getErrorMessage());
            }
        }
        
        static void move(const juce::File &fileToMove, const juce::String &destName)
        {
            if (!fileToMove.exists())
            {
                throw LogRotationException("log file not found");
            }
            
            if (!fileToMove.moveFileTo(fileToMove.getSiblingFile(destName)))
            {
                throw LogRotationException(std::strerror(errno));
            }
        }
        
        static void compress(const juce::File &fileToCompress, const juce::String &destName)
        {
            juce::FileOutputStream fos(fileToCompress.getSiblingFile(destName));
            juce::ZipFile::Builder new_zip;
            
            new_zip.addFile(fileToCompress, 7);
            
            if (!new_zip.writeToStream(fos, nullptr))
            {
                throw LogRotationException("log file could not be compressed");
            }
        }
        
        //==============================================================================================================
        RotationHandler         rotationHandler;
        juce::File              logFile;
        FileManagementBehaviour behaviour { FileManagementBehaviour::Compress };
        int                     maxFiles  { 5 };
        
        //==============================================================================================================
        void rotateLogs(const juce::File &folderOfLogsToRotate, const juce::String &logFileName) const
        {
            const juce::String log_extension = (behaviour == FileManagementBehaviour::Move ? ".log" : ".gz");
            
            for (int i = maxFiles; i > 0; --i)
            {
                const juce::File log_file(logFileName + '.' + juce::String(i) + log_extension);
                
                if (log_file.exists())
                {
                    if (i == maxFiles)
                    {
                        if (!log_file.deleteFile())
                        {
                            throw LogRotationException(std::strerror(errno));
                        }
                        
                        continue;
                    }
                    
                    juce::String new_log_name;
                    new_log_name << logFileName << '.' << (i + 1) << log_extension;
                    
                    if (!log_file.moveFileTo(log_file.getSiblingFile(new_log_name)))
                    {
                        throw LogRotationException(std::strerror(errno));
                    }
                }
            }
        }
    };
}