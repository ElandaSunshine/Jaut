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
    @file   jaut_PolicyCombined.h
    @date   26, January 2023

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/rotation/policies/jaut_RotationPolicy.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    /** A combined policy. */
    class JAUT_API PolicyCombined
    {
    public:
        /**
         *  Creates a combined policy.
         *  @param policies The policies to combine
         */
        explicit PolicyCombined(std::vector<RotationPolicy> policies) noexcept;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool operator()(const RotationPolicyArgs&) const noexcept;
        
    private:
        std::vector<RotationPolicy> policies;
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(PolicyCombined)
    };
}
