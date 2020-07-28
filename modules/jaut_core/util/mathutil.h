/**
    ===============================================================
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

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda (elanda@elandasunshine.xyz)
    @file   mathhelper.h
    @date   04, March 2020

    ===============================================================
 */

#pragma once

namespace jaut
{
enum JAUT_API FitMode
{
    /** Test if value is between lower and upper. */
    FitClamped,
    
    /** Test if value is between lower and upper or equals lower. */
    FitLower,
    
    /** Test if value is between lower and upper or equals upper. */
    FitUpper,
    
    /** Test if value is between lower and upper or equals lower and/or upper. */
    FitRange
};

/**
 *  Tests whether a given number fits the given bound requirements according to the specified mode.
 *
 *  @tparam Mode      The test mode
 *  @tparam ValueType The type of the parameters
 *  @param test       The number to test
 *  @param lower      The lower bound of the checked range
 *  @param upper      The upper bound of the checked range
 *  @return True if the tested value fits inside the range
 */
template <class ValueType, FitMode Mode = FitLower>
JAUT_API inline constexpr bool fit(ValueType test, ValueType lower, ValueType upper) noexcept
{
    if constexpr(Mode == FitClamped)
    {
        return test > lower && test < upper;
    }
    else if constexpr(Mode == FitLower)
    {
        return test >= lower && test < upper;
    }
    else if constexpr(Mode == FitUpper)
    {
        return test > lower && test <= upper;
    }
    
    return test >= lower && test <= upper;
}

template <class ValueType>
JAUT_API inline constexpr ValueType compareAndGet(ValueType value, ValueType isNot, ValueType elseReturn) noexcept
{
    return value != isNot ? value : elseReturn;
}
}
