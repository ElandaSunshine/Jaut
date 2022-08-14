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
    
    Copyright (c) 2022 ElandaSunshine
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
    T left   {};
    T top    {};
    T right  {};
    T bottom {};
    
    //==================================================================================================================
    constexpr Distance() noexcept = default;
    
    constexpr Distance(T leftDistance, T topDistance, T rightDistance, T bottomDistance) noexcept
        : left(leftDistance), top(topDistance), right(rightDistance), bottom(bottomDistance)
    {}
    
    constexpr Distance(T topDistance, T leftAndRightDistance, T bottomDistance) noexcept
        : Distance(leftAndRightDistance, topDistance, leftAndRightDistance, bottomDistance)
    {}
    
    constexpr Distance(T leftAndRightDistance, T topAndBottomDistance) noexcept
        : Distance(topAndBottomDistance, leftAndRightDistance, topAndBottomDistance)
    {}
    
    constexpr Distance(T allSides) noexcept // NOLINT
        : Distance(allSides, allSides)
    {}
    
    //==================================================================================================================
    bool operator==(const Distance &other) const noexcept
    {
        return other.left == left && other.top == top && other.right == right && other.bottom == bottom;
    }
    
    bool operator!=(const Distance &other) const noexcept
    {
        return !operator==(other);
    }
    
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
    
    //==================================================================================================================
    juce::Rectangle<T> trimRectangle(juce::Rectangle<T> rect) const noexcept
    {
        return rect.withTrimmedLeft(left).withTrimmedTop(top).withTrimmedRight(right).withTrimmedBottom(bottom);
    }
};

template<class T>
using Thickness = Distance<T>;
using Margin    = Distance<int>;
using Padding   = Distance<int>;
}
