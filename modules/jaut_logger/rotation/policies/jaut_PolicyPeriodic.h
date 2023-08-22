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
    @file   jaut_PolicyPeriodic.h
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
    /** A timed rotation policies, this will rotate the log file at the specified interval. */
    class JAUT_API PolicyPeriodic
    {
    public:
        /**
         *  Creates a new periodic policy instance.
         *  @param intervalInSeconds The interval in seconds when this policy returns true
         */
        explicit PolicyPeriodic(std::int64_t intervalInSeconds) noexcept;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool operator()(const RotationPolicyArgs&) noexcept;
        
    private:
        juce::int64 interval;
        juce::int64 begin { juce::Time::currentTimeMillis() };
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(PolicyPeriodic)
    };
}
