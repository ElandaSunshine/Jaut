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
    @file   typecontainer.h
    @date   28, June 2020

    ===============================================================
 */

#pragma once

#include <variant>

namespace jaut
{
#ifndef JAUT_TYPE_TRAITS_TYPE_CONTAINER_FULL_SPEC
#   ifndef JAUT_TYPE_TRAITS_TYPE_CONTAINER_SIZE_TYPE
#       define JAUT_TYPE_TRAITS_TYPE_CONTAINER_SIZE_TYPE std::size_t
#   endif
/** Declares all the base types, type-containers will use internally. */
struct JAUT_API TypeContainerTraits
{
    /** The size type to use for the type-containers. */
    using SizeType = JAUT_TYPE_TRAITS_TYPE_CONTAINER_SIZE_TYPE;
};
#endif

#define JAUT_TYPE_ARRAY_GET_FOR_LOOP_TYPE(NAME, TYPE_VARIABLE) using NAME = typename decltype(TYPE_VARIABLE)::type

namespace internal
{
template<class T, T Value, T Start, T End>
struct JAUT_API OutOfBoundsAssert
{
    static_assert(Value >= Start && Value < End, "Index out of bounds");
};
}

/**
 *  Checks whether a given type-list contains a specific type.
 *
 *  @tparam Wrapper    The type-list to check
 *  @tparam TypeToFind The type to check for
 */
template<class Wrapper, class TypeToFind>
struct JAUT_API hasTemplateType;

template<template<class...> class Wrapper, class TypeToFind, class ...Args>
struct JAUT_API hasTemplateType<Wrapper<Args...>, TypeToFind> :
    std::bool_constant<(std::is_same_v<TypeToFind, Args> || ...)> {};

/**
 *  Checks whether a given type-list contains a specific type.
 *
 *  @tparam Wrapper    The type-list to check
 *  @tparam TypeToFind The type to check for
 *  @return True if Wrapper's type-list contains TypeToFind
 */
template<class Wrapper, class TypeToFind>
JAUT_API inline constexpr bool hasTemplateType_v = hasTemplateType<Wrapper, TypeToFind>::value;

/**
 *  Gets the amount of types in a type-list.
 *  @tparam Wrapper The type-list to check
 */
template<class Wrapper>
struct JAUT_API getTypesLength;

template<template<class...> class Wrapper, class ...Args>
struct JAUT_API getTypesLength<Wrapper<Args...>>
{
    static constexpr auto value = static_cast<TypeContainerTraits::SizeType>(sizeof...(Args));
};

/**
 *  Gets the amount of types in a type-list.
 *
 *  @tparam Wrapper The type-list to check
 *  @return The amount of types
 */
template<class Wrapper>
static constexpr TypeContainerTraits::SizeType getTypesLength_v = getTypesLength<Wrapper>::value;

/**
 *  Gets a type of a type-list at the specified index.
 *
 *  @tparam Wrapper The type-list to get the type from
 *  @tparam Index   The index of the type to fetch
 */
template<class Wrapper, TypeContainerTraits::SizeType Index>
struct JAUT_API getTypeAt;

template<template<class...> class Wrapper, TypeContainerTraits::SizeType Index, class ...Args>
struct JAUT_API getTypeAt<Wrapper<Args...>, Index>
{
private:
    template<auto SearchIndex, class First, class ...Others>
    struct TypeResolver
    {
        using type = std::conditional_t<
            SearchIndex == 0,
            First,
            typename TypeResolver<SearchIndex - 1, Others...>::type
        >;
    };
    
    template<auto SearchIndex, class First>
    struct TypeResolver<SearchIndex, First>
    {
        using type = std::conditional_t<
            SearchIndex == 0,
            First,
            internal::OutOfBoundsAssert<
                TypeContainerTraits::SizeType,
                SearchIndex, 0, 0
            >
        >;
    };

public:
    using type = typename TypeResolver<Index, Args...>::type;
};

/**
 *  Gets a type of a type-list at the specified index.
 *
 *  @tparam Wrapper The type-list to get the type from
 *  @tparam Index   The index of the type to fetch
 *  @return The type at that index
 */
template<class Wrapper, TypeContainerTraits::SizeType Index>
JAUT_API using getTypeAt_t = typename getTypeAt<Wrapper, Index>::type;

/**
 *  Gets the index of the specified type in a type-list.
 *
 *  @tparam Wrapper    The type-list to search
 *  @tparam TypeToFind The type to find the index for
 */
template<class Wrapper, class TypeToFind>
struct JAUT_API getIndexOf;

template<template<class...> class Wrapper, class TypeToFind, class ...Args>
struct JAUT_API getIndexOf<Wrapper<Args...>, TypeToFind>
{
private:
    template<TypeContainerTraits::SizeType Index, class ...Types>
    struct IndexResolver
    {
        static constexpr auto value = static_cast<TypeContainerTraits::SizeType>(-1);
    };
    
    template<TypeContainerTraits::SizeType Index, class First, class ...Others>
    struct IndexResolver<Index, First, Others...>
    {
        static constexpr auto value = std::conditional_t<
            std::is_same_v<First, TypeToFind>,
            std::integral_constant<TypeContainerTraits::SizeType , Index>,
            IndexResolver<Index + 1, Others...>
        >::value;
    };
    
    template<TypeContainerTraits::SizeType Index, class First>
    struct IndexResolver<Index, First>
    {
        static constexpr auto value = Index;
    };
    
public:
    static_assert(hasTemplateType_v<Wrapper<Args...>, TypeToFind>,
                  "TypeToFind is not a type of your Wrapper type type-list");
    
    //==================================================================================================================
    static constexpr auto value = IndexResolver<0, Args...>::value;
};

/**
 *  Gets the index of the specified type in a type-list.
 *
 *  @tparam Wrapper    The type-list to search
 *  @tparam TypeToFind The type to find the index for
 *  @return The index of TypeToFind
 */
template<class Wrapper, class TypeToFind>
JAUT_API inline constexpr auto getIndexOf_v = getIndexOf<Wrapper, TypeToFind>::value;

/**
 *  Removes a specified type from a type-list if it exists and gets the new version.
 *
 *  @tparam Wrapper       The type-list to modify
 *  @tparam TypesToRemove The types to remove
 */
template<class Wrapper, class ...TypesToRemove>
struct JAUT_API removeFromUnchecked;

template<template<class...> class Wrapper, class ...TypesToRemove, class ...Args>
struct JAUT_API removeFromUnchecked<Wrapper<Args...>, TypesToRemove...>
{
private:
    template<class Type>
    static constexpr bool isTypeAbsent = (!std::is_same_v<TypesToRemove, Type> && ...);
    
    template<class ...Types>
    struct InternalTypeList
    {
        template<class NewType>
        using getWith = InternalTypeList<Types..., NewType>;
        
        template<template<class...> class WrapperType>
        using makeType = WrapperType<Types...>;
    };
    
    template<class ListType, class ...ListArgs>
    struct RemoveResolver
    {
        using type = ListType;
    };
    
    template<class ListType, class First, class ...Others>
    struct RemoveResolver<ListType, First, Others...>
    {
        using type = typename RemoveResolver<
            std::conditional_t<
                isTypeAbsent<First>,
                typename ListType::template getWith<First>,
                ListType
            >,
            Others...
        >::type;
    };
    
    template<class ListType, class First>
    struct RemoveResolver<ListType, First>
    {
        using type = std::conditional_t<
            isTypeAbsent<First>,
            typename ListType::template getWith<First>,
            ListType
        >;
    };
    
public:
    using type = typename RemoveResolver<InternalTypeList<>, Args...>::type::template makeType<Wrapper>;
};

/**
 *  Removes a specified type from a type-list if it exists and gets the new version.
 *
 *  @tparam Wrapper       The type-list to modify
 *  @tparam TypesToRemove The types to remove
 *  @return The modified type-list
 */
template<class Wrapper, class ...TypesToRemove>
JAUT_API using removeFromUnchecked_t = typename removeFromUnchecked<Wrapper, TypesToRemove...>::type;

/**
 *  Removes a specified type from the type-list and gets the new version.
 *  Other than jaut::removeFromUnchecked, this won't compile if the type is not inside the type-list.
 *
 *  @tparam Wrapper       The type-list to modify
 *  @tparam TypesToRemove The types to remove
 */
template<class Wrapper, class ...TypesToRemove>
struct JAUT_API removeFrom;

template<template<class...> class Wrapper, class ...TypesToRemove, class ...Args>
struct JAUT_API removeFrom<Wrapper<Args...>, TypesToRemove...>
{
    static_assert((hasTemplateType_v<Wrapper<Args...>, TypesToRemove> && ...),
                  "TypesToRemove contains types that are not in the Wrapper type");

    using type = removeFromUnchecked_t<Wrapper<Args...>, TypesToRemove...>;
};

/**
 *  Removes a specified type from the type-list and gets the new version.
 *  Other than jaut::removeFromUnchecked, this won't compile if the type is not inside the type-list.
 *
 *  @tparam Wrapper       The type-list to modify
 *  @tparam TypesToRemove The types to remove
 *  @return The modified type-list
 */
template<class Wrapper, class ...TypesToRemove>
JAUT_API using removeFrom_t = typename removeFrom<Wrapper, TypesToRemove...>::type;

/**
 *  Removes a type from the type-list if a certain condition is met for that type.
 *  For more information on Predicates, refer to jaut::Predicate.
 *
 *  @tparam Wrapper   The type-list to modify
 *  @tparam Predicate The predicate which must be met
 *  @see jaut::Predicate
 */
template<class Wrapper, class Predicate>
struct JAUT_API removeIf;

template<template<class...>class Wrapper, class Condition, bool EvaluatesTo, class ...Args>
struct JAUT_API removeIf<Wrapper<Args...>, Predicate<Condition, EvaluatesTo>>
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
    
    template<class ListType, class ...ListArgs>
    struct RemoveResolver
    {
        using type = ListType;
    };
    
    template<class ListType, class First, class ...Others>
    struct RemoveResolver<ListType, First, Others...>
    {
        using type = typename RemoveResolver<
            std::conditional_t<
                Predicate<Condition, EvaluatesTo>::template validate<First>,
                ListType,
                typename ListType::template getWith<First>
            >,
            Others...
        >::type;
    };
    
    template<class ListType, class First>
    struct RemoveResolver<ListType, First>
    {
        using type = std::conditional_t<
            Predicate<Condition, EvaluatesTo>::template validate<First>,
            ListType,
            typename ListType::template getWith<First>
        >;
    };

public:
    using type = typename RemoveResolver<InternalTypeList<>, Args...>::type;
};

/**
 *  Removes a type from the type-list if a certain condition is met for that type.
 *  For more information on Predicates, refer to jaut::Predicate.
 *
 *  @tparam Wrapper   The type-list to modify
 *  @tparam Predicate The predicate which must be met
 *  @return The modified type-list
 *  @see jaut::Predicate
 */
template<class Wrapper, class Predicate>
using removeIf_t = typename removeIf<Wrapper, Predicate>::type;

/**
 *  Adds a type to a type-list if it isn't already contained.
 *
 *  @tparam Wrapper    The type-list to modify
 *  @tparam TypesToAdd The types to add
 */
template<class Wrapper, class ...TypesToAdd>
struct addIfAbsent;

template<template<class...> class Wrapper, class ...TypesToAdd, class ...Args>
struct addIfAbsent<Wrapper<Args...>, TypesToAdd...>
{
private:
    template<class Type>
    static constexpr bool isTypeAbsent = !hasTemplateType_v<Wrapper<Args...>, Type>;
    
    template<class ...Types>
    struct InternalTypeList
    {
        template<class NewType>
        using getWith = InternalTypeList<Types..., NewType>;
        
        template<template<class...> class WrapperType>
        using makeType = WrapperType<Types...>;
    };
    
    template<class ListType, class ...ListArgs>
    struct AddResolver
    {
        using type = ListType;
    };
    
    template<class ListType, class First, class ...Others>
    struct AddResolver<ListType, First, Others...>
    {
        using type = typename AddResolver<
            std::conditional_t<
                isTypeAbsent<First>,
                typename ListType::template getWith<First>,
                ListType
            >,
            Others...
        >::type;
    };
    
    template<class ListType, class First>
    struct AddResolver<ListType, First>
    {
        using type = std::conditional_t<
            isTypeAbsent<First>,
            typename ListType::template getWith<First>,
            ListType
        >;
    };
    
public:
    using type = typename AddResolver<InternalTypeList<Args...>, TypesToAdd...>::type;
};

/**
 *  Adds a type to a type-list if it isn't already contained.
 *
 *  @tparam Wrapper    The type-list to modify
 *  @tparam TypesToAdd The types to add
 *  @return The modified type-list
 */
template<class Wrapper, class ...TypesToAdd>
using addIfAbsent_t = typename addIfAbsent<Wrapper, TypesToAdd...>::type;

template<class...> struct TypeArray;

/**
 *  Converts any type-list to a jaut::TypeArray.
 *  @tparam Wrapper The type-list to convert
 */
template<class Wrapper>
struct JAUT_API toTypeArray;

template<template<class...> class Wrapper, class ...Args>
struct JAUT_API toTypeArray<Wrapper<Args...>>
{
    using type = TypeArray<Args...>;
};

/**
 *  Converts any type-list to a jaut::TypeArray.
 *
 *  @tparam Wrapper The type-list to convert
 *  @return The new jaut::TypeArray
 */
template<class Wrapper>
JAUT_API using toTypeArray_t = typename toTypeArray<Wrapper>::type;

//======================================================================================================================
/**
 *  A wrapper to contain a type to be acquired in for loops.
 *  @tparam Type The type to be contained.
 */
template<class Type>
struct TypeWrapper { using type = Type; };

namespace internal
{
template<class>
struct forEachResolve;

template<class ...Types>
struct forEachResolve<TypeArray<Types...>>
{
    template <class F, class T, class = T>
    struct isCastable : std::false_type {};
    
    template <class F, class T>
    struct isCastable<F, T, decltype(static_cast<T>(std::declval<F>()))> : std::true_type {};
    
    template<class Fn, class Return, class ...Args>
    static constexpr auto loop(Fn &&functor, Args &&...args)
        noexcept(noexcept((std::forward<Fn>(functor).template operator()<Types>(std::forward<Args>(args)...), ...)))
        -> std::conditional_t<not std::is_void_v<Return> && isCastable<Fn, Return>::value, Return, void>
    {
        (std::forward<Fn>(functor).template operator()<Types>(std::forward<Args>(args)...), ...);
    
        if constexpr (!std::is_void_v<Return>)
        {
            constexpr bool castable = isCastable<Fn, Return>::value;
            static_assert(castable, "Your functor does not specify a valid conversion operator");
            
            if constexpr (castable)
            {
                return static_cast<Return>(std::forward<Fn>(functor));
            }
        }
    }
};
}

//======================================================================================================================
/**
 *  The type to be returned by TypeArray<> operations that can't be made on an empty TypeArray.
 *  @tparam ... Any arguments, this is just for cases where arguments are mandatory, they have no effect otherwise
 */
template<class...>
struct NoType {};

/**
 *  A useful type-list utility that declares a lot of useful helper functions to work on and with a list of types
 *  directly and convenient.
 *
 *  @tparam Types The list of types
 */
template<class ...Types>
struct JAUT_API TypeArray
{
    /** Gets an empty TypeArray. */
    using clear = TypeArray<>;
    
    /**
     *  Creates a new type of the types of this TypeArray.
     *  @tparam Wrapper The class to adopt this' type-list
     */
    template<template<class...> class Wrapper>
    using to = Wrapper<Types...>;
    
    /** Returns an variant array with the types of this TypeArray. */
    using toArray = std::array<std::variant<Types...>, sizeof...(Types)>;
    
    /**
     *  Creates a new TypeArray from another type-list.
     *  @tparam Wrapper The type-list to adopt the types from
     */
    template<class Wrapper>
    using of = toTypeArray_t<Wrapper>;
    
    /**
     *  Gets the type at a specified index in this TypeArray.
     *  @tparam Index The index of the type to fetch
     */
    template<TypeContainerTraits::SizeType Index>
    using at = getTypeAt_t<TypeArray, Index>;
    
    /**
     *  Adds the specified types to this TypeArray and returns a new one.
     *  @tparam TypesToAdd The types to append to the end of the list
     */
    template<class ...TypesToAdd>
    using add = TypeArray<Types..., TypesToAdd...>;
    
    /**
     *  Adds the specified types, that do not exist already, to this TypeArray and returns a new one.
     *  @tparam TypesToAdd The types to append to the end of the list
     */
    template<class ...TypesToAdd>
    using addIfAbsent = addIfAbsent_t<TypeArray, TypesToAdd...>;
    
    /**
     *  Removes the specified types from this TypeArray and returns a new one.
     *  If TypesToRemove contains a type that is not inside this TypeArray, it won't compile.
     *
     *  @tparam TypesToRemove The types to remove
     */
    template<class ...TypesToRemove>
    using remove = removeFrom_t<TypeArray, TypesToRemove...>;
    
    /**
     *  Removes the specified types from this TypeArray and returns a new one.
     *  @tparam TypesToRemove The types to remove
     */
    template<class ...TypesToRemove>
    using removeUnchecked = removeFromUnchecked_t<TypeArray, TypesToRemove...>;
    
    /**
     *  Removes the specified types from this TypeArray that meet the specified condition and returns a new one.
     *  @tparam Predicate The condition to be met
     *  @see jaut::Predicate
     */
    template<class Predicate>
    using removeIf = removeIf_t<TypeArray, Predicate>;
    
    /** Gets the first type of this TypeArray. */
    using front = at<0>;
    
    /** Gets the last type of this TypeArray. */
    using back = at<sizeof...(Types) - 1>;
    
    //==================================================================================================================
    /**
     *  Gets whether this TypeArray and the specified TypeArray are equal.
     *  @tparam Other The TypeArray to check against
     */
    template<class Other>
    static constexpr bool equals = std::is_same_v<TypeArray, Other>;
    
    /**
     *  Gets whether the type-list of this TypeArray and the specified type-list are equal.
     *  @tparam Other The TypeArray to check against
     */
    template<class Other>
    static constexpr bool same = hasSameTypeList_v<TypeArray, Other>;
    
    /** Gets whether this TypeArray contains any types. */
    static constexpr bool isEmpty = false;
    
    /** Gets the amount of types in this TypeArray. */
    static constexpr TypeContainerTraits::SizeType size = static_cast<TypeContainerTraits::SizeType>(sizeof...(Types));
    
    /**
     *  Gets the index of the specified type in this TypeArray.
     *  @tparam TypeToSearch The type to find the index for
     */
    template<class TypeToSearch>
    static constexpr TypeContainerTraits::SizeType indexOf = getIndexOf_v<TypeArray, TypeToSearch>;
    
    /**
     *  Gets whether this TypeArray contains the specified type.
     *  @tparam TypeToFind The type to be found
     */
    template<class TypeToFind>
    static constexpr bool contains = hasTemplateType_v<TypeArray, TypeToFind>;
    
    //==================================================================================================================
    /**
     *  Calls a callback for each type contained in this TypeArray.
     *  The callback must be a functor:
     *  @code
     *  using MyTypeList = jaut::TypeArray<int, double, float>;
     *
     *  struct MyFunctor
     *  {
     *      template<class Type>
     *      void operator()(int test)
     *      {
     *          // Do stuff with Type
     *      }
     *  }
     *
     *  // Later in code
     *  MyTypeList::forEach<MyFunctor>(8);
     *  @endcode
     *
     *  @tparam Functor The functor to be called
     *  @tparam Args    Automatically deduced argument list that will be passed to the functor
     *  @param args The arguments that will be passed to the functor
     */
    template<class Functor, class Return = void, class ...Args>
    static constexpr auto forEach(Args &&...args)
        noexcept(noexcept(internal::forEachResolve<TypeArray>::template
                                                         loop<Functor, Return>(Functor(), std::forward<Args>(args)...)))
        -> decltype(internal::forEachResolve<TypeArray>::template
                                                          loop<Functor, Return>(Functor(), std::forward<Args>(args)...))
    {
        if constexpr (std::is_void_v<Return>)
        {
            internal::forEachResolve<TypeArray>::template loop<Functor, Return>(Functor(), std::forward<Args>(args)...);
        }
        else
        {
            return internal::forEachResolve<TypeArray>::template loop<Functor, Return>(Functor(),
                                                                                       std::forward<Args>(args)...);
        }
    }
    
    /**
     *  Calls a functor for each type contained in this TypeArray individually.
     *
     *  The functor must be a functor like:
     *  @code
     *  using MyTypeList = jaut::TypeArray<int, double, float>;
     *
     *  struct MyFunctor
     *  {
     *      template<class Type>
     *      void operator()(int test)
     *      {
     *          // Do stuff with Type
     *      }
     *  }
     *
     *  // Later in code
     *  MyTypeList::forEach(MyFunctor(), 8);
     *  @endcode
     *
     *  @tparam Functor The functor to be called
     *  @tparam Args    Automatically deduced argument list that will be passed to the functor
     *  @param functor The functor instance to call
     *  @param args    The arguments that will be passed to the functor
     */
    template<class Functor, class Return = void, class ...Args>
    static constexpr auto forEach(Functor &&functor, Args &&...args)
        noexcept(noexcept(internal::forEachResolve<TypeArray>::template
                                    loop<Functor, Return>(std::forward<Functor>(functor), std::forward<Args>(args)...)))
        -> decltype(internal::forEachResolve<TypeArray>::template
                                     loop<Functor, Return>(std::forward<Functor>(functor), std::forward<Args>(args)...))
    {
        if constexpr (std::is_void_v<Return>)
        {
            internal::forEachResolve<TypeArray>::template loop<Functor, Return>(std::forward<Functor>(functor),
                                                                                std::forward<Args>(args)...);
        }
        else
        {
            return internal::forEachResolve<TypeArray>::template loop<Functor, Return>(std::forward<Functor>(functor),
                                                                                       std::forward<Args>(args)...);
        }
    }
    
    /**
     *  Calls a functor exactly once with all types.
     *  Other than forEach, this does not call the functor individually for each single argument but rather
     *  all together which allows for parameter pack expansions.
     *
     *  The functor must be a functor like:
     *  @code
     *  using MyTypeList = jaut::TypeArray<int, double, float>;
     *
     *  struct MyFunctor
     *  {
     *      template<class ...Types>
     *      void operator()(int test)
     *      {
     *          // Do stuff with Types
     *      }
     *  }
     *
     *  // Later in code
     *  MyTypeList::forAll<MyFunctor>(8);
     *  @endcode
     *
     *  @tparam Functor The functor to be called
     *  @tparam Args    Automatically deduced argument list that will be passed to the functor
     *  @param args The arguments that will be passed to the functor
     *  @return The value that will be returned by the functor used
     */
    template<class Functor, class ...Args>
    static auto forAll(Args &&...args)
        noexcept(noexcept(Functor().template operator()<Types...>(std::forward<Args>(args)...)))
        -> decltype(Functor().template operator()<Types...>(std::forward<Args>(args)...))
    {
        return Functor().template operator()<Types...>(std::forward<Args>(args)...);
    }
    
    /**
     *  Calls a functor exactly once with all types.
     *  Other than forEach, this does not call the functor individually for each single argument but rather
     *  all together which allows for parameter pack expansions.
     *
     *  The functor must be a functor like:
     *  @code
     *  using MyTypeList = jaut::TypeArray<int, double, float>;
     *
     *  struct MyFunctor
     *  {
     *      template<class ...Types>
     *      void operator()(int test)
     *      {
     *          // Do stuff with Types
     *      }
     *  }
     *
     *  // Later in code
     *  MyTypeList::forAll(MyFunctor(), 8);
     *  @endcode
     *
     *  @tparam Functor The functor to be called
     *  @tparam Args    Automatically deduced argument list that will be passed to the functor
     *  @param functor The functor instance to call
     *  @param args    The arguments that will be passed to the functor
     *  @return The value that will be returned by the functor used
     */
    template<class Functor, class ...Args>
    static auto forAll(Functor &&functor, Args &&...args)
        noexcept(noexcept(std::forward<Functor>(functor).template operator()<Types...>(std::forward<Args>(args)...)))
        -> decltype(std::forward<Functor>(functor).template operator()<Types...>(std::forward<Args>(args)...))
    {
        return std::forward<Functor>(functor).template operator()<Types...>(std::forward<Args>(args)...);
    }
    
    /**
     *  Creates an std::array with an variant type and fills them in order the types appear.
     *  This only works if all types have at least one constructor that complies with the other types.
     *
     *  @tparam Args Automatically deduced argument list that will be passed to the functor
     *  @param args The constructor arguments each type should be constructed with
     *  @return The filled std::array
     */
    template<class ...Args>
    static toArray fillArray(Args &&...args)
    {
        return { to<std::variant>(std::in_place_type<Types>, std::forward<Args>(args)...)... };
    }
    
    /**
     *  Constructs a new object from the type at the specified index.
     *
     *  @tparam Index The index of the type to construct
     *  @tparam Args  The arguments types to parse to the constructor
     *  @param args The arguments to parse to the constructor
     *  @return The newly constructed object
     */
    template<TypeContainerTraits::SizeType Index, class ...Args>
    static at<Index> construct(Args &&...args)
    {
        return at<Index>(std::forward<Args>(args)...);
    }
    
    /**
     *  Allocates a new object from the type at the specified index.
     *
     *  @tparam Index The index of the type to construct
     *  @tparam Args  The arguments types to parse to the constructor
     *  @param args The arguments to parse to the constructor
     *  @return The newly allocated object
     */
    template<TypeContainerTraits::SizeType Index, class ...Args>
    static at<Index>* allocate(Args &&...args)
    {
        return new at<Index>(std::forward<Args>(args)...);
    }
    
    /**
     *  Creates a new std::unique_ptr from the type at the specified index.
     *
     *  @tparam Index The index of the type to construct
     *  @tparam Args  The arguments types to parse to the constructor
     *  @param args The arguments to parse to the constructor
     *  @return The std::unique_ptr object
     */
    template<TypeContainerTraits::SizeType Index, class ...Args>
    static std::unique_ptr<at<Index>> makeUnique(Args &&...args)
    {
        return std::make_unique<at<Index>>(std::forward<Args>(args)...);
    }
    
    /**
     *  Creates a new std::shared_ptr from the type at the specified index.
     *
     *  @tparam Index The index of the type to construct
     *  @tparam Args  The arguments types to parse to the constructor
     *  @param args The arguments to parse to the constructor
     *  @return The std::shared_ptr object
     */
    template<TypeContainerTraits::SizeType Index, class ...Args>
    static std::shared_ptr<at<Index>> makeShared(Args &&...args)
    {
        return std::make_shared<at<Index>>(std::forward<Args>(args)...);
    }
};

template<>
struct JAUT_API TypeArray<>
{
    using clear = TypeArray;
    template<template<class...> class Wrapper> using to = Wrapper<>;
    template<class Wrapper> using of = toTypeArray_t<Wrapper>;
    template<TypeContainerTraits::SizeType> using at = NoType<>;
    template<class ...TypesToAdd> using add = TypeArray<TypesToAdd...>;
    template<class ...TypesToAdd> using addIfAbsent = addIfAbsent_t<TypeArray, TypesToAdd...>;
    template<class...> using remove = TypeArray;
    template<class...> using removeUnchecked = TypeArray;
    template<class> using removeIf = TypeArray;
    using front = NoType<>;
    using back  = NoType<>;
    template<class Other> static constexpr bool equals = std::is_same_v<TypeArray, Other>;
    template<class Other> static constexpr bool same = hasSameTypeList_v<TypeArray, Other>;
    static constexpr bool isEmpty = true;
    static constexpr TypeContainerTraits::SizeType size = 0;
    template<class>
    static constexpr TypeContainerTraits::SizeType indexOf = static_cast<TypeContainerTraits::SizeType>(-1);
    template<class> static constexpr bool contains = false;
    template<class Functor, class ...Args> static constexpr void forEach(Args&&...) noexcept {}
    template<class Functor, class ...Args> static constexpr void forEach(Functor&&, Args&&...) noexcept {}
    template<class Functor, class ...Args> static NoType<> forAll(Args&&...) noexcept { return {}; }
    template<class Functor, class ...Args> static NoType<> forAll(Functor&&, Args&&...) noexcept { return {}; }
    template<class ...Args> static std::array<std::variant<NoType<>>, 0> fillArray(Args&&...) noexcept { return {}; }
    template<TypeContainerTraits::SizeType, class ...Args>
    static NoType<> construct(Args&&...) noexcept { return {}; }
    template<TypeContainerTraits::SizeType, class ...Args>
    static NoType<>* allocate(Args&&...) noexcept { return nullptr; }
    template<TypeContainerTraits::SizeType, class ...Args>
    static std::unique_ptr<NoType<>> makeUnique(Args&&...) noexcept { return nullptr; }
    template<TypeContainerTraits::SizeType, class ...Args>
    static std::shared_ptr<NoType<>> makeShared(Args&&...) noexcept { return nullptr; }
};
}
