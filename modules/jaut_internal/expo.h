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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   expo.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  This is the jaut export header.
 *  This header file defines the export macro as well as useful utilities and internal settings.
 *
 *  You may change any of these, but be aware that changing something you shouldn't may break internal stuff.
 *  So better only alter sections explicitly stating you can do so to stay on the safe site!
 *
 *  Below you will find following sections (Be free to use any of these in your own project):
 *  - Export:      This section defines the jaut export macro for dlls on Windows based operating systems.
 *                 This macro can also be used to make sure that jaut is actually included in your project.
 *  - Macros:      This section defines several useful macros used in the jaut library which can as well be
 *                 used in your project.
 *                 Just be sure you know what you are doing!
 *  - Constants:   This section defines all constants for functions in the library which can take several modes
 *                 much like the OpenGL library system does it.
 *  - Typedefs:    These typedefs are just convenience typedefs to better describe certain types for a
 *                 particular context.
 *                 You may or may not use them in your project!
 *  - Settings:    The settings section defines various functionality parameters which direct the jaut
 *                 library what to do or what to be, depending on the use case and context!
 *  - Internal:    The internal section is just a quick way of wrapping certain functions in this file
 *                 to make them easier accessible inside the jaut library.
 *                 This section was not intended to be used outside of the jaut library.
 *                 However, you may still use them for your own code.
 *  - Utility:     The utility section of this header defines some useful utilities, which didn't fit
 *                 in any other class!
 *  - Errorcodes:  Error codes were intended to be used for certain situations where a function may
 *                 have returned different than expected.
 *                 However, these are currently unused and may be either used or removed in future!
 */

//region Export
/* ==================================================================================
 * ====================================== Export ====================================
 * ================================================================================== */
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

//======================================================================================================================
#ifndef JAUT_API
  #define JAUT_API
#endif
//endregion Export
//region Convenience Macros
/* ==================================================================================
 * =============================== Convenience Macros ===============================
 * ================================================================================== */

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
 * For lazy programmers to avoid implicit conversion warnings.
 * Casts any numeral type to float.
 */
#define JT_FIX_F(x) static_cast<float>(x)

/**
 * For lazy programmers to avoid implicit conversion warnings.
 * Casts any numeral type to double.
 */
#define JT_FIX_D(x) static_cast<double>(x)

/**
 * For lazy programmers to avoid implicit conversion warnings.
 * Casts any numeral type to int.
 */
#define JT_FIX(x) static_cast<int>(x)
//endregion Convenience Macros
//region Constants
/* ==================================================================================
 * ==================================== Constants ===================================
 * ================================================================================== */
enum JAUT_API JautFitMode
{
    FitModeWithStart,
    FitModeWithEnd,
    FitModeBetween,
    FitModeTotal
};
//endregion Constants
//region Typedefs
/* ==================================================================================
 * ==================================== Typedefs ====================================
 * ================================================================================== */
typedef unsigned int ErrInt;
//endregion Typedefs
//region Settings
/* ==================================================================================
 * ==================================== Settings ====================================
 * ================================================================================== */
namespace settings
{
//======================================================================================================================
// SECTION 0: GENERAL SETTINGS
/**
 * Determines whether strict mode should be enabled or not.
 * This enables strict mode for certain situations which you can individually turn on/off in section 1.
 *
 * Default: true
 */
inline constexpr bool IS_STRICT_ON = true;

//======================================================================================================================
// SECTION 1: STRICT MODE SETTINGS

/**
 * Should library classes force the user to use in-built interfaces to function correctly?
 * The reason for this is that some classes need certain functions to work and those interface classes provide all
 * needed methods.
 *
 * Default: true
 */
inline constexpr bool STRICT_TEMPLATES = true;

/**
 * If this is true, then the parser implementing this flag will be more strict parsing config files.
 *
 * For example:
 * XmlParser will stop parsing if the file contains invalid tags if this is true, else it will skip those tags
 * and go on!
 *
 * Default: false
 */
inline constexpr bool STRICT_CONFIG_PARSING = false;

/**
 * If this is true, when a jaut_audio is set to double precision processing and a float buffer is passed or vice-versa,
 * an assertion will be thrown.
 *
 * If you don't mind losing data or wasting more memory than needed, turn this option off!
 *
 *  Default: false
 */
inline constexpr bool STRICT_PROCESSING = true;

/**
 * If this is true, when the jaut_message thread tries to call a function not supposed to be called from the jaut_message thread,
 * this will assert.
 *
 * Default: 1
 */
inline constexpr bool STRICT_THREAD_DISTINCTION = true;

/**
 * If this is true, when any other thread than the jaut_message thread tries to access a function, then this will assert.
 *
 * Default: 1
 */
inline constexpr bool STRICT_THREAD_EXCLUSION = true;

//======================================================================================================================
// SECTION 2: DEFAULT SETTINGS

/**
 * This defines the starting colour id value of all jaut jaut_component colour ids.
 * If you are using components which ids may threaten to clash with jauts',
 * you may want to change this number to fix these issues.
 */
inline constexpr int DEFAULT_COMPONENT_COLOUR_STARTING_INDEX = 0x1000;
}
//endregion Settings
//region Internal
/* ==================================================================================
 * ==================================== Internal ====================================
 * ================================================================================== */
namespace internal
{
inline constexpr bool JAUT_STRICT(int strictSetting = -1)
{
    (void)(strictSetting);
    return JT_DEBUGGING_OR(settings::IS_STRICT_ON && (strictSetting != -1 ? static_cast<bool>(strictSetting) : true), 0);
}

inline constexpr bool STRICT_TEMPLATES_ENABLED()
{
    return JAUT_STRICT(settings::STRICT_TEMPLATES);
}

inline constexpr bool STRICT_CONFIG_PARSING_ENABLED()
{
    return JAUT_STRICT(settings::STRICT_CONFIG_PARSING);
}

inline constexpr bool STRICT_PROCESSING_ENABLED()
{
    return JAUT_STRICT(settings::STRICT_PROCESSING);
}

inline constexpr bool STRICT_THREAD_EXCLUSION_ENABLED()
{
    return JAUT_STRICT(settings::STRICT_THREAD_EXCLUSION);
}

inline bool IS_THIS_THE_MESSAGE_THREAD()
{
    /* You should really be only calling this on the jaut_message thread.
       If you desire to add a jaut_audio on a thread which is not the main event dispatch loop,
       do so at your own risk.
       To get rid of this assert, set jaut::settings::STRICT_THREAD_EXCLUSION to 0!
    */
    if constexpr(STRICT_THREAD_EXCLUSION_ENABLED())
    {
        if(!MessageManager::getInstance()->isThisTheMessageThread())
        {
            jassertfalse;
            return false;
        }
    }

    return true;
}

extern bool INTERNAL_DISABLE_THREAD_DIST;
extern bool EXPLICIT_DISABILITY;
}
//endregion Internal
//region Utility
/* ==================================================================================
 * ===================================== Utility ====================================
 * ================================================================================== */
/**
 *  This will disable the JAUT_ENSURE_MESSAGE_THREAD() check for once and reset it again when such an event has occurred.
 *  Beware to only use this when you really know what you're doing, as calling jaut_message thread code on the audio thread
 *  without proper synchronisation will most of the time go wrong.
 */
inline JAUT_API void JAUT_DISABLE_THREAD_DIST()
{
    JT_DEBUGGING
    (
        if (!internal::EXPLICIT_DISABILITY)
        {
            internal::INTERNAL_DISABLE_THREAD_DIST = true;
        }
    )
}

/**
 * This will disable the JAUT_ENSURE_AUDIO_THREAD() check for as long as you didn't set it back to true.
 */
inline JAUT_API void JAUT_DISABLE_THREAD_DIST_EXPLICIT(bool disable) noexcept
{
    ignoreUnused(disable);

    JT_DEBUGGING
    (
        internal::EXPLICIT_DISABILITY          = disable;
        internal::INTERNAL_DISABLE_THREAD_DIST = false;
    )
}

/**
 *  This makes sure that certain functions are only called on the messag thread.
 *  If a particular function which is not supposed to be called on the audio thread is called there,
 *  then this will assert.
 *
 *  You can also temporarily disable this with JAUT_DISABLE_THREAD_DIST() if you want to live on the bloody edge.
 */
inline JAUT_API void JAUT_ENSURE_MESSAGE_THREAD()
{
    JT_DEBUGGING
    (
        const bool flag = !internal::INTERNAL_DISABLE_THREAD_DIST && !internal::EXPLICIT_DISABILITY;

        if (internal::JAUT_STRICT(settings::STRICT_THREAD_DISTINCTION) && flag)
        {
            jassert(MessageManager::getInstance()->isThisTheMessageThread());
        }

        internal::INTERNAL_DISABLE_THREAD_DIST = false;
    )
}

template<int Increment>
inline constexpr JAUT_API int JAUT_COLOUR_ID = settings::DEFAULT_COMPONENT_COLOUR_STARTING_INDEX + Increment;

/**
 *  This class will make sure to disable explicit thread distinction as long as it persists and resets
 *  it once it's destroyed.
 */
struct JAUT_API ScopedATCD final
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

//======================================================================================================================
// Functions
/**
 *  Checks whether the number to test fits in the given range.
 * 
 *  @param toTest The number to test
 *  @param start  The start number (included)
 *  @param end    The end number (excluded)
 *  @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_s(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest >= start && toTest < end;
}

/**
 *  Checks whether the number to test fits in the given range.
 * 
 *  @param toTest The number to test
 *  @param start  The start number (excluded)
 *  @param end    The end number (included)
 *  @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_e(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest > start && toTest <= end;
}

/**
 *  Checks whether the number to test fits in the given range.
 * 
 *  @param toTest The number to test
 *  @param start  The start number (excluded)
 *  @param end    The end number (excluded)
 *  @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_n(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest > start && toTest < end;
}

/**
 *  Checks whether the number to test fits in the given range.
 * 
 *  @param toTest The number to test
 *  @param start  The start number (included)
 *  @param end    The end number (included)
 *  @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_a(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest >= start && toTest <= end;
}

/**
 *  Checks whether the number to test fits in the given range, checked by the specified mode.
 *
 *  @param toTest The number to test
 *  @param start  The start number
 *  @param end    The end number
 *  @param mode   The mode to test this range with
 *                 - FitModeWithStart: Test if the number is in range including start excluding end
 *                 - FitModeWithEnd: Test if the number is in range excluding start including end
 *                 - FitModeBetween: Test if the number is in range excluding start and end
 *                 - FitModeTotal: Test if the number is in range including start and end
 *  @return
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_m(NumberType toTest, NumberType start, NumberType end, JautFitMode mode) noexcept
{
    if(mode == FitModeWithStart)
    {
        return fit_s(toTest, start, end);
    }
    else if(mode == FitModeWithEnd)
    {
        return fit_e(toTest, start, end);
    }
    else if(mode == FitModeBetween)
    {
        return fit_n(toTest, start, end);
    }
    
    return fit_a(toTest, start, end);
}

template<class ValueType>
inline constexpr ValueType JAUT_API limdec(ValueType value, int maxDecimals) noexcept
{
    static_assert(std::is_arithmetic_v<ValueType>);
    const ValueType multiplier = static_cast<ValueType>(std::pow(10, maxDecimals));
    return std::round(value * multiplier) / multiplier;
}

//endregion Utility
//region Errorcodes
/* ==================================================================================
 * ==================================== Errorcodes ==================================
 * ================================================================================== */
inline constexpr ErrInt ERROR_TERMINATE             = 0x0000;
inline constexpr ErrInt ERROR_SUCCESS               = 0x0001;
inline constexpr ErrInt ERROR_INTERNAL_ISSUE        = 0x0002;
inline constexpr ErrInt ERROR_FILE_MISSING          = 0x0003;
inline constexpr ErrInt ERROR_CONFIG_INVALID_FORMAT = 0x0064;
inline constexpr ErrInt ERROR_CONFIG_INTERNAL       = 0x0065;
//endregion Errorcodes
}
