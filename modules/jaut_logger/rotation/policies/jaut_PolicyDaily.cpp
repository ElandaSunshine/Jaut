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
    @file   jaut_PolicyDaily.cpp
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/rotation/policies/jaut_PolicyDaily.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    juce::Time getTimeToRotate(int parHour, int parMinute, bool local)
    {
        const juce::Time time = juce::Time::getCurrentTime();
        
        const int hours   = time.getHours();
        const int minutes = time.getMinutes();
        const int day     = time.getDayOfMonth() + (parHour > hours || (parHour == hours && parMinute >= minutes));
        const int utc_off = (!local ? (time.getUTCOffsetSeconds() / 3600) : 0);
        
        return {
            time.getYear(),
            time.getMonth(),
            day,
            parHour + utc_off,
            parMinute,
            time.getSeconds(),
            time.getMilliseconds()
        };
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region PolicyDaily
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    PolicyDaily::PolicyDaily(int parHour, int parMinute, bool parUseLocalTime) noexcept
        : timeToRotate(::getTimeToRotate(parHour, parMinute, !parUseLocalTime))
    {
        // Looks like you are trying to break time-and-space here
        // Make sure the values are directly proportional to a real-world's time understanding;
        // breaking through the border between dimensions is not included with this class sadly
        jassert(parHour   < 24 && parHour   > -1 && "Hours should be a value between 0 and 23 (including)");
        jassert(parMinute < 60 && parMinute > -1 && "Minutes should be a value between 0 and 59 (including)");
    }
    
    //==================================================================================================================
    bool PolicyDaily::operator()(const RotationPolicyArgs&) noexcept
    {
        const juce::Time now = juce::Time::getCurrentTime();
        
        if (now >= timeToRotate)
        {
            timeToRotate = ::getTimeToRotate(timeToRotate.getHours(), timeToRotate.getMinutes(), true);
            return true;
        }
        
        return false;
    }
}
//======================================================================================================================
// endregion PolicyDaily
//**********************************************************************************************************************
