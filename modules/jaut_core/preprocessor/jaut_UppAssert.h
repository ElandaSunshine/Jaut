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
    @file   jaut_UppAssert.h
    @date   24, March 2020
    
    ===============================================================
 */
 
#pragma once

// Argument count check failed
#define JAUT_COND_ASSERT_JAUT_UPP_COND_NOT(...) \
                               "Returned '" JAUT_UPP_ARGS_STRINGIFY_ALL(__VA_ARGS__) "'. Must return FALSE!"

#define JAUT_COND_ASSERT_JAUT_UPP_COND_AND(...) \
                               "Returned [" JAUT_UPP_ARGS_STRINGIFY_ALL(__VA_ARGS__) "]. Both must be TRUE!"

#define JAUT_COND_ASSERT_JAUT_UPP_COND_OR(...) \
                               "Returned [" JAUT_UPP_ARGS_STRINGIFY_ALL(__VA_ARGS__) "]. At least one must be TRUE!"

#define JAUT_COND_ASSERT_JAUT_UPP_COND_XOR(...) \
                               "Returned [" JAUT_UPP_ARGS_STRINGIFY_ALL(__VA_ARGS__) "]. Only one must be TRUE!"
                               
#define JAUT_COND_ASSERT_JAUT_UPP_COND_ARGS_EQUAL(...) \
                                "Amount of arguments " \
                                "[" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) "] " \
                                "didn't match " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."

#define JAUT_COND_ASSERT_JAUT_UPP_COND_ARGS_GREATER_THAN(...) \
                                       "Amount of arguments " \
                                       "[" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) "] " \
                                       "didn't exceed " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."

#define JAUT_COND_ASSERT_JAUT_UPP_COND_ARGS_LESS_THAN(...) \
                                    "Amount of arguments " \
                                    "[" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) "] " \
                                    "reached " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."

#define JAUT_COND_ASSERT_JAUT_UPP_COND_GROUPS_EQUAL(...) \
                                     "Amount of groups " \
                                     "(" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) ") " \
                                     "didn't match " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."

#define JAUT_COND_ASSERT_JAUT_UPP_COND_GROUPS_GREATER_THAN(...) \
                                            "Amount of groups " \
                                            "(" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) ") " \
                                            "didn't exceed " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."

#define JAUT_COND_ASSERT_JAUT_UPP_COND_GROUPS_LESS_THAN(...) \
                                         "Amount of groups " \
                                         "(" JAUT_UPP_ARGS_STRINGIFY_ALL(JAUT_UPP_ARGS_TRIM(1, __VA_ARGS__)) ") " \
                                         "reached " JUCE_STRINGIFY(JAUT_UPP_ARGS_GET_AT(1, __VA_ARGS__)) "."
                                     
#define JAUT_COND_ASSERT_JAUT_UPP_COND_IS_ENCAPSULATED(X) \
                                   "Argument '" #X "' is not parenthesised, make sure it's a single value that is " \
                                   "encapsulated in parentheses."
