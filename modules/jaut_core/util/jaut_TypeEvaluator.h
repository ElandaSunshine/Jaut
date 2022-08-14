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
    @file   evaluator.h
    @date   28, June 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_DefUtils.h>

#include <cstddef>

namespace jaut
{
    template<class, class, class> struct replaceType;
    template<class, std::size_t>  struct getTypeAt;
    
    template<class>        struct Predicate;
    template<class>        struct PredicateNot;
    template<class, class> struct PredicateAnd;
    template<class, class> struct PredicateOr;
    template<class, class> struct PredicateXor;
    
    /** A placeholder used for jaut::Predicate to be replaced with a different type later on while operating. */
    template<std::size_t I = 0>
    struct JAUT_API PType {};
    
    namespace detail
    {
        template<class...> struct typeList;
        
        template<class Replacements, class List, class ...Types>
        struct replaceAllPlaceholders;
        
        template<class Replacements, class List, std::size_t I, class ...Types>
        struct replaceAllPlaceholders<Replacements, List, PType<I>, Types...>
            : replaceAllPlaceholders<Replacements,
                                     typename List::template getWith<typename getTypeAt<Replacements, I>::type>,
                                     Types...>
        {};
        
        template<class Replacements, class List, class First, class ...Types>
        struct replaceAllPlaceholders<Replacements, List, First, Types...>
            : replaceAllPlaceholders<Replacements, typename List::template getWith<First>, Types...>
        {};
        
        template<class Replacements, class List>
        struct replaceAllPlaceholders<Replacements, List> { using type = List; };
        
        template<class Replacements, class ...Types>
        using replaceAllPlaceholders_t = typename replaceAllPlaceholders<Replacements, typeList<>, Types...>::type;
        
        
        
        template<class Condition>
        struct ValidateCondition;
        
        template<template<class...> class Condition, class ...Types>
        struct ValidateCondition<Condition<Types...>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Types...>
                                                 ::template makeType<Condition>::value;
        };
        
        template<class Condition>
        struct ValidateCondition<Predicate<Condition>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Condition>
                                                 ::template makeType<Predicate>::template value<Replacements...>;
        };
        
        template<class Condition>
        struct ValidateCondition<PredicateNot<Condition>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Condition>
                                                 ::template makeType<PredicateNot>::template value<Replacements...>;
        };
        
        template<class ...Conditions>
        struct ValidateCondition<PredicateAnd<Conditions...>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Conditions...>
                                                 ::template makeType<PredicateAnd>::template value<Replacements...>;
        };
        
        template<class ...Conditions>
        struct ValidateCondition<PredicateOr<Conditions...>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Conditions...>
                                                 ::template makeType<PredicateOr>::template value<Replacements...>;
        };
        
        template<class ...Conditions>
        struct ValidateCondition<PredicateXor<Conditions...>>
        {
            template<class ...Replacements>
            static constexpr bool validate = replaceAllPlaceholders_t<typeList<Replacements...>, Conditions...>
                                                 ::template makeType<PredicateXor>::template value<Replacements...>;
        };
    }
    
    /**
     *  A delegated condition that will be checked later instead of immediate validation.
     *  Predicates can be nested with jaut::And, jaut::Or and jaut::Xor, which allows multi-conditional evaluation.
     *  
     *  Example predicate that evaluates to true if, and only if, the given type is either an integral
     *  or floating-point type:
     *  @code
     *  using FloatOrIntegral = jaut::Predicate<jaut::PredicateOr<std::is_integral<PType>, std::is_floating_point<PType>>>;
     *  
     *  // Can be evaluated later one replacing the type PType with the desired type
     *  static_assert(FloatOrIntegral::validate<int>, "Not integral and not floating-point"); // does not assert
     *  static_assert(FloatOrIntegral::validate<double>, "Not integral and not floating-point"); // does not assert
     *  static_assert(FloatOrIntegral::validate<std::string>, "Not integral and not floating-point"); // will assert
     *  // Here, int, double and std::string will be inserted were PType was specified
     *  @endcode
     *  
     *  @tparam Condition A conditional type that must contain a bool variable named value to be evaluated
     */
    template<class Condition>
    struct JAUT_API Predicate
    {
        template<class ...Replacements>
        static constexpr bool value = detail::ValidateCondition<Condition>::template validate<Replacements...>;
    };
    
    /**
     *  Returns true if the condition evaluated to false.
     *  @tparam Condition The first condition to evaluate
     */
    template<class Condition>
    struct JAUT_API PredicateNot
    {
        template<class ...Replacements>
        static constexpr bool value = !detail::ValidateCondition<Condition>::template validate<Replacements...>;
    };
    
    /**
     *  Collates two conditions and returns true if both conditions meet their requirements.
     *
     *  @tparam Condition1 The first condition to evaluate
     *  @tparam Condition2 The second condition to evaluate
     */
    template<class Condition1, class Condition2>
    struct JAUT_API PredicateAnd
    {
        template<class ...Replacements>
        static constexpr bool eval_1 = detail::ValidateCondition<Condition1>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool eval_2 = detail::ValidateCondition<Condition2>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool value = (eval_1<Replacements...> && eval_2<Replacements...>);
    };
    
    /**
     *  Collates two conditions and returns true if either one of the conditions meet their requirements.
     *  
     *  @tparam Condition1 The first condition to evaluate
     *  @tparam Condition2 The second condition to evaluate
     */
    template<class Condition1, class Condition2>
    struct JAUT_API PredicateOr
    {
        template<class ...Replacements>
        static constexpr bool eval_1 = detail::ValidateCondition<Condition1>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool eval_2 = detail::ValidateCondition<Condition2>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool value = (eval_1<Replacements...> || eval_2<Replacements...>);
    };
    
    /**
     *  Collates two conditions and returns true if only one of the conditions meet their requirements.
     *  
     *  @tparam Condition1 The first condition to evaluate
     *  @tparam Condition2 The second condition to evaluate
     */
    template<class Condition1, class Condition2>
    struct JAUT_API PredicateXor
    {
        template<class ...Replacements>
        static constexpr bool eval_1 = detail::ValidateCondition<Condition1>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool eval_2 = detail::ValidateCondition<Condition2>::template validate<Replacements...>;
        
        template<class ...Replacements>
        static constexpr bool value = (eval_1<Replacements...> != eval_2<Replacements...>);
    };
}
