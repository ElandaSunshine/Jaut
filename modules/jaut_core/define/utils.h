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
    @file   utils.h
    @date   16, June 2020

    ===============================================================
 */
 
#pragma once

#define JAUT_DECLARE_AUTOMATIC_STD_ARRAY(NAME, ...) \
        std::array<std::decay_t<decltype(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__))>, \
                            JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__)> NAME { __VA_ARGS__ }

#ifndef JAUT_COLOUR_ID_START
#   define JAUT_COLOUR_ID_START 0x11100000
#endif

#if JUCE_DEBUG
    /**
     *  Creates an anti-leak handler that helps a global instance of a class with a leak detector to be destroyed
     *  before the leak detector is destroyed.
     *
     *  The issue with leak detectors is, that when it is part of a class that you have a global instance of,
     *  it will complain about a possible leak even though there might not even be one.
     *  This is due to the fact that statics are destroyed in order of initialisation, and while initialisation order
     *  of statics is not clear, it is pretty certain that your object's static will be created before
     *  the leak detector's counter.
     *
     *  The leak detector, upon destruction, then sees that there still is an object not destroyed, because your object
     *  was created first.
     *  That is due to lazy initialisation which the juce leak detector utilises to create its counter object.
     *
     *  To circumvent this, you can use this anti-leak handler, that, when you place it after your leak detector
     *  declaration, is safe to be initialised after your leak detector.
     *  Hence, you can use this to destroy your global objects by hand just before the leak detector is destroyed.
     */
    #define JAUT_CREATE_ANTILEAK_HANDLER(F)         \
        class AntiLeakHandler                       \
        {                                           \
        public:                                     \
            AntiLeakHandler() { createAntiLeak(); } \
        private:                                    \
            static void createAntiLeak()            \
            {                                       \
                struct AntiLeakWatcher              \
                {                                   \
                    ~AntiLeakWatcher() { F }        \
                };                                  \
                static AntiLeakWatcher watcher;     \
            }                                       \
        };
#else
    #define JAUT_CREATE_ANTILEAK_HANDLER(F)
#endif
    
namespace jaut
{
#if defined(_WIN32) || defined(WIN32)
inline constexpr char newLine = '\r\n';
#else
inline constexpr char newLine = '\n';
#endif

template<int Add>
inline constexpr int nextColourId = JAUT_COLOUR_ID_START + Add; // FUTURE next start 26

/** A dirty type previewer that will work in lots of IDEs. */
template<class> struct TView;

/** A dirty value previewer that will work in lots of IDEs. */
template<auto> struct VView;

struct JAUT_API SizeTypes
{
    using Array  = std::array        <void*, 0>    ::size_type;
    using Vector = std::vector       <void*>       ::size_type;
    using Map    = std::map          <void*, void*>::size_type;
    using UMap   = std::unordered_map<void*, void*>::size_type;
    using List   = std::list         <void*>       ::size_type;
    using Deque  = std::deque        <void*>       ::size_type;
};
}
