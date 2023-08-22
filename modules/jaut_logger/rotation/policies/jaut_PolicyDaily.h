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
    @file   jaut_PolicyDaily.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/rotation/policies/jaut_RotationPolicy.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /** A scheduled rotation policy, this will rotate the log file at the specified time. */
    class JAUT_API PolicyDaily
    {
    public:
        static constexpr bool default_useLocalTime = true;
        
        //==============================================================================================================
        /**
         *  Creates a daily rotator.
         *  
         *  @param hour         The hour of the day
         *  @param minute       The minute of the hour
         *  @param useLocalTime If the time of rotation should depend on the local time or UTC time
         */
        PolicyDaily(int hour, int minute, bool useLocalTime = default_useLocalTime) noexcept;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool operator()(const RotationPolicyArgs&) noexcept;
        
    private:
        juce::Time timeToRotate;
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(PolicyDaily)
    };
}
