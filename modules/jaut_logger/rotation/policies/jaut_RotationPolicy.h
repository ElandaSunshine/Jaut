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
    @file   jaut_RotationPolicy.h
    @date   26, January 2023

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /** The arguments passed to a rotation policy to which the next rotation action should be determined from. */
    struct JAUT_API RotationPolicyArgs
    {
        /**
         *  The current file that is being logged to and has not yet been rotated.<br>
         *  This is where you'd want to check whether a rotation might be necessary.
         */
        const juce::File &currentLogFile;
        
        /**
         *  The last message event from the logger.<br>
         *  You wouldn't normally base the rotation on this data, but for special cases it might still be used.
         */
        const LogMessage &message;
        
        /**
         *  The rendered message from the last log event.<br>
         *  This will be your RotationArgs::message but already fed through the formatter that was used,
         *  this is what you'd usually base your rotation on.
         */
        const juce::String &messageRendered;
    };
    
    //==================================================================================================================
    /**
     *  The rotation policy.<br>
     *  The policy will determine whether the log should be rotated when it returns true.
     *  Usually you can do that with a lambda, but if you have a more complex policy a functor class is preferable.
     *  
     *  @param rotationArgs The data that is needed to calculate whether it should be rotated
     *  @return True if the rotation should happen now, false if no rotation is necessary
     */
    JAUT_API using RotationPolicy = std::function<bool(const RotationPolicyArgs &rotationArgs)>;
};
