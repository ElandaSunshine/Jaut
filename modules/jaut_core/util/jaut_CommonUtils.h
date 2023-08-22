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
    @file   jaut_CommonUtils.h
    @date   09, May 2022

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_TypeContainer.h>

#include <stdexcept>

#ifdef __GNUG__
    #include <cxxabi.h>
#endif


#if JUCE_DEBUG
    /**
     *  Uses JUCE's jasserts to always assert false, but prints a message first.
     *  @param MSG The message to print before asserting
     */
    #define dbgassertfalse(MSG) { DBG(MSG); jassertfalse; }
    
    /**
     *  Uses JUCE's jasserts to assert false if the condition was not true, but prints a message first.
     *  
     *  @param COND The condition to check if it should assert
     *  @param MSG  The message to print before asserting
     */
    #define dbgassert(COND, MSG) if (!COND) dbgassertfalse(MSG)
#else
    /**
     *  Uses JUCE's jasserts to always assert false, but prints a message first.
     *  @param MSG The message to print before asserting
     */
    #define dbgassertfalse(MSG)
    
    /**
     *  Uses JUCE's jasserts to assert false if the condition was not true, but prints a message first.
     *  
     *  @param COND The condition to check if it should assert
     *  @param MSG  The message to print before asserting
     */
    #define dbgassert(COND, MSG)
#endif

namespace jaut
{
    //==================================================================================================================
    /** A dirty type previewer that will work in lots of IDEs. */
    template<class> struct TView;
    
    /** A dirty value previewer that will work in lots of IDEs. */
    template<auto> struct VView;
    
    /**
     *  A convenience type-wrapper for function pointers.
     *
     *  @tparam Ret   The return type
     *  @tparam Types The function arguments
     */
    template<class Ret, class ...Types>
    JAUT_API using FunctionPointer_t = Ret(*)(Types...);
    
    /**
     *  A convenience type-wrapper for pointer-to-member functions.
     *
     *  @tparam Obj   The object containing the non-static member function
     *  @tparam Ret   The return type
     *  @tparam Types The function arguments
     */
    template<class Obj, class Ret, class ...Types>
    JAUT_API using MemberFunctionPointer_t = Ret(Obj::*)(Types...);
    
    /**
     *  A convenience type-wrapper for pointer-to-member objects.
     *  
     *  @tparam Obj The object containing the non-static member object
     *  @tparam T   The object's type
     */
    template<class Obj, class T>
    JAUT_API using MemberDataPointer_t = T Obj::*;
    
    template<class ...Args>
    JAUT_API constexpr void ignore(Args &&...) {}
    
    //==================================================================================================================
    namespace detail
    {
        //==============================================================================================================
        template<class T>
        struct PointerResolver;
    
        template<class T>
        struct PointerResolver<T*>
        {
            using Type = T;
        };
        
        template<template<class...> class Wrapper, class T, class ...Misc>
        struct PointerResolver<Wrapper<T, Misc...>>
        {
            using Type = T;
        };
    }
    
    //==================================================================================================================
    #ifdef __GNUG__
        template<class T>
        JAUT_NODISCARD
        juce::String getActualTypeName(JAUT_MUNUSED T *const type)
        {
            jassert(type != nullptr);
            
            int                                   status {};
            std::unique_ptr<char, void(*)(void*)> name   {
                abi::__cxa_demangle(typeid(*type).name(), nullptr, nullptr, &status),
                std::free
            };
            
            return (!status ? name.get() : typeid(*type).name());
        }
        
        template<class T>
        JAUT_NODISCARD
        juce::String getActualTypeName()
        {
            int                                   status {};
            std::unique_ptr<char, void(*)(void*)> name   {
                abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status),
                std::free
            };
            
            return (!status ? name.get() : typeid(T).name());
        }
    #else
        template<class T>
        JAUT_NODISCARD
        juce::String getActualTypeName(JAUT_MUNUSED T *const type)
        {
            jassert(type != nullptr);
            return juce::String(typeid(*type).name()).fromFirstOccurrenceOf(" ", false, true);
        }
        
        template<class T>
        JAUT_NODISCARD
        juce::String getActualTypeName()
        {
            return juce::String(typeid(T).name()).fromFirstOccurrenceOf(" ", false, true);
        }
    #endif
    
    template<class T>
    JAUT_NODISCARD
    juce::String getActualTypeName(JAUT_MUNUSED T &type)
    {
        return getActualTypeName(&type);
    }
    
    //==================================================================================================================
    /**
     *  Allows to pass a pointer that should not be assigned nullptr.
     *
     *  Note that if pointers that were nullptr assigned prior to passing, this will make a runtime check
     *  and responds correspondingly depending on the mode that was set with JAUT_CORE_NONNULL_HANDLE_NULLPTRS.
     *
     *  <table>
     *      <caption>There is three possible approaches</caption>
     *      <tr>
     *          <th>Key</th>
     *          <th>Description</th>
     *      </tr>
     *      <tr>
     *          <td>Throwing: 0 (default)</td>
     *          <td>Throws if a nullptr was encountered</td>
     *      </tr>
     *      <tr>
     *          <td>Terminating: 1</td>
     *          <td>Terminates if a nullptr was encountered</td>
     *      </tr>
     *      <tr>
     *          <td>Ignoring: 2</td>
     *          <td>Ignores the nullptr and continues as though nothing has happened</td>
     *      </tr>
     *  </table>
     *
     *  Note that for smart pointers, this does not make sure they can't be set to nullptr upon deletion or moving,
     *  so you will need to handle these cases carefully yourself.
     *
     *  @tparam T
     */
    template<class T>
    class JAUT_API NonNull
    {
    public:
        /** The full type passed to the class. */
        using ObjectType = T;
        
        /** The content type passed to the class. */
        using ValueType = typename detail::PointerResolver<T>::Type;
        
        //==============================================================================================================
        NonNull(std::nullptr_t) = delete; // NOLINT
        
        template<class U, std::enable_if_t<std::is_constructible_v<ObjectType, U>>* = nullptr>
        constexpr NonNull(U &&obj) // NOLINT
            : data(std::forward<U>(obj))
        {
            validatePointer(this->data);
        }
        
        //==============================================================================================================
        NonNull& operator=(std::nullptr_t) = delete;
        
        template<class U, std::enable_if_t<std::is_convertible_v<U, ObjectType>>* = nullptr>
        constexpr NonNull& operator=(U &&obj)
        {
            data = std::forward<U>(obj);
            validatePointer(this->data);
            
            return *this;
        }
        
        //==============================================================================================================
        operator bool() = delete;
        
        //==============================================================================================================
        template<class Detail = T, std::enable_if_t<std::is_pointer_v<Detail>>* = nullptr>
        JAUT_NODISCARD
        constexpr ObjectType* get() const noexcept
        {
            return data;
        }
        
        template<class Detail = T, std::enable_if_t<!std::is_pointer_v<Detail>>* = nullptr>
        JAUT_NODISCARD
        constexpr ObjectType& get() & noexcept
        {
            return data;
        }
        
        template<class Detail = T, std::enable_if_t<!std::is_pointer_v<Detail>>* = nullptr>
        JAUT_NODISCARD
        constexpr const ObjectType& get() const & noexcept
        {
            return data;
        }
    
        template<class Detail = T, std::enable_if_t<!std::is_pointer_v<Detail>>* = nullptr>
        JAUT_NODISCARD
        constexpr ObjectType& get() &&
        {
            return std::move(data);
        }
    
        template<class Detail = T, std::enable_if_t<!std::is_pointer_v<Detail>>* = nullptr>
        JAUT_NODISCARD
        constexpr const ObjectType& get() const && noexcept
        {
            return data;
        }
        
        //==============================================================================================================
        template<class Detail = T>
        JAUT_NODISCARD
        explicit constexpr operator std::enable_if_t<std::is_pointer_v<Detail>, ObjectType*>() const noexcept
        {
            return data;
        }
    
        template<class Detail = T>
        JAUT_NODISCARD
        explicit constexpr operator std::enable_if_t<!std::is_pointer_v<Detail>, ObjectType&>() & noexcept
        {
            return data;
        }
    
        template<class Detail = T>
        JAUT_NODISCARD
        explicit constexpr operator std::enable_if_t<!std::is_pointer_v<Detail>, const ObjectType&>() const & noexcept
        {
            return data;
        }
    
        template<class Detail = T>
        JAUT_NODISCARD
        explicit constexpr operator std::enable_if_t<!std::is_pointer_v<Detail>, const ObjectType&>() const && noexcept
        {
            return data;
        }
    
        template<class Detail = T>
        JAUT_NODISCARD
        explicit constexpr operator std::enable_if_t<!std::is_pointer_v<Detail>, ObjectType&&>() &&
        {
            return std::move(data);
        }
        
        //==============================================================================================================
        JAUT_NODISCARD constexpr ValueType* operator->() const { return &*data; }
        JAUT_NODISCARD constexpr ValueType& operator*()  const { return *data;  }
        
    private:
        template<class U>
        static void validatePointer(const U &obj)
        {
            // Trying to assign a nullptr to a non-null object, huh?
            jassert(obj);
            
            if (obj == nullptr)
            {
                #if JAUT_CORE_NONNULL_HANDLE_NULLPTRS == 0
                    const juce::String object_name = getActualTypeName(&obj);
                    throw std::runtime_error("Object '" + object_name.toStdString()
                                             + "' marked jaut::NonNull has been determined nullptr");
                #elif JAUT_CORE_NONNULL_HANDLE_NULLPTRS == 1
                    std::terminate();
                #elif JAUT_CORE_NONNULL_HANDLE_NULLPTRS == 2
                    ignore(obj);
                #else
                    #error "Invalid JAUT_CORE_NONNULL_HANDLE_NULLPTRS mode, only 0, 1 or 2 are allowed"
                #endif
            }
        }
        
        //==============================================================================================================
        ObjectType data;
    };
    
    //==================================================================================================================
    template<class U>
    NonNull(NonNull<U>&) -> NonNull<U>;
    
    template<class U>
    NonNull(NonNull<U>&&) -> NonNull<U>;
    
    //==================================================================================================================
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline auto makeNonNull(T &&obj)
        noexcept(noexcept(NonNull(std::forward<T>(obj))))
        -> NonNull<T>
    {
        return std::forward<T>(obj);
    }
    
    //==================================================================================================================
    // Disambiguation helpers
    /** A type used for optional function overloads to instruct to allocate the object as std::unique_ptr. */
    struct unique_construct_t {};
    inline constexpr unique_construct_t unique_construct;
    
    /** A type used for optional function overloads to instruct to allocate the object as std::shared_ptr. */
    struct shared_construct_t {};
    inline constexpr unique_construct_t shared_construct;
    
    //==================================================================================================================
    namespace detail
    {
        template<class Sequence, int Next>
        struct addInteger;
        
        template<int Next, int ...Contained>
        struct addInteger<std::integer_sequence<int, Contained...>, Next>
        {
            using type = std::integer_sequence<int, Contained..., Next>;
        };
        
        template<class Predicate, class TList, int I, int Size, bool ShouldAdd, class FilterSeq, class ExcludeSeq,
                 bool Last>
        struct getIndices_impl;
        
        template<class Predicate, class TList, int I, bool ShouldAdd, class FilterSeq, class ExcludeSeq, bool Last>
        struct getIndices_impl<Predicate, TList, I, 0, ShouldAdd, FilterSeq, ExcludeSeq, Last>
        {
            using Filtered = FilterSeq;
            using Excluded = ExcludeSeq;
        };
        
        template<class Predicate, class TList, int I, int Size, class FilterSeq, class ExcludeSeq>
        struct getIndices_impl<Predicate, TList, I, Size, true, FilterSeq, ExcludeSeq, true>
        {
            using Filtered = typename addInteger<FilterSeq, Size - 1>::type;
            using Excluded = ExcludeSeq;
        };
        
        template<class Predicate, class TList, int I, int Size, class FilterSeq, class ExcludeSeq>
        struct getIndices_impl<Predicate, TList, I, Size, false, FilterSeq, ExcludeSeq, true>
        {
            using Filtered = FilterSeq;
            using Excluded = typename addInteger<ExcludeSeq, Size - 1>::type;
        };
        
        template<class Predicate, class TList, int I, int Size, class FilterSeq, class ExcludeSeq>
        struct getIndices_impl<Predicate, TList, I, Size, true, FilterSeq, ExcludeSeq, false>
            : getIndices_impl<
                  Predicate,
                  TList,
                  I + 1,
                  Size,
                  Predicate::template value<typename TList::template at<I + 1>>,
                  typename addInteger<FilterSeq, I>::type,
                  ExcludeSeq,
                  (I + 1 == Size - 1)
              >
        {};
        
        template<class Predicate, class TList, int I, int Size, class FilterSeq, class ExcludeSeq>
        struct getIndices_impl<Predicate, TList, I, Size, false, FilterSeq, ExcludeSeq, false>
            : getIndices_impl<
                  Predicate,
                  TList,
                  I + 1,
                  Size,
                  Predicate::template value<typename TList::template at<I + 1>>,
                  FilterSeq,
                  typename addInteger<ExcludeSeq, I>::type,
                  (I + 1 == Size - 1)
              >
        {};
        
        template<class Predicate, class ...Args>
        struct getIndices
            : getIndices_impl<
                  Predicate,
                  TypeArray<Args...>,
                  0,
                  sizeof ...(Args),
                  Predicate::template value<typename TypeArray<Args...>::template at<0>>,
                  std::integer_sequence<int>,
                  std::integer_sequence<int>,
                  (sizeof...(Args) == 1)
              >
        {};
        
        template<class Fn, class ...Args, int ...Indices, class TypeList = TypeArray<Args...>>
        inline constexpr decltype(auto) filterArguments(std::integer_sequence<int, Indices...>,
                                                        Fn                  &&invocable,
                                                        std::tuple<Args...> &&tuple)
            noexcept(noexcept(std::forward<Fn>(invocable)
                                  (std::forward<typename TypeList::template at<Indices>>(std::get<Indices>(tuple))...)))
        {
            return std::forward<Fn>(invocable)
                       (std::forward<typename TypeList::template at<Indices>>(std::get<Indices>(tuple))...);
        }
    }
    
    /**
     *  Provides a filter for variadic function templates, which allows, based on the given predicate, to filter
     *  out a set of arguments and process them whatever way needed.
     *  
     *  @tparam Predicate The predicate to filter out the arguments
     *  @tparam Args      The argument list of all arguments
     */
    template<class Predicate, class ...Args>
    struct ArgFilter
    {
    private:
        using FilterResult = detail::getIndices<Predicate, Args...>;
        
    public:
        //==============================================================================================================
        static_assert(isPredicate_v<Predicate>);
        
        //==============================================================================================================
        /** An std::integer_sequence containing all indices to the arguments that matched the predicate. */
        using Filtered = typename FilterResult::Filtered;
        
        /** An std::integer_sequence containing all indices to the arguments that did not match the predicate. */
        using Excluded = typename FilterResult::Excluded;
        
        //==============================================================================================================
        /**
         *  Filters a list of arguments based on the given jaut::Predicate and passes the filtered arguments to the
         *  given invocable.
         *  
         *  @param invocable The invocable that should be invoked with the filtered arguments
         *  @param args      The list of arguments to filter
         *  @return The returned value of the invocable
         */
        template<class Fn, class ...FArgs>
        static constexpr decltype(auto) invokeFiltered(Fn &&invocable, FArgs &&...args)
            noexcept(noexcept(detail::filterArguments(Filtered{}, std::forward<Fn>(invocable),
                                                      std::forward_as_tuple(std::forward<FArgs>(args)...))))
        {
            static_assert(TypeArray<Args...>::template same<TypeArray<FArgs...>>,
                          JAUT_ASSERT_ARG_FILTER_NOT_SAME_ARG_LIST);
            
            return detail::filterArguments(Filtered{}, std::forward<Fn>(invocable),
                                           std::forward_as_tuple(std::forward<FArgs>(args)...));
        }
        
        /**
         *  Filters a list of arguments based on the given jaut::Predicate and passes the excluded arguments to the
         *  given invocable.
         * 
         *  @param invocable The invocable that should be invoked with the filtered arguments
         *  @param args      The list of arguments to filter
         *  @return The returned value of the invocable
         */
        template<class Fn, class ...FArgs>
        static constexpr decltype(auto) invokeExcluded(Fn &&invocable, FArgs &&...args)
            noexcept(noexcept(detail::filterArguments(Excluded{}, std::forward<Fn>(invocable),
                                                      std::forward_as_tuple(std::forward<FArgs>(args)...))))
        {
            static_assert(TypeArray<Args...>::template same<TypeArray<FArgs...>>,
                          JAUT_ASSERT_ARG_FILTER_NOT_SAME_ARG_LIST);
            
            return detail::filterArguments(Excluded{}, std::forward<Fn>(invocable),
                                           std::forward_as_tuple(std::forward<FArgs>(args)...));
        }
    };
    
    //==================================================================================================================
    /**
     *  Tries to downcast a base class to a derived class without the overhead of "dynamic_cast", this should be used
     *  when you are 100% certain that a base class pointer will be an instance to a derived class.<br>
     *  If this assumption is false, in debug builds will assert.<br>
     *  This makes it possible for making sure a type is of a certain polymorphic type in debug builds, and when it is,
     *  remove this check in release builds.
     *  
     *  @tparam Derived The type to cast the base type to
     *  @param base The pointer to the base class
     *  @return The casted derived object
     */
    template<class Derived, class Base>
    JAUT_NODISCARD
    inline Derived stynamic_cast(Base *const base) noexcept
    {
        dbgassert((!dynamic_cast<Derived>(base)),
                  "Tried converting pointer-to-base of type '" << getActualTypeName<Base>()
                      << "' to pointer-to-derived of type '"   << getActualTypeName<Derived>() << "', "
                      "when it actually was of type '"         << getActualTypeName(base) << "'" << juce::newLine;)
        
        return static_cast<Derived>(base);
    }
    
    /**
     *  Tries to downcast a base class to a derived class without the overhead of "dynamic_cast", this should be used
     *  when you are 100% certain that a base class pointer will be an instance to a derived class.<br>
     *  If this assumption is false, in debug builds will assert.<br>
     *  This makes it possible for making sure a type is of a certain polymorphic type in debug builds, and when it is,
     *  remove this check in release builds.
     *  
     *  @tparam Derived The type to cast the base type to
     *  @param base The reference to the base class
     *  @return The casted derived object
     */
    template<class Derived, class Base>
    JAUT_NODISCARD
    inline Derived stynamic_cast(Base &base) noexcept
    {
        return stynamic_cast<Derived>(&base);
    }
    
    //==================================================================================================================
    /**
     *  Will assert in debug mode before throwing an exception, so as to that it's easier to find the root cause of a
     *  problem in your program.<br>
     *  The error will also be printed to the debug console first, to make sure you have a reasonable message to
     *  understand the issue at hand.
     *  
     *  @tparam Exception The exception class to throw
     *  
     *  @param errorBuilder A lambda returning a string,
     *                      this is so that the string has not to be created if no error occurs
     */
    template<class Exception>
    void throwAssertFalse(const juce::String &message)
    {         
        dbgassertfalse(message)
        throw Exception(message.toStdString());
    }
    
    /**
     *  Will assert in debug mode before throwing an exception when the condition holds true,
     *  so as to that it's easier to find the root cause of a problem in your program.<br>
     *  The error will also be printed to the debug console first, to make sure you have a reasonable message to
     *  understand the issue at hand.
     *  <br><br>
     *  The message will be passed by lambda, so that when the evaluation works in favour of not throwing, the string
     *  does not have to be unnecessarily created.
     *  
     *  @tparam Exception The exception class to throw
     *  
     *  @param mustBeTrue   The condition that must be true to avoid an assert throw
     *  @param errorBuilder A lambda returning a string
     */
    template<class Exception, class Fn>
    void throwAssert(bool mustBeTrue, Fn &&errorBuilder)
    {
        if (!mustBeTrue)
        {
            throwAssertFalse<Exception>(std::forward<Fn>(errorBuilder)());
        }
    }
}
