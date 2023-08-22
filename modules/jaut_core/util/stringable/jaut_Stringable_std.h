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
    @file   jaut_Stringable_std.h
    @date   10, October 2022

    ===============================================================
 */
 
#pragma once

#include <forward_list>
#include <stack>
#include <string_view>
#include <type_traits>
#include <vector>



namespace jaut
{
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::basic_string_view<T, Ect...>>
    {
        JAUT_NODISCARD
        static juce::String toString(const std::basic_string_view<T, Ect...> &object)
        {
            return juce::String(object.data(), object.length());
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::basic_string_view<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, std::size_t N>
    struct JAUT_API Stringable<std::array<T, N>>
    {
        JAUT_NODISCARD
        static juce::String toString(const std::array<T, N> &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(static_cast<int>(object.size()));
        
            for (const T &obj : object)
            {
                elements.add(jaut::toString(obj));
            }
        
            return '[' + elements.joinIntoString(", ") + ']';
        }
    
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::array<" + jaut::toName<T>() + ", " + juce::var(N).toString() + '>';
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::vector<T, Ect...>> : public detail::SequencedImpl<std::vector, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            if constexpr (std::is_same_v<std::vector<T, Ect...>, std::vector<T>>)
            {
                return "std::vector<" + jaut::toName<T>() + ">";
            }
            else
            {
                return "std::vector<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
            }
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::deque<T, Ect...>> : public detail::SequencedImpl<std::deque, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            if constexpr (std::is_same_v<std::deque<T, Ect...>, std::deque<T>>)
            {
                return "std::deque<" + jaut::toName<T>() + ">";
            }
            else
            {
                return "std::deque<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
            }
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::list<T, Ect...>> : public detail::SequencedImpl<std::list, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            if constexpr (std::is_same_v<std::list<T, Ect...>, std::list<T>>)
            {
                return "std::list<" + jaut::toName<T>() + ">";
            }
            else
            {
                return "std::list<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
            }
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::set<T, Ect...>> : public detail::SequencedImpl<std::set, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::set<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::multiset<T, Ect...>> : public detail::SequencedImpl<std::multiset, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::multiset<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class U, class ...Ect>
    struct JAUT_API Stringable<std::map<T, U, Ect...>> : public detail::AssociativeImpl<std::map, T, U, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::map<" + jaut::toName<T>() + ", " + jaut::toName<U>()
                   + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class U, class ...Ect>
    struct JAUT_API Stringable<std::multimap<T, U, Ect...>>
        : public detail::AssociativeImpl<std::multimap, T, U, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::multimap<" + jaut::toName<T>() + ", " + jaut::toName<U>()
                       + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::unordered_set<T, Ect...>>
        : public detail::SequencedImpl<std::unordered_set, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::unordered_set<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<std::unordered_multiset<T, Ect...>>
        : public detail::SequencedImpl<std::unordered_multiset, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::unordered_multiset<" + jaut::toName<T>() + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class U, class ...Ect>
    struct JAUT_API Stringable<std::unordered_map<T, U, Ect...>>
        : public detail::AssociativeImpl<std::unordered_map, T, U, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
             return "std::unordered_map<" + jaut::toName<T>() + ", " + jaut::toName<U>()
                       + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<class T, class U, class ...Ect>
    struct JAUT_API Stringable<std::unordered_multimap<T, U, Ect...>>
        : public detail::AssociativeImpl<std::unordered_multimap, T, U, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::unordered_multimap<" + jaut::toName<T>() + ", " + jaut::toName<U>()
                       + ((", " + jaut::toName<Ect>()) + ...) + ">";
        }
    };
    
    template<>
    struct JAUT_API Stringable<std::thread::id>
    {
        JAUT_NODISCARD
        static juce::String toString(const std::thread::id &object)
        {
            std::stringstream ss;
            ss << object;
            
            return ss.str();
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "std::thread::thread_id";
        }
    };
}
