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
    @file   jaut_Stringable_cpp.h
    @date   10, October 2022

    ===============================================================
 */
 
#pragma once



namespace jaut
{
    template<>
    struct JAUT_API Stringable<bool>
    {
        JAUT_NODISCARD
        static juce::String toString(bool object)
        {
            return (object ? "true" : "false");
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "bool";
        }
    };
    
    template<>
    struct JAUT_API Stringable<std::nullptr_t>
    {
        JAUT_NODISCARD
        static juce::String toString(std::nullptr_t)
        {
            return "null";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::nullptr_t";
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<T*>
    {
        JAUT_NODISCARD
        static juce::String toString(const T *object)
        {
            return (object ? detail::addressToString(object) : "null");
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return jaut::toName<T>() + '*';
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<T*&> : private Stringable<T*>
    {
        using Stringable<T*>::toString;
        using Stringable<T*>::name;
    };
}
