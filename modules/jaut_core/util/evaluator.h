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
    @file   evaluator.h
    @date   28, June 2020

    ===============================================================
 */
 
#pragma once

namespace jaut
{
/** A placeholder used for jaut::Predicate to be replaced with a different type later on while operating. */
struct JAUT_API PType {};

template<class, class, bool, bool> struct And;
template<class, class, bool, bool> struct Or;
template<class, class, bool, bool> struct Xor;

namespace internal
{
template<class Condition>
struct ValidateCondition
{
    template<class InPlaceType>
    static constexpr bool validate = replaceType_t<Condition, PType, InPlaceType>::value;
};

template<class _1, class _2, bool _3, bool _4>
struct ValidateCondition<And<_1, _2, _3, _4>>
{
    template<class InPlaceType>
    static constexpr bool validate = And<_1, _2, _3, _4>::template validate<InPlaceType>;
};

template<class _1, class _2, bool _3, bool _4>
struct ValidateCondition<Or<_1, _2, _3, _4>>
{
    template<class InPlaceType>
    static constexpr bool validate = Or<_1, _2, _3, _4>::template validate<InPlaceType>;
};

template<class _1, class _2, bool _3, bool _4>
struct ValidateCondition<Xor<_1, _2, _3, _4>>
{
    template<class InPlaceType>
    static constexpr bool validate = Xor<_1, _2, _3, _4>::template validate<InPlaceType>;
};
}

/**
 *  A delegated condition that will be checked later instead of immediate validation.
 *  Predicates can be nested with jaut::And, jaut::Or and jaut::Xor, which allows multi-conditional evaluation.
 *
 *  Example predicate that evaluates to true if, and only if, the given type is either an integral
 *  or floating-point type:
 *  @code
 *  using FloatOrIntegral = jaut::Predicate<jaut::Or<std::is_integral<PType>, std::is_floating_point<PType>>>;
 *
 *  // Can be evaluated later one replacing the type PType with the desired type
 *  static_assert(FloatOrIntegral::validate<int>, "Not integral and not floating-point"); // does not assert
 *  static_assert(FloatOrIntegral::validate<double>, "Not integral and not floating-point"); // does not assert
 *  static_assert(FloatOrIntegral::validate<std::string>, "Not integral and not floating-point"); // will assert
 *  // Here, int, double and std::string will be inserted were PType was specified
 *  @endcode
 *
 *  @tparam Condition   A conditional type that must contain a bool variable named value to be evaluated
 *  @tparam EvaluatesTo If true, this Predicate will validate true if the condition was true,
 *                      otherwise will validate true if the condition was false
 */
template<class Condition, bool EvaluatesTo = true>
struct JAUT_API Predicate : std::bool_constant<EvaluatesTo == Condition::value>
{
    template<class InPlaceType>
    static constexpr bool validate =
        (EvaluatesTo == internal::ValidateCondition<Condition>::template validate<InPlaceType>);
};

/**
 *  A jaut::Predicate typedef that evaluates to true if the condition was false.
 *  @tparam Condition A conditional type that must contain a bool variable named value to be evaluated
 */
template<class Condition>
JAUT_API using NPredicate = Predicate<Condition, false>;

/**
 *  Collates two conditions and returns true if both conditions meet their requirements.
 *
 *  @tparam Condition1            The first condition to evaluate
 *  @tparam Condition2            The second condition to evaluate
 *  @tparam Condition1EvaluatesTo Whether the first condition should evaluate true if true or false
 *  @tparam Condition2EvaluatesTo Whether the second condition should evaluate true if true or false
 */
template<class Condition1, class Condition2, bool Condition1EvaluatesTo = true, bool Condition2EvaluatesTo = true>
struct JAUT_API And : std::bool_constant<Condition1EvaluatesTo == Condition1::value &&
                                         Condition2EvaluatesTo == Condition2::value>
{
    template<class InPlaceType>
    static constexpr bool validate =
           Condition1EvaluatesTo == internal::ValidateCondition<Condition1>::template validate<InPlaceType> &&
           Condition2EvaluatesTo == internal::ValidateCondition<Condition2>::template validate<InPlaceType>;
};

/**
 *  Collates two conditions and returns true if either one of the conditions meet their requirements.
 *
 *  @tparam Condition1            The first condition to evaluate
 *  @tparam Condition2            The second condition to evaluate
 *  @tparam Condition1EvaluatesTo Whether the first condition should evaluate true if true or false
 *  @tparam Condition2EvaluatesTo Whether the second condition should evaluate true if true or false
 */
template<class Condition1, class Condition2, bool Condition1EvaluatesTo = true, bool Condition2EvaluatesTo = true>
struct JAUT_API Or : std::bool_constant<Condition1EvaluatesTo == Condition1::value ||
                                        Condition2EvaluatesTo == Condition2::value>
{
    template<class InPlaceType>
    static constexpr bool validate =
           Condition1EvaluatesTo == internal::ValidateCondition<Condition1>::template validate<InPlaceType> ||
           Condition2EvaluatesTo == internal::ValidateCondition<Condition2>::template validate<InPlaceType>;
};

/**
 *  Collates two conditions and returns true if only one of the conditions meet their requirements.
 *
 *  @tparam Condition1            The first condition to evaluate
 *  @tparam Condition2            The second condition to evaluate
 *  @tparam Condition1EvaluatesTo Whether the first condition should evaluate true if true or false
 *  @tparam Condition2EvaluatesTo Whether the second condition should evaluate true if true or false
 */
template<class Condition1, class Condition2, bool Condition1EvaluatesTo = true, bool Condition2EvaluatesTo = true>
struct JAUT_API Xor : std::bool_constant<(Condition1EvaluatesTo == Condition1::value) !=
                                         (Condition2EvaluatesTo == Condition2::value)>
{
    template<class InPlaceType>
    static constexpr bool validate =
           (Condition1EvaluatesTo == internal::ValidateCondition<Condition1>::template validate<InPlaceType>) !=
           (Condition2EvaluatesTo == internal::ValidateCondition<Condition2>::template validate<InPlaceType>);
};
}
