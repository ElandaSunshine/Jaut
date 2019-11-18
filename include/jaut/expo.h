/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: jaut.h
 * Time: 4, Mai 2019
 *
 * ===============================================================
 */

#pragma once

#include <JuceHeader.h>

/**********************************************************************************************************************
             DON'T TOUCH BEGIN
 **********************************************************************************************************************/
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
// BEGIN: EXPORT

#undef JAUT_API

#if JUCE_MSVC
  #ifdef JAUT_DLL_BUILD
    #define JAUT_API __declspec (dllexport)
    #pragma warning (disable: 4251)
  #elif defined (JAUT_DLL)
    #define JAUT_API __declspec (dllimport)
    #pragma warning (disable: 4251)
  #endif
  #ifdef __INTEL_COMPILER
    #pragma warning (disable: 1125)
  #endif
#elif defined (JAUT_DLL) || defined (JAUT_DLL_BUILD)
  #define JAUT_API __attribute__ ((visibility("default")))
#endif

//=====================================================================================================================
#ifndef JAUT_API
  #define JAUT_API
#endif

// END: EXPORT
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================


//=====================================================================================================================
// MACRO TOOLS
#ifdef JUCE_DEBUG
  #define JT_DEBUGGING(x) x
  #define JT_NDEBUGGING(x)
  #define JT_DEBUGGING_OR(x, y) x
#else
  #define JT_DEBUGGING(x)
  #define JT_NDEBUGGING(x) x
  #define JT_DEBUGGING_OR(x, y) y
#endif

/**
    For lazy programmers to avoid implicit conversion warnings.
    Casts any numeral type to float.
 */
#define JT_FFIX(x) static_cast<float>(x)

/**
    For lazy programmers to avoid implicit conversion warnings.
    Casts any numeral type to double.
*/
#define JT_DFIX(x) static_cast<double>(x)

/**
    For lazy programmers to avoid implicit conversion warnings.
    Casts any numeral type to int.
*/
#define JT_IFIX(x) static_cast<int>(x)

//=====================================================================================================================
// Convenience typedefs
typedef unsigned int ErrInt;

/**********************************************************************************************************************
             DON'T TOUCH END
 **********************************************************************************************************************/

namespace jaut
{
namespace settings
{
//=====================================================================================================================
// GENERAL LIBRARY SETTINGS

/**
    Determines whether strict mode should be enabled or not.
    This enables strict mode for certain situations which you can individually turn on/off in section 1.

    Default: true
 */
constexpr bool IS_STRICT_ON = 1;

//=====================================================================================================================
// SECTION 1: STRICT MODE SETTINGS

/**
    Should library classes force the user to use in-built interfaces to function correctly?
    The reason for this is that some classes need certain functions to work and those interface classes provide all
    needed methods.

    Default: true
 */
constexpr bool STRICT_TEMPLATES = 1;

/**
    If this is true, then the parser implementing this flag will be more strict parsing config files.

    For example:
        XmlParser will stop parsing if the file contains invalid tags if this is true, else it will skip those tags
        and go on!

    Default: false
 */
constexpr bool STRICT_CONFIG_PARSING = 0;

/**
    If this is true, when a processor is set to double precision processing and a float buffer is passed or vice-versa, an assertion will be thrown.

    If you don't mind losing data or wasting more memory than needed, turn this option off!

    Default: false
  */
constexpr bool STRICT_PROCESSING = 1;

/**
    If this is true, when the message thread tries to call a function not supposed to be called from the message thread,
    this will throw an assert.

    Default: 1
  */
constexpr bool STRICT_THREAD_DISTINCTION = 1;

//=====================================================================================================================
// SECTION 2: DEFAULT SETTINGS


}
//=====================================================================================================================
// BEGIN INTERNAL (DON'T TOUCH THIS SECTION)
//=====================================================================================================================
namespace internal
{
constexpr bool JAUT_STRICT(const int strictSetting = 2) noexcept
{
    return JT_DEBUGGING_OR(settings::IS_STRICT_ON && (strictSetting < 2 ? strictSetting : 1), 0);
}

constexpr bool STRICT_TEMPLATES_ENABLED() noexcept
{
    return JAUT_STRICT(settings::STRICT_TEMPLATES);
}

constexpr bool STRICT_CONFIG_PARSING_ENABLED() noexcept
{
    return JAUT_STRICT(settings::STRICT_CONFIG_PARSING);
}

constexpr bool STRICT_PROCESSING_ENABLED() noexcept
{
    return JAUT_STRICT(settings::STRICT_PROCESSING);
}

extern bool INTERNAL_DISABLE_THREAD_DIST;
extern bool EXPLICIT_DISABILITY;
}
//=====================================================================================================================
//  END INTERNAL
//=====================================================================================================================

/**
    This will disable the JAUT_ENSURE_AUDIO_THREAD() check for once and reset it again when such an event has occured.
    Beware to only use this when you really know what you're doing, as calling audio thread code on the gui without
    proper synchronisation will most of the time go wrong.
 */
inline void JAUT_DISABLE_THREAD_DIST() noexcept
{
    JT_NDEBUGGING(return);

    if (!internal::EXPLICIT_DISABILITY)
    {
        internal::INTERNAL_DISABLE_THREAD_DIST = true;
    }
}

/**
    This will disable the JAUT_ENSURE_AUDIO_THREAD() check for once and reset it again when such an event has occured.
    Beware to only use this when you really know what you're doing, as calling audio thread code on the gui without
    proper synchronization will most of the time go wrong.
 */
inline void JAUT_DISABLE_THREAD_DIST_EXPLICIT(const bool disable) noexcept
{
    JT_NDEBUGGING(return);

    internal::EXPLICIT_DISABILITY = disable;
    internal::INTERNAL_DISABLE_THREAD_DIST = false;
}

/**
    This makes sure that certain functions are only called on the audio thread.
    If a particular function which is not supposed to be called on the message thread is called there,
    then this will throw an assert.

    Also be aware that this will only make sure that a function can't be called on the message thread,
    if your implementation includes another thread besides the main event-dispatch loop and the audio thread,
    this will not throw if you call it from there!

    You can also temporarily disable this with JAUT_DISABLE_THREAD_DIST() if you want to live on the bloody edge.
 */
inline void JAUT_ENSURE_AUDIO_THREAD()
{
    JT_NDEBUGGING(return);

    const bool flag = !internal::INTERNAL_DISABLE_THREAD_DIST && !internal::EXPLICIT_DISABILITY;

    if (internal::JAUT_STRICT(settings::STRICT_THREAD_DISTINCTION) && flag)
    {
        jassert(!MessageManager::getInstance()->isThisTheMessageThread());
    }

    internal::INTERNAL_DISABLE_THREAD_DIST = false;
}

struct ScopedATCD final
{
    ScopedATCD() noexcept
    {
        JAUT_DISABLE_THREAD_DIST_EXPLICIT(true);
    }

    ~ScopedATCD() noexcept
    {
        JAUT_DISABLE_THREAD_DIST_EXPLICIT(false);
    }
};

//=====================================================================================================================
// LIBRARY ERROR CODES (currently unused)
constexpr ErrInt ERROR_TERMINATE             = 0x0000;
constexpr ErrInt ERROR_SUCCESS               = 0x0001;
constexpr ErrInt ERROR_INTERNAL_ISSUE        = 0x0002;
constexpr ErrInt ERROR_FILE_MISSING          = 0x0003;
constexpr ErrInt ERROR_CONFIG_INVALID_FORMAT = 0x0064;
constexpr ErrInt ERROR_CONFIG_INTERNAL       = 0x0065;
}
