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

    @author Elanda (elanda@elandasunshine.xyz)
    @file   jaut_MathUtil.h
    @date   04, March 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

namespace jaut
{
    enum class JAUT_API FitMode
    {
        /** Test if value is between lower and upper. */
        Clamped,
        
        /** Test if value is between lower and upper or equals lower. */
        Lower,
        
        /** Test if value is between lower and upper or equals upper. */
        Upper,
        
        /** Test if value is between lower and upper or equals lower and/or upper. */
        Range
    };

    /**
     *  Tests whether a given number fits the given bound requirements according to the specified mode.
     *
     *  @tparam T    The type to test
     *  @tparam Mode The testing mode
     *  @param value The number to test
     *  @param lower The lower bound of the checked range
     *  @param upper The upper bound of the checked range
     *  @return True if the tested value fits inside the range
     */
    template<class T, FitMode Mode = FitMode::Lower>
    JAUT_NODISCARD
    JAUT_API inline constexpr bool fit(T value, T lower, T upper) noexcept
    {
        if constexpr (Mode == FitMode::Clamped)
        {
            return (value > lower && value < upper);
        }
        else if constexpr (Mode == FitMode::Lower)
        {
            return (value >= lower && value < upper);
        }
        else if constexpr (Mode == FitMode::Upper)
        {
            return (value > lower && value <= upper);
        }
        else
        {
            return (value >= lower && value <= upper);
        }
    }
}
