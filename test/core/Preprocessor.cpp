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
    @file   Preprocessor.cpp
    @date   18, April 2022

    ===============================================================
 */

#include <gtest/gtest.h>

#include <jaut_core/preprocessor/arguments/jaut_UppArgs.h>
#include <jaut_core/preprocessor/conditional/jaut_UppConditional.h>
#include <jaut_core/preprocessor/conditional/jaut_UppIf.h>
#include <jaut_core/preprocessor/jaut_UppMisc.h>

#include <utility>

//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    template<class T, std::size_t ...I>
    constexpr std::array<T, sizeof...(I)> fillConstruct(T value, std::index_sequence<I...>)
    {
        return { ((void) I, value)... };
    }

    template<class T, std::size_t N>
    constexpr std::array<T, N> fillConstruct(T value)
    {
        return fillConstruct(value, std::make_index_sequence<N>{});
    }
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(PreprocessorSuite, TestMiscMacros)
{
    EXPECT_TRUE(JAUT_UPP_MISC_CONCAT(tr, ue));
    EXPECT_FALSE(JAUT_UPP_MISC_CONCAT(fal, se));
}

TEST(PreprocessorSuite, TestArgsMacros)
{
    EXPECT_EQ(JAUT_UPP_ARGS_GET_LENGTH(0, 1, 2, 3, 4, 5, 6, 7, 8, 9), 10);
    
    
    
    #define TEST_OVERLOAD_1(PAR1) true
    #define TEST_OVERLOAD_2(PAR1, PAR2) false
    #define TEST_OVERLOAD(...) JAUT_UPP_ARGS_OVERLOAD(TEST_OVERLOAD, __VA_ARGS__)
    
    EXPECT_TRUE (TEST_OVERLOAD(0));
    EXPECT_FALSE(TEST_OVERLOAD(0, 1));
    
    
    
    constexpr std::array d1 { 0, 1, 2, 3, 4, 5, 6 };
    constexpr std::array d2 { JAUT_UPP_ARGS_REVERSE(6,5,4,3,2,1,0) };
    EXPECT_EQ(d1, d2);
    
    
    
    #define LIST 23, 43, 6346, 32 ,53253, 523, 55325
    
    EXPECT_EQ(JAUT_UPP_ARGS_GET_LAST(LIST), 55325);
    EXPECT_EQ(JAUT_UPP_ARGS_GET_AT(3, LIST), 6346);
    
    
    
    #define PREPEND_D(D) "d" D
    constexpr std::array s1 { "d0", "d1", "d2", "d3", "d4", "d5", "d6" };
    constexpr std::array s2 { JAUT_UPP_ARGS_FOR_EACH(PREPEND_D, "0", "1", "2", "3", "4", "5", "6") };
    EXPECT_EQ(s1, s2);
    
    
    #define CONCAT_STRING_SNAKE_CASE(S) "_" S
    constexpr std::string_view string = JAUT_UPP_ARGS_FOR_EACH_UNCO(CONCAT_STRING_SNAKE_CASE, "THIS", "IS", "A", "STRING");
    EXPECT_STREQ(string.data(), "_THIS_IS_A_STRING");
    
    
    constexpr std::array s21 { "d0", "d1", "d2", "d3", "4", "5", "6" };
    constexpr std::array s22 { JAUT_UPP_ARGS_FOR_N(4, PREPEND_D, "0", "1", "2", "3", "4", "5", "6") };
    EXPECT_EQ(s21, s22);
    
    
    constexpr std::array t1 { "4", "5", "6" };
    constexpr std::array t2 { JAUT_UPP_ARGS_TRIM(4, "0", "1", "2", "3", "4", "5", "6") };
    EXPECT_EQ(t1, t2);
    
    
    constexpr std::array r1 { "0", "1", "2", "3" };
    constexpr std::array r2 { JAUT_UPP_ARGS_REMOVE(3, "0", "1", "2", "3", "4", "5", "6") };
    EXPECT_EQ(r1, r2);
    
    
    constexpr std::array l1 { "0", "1", "2", "3" };
    constexpr std::array l2 { JAUT_UPP_ARGS_LIMIT(4, "0", "1", "2", "3", "4", "5", "6") };
    EXPECT_EQ(l1, l2);
    
    
    constexpr std::array g1 = ::fillConstruct<int, 50>(420);    
    constexpr std::array g2 { JAUT_UPP_ARGS_GENERATE(50, 420) };
    EXPECT_EQ(g1, g2);
}

TEST(PreprocessorSuite, TestIfMacros)
{
    EXPECT_TRUE (JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_EQUAL(3, 2, 3, 3)));
    EXPECT_FALSE(JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_EQUAL(2, 2, 3, 5)));
    
    EXPECT_TRUE (JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_GREATER_THAN(1, 2, 3)));
    EXPECT_FALSE(JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_GREATER_THAN(3, 2, 3)));
    
    EXPECT_TRUE (JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_LESS_THAN(3, 2, 3)));
    EXPECT_FALSE(JAUT_UPP_IF_EVAL(JAUT_UPP_COND_ARGS_LESS_THAN(1, 2, 3)));
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
