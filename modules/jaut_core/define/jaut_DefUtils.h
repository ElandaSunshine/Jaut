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
    @file   jaut_DefUtils.h
    @date   16, June 2020

    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_Define.h>

#define JAUT_DECLARE_AUTOMATIC_STD_ARRAY(NAME, ...) \
        std::array<std::decay_t<decltype(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__))>, \
                            JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__)> NAME { __VA_ARGS__ }

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

#if NDEBUG
    #define jdconst
#else
    #define jdconst const
#endif
