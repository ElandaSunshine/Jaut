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
 *  Determines whether Type is of same type as Required.
 *  The difference to the std version is that this one only checks for the type class regardless its templates.
 *
 *  @tparam Required The required type without its template list
 *  @tparam Type     The type to check
 */
template<template<class...> class TemplatedType, class TypeToCheck>
struct JAUT_API sameTypeIgnoreTemplate : public std::false_type {};

template<template<class...> class TemplatedType, class ...Args>
struct JAUT_API sameTypeIgnoreTemplate<TemplatedType, TemplatedType<Args...>> : public std::true_type {};

/**
 *  Determines whether Type is of same type as Required.
 *  The difference to the std version is that this one only checks for the type class regardless its templates.
 *
 *  @tparam Required The required type without its template list
 *  @tparam Type     The type to check
 *  @return True if TypeToCheck has the same type as TemplatedType regardless its type-list types
 */
template<template<class...> class TemplatedType, class TypeToCheck>
JAUT_API inline constexpr bool sameTypeIgnoreTemplate_v = sameTypeIgnoreTemplate<TemplatedType, TypeToCheck>::value;

/**
 *  Checks whether type T is a templated type or not.
 *  @tparam T The type to check
 */
template<class T>
struct JAUT_API isTemplated : std::false_type {};

template<template<class...> class T, class ...Args>
struct JAUT_API isTemplated<T<Args...>> : std::true_type {};

/**
 *  Checks whether type T is a templated type or not.
 *
 *  @tparam T The type to check
 *  @return True if T is a class with a type-list
 */
template<class T>
JAUT_API inline constexpr bool isTemplated_v = isTemplated<T>::value;

/**
 *  Checks whether type T's template list is empty or not.
 *  @tparam T The type to check
 */
template<class T>
struct JAUT_API hasArguments;

template<template<class...> class T, class ...Args>
struct JAUT_API hasArguments<T<Args...>> : std::bool_constant<sizeof...(Args) != 0> {};

/**
 *  Checks whether type T's template list is empty or not.
 *
 *  @tparam T The type to check
 *  @return True if the given Type has any types in its type-list
 */
template<class T>
JAUT_API inline constexpr bool hasArguments_v = hasArguments<T>::value;

/**
 *  Gets whether the type-list of two different classes are equal.
 *
 *  @tparam LeftTypeList  The left type
 *  @tparam RightTypeList The right type
 */
template<class LeftTypeList, class RightTypeList>
struct hasSameTypeList : std::false_type {};

template<template<class...> class LeftTypeList, template<class...> class RightTypeList, class ...Args>
struct hasSameTypeList<LeftTypeList<Args...>, RightTypeList<Args...>> : std::true_type {};

/**
 *  Gets whether the type-list of two different classes are equal.
 *
 *  @tparam LeftTypeList  The left type
 *  @tparam RightTypeList The right type
 *  @return True if both types have the same type-list
 */
template<class LeftTypeList, class RightTypeList>
static constexpr bool hasSameTypeList_v = hasSameTypeList<LeftTypeList, RightTypeList>::value;

/**
 *  Replaces a specific type from the type-list of a class with another.
 *  This replaces as many as it finds.
 *
 *  @tparam Wrapper       The class with the type-list
 *  @tparam TypeToReplace The type to be replaced
 *  @tparam ReplaceWith   The new type that will be inserted instead
 */
template<class Wrapper, class TypeToReplace, class ReplaceWith>
struct JAUT_API replaceType;

template<template<class...> class Wrapper, class TypeToReplace, class ReplaceWith, class ...Args>
struct JAUT_API replaceType<Wrapper<Args...>, TypeToReplace, ReplaceWith>
{
private:
    template<class ...Types>
    struct InternalTypeList
    {
        template<class NewType>
        using getWith = InternalTypeList<Types..., NewType>;
        
        template<template<class...> class WrapperType>
        using makeType = WrapperType<Types...>;
    };
    
    template<class TypeList, class InPlaceType, class...>
    struct PlaceholderResolver
    {
        using type = TypeList;
    };
    
    template<class TypeList, class InPlaceType, class First>
    struct PlaceholderResolver<TypeList, InPlaceType, First>
    {
        using type = typename TypeList::template getWith<
            std::conditional_t<
                std::is_same_v<First, TypeToReplace>,
                InPlaceType,
                First
            >
        >;
    };
    
    template<class TypeList, class InPlaceType, class First, class ...Others>
    struct PlaceholderResolver<TypeList, InPlaceType, First, Others...>
    {
        using type = typename PlaceholderResolver<
            typename TypeList::template getWith<
                std::conditional_t<
                    std::is_same_v<First, TypeToReplace>,
                    InPlaceType,
                    First
                >
            >,
            InPlaceType, Others...
        >::type;
    };

public:
    using type = typename PlaceholderResolver<InternalTypeList<>, ReplaceWith, Args...>
                          ::type::template makeType<Wrapper>;
};

/**
 *  Replaces a specific type from the type-list of a class with another.
 *  This replaces as many as it finds.
 *
 *  @tparam Wrapper       The class with the type-list
 *  @tparam TypeToReplace The type to be replaced
 *  @tparam ReplaceWith   The new type that will be inserted instead
 *  @return The new type with the modified type-list
 */
template<class Holder, class TypeToReplace, class ReplaceWith>
JAUT_API using replaceType_t = typename replaceType<Holder, TypeToReplace, ReplaceWith>::type;
}
