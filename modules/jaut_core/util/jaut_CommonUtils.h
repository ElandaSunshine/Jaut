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

    @author Elanda (elanda@elandasunshine.com)
    @file   jaut_CommonUtils.h
    @date   09, May 2022

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_TypeTraits.h>

#include <stdexcept>

namespace jaut
{
    //==================================================================================================================
    #if defined(_WIN32) || defined(WIN32)
    inline constexpr char newLine = '\r\n';
    #else
    inline constexpr char newLine = '\n';
    #endif
    
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
    /**
     *  Allows to pass a pointer that should not be assigned nullptr.
     *
     *  Note that if pointers that were nullptr assigned prior to passing, this will make a runtime check
     *  and responds correspondingly depending on the mode that was set with JAUT_CORE_NOTNULL_HANDLE_NULLPTRS.
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
        
        template<class U, class = std::enable_if_t<std::is_constructible_v<ObjectType, U>>>
        constexpr NonNull(U &&obj) // NOLINT
            : data(std::forward<U>(obj))
        {
            validatePointer(this->data);
        }
        
        //==============================================================================================================
        NonNull& operator=(std::nullptr_t) = delete;
        
        template<class U, class = std::enable_if_t<std::is_convertible_v<U, ObjectType>>>
        constexpr NonNull& operator=(U &&obj)
        {
            data = std::forward<U>(obj);
            validatePointer(this->data);
            return *this;
        }
        
        //==============================================================================================================
        operator bool() = delete;
        
        //==============================================================================================================
        template<class Detail = T>
        JAUT_NODISCARD
        constexpr auto get() const noexcept
            -> std::enable_if_t<std::is_pointer_v<Detail>, ObjectType*>
        {
            return data;
        }
        
        template<class Detail = T>
        JAUT_NODISCARD
        constexpr auto get() & noexcept
            -> std::enable_if_t<!std::is_pointer_v<Detail>, ObjectType&>
        {
            return data;
        }
        
        template<class Detail = T>
        JAUT_NODISCARD
        constexpr auto get() const & noexcept
            -> std::enable_if_t<!std::is_pointer_v<Detail>, const ObjectType&>
        {
            return data;
        }
        
        template<class Detail = T>
        JAUT_NODISCARD
        constexpr auto get() const && noexcept
            -> std::enable_if_t<!std::is_pointer_v<Detail>, const ObjectType&>
        {
            return data;
        }
    
        template<class Detail = T>
        JAUT_NODISCARD
        constexpr auto get() &&
            -> std::enable_if_t<!std::is_pointer_v<Detail>, ObjectType&&>
        {
            return std::move(data);
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
            if (obj == nullptr)
            {
                #if JAUT_CORE_NOTNULL_HANDLE_NULLPTRS == 0
                throw std::runtime_error("Object marked jaut::notNull has been determined nullptr");
                #elif JAUT_CORE_NOTNULL_HANDLE_NULLPTRS == 1
                std::terminate();
                #elif JAUT_CORE_NOTNULL_HANDLE_NULLPTRS == 2
                ignore(obj);
                #else
                    #error "Invalid JAUT_CORE_NOTNULL_HANDLE_NULLPTRS mode, only 0, 1 or 2 are allowed"
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
    JAUT_API inline auto makeNonNull(T &&obj) noexcept(noexcept(NonNull(std::forward<T>(obj))))
    {
        return NonNull(std::forward<T>(obj));
    }
}
