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
    @file   typecontainer.h
    @date   28, June 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_DefUtils.h>
#include <jaut_core/util/jaut_TypeEvaluator.h>
#include <jaut_core/util/jaut_TypeTraits.h>

#include <array>
#include <limits>
#include <memory>
#include <tuple>
#include <variant>



namespace jaut
{    
    template<class...> struct TypeArray;
    
    /**
     *  Converts any type-list to a jaut::TypeArray.
     *  @tparam T The type-list to convert
     */
    template<class T>
    struct JAUT_API toTypeArray;
    
    template<template<class...> class T, class ...Types>
    struct JAUT_API toTypeArray<T<Types...>>
    {
        using type = TypeArray<Types...>;
    };
    
    /**
     *  Converts any type-list to a jaut::TypeArray.
     *  @tparam T The type-list to convert
     */
    template<class T>
    JAUT_API using toTypeArray_t = typename toTypeArray<T>::type;
    
    //==================================================================================================================
    /**
     *  The type to be returned by TypeArray<> operations that can't be made on an empty TypeArray.
     *  @tparam ... Any arguments, this is just for cases where arguments are mandatory, they have no effect otherwise
     */
    struct EmptyType {};
    
    template<std::size_t I>
    struct TypeArrayIndex : std::integral_constant<std::size_t, I> {};
    
    //==================================================================================================================
    namespace detail
    {
        template<class>
        struct forEach_impl;
        
        template<class ...Types>
        struct forEach_impl<TypeArray<Types...>>
        {
            template<class Fn, class ...Args>
            static constexpr void loop(Fn &&func, Args &&...args)
                noexcept(noexcept((std::forward<Fn>(func)
                                       .template operator()<Types>(std::forward<Args>(args)...), ...)))
            {
                (std::forward<Fn>(func).template operator()<Types>(std::forward<Args>(args)...), ...);
            }
        };
        
        template<class Fn, std::size_t ...Indices>
        static constexpr void iterate(std::index_sequence<Indices...>, Fn &&func)
            noexcept(noexcept((std::forward<Fn>(func)(TypeArrayIndex<Indices>{}), ...)))
        {
            (std::forward<Fn>(func)(TypeArrayIndex<Indices>{}), ...);
        }
    }
    
    template<class T>
    struct JAUT_API LambdaType
    {
        using type = T;
    };
    
    /**
     *  A useful type-list utility that declares a lot of useful helper functions to work on and with a list of types
     *  directly and convenient.
     *
     *  @tparam Types The list of types
     */
    template<class ...Types>
    struct JAUT_API TypeArray
    {
        //==============================================================================================================
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
        
        /**
         *  Gets the amount of types in this TypeArray. 
         *  This is the same as jaut::TypeArray::length.
         */
        static constexpr std::size_t size = sizeof...(Types);

        /**
         *  Gets the amount of types in this TypeArray. 
         *  This is the same as jaut::TypeArray::size.
         */
        static constexpr std::size_t length = size;
        
        /** Gets whether this TypeArray contains any types. */
        static constexpr bool isEmpty = false;

        /**
         *  Gets the index of the specified type in this TypeArray.
         *  If the TypeArray is empty, this will return std::numeric_limits<std::size_t>::max().
         *  
         *  @tparam T The type to find the index for
         */
        template<class T>
        static constexpr std::size_t indexOf = getIndexOf_v<TypeArray, T>;
        
        /**
         *  Gets whether this TypeArray contains the specified type.
         *  @tparam T The type to be found
         */
        template<class T>
        static constexpr bool contains = hasTemplateType_v<TypeArray, T>;
        
        //==============================================================================================================
        /** Gets an empty TypeArray. */
        using clear = TypeArray<>;
        
        /**
         *  Creates a new type of the types of this TypeArray.
         *  @tparam T The class to adopt this' type-list
         */
        template<template<class...> class T>
        using to = T<Types...>;
        
        /**
         *  Creates a variant type with the types of this TypeArray.
         *  If the TypeArray is empty, the variant will be a std::monostate variant.
         */
        using toVariant = to<std::variant>;
        
        /**
         * Returns a variant array type with the types of this TypeArray.
         * @tparam N The size of the array
         */
        template<std::size_t N>
        using toArray = std::array<toVariant, N>;
        
        /**
         *  Creates a new TypeArray from another type-list.
         *  @tparam T The type-list to adopt the types from
         */
        template<class T>
        using of = toTypeArray_t<T>;
        
        /**
         *  Gets the type at a specified index in this TypeArray.
         *  @tparam I The index of the type to fetch
         */
        template<std::size_t I>
        using at = getTypeAt_t<TypeArray, I>;
        
        /**
         *  Adds the specified types to this TypeArray and returns a new one.
         *  @tparam Add The types to append to the end of the list
         */
        template<class ...Add>
        using add = TypeArray<Types..., Add...>;
        
        /**
         *  Prepends the specified types to this TypeArray and returns a new one.
         *  @tparam Prepend The types to prepend to the start of the list
         */
        template<class ...Prepend>
        using prepend = TypeArray<Prepend..., Types...>;
        
        /**
         *  Adds the specified types, that do not exist already, to this TypeArray and returns a new one.
         *  @tparam Add The types to append to the end of the list
         */
        template<class ...Add>
        using addIfAbsent = addIfAbsent_t<TypeArray, Add...>;
        
        /**
         *  Removes the specified types from this TypeArray and returns a new one.
         *  @tparam Remove The types to remove
         */
        template<class ...Remove>
        using remove = removeFrom_t<TypeArray, Remove...>;
        
        /**
         *  Removes the specified types from this TypeArray that meet the specified condition and returns a new one.
         *  
         *  @tparam Predicate The condition to be met
         *  @see jaut::Predicate
         */
        template<class Predicate>
        using removeIf = removeIf_t<TypeArray, Predicate>;
        
        /**
         *  Gets the first type of this TypeArray.
         *  If the TypeArray is empty, this will return jaut::EmptyType.
         */
        using front = at<0>;
        
        /**
         *  Gets the last type of this TypeArray.
         *  If the TypeArray is empty, this will return jaut::EmptyType.
         */
        using back = at<sizeof...(Types) - 1>;
        
        //==============================================================================================================
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
         *  @tparam Fn The functor to be called
         *  @param args The arguments that will be passed to the functor
         */
        template<class Fn, class ...Args>
        static constexpr void forEach(Args &&...args)
            noexcept(noexcept(detail::forEach_impl<TypeArray>::template loop<Fn>(Fn(), std::forward<Args>(args)...)))
        {
            (void) detail::forEach_impl<TypeArray>::template loop<Fn>(Fn(), std::forward<Args>(args)...);
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
         *  @tparam Fn The functor to be called
         *  @param func The functor instance to call
         *  @param args The arguments that will be passed to the functor
         */
        template<class Fn, class ...Args>
        static constexpr void forEach(Fn &&functor, Args &&...args)
            noexcept(noexcept(detail::forEach_impl<TypeArray>::template loop<Fn>(std::forward<Fn>(functor),
                                                                                 std::forward<Args>(args)...)))
        {
            detail::forEach_impl<TypeArray>::template loop<Fn>(std::forward<Fn>(functor), std::forward<Args>(args)...);
        }
    
        template<class Fn>
        static constexpr void iterate(Fn &&callback)
        {
            detail::template iterate(std::make_index_sequence<sizeof...(Types)>(), std::forward<Fn>(callback));
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
         *  @tparam Fn The functor to be called
         *  @param args The arguments that will be passed to the functor
         *  @return The value that will be returned by the functor used
         */
        template<class Fn, class ...Args>
        static constexpr auto forAll(Args &&...args)
            noexcept(noexcept(Fn().template operator()<Types...>(std::forward<Args>(args)...)))
            -> decltype(Fn().template operator()<Types...>(std::forward<Args>(args)...))
        {
            return Fn().template operator()<Types...>(std::forward<Args>(args)...);
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
         *  @tparam Fn The functor to be called
         *  @param functor The functor instance to call
         *  @param args    The arguments that will be passed to the functor
         *  @return The value that will be returned by the functor used
         */
        template<class Fn, class ...Args>
        static constexpr auto forAll(Fn &&func, Args &&...args)
            noexcept(noexcept(std::forward<Fn>(func).template operator()<Types...>(std::forward<Args>(args)...)))
            -> decltype(std::forward<Fn>(func).template operator()<Types...>(std::forward<Args>(args)...))
        {
            return std::forward<Fn>(func).template operator()<Types...>(std::forward<Args>(args)...);
        }
        
        /**
         *  Creates an std::array with an variant type and fills them in order the types appear.
         *  This only works if all types have at least one constructor that complies with the other types.
         *
         *  @param args The constructor arguments each type should be constructed with
         *  @return The filled std::array
         */
        template<class ...Args>
        static constexpr toArray<length> fillArray(Args &&...args)
        {
            return { toVariant(std::forward<Args>(args))... };
        }
        
        /**
         *  Constructs a new object from the type at the specified index.
         *
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The newly constructed object
         */
        template<std::size_t I, class ...Args>
        static constexpr at<I> construct(Args &&...args)
        {
            return at<I>(std::forward<Args>(args)...);
        }
        
        /**
         *  Allocates a new object from the type at the specified index.
         *
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The newly allocated object
         */
        template<std::size_t I, class Allocator = std::allocator<at<I>>, class ...Args>
        static at<I>* allocate(Args &&...args)
        {
            using Traits = std::allocator_traits<Allocator>;
            using ObjPtr = typename Traits::pointer;

            Allocator alloc;
            ObjPtr obj = Traits::allocate(alloc, 1);
            Traits::construct(alloc, obj, std::forward<Args>(args)...);
            
            return obj;
        }
        
        /**
         *  Creates a new std::unique_ptr from the type at the specified index.
         *  
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The std::unique_ptr object
         */
        template<std::size_t I, class ...Args>
        static std::unique_ptr<at<I>> makeUnique(Args &&...args)
        {
            return std::make_unique<at<I>>(std::forward<Args>(args)...);
        }
        
        /**
         *  Creates a new std::shared_ptr from the type at the specified index.
         *  
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The std::shared_ptr object
         */
        template<std::size_t I, class ...Args>
        static std::shared_ptr<at<I>> makeShared(Args &&...args)
        {
            return std::make_shared<at<I>>(std::forward<Args>(args)...);
        }
    };
    
    /**
     *  A useful type-list utility that declares a lot of useful helper functions to work on and with a list of types
     *  directly and convenient.
     *
     *  @tparam Types The list of types
     */
    template<>
    struct JAUT_API TypeArray<>
    {
        //==============================================================================================================
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
        
        /**
         *  Gets the amount of types in this TypeArray. 
         *  This is the same as jaut::TypeArray::length.
         */
        static constexpr std::size_t size = 0;

        /**
         *  Gets the amount of types in this TypeArray. 
         *  This is the same as jaut::TypeArray::size.
         */
        static constexpr std::size_t length = 0;
        
        /** Gets whether this TypeArray contains any types. */
        static constexpr bool isEmpty = true;

        /**
         *  Gets the index of the specified type in this TypeArray.
         *  If the TypeArray is empty, this will return std::numeric_limits<std::size_t>::max().
         *  
         *  @tparam T The type to find the index for
         */
        template<class T>
        static constexpr std::size_t indexOf = std::numeric_limits<std::size_t>::max();
        
        /**
         *  Gets whether this TypeArray contains the specified type.
         *  @tparam T The type to be found
         */
        template<class T>
        static constexpr bool contains = false;
        
        //==============================================================================================================
        /** Gets an empty TypeArray. */
        using clear = TypeArray;
        
        /**
         *  Creates a new type of the types of this TypeArray.
         *  @tparam T The class to adopt this' type-list
         */
        template<template<class...> class T>
        using to = T<>;
        
        /**
         *  Creates a variant type with the types of this TypeArray.
         *  If the TypeArray is empty, the variant will be a std::monostate variant.
         */
        using toVariant = std::variant<std::monostate>;
        
        /**
         * Returns a variant array type with the types of this TypeArray.
         * @tparam N The size of the array
         */
        template<std::size_t N>
        using toArray = std::array<toVariant, N>;
        
        /**
         *  Creates a new TypeArray from another type-list.
         *  @tparam T The type-list to adopt the types from
         */
        template<class T>
        using of = toTypeArray_t<T>;
        
        /**
         *  Gets the type at a specified index in this TypeArray.
         *  @tparam I The index of the type to fetch
         */
        template<std::size_t I>
        using at = getTypeAt_t<TypeArray, I>;
        
        /**
         *  Adds the specified types to this TypeArray and returns a new one.
         *  @tparam Add The types to append to the end of the list
         */
        template<class ...Add>
        using add = TypeArray<Add...>;
        
        /**
         *  Prepends the specified types to this TypeArray and returns a new one.
         *  @tparam Prepend The types to prepend to the start of the list
         */
        template<class ...Prepend>
        using prepend = TypeArray<Prepend...>;
        
        /**
         *  Adds the specified types, that do not exist already, to this TypeArray and returns a new one.
         *  @tparam Add The types to append to the end of the list
         */
        template<class ...Add>
        using addIfAbsent = addIfAbsent_t<TypeArray, Add...>;
        
        /**
         *  Removes the specified types from this TypeArray and returns a new one.
         *  @tparam Remove The types to remove
         */
        template<class ...Remove>
        using remove = TypeArray;
        
        /**
         *  Removes the specified types from this TypeArray that meet the specified condition and returns a new one.
         *  
         *  @tparam Predicate The condition to be met
         *  @see jaut::Predicate
         */
        template<class Predicate>
        using removeIf = TypeArray;
        
        /**
         *  Gets the first type of this TypeArray.
         *  If the TypeArray is empty, this will return jaut::EmptyType.
         */
        using front = EmptyType;
        
        /**
         *  Gets the last type of this TypeArray.
         *  If the TypeArray is empty, this will return jaut::EmptyType.
         */
        using back = EmptyType;
        
        //==============================================================================================================
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
         *  @tparam Fn The functor to be called
         *  @param args The arguments that will be passed to the functor
         */
        template<class Fn, class ...Args>
        static constexpr void forEach(Args &&...args) noexcept { ignore(args...); }
        
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
         *  @tparam Fn The functor to be called
         *  @param func The functor instance to call
         *  @param args The arguments that will be passed to the functor
         */
        template<class Fn, class ...Args>
        static constexpr void forEach(Fn &&functor, Args &&...args) noexcept { ignore(functor, args...); }
        
        template<class Fn>
        static constexpr void iterate(Fn &&callback) noexcept { ignore(callback); }
        
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
         *  @tparam Fn The functor to be called
         *  @param args The arguments that will be passed to the functor
         *  @return The value that will be returned by the functor used
         */
        template<class Fn, class ...Args>
        static constexpr EmptyType forAll(Args &&...args) noexcept 
        {
            ignore(args...);
            return {};
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
         *  @tparam Fn The functor to be called
         *  @param functor The functor instance to call
         *  @param args    The arguments that will be passed to the functor
         *  @return The value that will be returned by the functor used
         */
        template<class Fn, class ...Args>
        static constexpr EmptyType forAll(Fn &&func, Args &&...args) noexcept
        {
            ignore(func, args...);
            return {};
        }
        
        /**
         *  Creates an std::array with an variant type and fills them in order the types appear.
         *  This only works if all types have at least one constructor that complies with the other types.
         *
         *  @param args The constructor arguments each type should be constructed with
         *  @return The filled std::array
         */
        template<class ...Args>
        static constexpr toArray<length> fillArray(Args &&...args) noexcept
        {
            ignore(args...);
            return {};
        }
        
        /**
         *  Constructs a new object from the type at the specified index.
         *
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The newly constructed object
         */
        template<std::size_t I, class ...Args>
        static constexpr EmptyType construct(Args &&...args)
        {
            ignore(args...);
            return {};
        }
        
        /**
         *  Allocates a new object from the type at the specified index.
         *
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The newly allocated object
         */
        template<std::size_t I, class Allocator = std::allocator<EmptyType>, class ...Args>
        static EmptyType* allocate(Args &&...args)
        {
            ignore(args...);
            
            using Traits = std::allocator_traits<Allocator>;
            using ObjPtr = typename Traits::pointer;

            Allocator alloc;
            ObjPtr obj = Traits::allocate(alloc, 1);
            Traits::construct(alloc, obj);
            
            return obj;
        }
        
        /**
         *  Creates a new std::unique_ptr from the type at the specified index.
         *  
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The std::unique_ptr object
         */
        template<std::size_t I, class ...Args>
        static std::unique_ptr<EmptyType> makeUnique(Args &&...args)
        {
            ignore(args...);
            return std::make_unique<EmptyType>();
        }
        
        /**
         *  Creates a new std::shared_ptr from the type at the specified index.
         *  
         *  @tparam I The index of the type to construct
         *  @param args The arguments to parse to the constructor
         *  @return The std::shared_ptr object
         */
        template<std::size_t I, class ...Args>
        static std::shared_ptr<EmptyType> makeShared(Args &&...args)
        {
            ignore(args...);
            return std::make_shared<EmptyType>();
        }
    };
    
    //==================================================================================================================
    namespace detail
    {
        template<class Ret, class T, class ...Params>
        inline constexpr TypeArray<Ret, T> getReturnAndClass(Ret(T::*)(Params...)) noexcept { return {}; }
        
        template<class Ret, class ...Params>
        inline constexpr TypeArray<Ret> getReturnAndClass(Ret(*)(Params...)) noexcept { return {}; }
        
        template<bool IsNoexcept, class T, class Ret, class Params, auto Func, bool LV, bool RV, bool CN, bool VOL>
        struct BaseMemberFunction;
        
        template<bool IsNoexcept, class T, class Ret, class ...Params, auto Func, bool LV, bool RV, bool CN, bool VOL>
        struct BaseMemberFunction<IsNoexcept, T, Ret, TypeArray<Params...>, Func, LV, RV, CN, VOL>
        {
            /** A type list of the function's parameters. */
            using Parameter = TypeArray<Params...>;
            
            /** The return type of the function. */
            using Return = Ret;
            
            /** The class this is a member function of, or std::nullptr_t if it is not a member function. */
            using Parent = T;
            
            /** The function pointer type. */
            using Pointer = std::remove_reference_t<decltype(Func)>;
            
            //==========================================================================================================
            /** The number of parameters the function has. */
            static constexpr int parameterCount = sizeof...(Params);
            
            /** Gets whether this function pointer is a member function. */
            static constexpr bool isMemberFunction = true;
            
            /** Gets whether this function is marked non-throwing. */
            //static constexpr bool isNoexcept = IsNoexcept;
            
            /** 
             *  Gets whether this function is const-qualified,
             *  for non-member functions this will always return false.
             */
            static constexpr bool isConst = CN;
            
            /** 
             *  Gets whether this function is volatile-qualified,
             *  for non-member functions this will always return false.
             */
            static constexpr bool isVolatile = VOL;
            
            /** 
             *  Gets whether this function is LValue-qualified,
             *  for non-member functions this will always return false.
             */
            static constexpr bool isLValueQualified = LV;
            
            /** 
             *  Gets whether this function is RValue-qualified,
             *  for non-member functions this will always return false.
             */
            static constexpr bool isRValueQualified = RV;
            
            //==========================================================================================================
            /**
             *  Invokes the function pointer with the given arguments, if possible.
             *  If this is a free function pointer, the first argument will be ignored.
             *  
             *  @param object The object this function pointer is a member off
             *  @param args   The arguments to pass to the function pointer
             *  @return The result of the function call
             */
            template<class U, class ...Args>
            static constexpr auto invoke(U &&object, Args &&...args)
                noexcept(noexcept((std::forward<U>(object).*Func)(std::forward<Args>(args)...)))
                -> decltype((std::forward<U>(object).*Func)(std::forward<Args>(args)...))
            {
                return (std::forward<U>(object).*Func)(std::forward<Args>(args)...);
            }
            
            /**
             *  Invokes the function pointer with the given arguments, if possible.
             *  If this is a free function pointer, the first argument will be ignored.
             *  
             *  @param object A pointer to the object this function pointer is a member off
             *  @param args   The arguments to pass to the function pointer
             *  @return The result of the function call
             */
            template<class U, class ...Args, bool IsRval = RV, std::enable_if_t<!IsRval>* = nullptr>
            static constexpr auto invoke(U *object, Args &&...args)
                noexcept(noexcept((object->*Func)(std::forward<Args>(args)...)))
                -> decltype((object->*Func)(std::forward<Args>(args)...))
            {
                return (object->*Func)(std::forward<Args>(args)...);
            }
        };
    }
    
    //==================================================================================================================
    template<auto FunctionPointer, class = decltype(FunctionPointer)>
    class JAUT_API Function;
    
    //==================================================================================================================
    // Member
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...)>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, false, false, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) const>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, false, true, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, false, false, true>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile const>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, false, true, true>
    {};
    
    
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) &>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, true, false, false, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) const &>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, true, false, true, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile &>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, true, false, false, true>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile const &>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, true, false, true, true>
    {};
    
    
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) &&>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, true, false, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) const &&>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, true, true, false>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile &&>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, true, false, true>
    {};
    
    template<class T, class Ret, class ...Params, Ret(T::*Func)(Params...) volatile const &&>
    struct JAUT_API Function<Func>
        : detail::BaseMemberFunction<false, T, Ret, TypeArray<Params...>, Func, false, true, true, true>
    {};
    
    //==================================================================================================================
    // Non Member
    template<class Ret, class ...Params, Ret(*Func)(Params...)>
    struct JAUT_API Function<Func>
    {
        /** A type list of the function's parameters. */
        using Parameter = TypeArray<Params...>;
        
        /** The return type of the function. */
        using Return = Ret;
        
        /** The class this is a member function of, or std::nullptr_t if it is not a member function. */
        using Parent = std::nullptr_t;
        
        /** The function pointer type. */
        using Pointer = std::remove_reference_t<decltype(Func)>;
        
        //==============================================================================================================
        /** The number of parameters the function has. */
        static constexpr int parameterCount = sizeof...(Params);
        
        /** Gets whether this function pointer is a member function. */
        static constexpr bool isMemberFunction = false;
        
        /** Gets whether this function is marked non-throwing. */
        //static constexpr bool isNoexcept = IsNoexcept;
        
        /** Gets whether this function is const-qualified, for non-member functions this will always return false. */
        static constexpr bool isConst = false;
        
        /** Gets whether this function is volatile-qualified, for non-member functions this will always return false. */
        static constexpr bool isVolatile = false;
        
        /** Gets whether this function is LValue-qualified, for non-member functions this will always return false. */
        static constexpr bool isLValueQualified = false;
        
        /** Gets whether this function is RValue-qualified, for non-member functions this will always return false. */
        static constexpr bool isRValueQualified = false;
        
        
        //==============================================================================================================
        /**
         *  Invokes the function pointer with the given arguments, if possible.
         *  If this is a free function pointer, the first argument will be ignored.
         *  
         *  @param object The object this function pointer is a member off
         *  @param args   The arguments to pass to the function pointer
         *  @return The result of the function call
         */
        template<class U, class ...Args>
        static constexpr auto invoke(JAUT_MUNUSED U &&object, Args &&...args)
            noexcept(noexcept((Func)(std::forward<Args>(args)...)))
            -> decltype((Func)(std::forward<Args>(args)...))
        {
            return (Func)(std::forward<Args>(args)...);
        }
        
        /**
         *  Invokes the function pointer with the given arguments, if possible.
         *  If this is a free function pointer, the first argument will be ignored.
         *  
         *  @param object A pointer to the object this function pointer is a member off
         *  @param args   The arguments to pass to the function pointer
         *  @return The result of the function call
         */
        template<class U, class ...Args>
        static constexpr auto invoke(JAUT_MUNUSED U *object, std::nullptr_t, Args &&...args)
            noexcept(noexcept((Func)(std::forward<Args>(args)...)))
            -> decltype((Func)(std::forward<Args>(args)...))
        {
            return (Func)(std::forward<Args>(args)...);
        }
    };
    
    enum class QualifierRef
    {
        None,
        LValue,
        RValue
    };
    
    enum class QualifierCv
    {
        None,
        Const,
        Volatile,
        ConstVolatile
    };
    
    namespace detail
    {
        template<class T>
        struct GetMemberFunctionPtr;
        
        template<class Ret, class ...Params>
        struct GetMemberFunctionPtr<Ret(Params...)>
        {
            template<class T>
            using type = Ret(T::*)(Params...);
        };
        
        template<class FuncType>
        struct OverloadBase
        {
            /**
             *  Gets the pointer-to-member-function of the specific overload.
             *  
             *  @param func The pointer-to-member-function
             *  @return The overloaded pointer-to-member-function
             */
            template<class T>
            JAUT_NODISCARD static constexpr auto from(typename GetMemberFunctionPtr<FuncType>::template type<T> func) noexcept { return func; }
        };
    }
    
    /**
     *  Used to disambiguate function pointers that point to functions with one or more overloads.
     *  @tparam Signature The function signature (e.g. "void(int, double)")
     */
    template<class Signature>
    struct JAUT_API Overload;
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...)> : detail::OverloadBase<Ret(Params...)>
    {
        /**
         *  Gets the pointer of the specific overload.
         *  
         *  @param func The function pointer
         *  @return The overloaded function pointer
         */
        template<class T>
        JAUT_NODISCARD static constexpr auto from(Ret(*func)(Params...)) noexcept { return func; }
        
    };
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const> : detail::OverloadBase<Ret(Params...) const> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) volatile> : detail::OverloadBase<Ret(Params...) volatile> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const volatile> : detail::OverloadBase<Ret(Params...) const volatile> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) &> : detail::OverloadBase<Ret(Params...) &> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const &> : detail::OverloadBase<Ret(Params...) const &> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) volatile &> : detail::OverloadBase<Ret(Params...) volatile &> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const volatile &> : detail::OverloadBase<Ret(Params...) const volatile &> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) &&> : detail::OverloadBase<Ret(Params...) &&> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const &&> : detail::OverloadBase<Ret(Params...) const &&> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) volatile &&> : detail::OverloadBase<Ret(Params...) volatile &&> {};
    
    template<class Ret, class ...Params>
    struct JAUT_API Overload<Ret(Params...) const volatile &&> : detail::OverloadBase<Ret(Params...) const volatile &&> {};
}
