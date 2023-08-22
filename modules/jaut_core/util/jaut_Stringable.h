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
    @file   jaut_Stringable.h
    @date   09, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_core/util/jaut_TypeContainer.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    template<class T>
    JAUT_NODISCARD
    juce::String toString(T&);
    
    template<class T>
    JAUT_NODISCARD
    juce::String toString(const T&);
    
    template<class T>
    JAUT_NODISCARD
    juce::String toString();
    
    template<class T>
    JAUT_NODISCARD
    juce::String toName();
    
    template<class T>
    JAUT_NODISCARD
    juce::String toName(const T&);
    
    //==================================================================================================================
    namespace detail
    {
        template<class T>
        struct MissingTypeNameProxy {};
        
        template<class T>
        struct MissingObjectProxy {};
        
        JAUT_UTIL_TYPE_TRAITS_DEFINE_METHOD_CHECK(name)
        
        template<template<class...> class C, class T, class ...Ect>
        struct SequencedImpl
        {
            JAUT_NODISCARD
            static juce::String toString(const C<T, Ect...> &object)
            {
                juce::StringArray elements;
                elements.ensureStorageAllocated(static_cast<int>(object.size()));
                
                for (const T &obj : object)
                {
                    elements.add(jaut::toString(obj));
                }
                
                return '[' + elements.joinIntoString(", ") + ']';
            }
        };
        
        template<template<class...> class C, class T, class U, class ...Ect>
        struct AssociativeImpl
        {
            JAUT_NODISCARD
            static juce::String toString(const C<T, U, Ect...> &object)
            {
                juce::StringArray elements;
                elements.ensureStorageAllocated(static_cast<int>(object.size()));
                
                for (const auto &[key, val] : object)
                {
                    elements.add(jaut::toString(key) + "=" + jaut::toString(val));
                }
                
                return '{' + elements.joinIntoString(", ") + '}';
            }
        };
        
        //==============================================================================================================
        template<class T>
        JAUT_NODISCARD
        inline juce::String addressToString(const T *ptr)
        {
            std::stringstream ss;
            ss << std::hex << ptr;
            return ss.str();
        }
    }
    
    //==================================================================================================================
    /**
     *  Defines a customisable template specialisation provider that allows you to convert objects to juce::String.
     *  <br><br>
     *  To define your own object you need to either specialise this template for your object
     *  and a corresponding toString(const Object&) static function or have a toString() method inside your object which
     *  returns a to juce::String convertible type.
     *  <br><br>
     *  You will only need to specialise your own type if the type is not implicitly convertible to
     *  juce::var or juce::String.<br>
     *  If no other way of representing this object was found, it will print the name of the class and its address in
     *  memory. (ns::MyClass@0x0000...)
     *  <br><br>
     *  Additionally, you can define your own way how type names should be serialised with a simple static name()
     *  function which returns a string with the name.<br>
     *  Otherwise it will try to derive the type name with jaut::getActualTypeName() which will try to get the name
     *  from its typeid.
     *  <br><br>
     *  The major intent behind this class is debugging and logging, but you can use it for anything you like.<br>
     *  Just note that, for the most part, this does not return a representation that is usable for JSON or any other
     *  parser, but instead just a visual guideline helping in printing out the most important parts of an object.<br>
     *  This is heavily inspired by the way Java handles these things.
     *  
     *  @tparam T The object type to convert to juce::String
     */
    template<class T>
    struct JAUT_API Stringable
    {
        JAUT_NODISCARD
        static juce::String toString(const T &object)
        {
            using Type = std::decay_t<T>;
            
            if constexpr (std::is_constructible_v<juce::var, T>)
            {
                return jaut::toString(juce::var(object));
            }
            else if constexpr (std::is_constructible_v<juce::String, T>)
            {
                return juce::String(object);
            }
            else if constexpr (std::is_base_of_v<std::exception, Type> || std::is_same_v<Type, std::exception>)
            {
                return juce::String(object.what());
            }
            else
            {
                return name() + "@" + jaut::toString(std::addressof(object));
            }
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return getActualTypeName<T>();
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<detail::MissingTypeNameProxy<T>>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return getActualTypeName<T>();
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<detail::MissingObjectProxy<T>>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return getActualTypeName<T>();
        }
    };
}

//======================================================================================================================
// CATEGORISED SPECIALISATIONS
#include <jaut_core/util/stringable/jaut_Stringable_cpp.h>
#include <jaut_core/util/stringable/jaut_Stringable_std.h>
#include <jaut_core/util/stringable/jaut_Stringable_juce_core.h>

//======================================================================================================================
namespace jaut
{
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline juce::String toString(T &object)
    {
        return Stringable<std::decay_t<T>>::toString(object);
    }
    
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline juce::String toString(const T &object)
    {
        return Stringable<std::decay_t<T>>::toString(object);
    }
    
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline juce::String toString()
    {
        return Stringable<std::decay_t<T>>::toString();
    }
    
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline juce::String toName()
    {
        using Type = std::decay_t<T>;
        
        if constexpr (detail::has_name<Type>::value)
        {
            return Stringable<Type>::name();
        }
        else
        {
            return Stringable<detail::MissingTypeNameProxy<Type>>::name();
        }
    }
    
    template<class T>
    JAUT_NODISCARD
    JAUT_API inline juce::String toName(const T&)
    {
        return toName<T>();
    }
}
