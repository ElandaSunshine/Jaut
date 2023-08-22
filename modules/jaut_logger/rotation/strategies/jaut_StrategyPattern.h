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
    @file   jaut_StrategyPattern.h
    @date   11, February 2023

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    class LogRotationManager;
    
    //==================================================================================================================
    /** A strategy that rotates the log files in a certain pattern. */
    class JAUT_API StrategyPattern
    {
    public:
        static constexpr int default_min              = 1;
        static constexpr int default_max              = 5;
        static constexpr int default_compressionLevel = 6;
        static constexpr int default_overwrite        = true;
        
        //==============================================================================================================
        struct Options
        {
            /** The minimum index for indexed log archives. (can not be less than 0 or more than max) */
            int min = default_min;
            
            /** 
             *  The maximum index for indexed log archives, after which the oldest will be deleted.
             *  (can not be less than min)
             */
            int max = default_max;
            
            /**
             *  For compressed archives, what compression level should be used.
             *  (0 to 9, where 0 is no compression and 9 is max compression)<br>
             *  For non compressed archives, this option will be ignored.
             */
            int compressionLevel = default_compressionLevel;
            
            /** Whether log archives with the same name should be overwritten by newer archives. */
            bool overwrite = default_overwrite;
        };
        
        //==============================================================================================================
        /**
         *  Creates a new instance.
         *  <br><br>
         *  The pattern that should be used to determine the naming of the archived log file.
         *  <br><br>
         *  Note that this specifies the path and not only the names of the rotated log files, this means that this
         *  can be an absolute path or a relative path starting in the executable's directory.
         *  <br><br>
         *  Also note that, the extension that is used, will determine the file format.<br>
         *  Available are ".zip" to store them in zip format, ".gz" to store them in gzip format and any other
         *  extension to store it as a plain text file with no compression.
         *  <br><br>
         *  Valid variables, that can be part of the name, are:
         *  <ul>
         *      <li>Any of the juce::Time::formatted specifiers</li>
         *      <li>'%i' for a counter based index of an archive with the same name</li>
         *  </ul>
         *  Just make sure to have a distinct name, otherwise archives will be overridden by newer ones.
         *  (this does not concern patterns that include the %i specifier)
         *  
         *  @param pattern The rotated file naming pattern
         *  @param options The options of pattern strategy
         */
        StrategyPattern(juce::String pattern, Options options);
        
        /**
         *  Creates a new instance.
         *  <br><br>
         *  The pattern that should be used to determine the naming of the archived log file.
         *  <br><br>
         *  Note that this specifies the path and not only the names of the rotated log files, this means that this
         *  can be an absolute path or a relative path starting in the executable's directory.
         *  <br><br>
         *  Also note that, the extension that is used, will determine the file format.<br>
         *  Available are ".zip" to store them in zip format, ".gz" to store them in gzip format and any other
         *  extension to store it as a plain text file with no compression.
         *  <br><br>
         *  Valid variables, that can be part of the name, are:
         *  <ul>
         *      <li>Any of the juce::Time::formatted specifiers</li>
         *      <li>'%i' for a counter based index of an archive with the same name</li>
         *  </ul>
         *  Just make sure to have a distinct name, otherwise archives will be overridden by newer ones.
         *  (this does not concern patterns that include the %i specifier)
         *  
         *  @param pattern The rotated file naming pattern
         */
        explicit StrategyPattern(juce::String pattern);
        
        //==============================================================================================================
        /**
         *  Executes the operation and tries to rotate files.<br>
         *  If the rotated archive by an equal name already exists and Options::overwrite is set to false,
         *  the return value of this function will be an invalid file.
         *  
         *  @param rotationManager The rotation manager
         *  @return The created archive or invalid if overwriting is disabled and a file by the same name already exists
         */
        juce::File operator()(const LogRotationManager &rotationManager);
        
    private:
        Options      options;
        juce::String pattern;
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(StrategyPattern)
    };
}
