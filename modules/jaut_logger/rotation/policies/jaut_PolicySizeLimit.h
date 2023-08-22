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
    @file   jaut_PolicySizeLimit.h
    @date   01, April 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/rotation/policies/jaut_RotationPolicy.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /**  Rotation policy that rotates the logs before a certain file size has been reached or exceeded.  */
    class JAUT_API PolicySizeLimit
    {
    public:
        static constexpr bool default_uponReaching = false;
        
        //==============================================================================================================
        /**
         *  Creates a new instance.
         *  
         *  @param maxSizeInBytes The maximum size of the log to reach before it gets rotated (in bytes)
         *  @param uponReaching   Should the log be rotated before the limit is reached or after it already has
         */
        explicit PolicySizeLimit(std::size_t maxSizeInBytes, bool uponReaching = default_uponReaching);
        
        //==============================================================================================================
        bool operator()(const RotationPolicyArgs &rotationArgs);
        
    private:
        inline static const std::size_t newLineSize = juce::String(juce::NewLine::getDefault()).getNumBytesAsUTF8();
        
        //==============================================================================================================
        std::size_t maxSize;
        std::size_t fileSize { 0 };
        bool        uponReaching;
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(PolicySizeLimit)
    };
}
