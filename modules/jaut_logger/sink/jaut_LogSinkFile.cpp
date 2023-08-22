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
    @file   jaut_LogSinkFile.cpp
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/sink/jaut_LogSinkFile.h>



//**********************************************************************************************************************
// region LogSinkFile
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LogSinkFile::LogSinkFile(juce::File parLogFile)
        : LogSinkFile(std::move(parLogFile), Options{})
    {}
    
    LogSinkFile::LogSinkFile(juce::File parLogFile, Options parOptions)
        : options(std::move(parOptions)),
          logFile(std::move(parLogFile))
    {
        if (logFile.exists() && (options.formatter->supportsReplacingFormatter() && !options.append))
        {
            content = logFile.loadFileAsString();
        }
        else
        {
            if (const juce::Result result = logFile.create();
                result.failed())
            {
                throw LogIOException("could not create log file '" + logFile.getFullPathName() + "': "
                                     + result.getErrorMessage());
            }
        }
        
        stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        
        if (options.buffered)
        {
            streamBuf.resize(options.bufferSize, 0);
            stream.rdbuf()->pubsetbuf(streamBuf.data(), options.bufferSize);
        }
        else
        {
            stream.rdbuf()->pubsetbuf(nullptr, 0);
        }
        
        try
        {
            stream.open(logFile.getFullPathName().toStdString(), std::ios::binary | std::ios::app);
        }
        catch (const std::exception &ex)
        {
            throw LogIOException(ex.what());
        }
    }
    
    LogSinkFile::~LogSinkFile() = default;
    
    //==================================================================================================================
    void LogSinkFile::print(const LogMessage &parLogMessage)
    {
        if (!stream.is_open())
        {
            throw LogIOException("stream could not be opened");
        }
        
        ILogFormat &formatter = *options.formatter;
        
        if (!options.append)
        {
            clearContents();
            
            if (formatter.supportsReplacingFormatter())
            {
                stream << (content = formatter.formatReplace(parLogMessage, content));
                return;
            }
        }
        
        stream << formatter.format(parLogMessage);
    }
    
    void LogSinkFile::flush()
    {
        if (stream.is_open())
        {
            stream << std::flush;
        }
    }
    
    //==================================================================================================================
    void LogSinkFile::onOpen()
    {
        if (!options.append)
        {
            return;
        }
        
        ILogFormat::Util::printHeader(getFormatter(), [&out = stream](const juce::String &header)
        {
            out << header << std::endl;
        });
    }
    
    void LogSinkFile::onClose()
    {
        if (!options.append)
        {
            return;
        }
        
        ILogFormat::Util::printFooter(getFormatter(), [&out = stream](const juce::String &footer)
        {
            out << footer << std::endl;
        });
    }
    
    //==================================================================================================================
    const juce::File& LogSinkFile::getLogFile() const noexcept
    {
        return logFile;
    }
    
    const LogSinkFile::Options& LogSinkFile::getOptions() const noexcept
    {
        return options;
    }
    
    const ILogFormat* LogSinkFile::getFormatter() const
    {
        return options.formatter.get().get();
    }
    
    //==================================================================================================================
    void LogSinkFile::clearContents()
    {
        try
        {
            if (stream.is_open())
            {
                stream.close();
            }
            
            stream.open(logFile.getFullPathName().toStdString(), std::ios::binary | std::ios::trunc);
        }
        catch (const std::exception &ex)
        {
            throw LogIOException(ex.what());
        }
    }
}
//======================================================================================================================
// endregion LogSinkFile
//**********************************************************************************************************************
