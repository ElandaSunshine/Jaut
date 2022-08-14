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
    @file   jaut_UppGroup.h
    @date   26, March 2020

    ===============================================================
 */

#pragma once

#if !DOXYGEN
  #define JAUT_INTERN_UPP_GROUP_EXPAND(X) X,
  #define JAUT_INTERN_UPP_GROUP_RESOLVE(X) JAUT_INTERN_UPP_GROUP_EXPAND X
  
  #define JAUT_INTERN_UPP_GROUP_UNPACK_ALL(...) __VA_ARGS__
  #define JAUT_INTERN_UPP_GROUP_UNPACK(...) JAUT_INTERN_UPP_GROUP_UNPACK_ALL __VA_ARGS__
  
  #define JAUT_INTERN_UPP_ON_PARENS(X, A, ...) JAUT_UPP_IF(JAUT_UPP_COND_IS_ENCAPSULATED(X), A, (__VA_ARGS__))
  #define JAUT_INTERN_UPP_GROUP_MAKE(X) (X)
  #define JAUT_INTERN_UPP_GROUP_REMAKE(...) JAUT_UPP_GROUP_MAKE(__VA_ARGS__)
  #define JAUT_INTERN_UPP_STARTING_GROUP(GROUPS) JAUT_INTERN_UPP_GROUP_RES_2(GROUPS)
#endif

/**
 *  Resolves all groups and delimits them by a comma.
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_RESOLVE(GROUPS) JAUT_UPP_ARGS_REMOVE(1, JAUT_UPP_ARGS_TRIM(1, JAUT_INTERN_UPP_GROUP_UNPACK(JAUT_INTERN_UPP_STARTING_GROUP(GROUPS))))



/**
 *  Resolves a single group only.
 *  This will be quicker than JAUT_UPP_GROUP_RESOLVE.
 *  @param X The element to unpack
 */
#define JAUT_UPP_GROUP_UNPACK(X) JAUT_UPP_IF_ELSE(JAUT_UPP_COND_IS_ENCAPSULATED(X), JAUT_UPP_MISC_EXPAND_VA_ARGS X, X)

/**
 *  Creates groups from arguments.
 *  @param ... The arguments
 */
#define JAUT_UPP_GROUP_MAKE(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_UPP_ARGS_UNCOMMATISE(JAUT_UPP_ARGS_FOR_EACH(JAUT_INTERN_UPP_GROUP_MAKE, __VA_ARGS__)))

/**
 *  Gets the amount of groups.
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_GET_LENGTH(GROUPS) JAUT_UPP_ARGS_GET_LENGTH(JAUT_UPP_GROUP_RESOLVE(GROUPS))

/**
 *  Reverses the given groups.
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_REVERSE(GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE \
                                       ( \
                                           JAUT_UPP_ARGS_REVERSE(JAUT_UPP_GROUP_RESOLVE(GROUPS)) \
                                       )

/**
 *  Gets the last group.
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_GET_LAST(GROUPS) JAUT_UPP_ARGS_GET_LAST(JAUT_UPP_GROUP_RESOLVE(GROUPS))

/**
 *  Gets the value of the nth group.
 *  @param N      The place of the group to resolve
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_GET_AT(N, GROUPS) JAUT_UPP_ARGS_GET_AT(N, JAUT_UPP_GROUP_RESOLVE(GROUPS))

/**
 *  Applies a macro to all groups.
 *  @param E      The macro to apply to each group
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_FOR_EACH(E, GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE(JAUT_UPP_ARGS_FOR_EACH(E, JAUT_UPP_GROUP_RESOLVE(GROUPS)))

/**
 *  Applies a macro to N groups
 *  @param E      The macro to apply to each group
 *  @param N      The amount of groups to process
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_FOR_N(N, E, GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE(JAUT_UPP_ARGS_FOR_EACH(N, E, JAUT_UPP_GROUP_RESOLVE(GROUPS)))


/**
 *  Removes N groups from the beginning.
 *  @param N      The amount of groups to remove
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_TRIM(N, GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE(JAUT_UPP_ARGS_TRIM(N, JAUT_UPP_GROUP_RESOLVE(GROUPS)))

/**
 *  Removes N groups from the end.
 *  @param N      The amount of groups to remove
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_REMOVE(N, GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE(JAUT_UPP_ARGS_REMOVE(N, JAUT_UPP_GROUP_RESOLVE(GROUPS)))

/**
 *  Gets n arguments from the argument list.
 *  @param N   The amount of arguments to keep
 *  @param GROUPS The groups
 */
#define JAUT_UPP_GROUP_LIMIT(N, GROUPS) JAUT_INTERN_UPP_GROUP_REMAKE(JAUT_UPP_ARGS_LIMIT(N, JAUT_UPP_GROUP_RESOLVE(GROUPS)))
