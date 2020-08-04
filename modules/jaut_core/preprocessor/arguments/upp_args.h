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
    @file   upp_args.h
    @date   07, March 2020

    ===============================================================
 */
#pragma once

#if !DOXYGEN
  #define JAUT_INTERN_UPP_OVM_GLUE(NAME, NUM) JAUT_UPP_MISC_CONCAT(NAME##_, NUM)
  #define JAUT_INTERN_UPP_OVM_COUNT(_X, _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16,\
                                        _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,\
                                        _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,\
                                        _49, _50, INDEX, ...) INDEX

  #define JAUT_INTERN_UPP_OVM_SIZE(ARGS) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                         ( \
                                             JAUT_INTERN_UPP_OVM_COUNT \
                                             ( \
                                                 _ ARGS, \
                                                 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, \
                                                 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
                                                 16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0 \
                                             ) \
                                         )
  
#endif

/**
 *  Gets the amount of arguments in the argument list.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_GET_LENGTH(...) JAUT_INTERN_UPP_OVM_SIZE(JAUT_UPP_VA_OPT_B(__VA_ARGS__))

/**
 *  This is a useful macro which helps in overloading a macro depending on its amount of arguments.
 *  Note that a macro must have the exact amount of arguments its overload number denotes!
 *
 *  Example:
 *  @code
 *  // Define the entry macro, it must be a variadic macro to take an arbitrary amount of arguments.
 *  // The first argument must be the name of your macro overloads without underscore and number!
 *  #define MY_MACRO(...) JAUT_UPP_MACRO_OVERLOADE(MY_MACRO, __VA_ARGS__)
 *
 *  // Now make your overloads, the name must consist of the macro name passed as first argument in
 *  // JAUT_UPP_MACRO_OVERLOAD and a number from 1 to 30 (_n)
 *  // Also note that the amount of arguments the macro can take must match its number.
 *  #define MY_MACRO_1(arg1) // Your macro action
 *  #define MY_MACRO_2(arg1, arg2) // Another macro action
 *  #define MY_MACRO_#(...
 *  @endcode
 *
 *  @param NAME The name of the macro without the number and underscore
 *  @param ...  The arguments
 */
#define JAUT_UPP_ARGS_OVERLOAD(NAME, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                          (\
                                              JAUT_INTERN_UPP_OVM_GLUE \
                                              ( \
                                                  NAME, JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__) \
                                              ) (__VA_ARGS__) \
                                          )
                      
/**
 *  Reverses the given argument list.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_REVERSE(...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                   ( \
                                       JAUT_UPP_ARGS_OVERLOAD(JAUT_INTERN_UPP_REVERSE, __VA_ARGS__) \
                                   )


/**
 *  Gets the last argument.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_GET_LAST(...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                    ( \
                                        JAUT_UPP_ARGS_OVERLOAD(JAUT_INTERN_UPP_MACRO_PROTOTYPE, __VA_ARGS__) \
                                    )

/**
 *  Gets an argument at the specified index from the argument list.
 *  @param N   The index of the argument
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_GET_AT(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                     ( \
                                         JAUT_INTERN_UPP_OVM_GLUE(JAUT_INTERN_UPP_MACRO_PROTOTYPE, N)(__VA_ARGS__) \
                                     )

/**
 *  Applies a macro to all elements in the argument list.
 *  @param E   The macro to apply to each argument
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_FOR_EACH(E, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                       ( \
                                           JAUT_INTERN_UPP_OVM_GLUE \
                                           ( \
                                               JAUT_INTERN_UPP_FOR, \
                                               JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__) \
                                           ) (E, __VA_ARGS__) \
                                       )

/**
 *  Applies a macro to all elements in the argument list.
 *  Other than JAUT_UPP_ARGS_FOR_EACH, this does not insert commas between each element.
 *
 *  @param E   The macro to apply to each argument
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_FOR_EACH_UNCO(E, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                            ( \
                                                JAUT_INTERN_UPP_OVM_GLUE \
                                                ( \
                                                    JAUT_INTERN_UPP_FOR_UNCO, \
                                                    JAUT_UPP_ARGS_GET_LENGTH(__VA_ARGS__) \
                                                ) (E, __VA_ARGS__) \
                                            )

/**
 *  Applies a macro to n elements in the argument list.
 *  @param E   The macro to apply to each argument
 *  @param N   The amount of arguments to process
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_FOR_N(N, E, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                      ( \
                                          JAUT_INTERN_UPP_OVM_GLUE(JAUT_INTERN_UPP_FOR, N)(E, __VA_ARGS__) \
                                          JAUT_INTERN_UPP_OVM_GLUE(JAUT_INTERN_UPP_CUT, N)(__VA_ARGS__) \
                                      )

/**
 *  Removes n arguments from the beginning.
 *  @param N   The amount of arguments to remove
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_TRIM(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                   ( \
                                       JAUT_INTERN_UPP_OVM_GLUE(JAUT_INTERN_UPP_CUT, N)(__VA_ARGS__) \
                                   )

/**
 *  Removes n arguments from the end.
 *  @param N   The amount of arguments to remove
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_REMOVE(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                     ( \
                                         JAUT_UPP_ARGS_REVERSE \
                                         ( \
                                             JAUT_UPP_ARGS_TRIM(N, JAUT_UPP_ARGS_REVERSE(__VA_ARGS__)) \
                                         ) \
                                     )

/**
 *  Gets n arguments from the argument list.
 *  @param N   The amount of arguments to keep
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_LIMIT(N, ...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                    ( \
                                        JAUT_INTERN_UPP_OVM_GLUE(JAUT_INTERN_UPP_LIMIT, N)(__VA_ARGS__) \
                                    )

/**
 *  Stringifies a list of arguments and keeps the commas.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_STRINGIFY(...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                     ( \
                                         JAUT_UPP_ARGS_FOR_EACH(JUCE_STRINGIFY, __VA_ARGS__) \
                                     )

/**
 *  Stringifies the whole argument list as one string without expanding its arguments.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_STRINGIFY_FORCE(...) #__VA_ARGS__

/**
 *  Stringifies the whole argument list as one string but expands its arguments first.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_STRINGIFY_ALL(...) JAUT_UPP_ARGS_STRINGIFY_FORCE(__VA_ARGS__)

/**
 *  Removes all commas from an argument list.
 *  @param ... The arguments
 */
#define JAUT_UPP_ARGS_UNCOMMATISE(...) JAUT_UPP_MISC_EXPAND_VA_ARGS \
                                       ( \
                                           JAUT_UPP_ARGS_OVERLOAD(JAUT_INTERN_UPP_EMPLACE, __VA_ARGS__) \
                                       )

/**
 *  Generates the given argument n times.
 *  @param N The amount of generations
 *  @param S The sequence to generate
 */
#define JAUT_UPP_ARGS_GENERATE(N, S) JAUT_UPP_ARGS_LIMIT(N, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,\
                                                            S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,\
                                                            S, S, S, S, S, S, S, S, S, S)

#ifdef __GNUG__
   /**
    *  Prepends a comma to the arguments list if it is not empty.
    *  @param ... The arguments
    */
#   define JAUT_UPP_VA_OPT_B(...) ,##__VA_ARGS__
#else
#   define JAUT_UPP_VA_OPT_B(...) __VA_ARGS__
#endif

/**
 *  Appends a comma to the arguments list if it is not empty.
 *  @param ... The arguments
 */
#define JAUT_UPP_VA_OPT(...) JAUT_UPP_ARGS_OVERLOAD(JAUT_INTERN_UPP_OPTIONAL, __VA_ARGS__)
