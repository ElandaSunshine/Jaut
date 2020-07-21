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
    
    @author Elanda
    @file   sizestructs.h
    @date   20, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
template<class T>
struct JAUT_API Distance
{
    T left;
    T top;
    T right;
    T bottom;
    
    //==================================================================================================================
    constexpr Distance(T left, T top, T right, T bottom) noexcept
        : left(left), top(top), right(right), bottom(bottom)
    {}
    
    constexpr Distance(T top, T leftAndRight, T bottom) noexcept
        : Distance(leftAndRight, top, leftAndRight, bottom)
    {}
    
    constexpr Distance(T leftAndRight, T topAndBottom) noexcept
        : Distance(topAndBottom, leftAndRight, topAndBottom)
    {}
    
    explicit constexpr Distance(T all) noexcept
        : Distance(all, all)
    {}
    
    //==================================================================================================================
    std::conditional_t<std::is_same_v<float, T>, Distance<float>&, Distance<float>> toFloat() const noexcept
    {
        if constexpr (std::is_same_v<T, float>)
        {
            return *this;
        }
        else
        {
            return { left, top, right, bottom };
        }
    }
    
    std::conditional_t<std::is_same_v<int, T>, Distance<int>&, Distance<int>> toInt() const noexcept
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return *this;
        }
        else
        {
            return { juce::roundToInt(left), juce::roundToInt(top), juce::roundToInt(right), juce::roundToInt(bottom) };
        }
    }
};

template<class T>
using Thickness = Distance<T>;
using Margin    = Distance<int>;
using Padding   = Distance<int>;
}
