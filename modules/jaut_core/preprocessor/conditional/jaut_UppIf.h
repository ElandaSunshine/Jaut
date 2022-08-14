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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   jaut_UppIf.h
    @date   24, March 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/preprocessor/jaut_UppMisc.h>
#include <jaut_core/preprocessor/conditional/jaut_UppConditional.h>

#if !DOXYGEN
  #define JAUT_INTERN_UPP_PRINT_VA_ARGS(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(__VA_ARGS__)
  #define JAUT_INTERN_UPP_UNBRACKETISE(x)    JAUT_INTERN_UPP_PRINT_VA_ARGS x
  
  #define JAUT_INTERN_UPP_CONDITION_IS_TRUE(x) x
  #define JAUT_INTERN_UPP_CONDITION_IS_FALSE(x) ()
  #define JAUT_INTERN_UPP_MAKE_IS_TYPE(RESULT) JAUT_INTERN_UPP_CONDITION_IS_##RESULT
  #define JAUT_INTERN_UPP_IF_IS(COND, RESULT)  JAUT_INTERN_UPP_MAKE_IS_TYPE(COND) ( (RESULT) )
  
  #define JAUT_INTERN_UPP_CONDITION_IS_NOT_TRUE(x) ()
  #define JAUT_INTERN_UPP_CONDITION_IS_NOT_FALSE(x) x
  #define JAUT_INTERN_UPP_MAKE_IS_NOT_TYPE(RESULT) JAUT_INTERN_UPP_CONDITION_IS_NOT_##RESULT
  #define JAUT_INTERN_UPP_IF_NOT(COND, RESULT)     JAUT_INTERN_UPP_MAKE_IS_NOT_TYPE(COND) ( (RESULT) )
  
  #define JAUT_INTERN_UPP_IF_MAKE_ASSERT(STRING, COND) \
                              static_assert(false, COND " (" STRING " at " JAUT_UPP_MISC_ERROR_HERE ")")
#endif

/**
 *  Checks whether the condition is true, and if so, expands the given content.
 *  
 *  @param COND The condition to check against
 *  @param TRUE The action to expand if COND is TRUE
 */
#define JAUT_UPP_IF(COND, TRUE) JAUT_INTERN_UPP_UNBRACKETISE(JAUT_INTERN_UPP_IF_IS(COND, TRUE))

/**
 *  Checks whether the condition is false or true and expands the corresponding content.
 *  
 *  @param COND  The condition to check against
 *  @param TRUE  The content to expand if COND is TRUE
 *  @param FALSE The content to expand if COND is FALSE
 */
#define JAUT_UPP_IF_ELSE(COND, TRUE, FALSE) JAUT_INTERN_UPP_UNBRACKETISE(JAUT_INTERN_UPP_IF_IS (COND, TRUE)) \
                                            JAUT_INTERN_UPP_UNBRACKETISE(JAUT_INTERN_UPP_IF_NOT(COND, FALSE))

#define JAUT_UPP_IF_EVAL(COND) JAUT_UPP_IF_ELSE(COND, true, false)

/**
 *  Makes a quick check against a condition and asserts if it fails.
 *  @param COND The condition to check against
 */
#define JAUT_UPP_IF_NOT_ASSERT(COND) JAUT_UPP_IF \
                                     ( \
                                         JAUT_UPP_COND_NOT(COND), \
                                         JAUT_INTERN_UPP_IF_MAKE_ASSERT( #COND , JAUT_COND_ASSERT_##COND ) \
                                     );
