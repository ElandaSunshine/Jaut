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
    @file   typetraits.h
    @date   08, March 2020
    
    ===============================================================
 */

#pragma once

#include <functional>
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_DefUtils.h>

#include <jaut_core/util/jaut_TypeEvaluator.h>

#include <juce_core/juce_core.h>

#include <cstddef>
#include <type_traits>



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
        inline static constexpr std::false_type NAME ()   { return std::false_type{}; }

#define JAUT_UTIL_TYPE_TRAITS_DEFINE_METHOD_CHECK(NAME) \
        template<class T> class has_##NAME { \
        private: \
            template<class C> static std::uint8_t  check(decltype(&C::NAME)) {} \
            template<class C> static std::uint16_t check(...)                {} \
        public: static constexpr bool value = (sizeof(check<T>(0)) == sizeof(std::uint8_t)); };

namespace jaut
{
    /**
     *  Determines whether TypeToCheck is of same type as TemplatedType.
     *  The difference to the std version is that this one only checks for the type class regardless its templates.
     *  
     *  Note that this does not work with mixed type/value parameters (except they share the same type-list),
     *  such as std::array.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, class U, auto N, auto N2>
     *      struct sameTypeIgnoreTemplate<std::array<T, N>, std::array<U, N2>> : public std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam Required The required type
     *  @tparam Tested   The tested type
     */
    template<template<class...> class Required, class Tested>
    struct JAUT_API sameTypeIgnoreTemplate : std::false_type {};
    
    template<template<class...> class T, class ...Types>
    struct JAUT_API sameTypeIgnoreTemplate<T, T<Types...>> : std::true_type {};
    
    /**
     *  Determines whether TypeToCheck is of same type as TemplatedType.
     *  The difference to the std version is that this one only checks for the type class regardless its templates.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, class U, auto N, auto N2>
     *      struct sameTypeIgnoreTemplate<std::array<T, N>, std::array<U, N2>> : public std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam Required The required type
     *  @tparam Tested   The tested type
     */
    template<template<class...> class Required, class Tested>
    JAUT_API inline constexpr bool sameTypeIgnoreTemplate_v = sameTypeIgnoreTemplate<Required, Tested>::value;
    
    /**
     *  Checks whether type T is a templated type or not.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, auto N>
     *      struct isTemplated<std::array<T, N>> : public std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The type to check
     */
    template<class T>
    struct JAUT_API isTemplated : std::false_type {};
    
    template<template<class...> class T, class ...Args>
    struct JAUT_API isTemplated<T<Args...>> : std::true_type {};
    
    template<template<auto...> class T, auto ...Vals>
    struct JAUT_API isTemplated<T<Vals...>> : std::true_type {};
    
    /**
     *  Checks whether type T is a templated type or not.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, auto N>
     *      struct isTemplated<std::array<T, N>> : public std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The type to check
     */
    template<class T>
    JAUT_API inline constexpr bool isTemplated_v = isTemplated<T>::value;
    
    /**
     *  Checks whether type T's template list is empty or not.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, auto N>
     *      struct hasArguments<std::array<T, N>> : std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The type to check
     */
    template<class T>
    struct JAUT_API hasArguments : std::false_type {};
    
    template<template<class...> class T, class ...Types>
    struct JAUT_API hasArguments<T<Types...>> : std::bool_constant<sizeof...(Types) != 0> {};
    
    template<template<auto...> class T, auto ...Vals>
    struct JAUT_API hasArguments<T<Vals...>> : std::bool_constant<sizeof...(Vals) != 0> {};
    
    /**
     *  Checks whether type T's template list is empty or not.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<class T, auto N>
     *      struct hasArguments<std::array<T, N>> : std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The type to check
     */
    template<class T>
    JAUT_API inline constexpr bool hasArguments_v = hasArguments<T>::value;
    
    /**
     *  Gets whether the type-list of two different classes are equal.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<template<class, auto> class T, class Par, auto Par2>
     *      struct hasSameTypeList<std::array<Par, Par2>, T<Par, Par2>> : std::true_type {};
     *  
     *      template<template<class, auto> class T, class Par, auto Par2>
     *      struct hasSameTypeList<T<Par, Par2>, std::array<Par, Par2>> : std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The left type
     *  @tparam U The right type
     */
    template<class T, class U>
    struct JAUT_API hasSameTypeList : std::false_type {};
    
    template<template<class...> class T, template<class...> class U, class ...Types>
    struct JAUT_API hasSameTypeList<T<Types...>, U<Types...>> : std::true_type {};
    
    template<template<auto...> class T, template<auto...> class U, auto ...Vals>
    struct JAUT_API hasSameTypeList<T<Vals...>, U<Vals...>> : std::true_type {};
    
    /**
     *  Gets whether the type-list of two different classes are equal.
     *  
     *  Note that this does not work with mixed type/value parameters,
     *  such as for std::array this will evaluate to false.
     *  But you can add your own specialisation for these cases:
     *  @code
     *  namespace jaut
     *  {
     *      template<template<class, auto> class T, class Par, auto Par2>
     *      struct hasSameTypeList<std::array<Par, Par2>, T<Par, Par2>> : std::true_type {};
     *  
     *      template<template<class, auto> class T, class Par, auto Par2>
     *      struct hasSameTypeList<T<Par, Par2>, std::array<Par, Par2>> : std::true_type {};
     *  }
     *  @endcode
     *  
     *  @tparam T The left type
     *  @tparam U The right type
     */
    template<class T, class U>
    JAUT_API static constexpr bool hasSameTypeList_v = hasSameTypeList<T, U>::value;
    
    namespace detail
    {
        //==============================================================================================================
        // A compile-time bounds assert
        template<class T, T Value, T Start, T End>
        struct outOfBoundsAssert
        {
            static_assert(Value >= Start && Value < End, JAUT_ASSERT_TYPE_TRAITS_OUT_OF_BOUNDS);
        };
        
        //==============================================================================================================
        // Makes a dependent static assert, so it will only get triggered if the specialisation gets used
        template<class, bool Output = false>
        struct dependentTypeAssert : std::bool_constant<Output> {};
        
        template<class T, bool Output = false>
        inline constexpr bool dependentTypeAssert_v = dependentTypeAssert<T, Output>::value;
        
        template<auto, bool Output = false>
        struct dependentValueAssert : std::bool_constant<Output> {};
        
        template<auto V, bool Output = false>
        inline constexpr bool dependentValueAssert_v = dependentValueAssert<V, Output>::value;
        
        //==============================================================================================================
        template<class ...Types>
        struct typeList
        {            
            template<class ...T>
            using prepend = typeList<T..., Types...>;
            
            template<class ...T>
            using getWith = typeList<Types..., T...>;
            
            template<template<class...> class T>
            using makeType = T<Types...>;
            
            template<class T>
            using appendList = typename T::template prepend<Types...>;
        };
        
        //==============================================================================================================
        template<class List, class Key, class Replacement, class ...Types>
        struct replaceType_impl;
        
        template<class List, class Key, class Replacement, class ...Types>
        struct replaceType_impl<List, Key, Replacement, Key, Types...>
            : replaceType_impl<typename List::template getWith<Replacement>, Key, Replacement, Types...>
        {};
        
        template<class List, class Key, class Replacement, class First, class ...Types>
        struct replaceType_impl<List, Key, Replacement, First, Types...>
            : replaceType_impl<typename List::template getWith<First>, Key, Replacement, Types...>
        {};
        
        template<class List, class Key, class Replacement>
        struct replaceType_impl<List, Key, Replacement> { using type = List; };
        
        //==============================================================================================================
        template<std::size_t I, std::size_t Wanted, class ...Types>
        struct getTypeAt_impl
        {
            static_assert(dependentValueAssert_v<I>, JAUT_ASSERT_TYPE_TRAITS_OUT_OF_BOUNDS);
        };
        
        template<std::size_t I, class First, class ...Others>
        struct getTypeAt_impl<I, I, First, Others...> { using type = First; };
        
        template<std::size_t I, std::size_t Wanted, class First, class ...Types>
        struct getTypeAt_impl<I, Wanted, First, Types...> : getTypeAt_impl<I + 1, Wanted, Types...> {};
        
        //==============================================================================================================
        template<std::size_t I, class Key, class ...Types>
        struct getIndexOf_impl;
        
        template<std::size_t I, class Key, class ...Others>
        struct getIndexOf_impl<I, Key, Key, Others...> : std::integral_constant<std::size_t, I> {};
        
        template<std::size_t I, class Key, class First, class ...Others>
        struct getIndexOf_impl<I, Key, First, Others...> : getIndexOf_impl<I + 1, Key, Others...> {};
        
        //==============================================================================================================
        template<class Remove, class List, class ...Types>
        struct scanAndExclude;
        
        template<class Remove, class List>
        struct scanAndExclude<Remove, List> { using type = List; };  
        
        template<class Remove, class List, class ...Types>
        struct scanAndExclude<Remove, List, Remove, Types...> : scanAndExclude<Remove, List, Types...> {};
        
        template<class Remove, class List, class First, class ...Types>
        struct scanAndExclude<Remove, List, First, Types...>
            : scanAndExclude<Remove, typename List::template getWith<First>, Types...>
        {};
        
        
        
        template<class List, class ...Remove>
        struct removeType_impl;
        
        template<template<class...> class List, class First, class ...Remove, class ...Types>
        struct removeType_impl<List<Types...>, First, Remove...>
            : removeType_impl<typename scanAndExclude<First, typeList<>, Types...>::type, Remove...>
        {};
        
        template<class List>
        struct removeType_impl<List>
        {
            using type = List;
        };
        
        //==============================================================================================================
        template<class List, class Predicate, class ...Types>
        struct removeIf_impl;
        
        template<class List, class Predicate, class First, class ...Others>
        struct removeIf_impl<List, Predicate, First, Others...>
            : removeIf_impl<std::conditional_t<Predicate::template value<First>, List,
                            typename List::template getWith<First>>, Predicate, Others...>
        {};
        
        template<class List, class Predicate>
        struct removeIf_impl<List, Predicate>
        {
            using type = List;
        };
        
        //==============================================================================================================
        template<class Add, class List, class ...Types>
        struct scanAndInclude; 
        
        template<class Add, class List, class First, class ...Types>
        struct scanAndInclude<Add, List, First, Types...>
            : scanAndInclude<Add, typename List::template getWith<First>, Types...>
        {};
        
        template<class Add, class List, class ...Types>
        struct scanAndInclude<Add, List, Add, Types...>
        {
            using type = typename List::template getWith<Add, Types...>;
        };
        
        template<class Add, class List>
        struct scanAndInclude<Add, List> { using type = typename List::template getWith<Add>; }; 
        
        
        
        template<class List, class ...Add>
        struct addIfAbsent_impl;
        
        template<template<class...> class List, class First, class ...Add, class ...Types>
        struct addIfAbsent_impl<List<Types...>, First, Add...>
            : addIfAbsent_impl<typename scanAndInclude<First, typeList<>, Types...>::type, Add...>
        {};
        
        template<class List>
        struct addIfAbsent_impl<List> { using type = List; };
    }
    
    //==================================================================================================================
    /**
     *  Replaces a specific type from the type-list of a class with another.
     *  This replaces as many as it finds.
     *
     *  @tparam T           The class with the type-list
     *  @tparam Key         The type to be replaced
     *  @tparam Replacement The new type that will be inserted instead
     */
    template<class T, class Key, class Replacement>
    struct JAUT_API replaceType
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class Key, class Replacement, class ...Types>
    struct JAUT_API replaceType<T<Types...>, Key, Replacement>
    {        
        using type = typename detail::replaceType_impl<detail::typeList<>, Key, Replacement, Types...>::type;
    };
    
    /**
     *  Replaces a specific type from the type-list of a class with another.
     *  This replaces as many as it finds.
     *
     *  If T is a non templated type or contains value parameters, this will give back T.
     *
     *  @tparam T           The class with the type-list
     *  @tparam Key         The type to be replaced
     *  @tparam Replacement The new type that will be inserted instead
     */
    template<class T, class Key, class Replacement>
    JAUT_API using replaceType_t = typename replaceType<T, Key, Replacement>::type;
    
    //==================================================================================================================
    /**
     *  Checks whether a given type-list contains a specific type at least once.
     *
     *  @tparam T   The type-list to check
     *  @tparam Key The type to check for
     */
    template<class T, class Key>
    struct JAUT_API hasTemplateType
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class Key, class ...Types>
    struct JAUT_API hasTemplateType<T<Types...>, Key> : std::bool_constant<(std::is_same_v<Key, Types> || ...)> {};
    
    /**
     *  Checks whether a given type-list contains a specific type at least once.
     *
     *  @tparam T   The type-list to check
     *  @tparam Key The type to check for
     */
    template<class T, class Key>
    JAUT_API inline constexpr bool hasTemplateType_v = hasTemplateType<T, Key>::value;
    
    //==================================================================================================================
    /**
     *  Gets the amount of types in a type-list.
     *  @tparam T The type-list to check
     */
    template<class T>
    struct JAUT_API getTypeListLength
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class ...Args>
    struct JAUT_API getTypeListLength<T<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {};
    
    /**
     *  Gets the amount of types in a type-list.
     *  @tparam T The type-list to check
     */
    template<class T>
    JAUT_API inline constexpr std::size_t getTypeListLength_v = getTypeListLength<T>::value;
    
    //==================================================================================================================
    /**
     *  Gets a type of a type-list at the specified index.
     *
     *  @tparam T The type-list to get the type from
     *  @tparam I The index of the type to fetch
     */
    template<class T, std::size_t I>
    struct JAUT_API getTypeAt
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, std::size_t I, class ...Types>
    struct JAUT_API getTypeAt<T<Types...>, I> : detail::getTypeAt_impl<0, I, Types...> {};
    
    /**
     *  Gets a type of a type-list at the specified index.
     *
     *  @tparam T The type-list to get the type from
     *  @tparam I The index of the type to fetch
     */
    template<class T, std::size_t I>
    JAUT_API using getTypeAt_t = typename getTypeAt<T, I>::type;
    
    //==================================================================================================================
    /**
     *  Gets the index of the first matching type in the given type-list.
     *
     *  @tparam T   The type-list to search
     *  @tparam Key The type to find the index for
     */
    template<class T, class Key>
    struct JAUT_API getIndexOf
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class Key, class ...Types>
    struct JAUT_API getIndexOf<T<Types...>, Key> : detail::getIndexOf_impl<0, Key, Types...> {};
    
    /**
     *  Gets the index of the first matching type in the given type-list.
     *
     *  @tparam T   The type-list to search
     *  @tparam Key The type to find the index for
     */
    template<class T, class Key>
    JAUT_API inline constexpr auto getIndexOf_v = getIndexOf<T, Key>::value;
    
    //==================================================================================================================
    /**
     *  Removes a specified type from a type-list if it exists and gives back the type-list without it.
     *  
     *
     *  @tparam T      The type-list to modify
     *  @tparam Remove The types to remove
     */
    template<class T, class ...Remove>
    struct JAUT_API removeFrom
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class ...Remove, class ...Types>
    struct JAUT_API removeFrom<T<Types...>, Remove...>
    {
        using type = typename detail::removeType_impl<detail::typeList<Types...>, Remove...>
                                    ::type::template makeType<T>;
    };
    
    /**
     *  Removes a specified type from a type-list if it exists and gets the new version.
     *
     *  @tparam T      The type-list to modify
     *  @tparam Remove The types to remove
     */
    template<class T, class ...Remove>
    JAUT_API using removeFrom_t = typename removeFrom<T, Remove...>::type;
    
    //==================================================================================================================
    /**
     *  Removes all types from the type-list that match the given predicate.
     *  For more information on Predicates, refer to jaut::Predicate.
     *
     *  @tparam T         The type-list to modify
     *  @tparam Predicate The predicate which must be met
     *  @see jaut::Predicate
     */
    template<class T, class Predicate>
    struct JAUT_API removeIf
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class Pred, class ...Types>
    struct JAUT_API removeIf<T<Types...>, Pred>
    {    
        using type = typename detail::removeIf_impl<detail::typeList<>, Predicate<Pred>, Types...>
                                    ::type::template makeType<T>;
    };
    
    /**
     *  Removes all types from the type-list that match the given predicate.
     *  For more information on Predicates, refer to jaut::Predicate.
     *
     *  @tparam T         The type-list to modify
     *  @tparam Predicate The predicate which must be met
     *  @see jaut::Predicate
     */
    template<class T, class Predicate>
    JAUT_API using removeIf_t = typename removeIf<T, Predicate>::type;
    
    //==================================================================================================================
    /**
     *  Adds a type to a type-list if it isn't already contained.
     *
     *  @tparam T   The type-list to modify
     *  @tparam Add The types to add
     */
    template<class T, class ...Add>
    struct JAUT_API addIfAbsent
    {
        static_assert(detail::dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST);
    };
    
    template<template<class...> class T, class ...Add, class ...Types>
    struct JAUT_API addIfAbsent<T<Types...>, Add...>
    {
        using type = typename detail::addIfAbsent_impl<detail::typeList<Types...>, Add...>::type::template makeType<T>;
    };
    
    /**
     *  Adds a type to a type-list if it isn't already contained.
     *
     *  @tparam T   The type-list to modify
     *  @tparam Add The types to add
     */
    template<class T, class ...Add>
    JAUT_API using addIfAbsent_t = typename addIfAbsent<T, Add...>::type;
    
    //==================================================================================================================
    template<bool Condition, class T>
    struct JAUT_API TypeCase { static constexpr int id = 0; };
    
    template<class T>
    struct JAUT_API DefaultCase { static constexpr int id = 1; };
    
    namespace detail
    {        
        template<class ...Cases>
        struct TypeLadder_impl
        {
            static_assert(sizeof...(Cases) > 0, JAUT_ASSERT_TYPE_TRAITS_NO_DEFAULT_CASES);
        };
        
        template<class First, class ...Cases>
        struct TypeLadder_impl<First, Cases...> : TypeLadder_impl<Cases...> {};
        
        template<class T, class ...Cases>
        struct TypeLadder_impl<DefaultCase<T>, Cases...>
        {
            static_assert(dependentTypeAssert_v<T>, JAUT_ASSERT_TYPE_TRAITS_DEFAULT_CASES);
        };
        
        template<class T, class ...Cases>
        struct TypeLadder_impl<TypeCase<true, T>, Cases...>
        {
            using type = T;
        };
        
        template<class T>
        struct TypeLadder_impl<DefaultCase<T>>
        {
            using type = T;
        };
    }
    
    template<class ...Cases>
    struct JAUT_API TypeLadder
    {
        using type = typename detail::TypeLadder_impl<Cases...>::type;
    };
    
    template<class ...Cases>
    JAUT_API using TypeLadder_t = typename TypeLadder<Cases...>::type;
    
    //==================================================================================================================
    namespace detail
    {
        template<class Left, class Right>
        using isComparable_impl = decltype((std::declval<Left>() == std::declval<Right>()));
    }
    
    /**
     *  Checks whether two types are equality comparable.
     *  @tparam Left  The left type of the equality operator
     *  @tparam Right The right type of the equality operator
     */
    template<class Left, class Right, class = std::void_t<>>
    struct JAUT_API isEqualityComparable : std::false_type {};
    
    template<class Left, class Right>
    struct JAUT_API isEqualityComparable<Left, Right, std::void_t<detail::isComparable_impl<Left, Right>>>
        : std::is_same<detail::isComparable_impl<Left, Right>, bool>
    {};
    
    /**
     *  Checks whether two types are equality comparable.
     *  @tparam Left  The left type of the equality operator
     *  @tparam Right The right type of the equality operator
     */
     template<class Left, class Right>
     JAUT_API inline constexpr bool isEqualityComparable_v = isEqualityComparable<Left, Right>::value;
    
    //==================================================================================================================
    /**
     *  Checks whether type T could qualify as a string type.
     *  Note that this does not check whether the type really is a string type, just that it could be one.
     *  
     *  @tparam T The type to check
     */
    template<class T>
    struct JAUT_API qualifiesAsString
        : std::bool_constant<std::is_same_v<std::decay_t<T>,                   juce::String>
                          || std::is_same_v<std::decay_t<T>,                   juce::StringRef>
                          || std::is_same_v<std::decay_t<T>,                   juce::String::CharPointerType>
                          || std::is_same_v<std::decay_t<T>,                   wchar_t*>
                          || std::is_same_v<std::decay_t<T>,                   char16_t*>
                          || std::is_same_v<std::decay_t<T>,                   char32_t*>
                          || std::is_same_v<std::make_signed<std::decay_t<T>>, std::make_signed<char*>>
                          || std::is_same_v<std::make_signed<std::decay_t<T>>, std::make_signed<juce::juce_wchar*>>>
    {};
    
    template<template<class...> class T, class ...Types>
    struct JAUT_API qualifiesAsString<T<Types...>>
        : std::bool_constant<std::is_same_v<std::decay_t<T<Types...>>, std::basic_string_view<Types...>>
                          || std::is_same_v<std::decay_t<T<Types...>>, std::basic_string<Types...>>>
    {};
    
    /**
     *  Checks whether type T could qualify as a string type.
     *  Note that this does not check whether the type actually is a string type, just that it could be one.
     *  
     *  @tparam T The type to check
     */
    template<class T>
    JAUT_API inline constexpr bool qualifiesAsString_v = qualifiesAsString<T>::value;
    
    //==================================================================================================================
    template<class T, class = void>
    struct JAUT_API isIterator
    {
        static constexpr bool value = false;
    };
    
    template<class T>
    struct JAUT_API isIterator<T, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<T>::value_type, void>>>
    {
        static constexpr bool value = true;
    };
    
    template<class T>
    JAUT_API inline constexpr bool isIterator_v = isIterator<T>::value;
}
