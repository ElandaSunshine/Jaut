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
    @file   upp_misc.h
    @date   07, March 2020

    ===============================================================
 */
#pragma once

/**
 *  This macro helps in concatenating macro arguments.
 *  This is mostly used because of the ## symbol preventing macro expansion.
 *
 *  @param x The argument to attach to
 *  @param y The argument to attach to the argument to attach to
 */
#define JAUT_UPP_MISC_CONCAT(X, Y) X##Y

/**
 *  When working with the MSVC, it can happen that when a variadic macro expands another macro, that the __VA_ARGS__
 *  object will only occupy one of the macro's arguments, this is an internal preprocessor bug.
 *
 *  To workaround this issue, we need another expansion that will first fill in the __VA_ARGS__ object and then
 *  expand the underlying macro.
 *
 *  Example:
 *  @code
 *  #define MY_ARGS_MACRO(X, Y, Z) ... whatever
 *
 *  // In this special case, the __VA_ARGS__ macro will only expand to Y and not Z, regardless the amount of arguments.
 *  #define MY_VARIADIC_MACRO(...) MY_ARGS_MACRO(0, __VA_ARGS__)
 *
 *  // As a workaround, we double expand it, so that the __VA_ARGS__ will be replaced before the actual macro
 *  // will be expanded
 *  #define MY_VARIADIC_MACRO(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(MY_ARGS_MACRO(0, __VA_ARGS__))
 *  @endcode
 *
 *  @param The full expression containing your __VA_ARGS__
 */
#define JAUT_UPP_MISC_EXPAND_VA_ARGS(X) X

/**
 *  Gets the current line number as string.
 */
#define JAUT_UPP_MISC_GET_CURRENT_LINE_AS_STRING JUCE_STRINGIFY(__LINE__)

/**
 *  Gets the current file and line as string.
 */
#define JAUT_UPP_MISC_ERROR_HERE __FILE__ ":" JAUT_UPP_MISC_GET_CURRENT_LINE_AS_STRING

/**
 *  Does nothing but stealing your cookies! ლ(ಠ益ಠლ)
 *  @param ... The arguments to nothingfy
 */
#define JAUT_UPP_MISC_NOTHING(...)
