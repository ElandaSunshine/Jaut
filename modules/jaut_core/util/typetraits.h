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
    @file   typetraits.h
    @date   08, March 2020

    ===============================================================
 */
#pragma once

/**
 *  This macro is a quick access compile-time type checker.
 *  The way this works is that it checks for a nested type of a class and if that doesn't exist, it will return false.
 *
 *  Example:
 *  @code
 *  struct YourClassOrStruct
 *  {
 *      // Define a new type checker
 *      JAUT_UTIL_TYPE_TRAITS_DEFINE_TYPE_CHECK(hasMyType, MyType)
 *
 *      // Define another type checker
 *      JAUT_UTIL_TYPE_TRAITS_DEFINE_TYPE_CHECK(hasMyType2, MyType2)
 *
 *      using MyType = unsigned;
 *
 *      void testExistence()
 *      {
 *          std::cout << hasMyType_v<YourClassOrStruct>  << std::endl; // This will return true, type exists
 *          std::cout << hasMyType2_v<YourClassOrStruct> << std::endl; // This will return false, type does not exist
 *      }
 *  }
 *
 *  @endcode
 *
 *  @param NAME The name of the function
 *  @param TYPE The nested type to test for
 */
#define JAUT_UTIL_TYPE_TRAITS_DEFINE_TYPE_CHECK(NAME, TYPE) \
        template <typename Type> inline static constexpr auto NAME () -> \
        decltype (typename Type:: TYPE (), std::true_type{})  { return std::true_type{}; } \
        inline static constexpr std::false_type NAME ()   { return std::false_type{}; } \
        template <typename Type> inline static constexpr bool NAME##_v = NAME <Type>().value;

namespace jaut
{
/**
 *  Determines whether Type is of same type as Required regardless it's template specification.
 *
 *  @tparam Type     The type to check
 *  @tparam Required The required type without its template list
 */
template<template<class...> class, class...>
struct JAUT_API is_of_templated_type : public std::false_type {};

template<template<class...> class Required, class... Type>
struct JAUT_API is_of_templated_type<Required, Required<Type...>> : public std::true_type {};
}
