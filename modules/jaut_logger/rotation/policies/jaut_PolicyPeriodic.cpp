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
    @file   jaut_PolicyPeriodic.cpp
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/rotation/policies/jaut_PolicyPeriodic.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    constexpr juce::int64 milisecondsInADay    = 86400000;
    constexpr juce::int64 milisecondsInASecond = 1000;
    constexpr juce::int64 secondsInAHour       = 3600;
    constexpr juce::int64 secondsInAMinute     = 60;
    
    //==================================================================================================================
    JAUT_NODISCARD
    constexpr juce::int64 getTimeInMiliseconds(int hour, int minute, int second) noexcept
    {
        return ((hour * secondsInAHour) + (minute * secondsInAMinute) + second) * milisecondsInASecond;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region PolicyPeriodic
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    PolicyPeriodic::PolicyPeriodic(std::int64_t parIntervalInSeconds) noexcept
        : interval(parIntervalInSeconds * 1000)
    {}
    
    //==================================================================================================================
    bool PolicyPeriodic::operator()(const RotationPolicyArgs&) noexcept
    {
        const juce::int64 now = juce::Time::currentTimeMillis();
        
        if ((now - begin) >= interval)
        {
            begin = now;
            return true;
        }
        
        return false;
    }
}
//======================================================================================================================
// endregion PolicyPeriodic
//**********************************************************************************************************************
