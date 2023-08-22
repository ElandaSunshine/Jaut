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
    @file   jaut_StrategyPattern.cpp
    @date   11, February 2023

    ===============================================================
 */

#pragma once

#include <jaut_logger/rotation/strategies/jaut_StrategyPattern.h>

#include <jaut_logger/exception/jaut_LogIOException.h>
#include <jaut_logger/rotation/jaut_LogRotationManager.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    bool copyTo(const juce::File &src, const juce::File &dest, int compressionLevel)
    {
        const juce::String path = dest.getFileName();
        
        if (const juce::File parent = dest.getParentDirectory();
            !parent.exists())
        {
            if (parent.createDirectory().failed())
            {
                throw jaut::LogIOException("could not create log archive parent structure");
            }
        }
        
        if (path.endsWith(".zip"))
        {
            juce::ZipFile::Builder builder;
            builder.addFile(src, compressionLevel);
            
            juce::FileOutputStream fos(dest);
            
            if (!builder.writeToStream(fos, nullptr))
            {
                return false;
            }
        }
        else if (path.endsWith(".gz"))
        {
            juce::FileOutputStream           fos (dest);
            juce::GZIPCompressorOutputStream gzos(fos, compressionLevel);
            
            if (!gzos.writeText(src.loadFileAsString(), false, false, nullptr))
            {
                return false;
            }
            
            gzos.flush();
        }
        else
        {
            return src.copyFileTo(dest);
        }
        
        return true;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region StrategyPattern
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    StrategyPattern::StrategyPattern(juce::String parPattern, Options parOptions)
        : options(parOptions),
          pattern(std::move(parPattern))
    {
        jassert(options.min >= 0 && options.min <= parOptions.max);
    }
    
    StrategyPattern::StrategyPattern(juce::String parPattern)
        : StrategyPattern(std::move(parPattern), {})
    {}
    
    //==================================================================================================================
    juce::File StrategyPattern::operator()(const LogRotationManager &parRotationManager)
    {
        const juce::String formatted  = juce::Time::getCurrentTime().formatted(pattern);
        const juce::File   file_start = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
        
        if (!formatted.contains("%i"))
        {
            juce::File dest = file_start.getSiblingFile(formatted);
            
            if (dest.exists())
            {
                if (!options.overwrite)
                {
                    return {};
                }
                
                (void) dest.deleteFile();
            }
            
            if (!::copyTo(parRotationManager.getLogFile(), dest, options.compressionLevel))
            {
                throw LogIOException("could not rotate log '"
                                     + parRotationManager.getLogFile().getFullPathName() + "'");
            }
            
            return dest;
        }
        else
        {
            std::vector<juce::File> archives;
            archives.reserve(options.max - options.min);
            
            for (int i = options.min; i <= options.max; ++i)
            {
                const juce::String archive_name = formatted .replace("%i", juce::String(i));
                juce::File         archive_file = file_start.getSiblingFile(archive_name);
                
                if (archive_file.exists())
                {
                    if (i == options.max)
                    {
                        (void) archives[0].deleteFile();
                        
                        for (int j = 1; j < archives.size(); ++j)
                        {
                            const juce::File &ac1 = archives[j];
                            const juce::File &ac2 = archives[j - 1];
                            
                            if (!ac1.moveFileTo(ac2))
                            {
                                throw LogIOException("could not move log archive '" + ac1.getFileName() + "' to '"
                                                     + ac2.getFileName() + "'");
                            }
                        }
                        
                        if (const juce::File &dest = archives[archives.size() - 1]; !archive_file.moveFileTo(dest))
                        {
                            throw LogIOException("could not move log archive '" + archive_file.getFileName() + "' to '"
                                                 + dest.getFileName() + "'");
                        }
                    }
                    else
                    {
                        (void) archives.emplace_back(archive_file);
                        continue;
                    }
                }
                
                if (!::copyTo(parRotationManager.getLogFile(), archive_file, options.compressionLevel))
                {
                    throw LogIOException("could not rotate log '"
                                         + parRotationManager.getLogFile().getFullPathName() + "'");
                }
                
                return archive_file;
            }
        }
        
        return {};
    }
}
//======================================================================================================================
// endregion StrategyPattern
//**********************************************************************************************************************
