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
    @file   jaut_StaticVector.h
    @date   10, August 2022
    
    ===============================================================
 */
 
#pragma once

#include <array>



namespace detail
{

}

template<class T, std::size_t Capacity = 32, class Container = std::array<T, Capacity>>
class StaticVector
{
public:
    using value_type             = T;
    using pointer                = value_type*;
    using const_pointer          = const pointer;
    using reference              = value_type&;
    using const_reference        = const reference;
    using iterator               = value_type*;
    using const_iterator         = const iterator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    using ContainerType = Container;
    
    //==================================================================================================================
    static constexpr std::size_t capacity = Capacity;
    
    //==================================================================================================================
    constexpr StaticVector() noexcept = default;
    
    constexpr StaticVector(size_type count, const T& value) noexcept
    {
        std::fill_n(begin(), count, value);
    }
    
    //==================================================================================================================
    constexpr iterator begin() noexcept
    {
        return container.begin();
    }
    
    constexpr iterator end() noexcept
    {
        return last;
    }
    
private:
    Container container;
    pointer   last;
};

