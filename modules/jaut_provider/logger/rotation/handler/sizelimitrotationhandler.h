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

    @author Elanda (elanda@elandasunshine.com)
    @file   sizelimitrotationhandler.h
    @date   01, April 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  Rotation handler that rotates the logs when a certain file size has been reached or exceeded.
     *  @tparam MaxFileSizeInBytes The maximum file size (in bytes) the log should have before it gets rotated
     */
    template<std::size_t MaxFileSizeInBytes>
    class SizeLimitRotationHandler
    {
    public:
        /**
         *  Creates a new instance.
         *  @param maxFileSize The maximum size of the log to reach before it gets rotated (in bytes)
         */
        explicit SizeLimitRotationHandler(const juce::File &file)
            : fileSize(file.getSize())
        {}
        
        //==============================================================================================================
        bool needsToBeRotated(const juce::String &message, Logger::LogLevel)
        {
            if (fileSize >= MaxFileSizeInBytes)
            {
                fileSize = 0;
                return true;
            }
            
            fileSize += (message.getNumBytesAsUTF8() + newLineSize);
            return false;
        }
        
        void reset() {}
        
    private:
        const std::size_t newLineSize { juce::String('\n').getNumBytesAsUTF8() };
        std::size_t fileSize;
    };
}
