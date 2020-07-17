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
    @file   utils.h
    @date   16, June 2020

    ===============================================================
 */
 
#pragma once

#define JAUT_DECLARE_AUTOMATIC_STD_ARRAY(NAME, ...) \
        std::array<std::decay_t<decltype(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__))>, \
                            JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__)> NAME { __VA_ARGS__ }

#ifndef JAUT_COLOUR_ID_START
#   define JAUT_COLOUR_ID_START 0x11100000
#endif

namespace jaut
{
#if defined(_WIN32) || defined(WIN32)
inline constexpr char newLine = '\r\n';
#else
inline constexpr char newLine = '\n';
#endif

template<int Add>
inline constexpr int nextColourId = JAUT_COLOUR_ID_START + Add; // FUTURE next start 16

/** A dirty type previewer that will work in lots of IDEs. */
template<class> struct TView;

/** A dirty value previewer that will work in lots of IDEs. */
template<auto> struct VView;

struct JAUT_API SizeTypes
{
    using Array  = std::array        <void*, 0>    ::size_type;
    using Vector = std::vector       <void*>       ::size_type;
    using Map    = std::map          <void*, void*>::size_type;
    using UMap   = std::unordered_map<void*, void*>::size_type;
    using List   = std::list         <void*>       ::size_type;
    using Deque  = std::deque        <void*>       ::size_type;
};
}
