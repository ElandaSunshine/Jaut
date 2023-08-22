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
    @file   jaut_PolicySizeLimit.cpp
    @date   01, April 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/rotation/policies/jaut_PolicySizeLimit.h>



//**********************************************************************************************************************
// region PolicySizeLimit
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    PolicySizeLimit::PolicySizeLimit(std::size_t parMaxSizeInBytes, bool parUponReaching)
        : maxSize     (parMaxSizeInBytes),
          uponReaching(parUponReaching)
    {}
    
    //==================================================================================================================
    bool PolicySizeLimit::operator()(const RotationPolicyArgs &parRotationArgs)
    {
        if (fileSize == 0)
        {
            fileSize = parRotationArgs.currentLogFile.getSize();
        }
        
        const std::size_t new_size = (fileSize + parRotationArgs.messageRendered.getNumBytesAsUTF8());
        
        if ((uponReaching ? (new_size >= maxSize) : (fileSize >= maxSize)))
        {
            fileSize = 0;
            return true;
        }
        
        fileSize = new_size;
        return false;
    }
}
//======================================================================================================================
// endregion PolicySizeLimit
//**********************************************************************************************************************
