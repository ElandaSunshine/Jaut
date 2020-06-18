/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda
    @file   jautdef.h
    @date   02, March 2020

    ===============================================================
 */

#pragma once

/** JAUT exporter */
#undef JAUT_API

#if JUCE_MSVC
  #ifdef JAUT_DLL_BUILD
    #define JAUT_API __declspec(dllexport)
    #pragma warning (disable: 4251)
  #elif defined (JAUT_DLL)
    #define JAUT_API __declspec(dllimport)
    #pragma warning(disable: 4251)
  #endif
  #ifdef __INTEL_COMPILER
    #pragma warning(disable: 1125)
  #endif
#elif defined(JAUT_DLL) || defined(JAUT_DLL_BUILD)
  #define JAUT_API __attribute__((visibility("default")))
#endif

#ifndef JAUT_API
  #define JAUT_API
#endif

/** Fancy helper macro for debugging */
#undef JAUT_DEBUGGING
#undef JAUT_NDEBUGGING
#undef JAUT_DEBUGGING_OR

#ifdef JUCE_DEBUG
  #define JAUT_DEBUGGING(x) x
  #define JAUT_NDEBUGGING(x)
  #define JAUT_DEBUGGING_OR(x, y) x
#else
  #define JAUT_DEBUGGING(x)
  #define JAUT_NDEBUGGING(x) x
  #define JAUT_DEBUGGING_OR(x, y) y
#endif
