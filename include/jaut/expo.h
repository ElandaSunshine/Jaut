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

#include <JuceHeader.h>

/**
 * This is the jaut export header.
 * This header file defines the export macro as well as useful utilities and internal settings.
 *
 * You may change any of these, but be aware that changing something you shouldn't may break internal stuff.
 * So better only alter sections explicitly stating you can do so to stay on the safe site!
 *
 * Below you will find following sections (Be free to use any of these in your own project):
 * - Export:      This section defines the jaut export macro for dlls on Windows based operating systems.
 *                This macro can also be used to make sure that jaut is actually included in your project.
 * - Macros:      This section defines several useful macros used in the jaut library which can as well be
 *                used in your project.
 *                Just be sure you know what you are doing!
 * - Constants:   This section defines all constants for functions in the library which can take several modes
 *                much like the OpenGL library system does it.
 * - Typedefs:    These typedefs are just convenience typedefs to better describe certain types for a
 *                particular context.
 *                You may or may not use them in your project!
 * - Settings:    The settings section defines various functionality parameters which direct the jaut
 *                library what to do or what to be, depending on the use case and context!
 * - Internal:    The internal section is just a quick way of wrapping certain functions in this file
 *                to make them easier accessible inside the jaut library.
 *                This section was not intended to be used outside of the jaut library.
 *                However, you may still use them for your own code.
 * - Utility:     The utility section of this header defines some useful utilities, which didn't fit
 *                in any other class!
 * - Error codes: Error codes were intended to be used for certain situations where a function may
 *                have returned different than expected.
 *                However, these are currently unused and may be either used or removed in future!
 */

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
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

//======================================================================================================================
#ifndef JAUT_API
  #define JAUT_API
#endif

// END: EXPORT
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================


namespace jaut
{
/* ==================================================================================
 * =============================== Convenience Macros ===============================
 * ================================================================================== */
#if (1) // Convenience macros (Don't touch)
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


#if JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP
  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   *
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   *
   * @param x The value which should be expanded if it's run as standalone (doesn't do anything if the default
   *          standalone filter is defined)
   * @param y The value which should be expanded if it's not run as standalone (this will always be evaluated if the
   *          default standalone filter is defined)
   */
  #define JT_IS_STANDALONE_INLINE(x, y) \
          JUCEApplicationBase::isStandaloneApp() ? (x) : (y)

  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   * 
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   * 
   * @param x The value which should be expanded if it's run as standalone (doesn't do anything if the default
   *          standalone filter is defined)
   */
  #define JT_IS_STANDALONE(x) \
          if(JUCEApplicationBase::isStandaloneApp()) {x}

  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   * 
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   * 
   * @param x The value which should be expanded if it's not run as standalone (this will always be evaluated if the
   *          default standalone filter is defined)
   */
  #define JT_STANDALONE_ELSE(x) \
          else {x}
#else
  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   * 
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   * 
   * @param x The value which should be expanded if it's run as standalone (doesn't do anything if the default
   *          standalone filter is defined)
   * @param y The value which should be expanded if it's not run as standalone (this will always be evaluated if the
   *          default standalone filter is defined)
   */
  #define JT_IS_STANDALONE_INLINE(x, y) y

  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   * 
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   * 
   * @param x The value which should be expanded if it's run as standalone (doesn't do anything if the default
   *          standalone filter is defined)
   */
  #define JT_IS_STANDALONE(x) if constexpr(false){x}

  /**
   * This comes in handy when messing with custom standalone windows and plugin versions.
   * As you may want to return to the default standalone window throughout development,
   * some things may not work anymore.
   * Use this to make sure certain things only work as default standalone and as plugin.
   * 
   * Long story short, if you defined your custom standalone window, this will make sure that
   * certain functionality runs only when your code is compiled as a plugin.
   * If you defined the default standalone window, this will make the code run the same in the
   * standalone version as also in the plugin version.
   * 
   * @param x The value which should be expanded if it's not run as standalone (this will always be evaluated if the
   *          default standalone filter is defined)
   */
  #define JT_STANDALONE_ELSE(x) else {x}
#endif
#endif // Convenience macros (Don't touch)



/* ==================================================================================
 * ==================================== Constants ===================================
 * ================================================================================== */
inline constexpr int Jt_FitWithStart = 1;
inline constexpr int Jt_FitWithEnd   = 2;
inline constexpr int Jt_FitBetween   = 3;
inline constexpr int Jt_FitTotal     = 4;


/* ==================================================================================
 * ============================== Convenience Typedefs ==============================
 * ================================================================================== */
#if (1) // Convenience typedefs (Don't touch)
typedef unsigned int ErrInt;
#endif // Convenience typedefs (Don't touch) 



/* ==================================================================================
 * ================================= jaut::settings =================================
 * ================================================================================== */
#if(1) // Settings (You can change these)
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
 * If this is true, when a processor is set to double precision processing and a float buffer is passed or vice-versa,
 * an assertion will be thrown.
 *
 * If you don't mind losing data or wasting more memory than needed, turn this option off!
 *
 *  Default: false
 */
inline constexpr bool STRICT_PROCESSING = true;

/**
 * If this is true, when the message thread tries to call a function not supposed to be called from the message thread,
 * this will throw an assert.
 *
 * Default: 1
 */
inline constexpr bool STRICT_THREAD_DISTINCTION = true;

//======================================================================================================================
// SECTION 2: DEFAULT SETTINGS

/**
 * This defines the starting colour id value of all jaut component colour ids.
 * If you are using components which ids may threaten to clash with jauts',
 * you may want to change this number to fix these issues.
 */
inline constexpr int DEFAULT_COMPONENT_COLOUR_STARTING_INDEX = 0x1000;
}
#endif // Settings (You can change these)



/* ==================================================================================
 * ================================= jaut::internal =================================
 * ================================================================================== */
#if(1) // Internal (Don't touch)
//======================================================================================================================
// BEGIN INTERNAL (DON'T TOUCH THIS SECTION)
//======================================================================================================================
namespace internal
{
inline constexpr bool JAUT_STRICT(int strictSetting = 2)
{
    return JT_DEBUGGING_OR(settings::IS_STRICT_ON && (strictSetting < 2 ? strictSetting : 1), 0);
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

extern bool INTERNAL_DISABLE_THREAD_DIST;
extern bool EXPLICIT_DISABILITY;
}
//======================================================================================================================
//  END INTERNAL
//======================================================================================================================
#endif // Internal (Don't touch)



/* ==================================================================================
 * ================================= jaut - utility =================================
 * ================================================================================== */
#if(1) // Utility (Don't touch)
/**
 * This will disable the JAUT_ENSURE_AUDIO_THREAD() check for once and reset it again when such an event has occurred.
 * Beware to only use this when you really know what you're doing, as calling audio thread code on the gui without
 * proper synchronisation will most of the time go wrong.
 */
inline JAUT_API void JAUT_DISABLE_THREAD_DIST()
{
    JT_NDEBUGGING(return);

    if (!internal::EXPLICIT_DISABILITY)
    {
        internal::INTERNAL_DISABLE_THREAD_DIST = true;
    }
}

/**
 * This will disable the JAUT_ENSURE_AUDIO_THREAD() check for as long as you didn't set it back to true.
 */
inline JAUT_API void JAUT_DISABLE_THREAD_DIST_EXPLICIT(bool disable) noexcept
{
    JT_NDEBUGGING(return);

    internal::EXPLICIT_DISABILITY          = disable;
    internal::INTERNAL_DISABLE_THREAD_DIST = false;
}

/**
 * This makes sure that certain functions are only called on the audio thread.
 * If a particular function which is not supposed to be called on the message thread is called there,
 * then this will throw an assert.
 *
 * Also be aware that this will only make sure that a function can't be called on the message thread,
 * if your implementation includes another thread besides the main event-dispatch loop and the audio thread,
 * this will not throw if you call it from there!
 *
 * You can also temporarily disable this with JAUT_DISABLE_THREAD_DIST() if you want to live on the bloody edge.
 */
inline JAUT_API void JAUT_ENSURE_AUDIO_THREAD()
{
    JT_NDEBUGGING(return);

    const bool flag = !internal::INTERNAL_DISABLE_THREAD_DIST && !internal::EXPLICIT_DISABILITY;

    if (internal::JAUT_STRICT(settings::STRICT_THREAD_DISTINCTION) && flag)
    {
        jassert(!MessageManager::getInstance()->isThisTheMessageThread());
    }

    internal::INTERNAL_DISABLE_THREAD_DIST = false;
}

template<int Increment>
inline constexpr JAUT_API int JAUT_COLOUR_ID = settings::DEFAULT_COMPONENT_COLOUR_STARTING_INDEX + Increment;

/**
 * This class will make sure to disable explicit thread distinction as long as it persists and resets
 * it once it's destroyed.
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
 * Checks whether the number to test fits in the given range.
 * 
 * @param toTest The number to test
 * @param start  The start number (included)
 * @param end    The end number (excluded)
 * @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_s(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest >= start && toTest < end;
}

/**
 * Checks whether the number to test fits in the given range.
 * 
 * @param toTest The number to test
 * @param start  The start number (excluded)
 * @param end    The end number (included)
 * @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_e(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest > start && toTest <= end;
}

/**
 * Checks whether the number to test fits in the given range.
 * 
 * @param toTest The number to test
 * @param start  The start number (excluded)
 * @param end    The end number (excluded)
 * @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_n(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest > start && toTest < end;
}

/**
 * Checks whether the number to test fits in the given range.
 * 
 * @param toTest The number to test
 * @param start  The start number (included)
 * @param end    The end number (included)
 * @return True if the number fits in the given range, false if not.
 */
template<class NumberType>
inline constexpr bool JAUT_API fit_a(NumberType toTest, NumberType start, NumberType end) noexcept
{
    return toTest >= start && toTest <= end;
}

template<class NumberType>
inline constexpr bool JAUT_API fit_m(NumberType toTest, NumberType start, NumberType end, int mode) noexcept
{
    if(mode == Jt_FitWithStart)
    {
        return fit_s(toTest, start, end);
    }
    else if(mode == Jt_FitWithEnd)
    {
        return fit_e(toTest, start, end);
    }
    else if(mode == Jt_FitBetween)
    {
        return fit_n(toTest, start, end);
    }
    
    return fit_a(toTest, start, end);
}
#endif // Utility (Don't touch)



/* ==================================================================================
 * ================================ jaut - errorcodes ===============================
 * ================================================================================== */
#if(1) // Error codes (Don't touch)
inline constexpr ErrInt ERROR_TERMINATE             = 0x0000;
inline constexpr ErrInt ERROR_SUCCESS               = 0x0001;
inline constexpr ErrInt ERROR_INTERNAL_ISSUE        = 0x0002;
inline constexpr ErrInt ERROR_FILE_MISSING          = 0x0003;
inline constexpr ErrInt ERROR_CONFIG_INVALID_FORMAT = 0x0064;
inline constexpr ErrInt ERROR_CONFIG_INTERNAL       = 0x0065;
#endif // Error codes (Don't touch)
}
