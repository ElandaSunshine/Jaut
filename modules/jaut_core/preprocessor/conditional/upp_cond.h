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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   upp_cond.h
    @date   24, March 2020
    
    ===============================================================
 */
 
#pragma once

#if !DOXYGEN
  #define JAUT_INTERN_UPP_COND_COUNT(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16, \
                                     _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, \
                                     _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, \
                                     _49, _50, INDEX, ...) INDEX
  #define JAUT_INTERN_UPP_COND_GEOL(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_INTERN_UPP_COND_COUNT(__VA_ARGS__))
  
  #define JAUT_INTERN_UPP_COND(OP, N, V, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_UPP_MISC_CONCAT(JAUT_INTERN_UPP_ARGS_COMP_, N)(JAUT_UPP_MISC_CONCAT(JAUT_INTERN_UPP_COND_, OP), V, __VA_ARGS__))
  #define JAUT_INTERN_UPP_COND_EQ(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_INTERN_UPP_COND_GEOL(__VA_ARGS__, TRUE,  JAUT_UPP_ARGS_GENERATE(50, FALSE)))
  #define JAUT_INTERN_UPP_COND_GT(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_INTERN_UPP_COND_GEOL(__VA_ARGS__, FALSE, JAUT_UPP_ARGS_GENERATE(50, FALSE)))
  #define JAUT_INTERN_UPP_COND_LT(...) JAUT_UPP_MISC_EXPAND_VA_ARGS(JAUT_INTERN_UPP_COND_GEOL(__VA_ARGS__, FALSE, JAUT_UPP_ARGS_GENERATE(50, TRUE)))

  #define JAUT_INTERN_UPP_COND_MAKE_TYPE_1(_1, _2) JAUT_UPP_MISC_CONCAT(_1, _2)
  #define JAUT_INTERN_UPP_COND_MAKE_TYPE(OP, _1, _2) JAUT_INTERN_UPP_COND_MAKE_TYPE_1(JAUT_UPP_MISC_CONCAT( JAUT_INTERN_UPP_COND_##OP, _##_1 ), _##_2 ) ()

  #define JAUT_INTERN_UPP_COND_AND_TRUE_TRUE()   TRUE
  #define JAUT_INTERN_UPP_COND_AND_TRUE_FALSE()  FALSE
  #define JAUT_INTERN_UPP_COND_AND_FALSE_TRUE()  FALSE
  #define JAUT_INTERN_UPP_COND_AND_FALSE_FALSE() FALSE
  
  #define JAUT_INTERN_UPP_COND_OR_TRUE_TRUE()   TRUE
  #define JAUT_INTERN_UPP_COND_OR_TRUE_FALSE()  TRUE
  #define JAUT_INTERN_UPP_COND_OR_FALSE_TRUE()  TRUE
  #define JAUT_INTERN_UPP_COND_OR_FALSE_FALSE() FALSE
  
  #define JAUT_INTERN_UPP_COND_XOR_TRUE_TRUE()   FALSE
  #define JAUT_INTERN_UPP_COND_XOR_TRUE_FALSE()  TRUE
  #define JAUT_INTERN_UPP_COND_XOR_FALSE_TRUE()  TRUE
  #define JAUT_INTERN_UPP_COND_XOR_FALSE_FALSE() FALSE
  
  #define JAUT_INTERN_UPP_COND_UNPACK(X) _, _
  #define JAUT_INTERN_UPP_COND_EXTRACT(X) JAUT_INTERN_UPP_COND_UNPACK X
  #define JAUT_INTERN_UPP_COND_CHECK_CAPSULE(...) JAUT_UPP_IF_ELSE(JAUT_UPP_COND_ARGS_EQUAL(2, __VA_ARGS__), TRUE, FALSE)
#endif

/**
 *  Will return TRUE if the condition was FALSE.
 *  @param COND The condition to check against
 */
#define JAUT_UPP_COND_NOT(COND) JAUT_UPP_IF_ELSE(COND, FALSE, TRUE)

/**
 *  Will return TRUE if both conditions were TRUE.
 *  @param COND1 The first condition to check against
 *  @param COND2 The second condition to check against
 */
#define JAUT_UPP_COND_AND(COND1, COND2) JAUT_UPP_IF_ELSE(JAUT_INTERN_UPP_COND_MAKE_TYPE(AND, COND1, COND2), TRUE, FALSE)

/**
 *  Will return TRUE if at least one of the conditions was TRUE.
 *  @param COND1 The first condition to check against
 *  @param COND2 The second condition to check against
 */
#define JAUT_UPP_COND_OR(COND1, COND2) JAUT_UPP_IF_ELSE(JAUT_INTERN_UPP_COND_MAKE_TYPE(OR, COND1, COND2), TRUE, FALSE)

/**
 *  Will return TRUE if only one of the conditions was TRUE.
 *  @param COND1 The first condition to check against
 *  @param COND2 The second condition to check against
 */
#define JAUT_UPP_COND_XOR(COND1, COND2) JAUT_UPP_IF_ELSE(JAUT_INTERN_UPP_COND_MAKE_TYPE(XOR, COND1, COND2), TRUE, FALSE)

/**
 *  Determines whether the amount of arguments matches N.
 *  @param N   The amount of arguments it should have
 *  @param ... The arguments
 */
#define JAUT_UPP_COND_ARGS_EQUAL(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                         ( \
                                             JAUT_INTERN_UPP_COND(EQ, N, FALSE, __VA_ARGS__) \
                                         )

/**
 *  Determines whether the amount of arguments is bigger than N.
 *  @param N   The amount of arguments it should exceed
 *  @param ... The arguments
 */
#define JAUT_UPP_COND_ARGS_GREATER_THAN(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                                ( \
                                                    JAUT_INTERN_UPP_COND(GT, N, TRUE, __VA_ARGS__) \
                                                )
/**
 *  Determines whether the amount of arguments is less than N.
 *  @param N   The amount of arguments it should not reach
 *  @param ... The arguments
 */
#define JAUT_UPP_COND_ARGS_LESS_THAN(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                             ( \
                                                 JAUT_INTERN_UPP_COND(LT, N, FALSE, __VA_ARGS__) \
                                             )

/**
 *  Determines whether the amount of groups matches N.
 *  @param N      The amount of arguments it should have
 *  @param GROUPS The groups
 */
#define JAUT_UPP_COND_GROUPS_EQUAL(N, GROUPS) JAUT_UPP_COND_ARGS_EQUAL(N, JAUT_UPP_GROUP_RESOLVE(GROUPS))

/**
 *  Determines whether the amount of groups is bigger than N.
 *  @param N   The amount of arguments it should exceed
 *  @param GROUPS The groups
 */
#define JAUT_UPP_COND_GROUPS_GREATER_THAN(N, GROUPS) JAUT_UPP_COND_ARGS_GREATER_THAN(N, JAUT_UPP_GROUP_RESOLVE(GROUPS))
/**
 *  Determines whether the amount of groups is less than N.
 *  @param N   The amount of arguments it should not reach
 *  @param GROUPS The groups
 */
#define JAUT_UPP_COND_GROUPS_LESS_THAN(N, GROUPS) JAUT_UPP_COND_ARGS_LESS_THAN(N, JAUT_UPP_GROUP_RESOLVE(GROUPS))
                                             
/**
 *  Determines whether argument X is parenthesised or not.
 *  @param X The argument to check for
 */
#define JAUT_UPP_COND_IS_ENCAPSULATED(X) JAUT_INTERN_UPP_COND_CHECK_CAPSULE(JAUT_INTERN_UPP_COND_EXTRACT(X))
