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

    Copyright (c) 2022 ElandaSunshine
    ===============================================================

    @author Elanda (elanda@elandasunshine.com)
    @file   Numeric.cpp
    @date   15, April 2022

    ===============================================================
 */

#include <gtest/gtest.h>

#include <jaut_core/math/jaut_Numeric.h>
#include <jaut_core/preprocessor/arguments/jaut_UppArgs.h>

#include <limits>
#include <type_traits>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
#define ASSERT_TYPE_BASED_1(VALUE) ASSERT_TYPE_BASED_2(numeric.numericValue, VALUE)

#define ASSERT_TYPE_BASED_2(CHECK, VALUE)                                                      \
    if constexpr (std::is_same_v<TypeParam, double> || std::is_same_v<TypeParam, long double>) \
        ASSERT_DOUBLE_EQ(CHECK, static_cast<TypeParam>(VALUE));                                \
    else if constexpr (std::is_same_v<TypeParam, float>)                                       \
        ASSERT_FLOAT_EQ(CHECK, static_cast<TypeParam>(VALUE));                                 \
    else                                                                                       \
        ASSERT_EQ(CHECK, VALUE)

#define ASSERT_TYPE_BASED(...) JAUT_UPP_ARGS_OVERLOAD(ASSERT_TYPE_BASED, __VA_ARGS__)

#define EXPECT_TYPE_BASED_1(VALUE) EXPECT_TYPE_BASED_2(numeric.numericValue, VALUE)

#define EXPECT_TYPE_BASED_2(CHECK, VALUE)                                                      \
    if constexpr (std::is_same_v<TypeParam, double> || std::is_same_v<TypeParam, long double>) \
        EXPECT_DOUBLE_EQ(CHECK, static_cast<TypeParam>(VALUE));                                \
    else if constexpr (std::is_same_v<TypeParam, float>)                                       \
        EXPECT_FLOAT_EQ(CHECK, static_cast<TypeParam>(VALUE));                                 \
    else                                                                                       \
        EXPECT_EQ(CHECK, VALUE)

#define EXPECT_TYPE_BASED(...) JAUT_UPP_ARGS_OVERLOAD(EXPECT_TYPE_BASED, __VA_ARGS__)

#define GET_OVERFLOW(T)                                                    \
    (std::numeric_limits<TypeParam>::max() > std::numeric_limits<T>::max() \
        ? static_cast<T>(std::numeric_limits<TypeParam>::max())            \
        : numeric.numericValue)

#define GET_OVERFLOW_R(T)                                                  \
    (std::numeric_limits<T>::max() > std::numeric_limits<TypeParam>::max() \
        ? static_cast<TypeParam>(std::numeric_limits<T>::max())            \
        : numeric.numericValue)

#define GET_UNDERFLOW(T) static_cast<T>(std::numeric_limits<TypeParam>::min())

#define GET_UNDERFLOW_R(T) static_cast<TypeParam>(std::numeric_limits<T>::min())

#define EXPECT_ASSIGN_OVERFLOW(T)            \
    numeric = std::numeric_limits<T>::max(); \
    EXPECT_EQ(numeric.numericValue, GET_OVERFLOW_R(T))

#define EXPECT_ASSIGN_UNDERFLOW(T)           \
    numeric = std::numeric_limits<T>::min(); \
    EXPECT_EQ(numeric.numericValue, GET_UNDERFLOW_R(T))

#define ASSIGN_AND_TEST(T, VAL)    \
    numeric = static_cast<T>(VAL); \
    EXPECT_EQ(numeric.numericValue, static_cast<T>(VAL))

//======================================================================================================================
namespace
{
    //******************************************************************************************************************
    // region Fixture Setup
    //==================================================================================================================
    template<class T>
    class NumericFixture : public testing::Test
    {
    public:
        jaut::Numeric<T> numeric;
        jaut::Numeric<T, jaut::NumericCheck::ArithmeticError, jaut::NumericCheck::Change> numericChecked;
    };
    //==================================================================================================================
    // endregion Fixture Setup
    //******************************************************************************************************************
    // region Testing Facilities
    //==================================================================================================================
    using TestedTypes = testing::Types<wchar_t, char, short, int, long, long long, unsigned char, unsigned short,
                                       unsigned int, unsigned long, unsigned long long, float, double, long double>;    
    //==================================================================================================================
    template<class T, class U>
    constexpr T underflow() noexcept { return static_cast<T>(std::numeric_limits<U>::min()); }
    
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TYPED_TEST_SUITE_P(NumericFixture);

//======================================================================================================================
TYPED_TEST_P(NumericFixture, TestArithmetics)
{
    jaut::Numeric<TypeParam> &numeric  = this->numeric;
    ASSERT_EQ(numeric.numericValue, 0) << "Default construction failed";
    
    EXPECT_TYPE_BASED((numeric + 120).numericValue, 120);
    numeric += 120;
    ASSERT_TYPE_BASED(120);
    
    EXPECT_TYPE_BASED((numeric - 60).numericValue, 60);
    numeric -= 60;
    ASSERT_TYPE_BASED(60);
    
    EXPECT_TYPE_BASED((numeric * 2).numericValue, 120);
    numeric *= 2;
    ASSERT_TYPE_BASED(120);
    
    EXPECT_TYPE_BASED((numeric / 4).numericValue, 30);
    numeric /= 4;
    ASSERT_TYPE_BASED(30);
    
    EXPECT_TYPE_BASED((numeric % 2).numericValue, 0);
    numeric %= 2;
    ASSERT_TYPE_BASED(0);
    
    numeric = 50;
    ASSERT_TYPE_BASED(50);
    
    TypeParam in_val;
    
    if constexpr (std::is_unsigned_v<TypeParam>)
    {
        EXPECT_EQ((numeric << 1).numericValue, 100);
        numeric <<= 1;
        ASSERT_EQ(numeric.numericValue, 100);
        
        EXPECT_EQ((numeric >> 2).numericValue, 25);
        numeric >>= 2;
        ASSERT_EQ(numeric.numericValue, 25);
        
        EXPECT_EQ((numeric & 12).numericValue, 8);
        numeric &= 12;
        ASSERT_EQ(numeric.numericValue, 8);
        
        EXPECT_EQ((numeric | 4).numericValue, 12);
        numeric |= 4;
        ASSERT_EQ(numeric.numericValue, 12);
        
        EXPECT_EQ((numeric ^ 6).numericValue, 10);
        numeric ^= 6;
        ASSERT_EQ(numeric.numericValue, 10);
        
        EXPECT_EQ((-numeric).numericValue, static_cast<TypeParam>(-10));
        EXPECT_EQ((+numeric).numericValue, 10);
        
        EXPECT_FALSE(static_cast<TypeParam>(!numeric));
    }
    else
    {
        EXPECT_TYPE_BASED((-numeric).numericValue, -50);
        EXPECT_TYPE_BASED((+numeric).numericValue,  50);
        
        EXPECT_FALSE(static_cast<TypeParam>(!numeric));
    }
    
    numeric = 20;
    ASSERT_TYPE_BASED(20);
    
    ++numeric;
    ASSERT_TYPE_BASED(21);
    
    in_val = (numeric++).numericValue;
    ASSERT_TYPE_BASED(22);
    ASSERT_TYPE_BASED(in_val, 21);
    
    --numeric;
    ASSERT_TYPE_BASED(21);
    
    in_val = (numeric--).numericValue;
    ASSERT_TYPE_BASED(20);
    ASSERT_TYPE_BASED(in_val, 21);
}

TYPED_TEST_P(NumericFixture, TestComparisons)
{
    using NumericT = jaut::Numeric<TypeParam>;
    
    NumericT &numeric = this->numeric;
    ASSERT_EQ(numeric.numericValue, 0) << "Default construction failed";
    
    // Ignore these for floating point.
    // If they work for integers, they will also work for floating point as if they were without
    // the jaut::Numeric class (that means barely, that's why it would be a hassle to test for them)
    if constexpr (!std::is_floating_point_v<TypeParam>)
    {
        numeric = 100;
        
        //=========== Class numeric operators
        // Equals
        EXPECT_TRUE (numeric == 100);
        EXPECT_FALSE(numeric == 101);
        EXPECT_FALSE(numeric ==  99);
        
        // Not equals
        EXPECT_FALSE(numeric != 100);
        EXPECT_TRUE (numeric != 101);
        EXPECT_TRUE (numeric !=  99);
        
        // Greater or equals
        EXPECT_FALSE(numeric >= 101);
        EXPECT_TRUE (numeric >= 100);
        EXPECT_TRUE (numeric >=  99);
        
        // Smaller or equals
        EXPECT_FALSE(numeric <=  99);
        EXPECT_TRUE (numeric <= 100);
        EXPECT_TRUE (numeric <= 101);
        
        //=========== Class object operators
        EXPECT_TRUE (numeric == NumericT{ 100 });
        EXPECT_FALSE(numeric == NumericT{ 101 });
        EXPECT_FALSE(numeric == NumericT{  99 });
        
        // Not equals
        EXPECT_FALSE(numeric != NumericT{ 100 });
        EXPECT_TRUE (numeric != NumericT{ 101 });
        EXPECT_TRUE (numeric != NumericT{  99 });
        
        // Greater or equals
        EXPECT_FALSE(numeric >= NumericT{ 101 });
        EXPECT_TRUE (numeric >= NumericT{ 100 });
        EXPECT_TRUE (numeric >= NumericT{  99 });
        
        // Smaller or equals
        EXPECT_FALSE(numeric <= NumericT{  99 });
        EXPECT_TRUE (numeric <= NumericT{ 100 });
        EXPECT_TRUE (numeric <= NumericT{ 101 });
        
        //=========== Global operators
        EXPECT_TRUE (100 == numeric);
        EXPECT_FALSE(101 == numeric);
        EXPECT_FALSE( 99 == numeric);
        
        // Not equals
        EXPECT_FALSE(100 != numeric);
        EXPECT_TRUE (101 != numeric);
        EXPECT_TRUE ( 99 != numeric);
        
        // Greater or equals
        EXPECT_FALSE(99  >= numeric);
        EXPECT_TRUE (100 >= numeric);
        EXPECT_TRUE (101 >= numeric);
        
        // Smaller or equals
        EXPECT_FALSE(101 <= numeric);
        EXPECT_TRUE (100 <= numeric);
        EXPECT_TRUE (99  <= numeric);
    }
    
    numeric = 60;
    
    //=========== Class numeric operators
    // Smaller than
    EXPECT_TRUE (numeric < 61);
    EXPECT_FALSE(numeric < 59);
    
    // Greater than
    EXPECT_TRUE (numeric > 59);
    EXPECT_FALSE(numeric > 61);
    
    //=========== Class object operators
    // Smaller than
    EXPECT_TRUE (numeric < NumericT{ 61 });
    EXPECT_FALSE(numeric < NumericT{ 59 });
    
    // Greater than
    EXPECT_TRUE (numeric > NumericT{ 59 });
    EXPECT_FALSE(numeric > NumericT{ 61 });
    
    //=========== Global operators
    EXPECT_TRUE (59 < numeric);
    EXPECT_FALSE(61 < numeric);
    
    EXPECT_TRUE (61 > numeric);
    EXPECT_FALSE(59 > numeric);
}

TYPED_TEST_P(NumericFixture, TestCasts)
{
    using NumericT = jaut::Numeric<TypeParam>;
    
    NumericT &numeric = this->numeric;
    ASSERT_EQ(numeric.numericValue, 0) << "Default construction failed";
    
    // Value casting ===========================
    // Integral overflow casting only safe when converting unsigned integrals or signed numerics to unsigned integrals
    numeric = std::numeric_limits<TypeParam>::max();
    EXPECT_EQ(static_cast<unsigned char>     (numeric), GET_OVERFLOW(unsigned char));
    EXPECT_EQ(static_cast<unsigned short>    (numeric), GET_OVERFLOW(unsigned short));
    EXPECT_EQ(static_cast<unsigned long>     (numeric), GET_OVERFLOW(unsigned long));
    EXPECT_EQ(static_cast<unsigned long long>(numeric), GET_OVERFLOW(unsigned long long));
    
    numeric = std::numeric_limits<TypeParam>::min();
    EXPECT_EQ(static_cast<unsigned char>     (numeric), GET_UNDERFLOW(unsigned char));
    EXPECT_EQ(static_cast<unsigned short>    (numeric), GET_UNDERFLOW(unsigned short));
    EXPECT_EQ(static_cast<unsigned long>     (numeric), GET_UNDERFLOW(unsigned long));
    EXPECT_EQ(static_cast<unsigned long long>(numeric), GET_UNDERFLOW(unsigned long long));
    
    numeric = 100;
    EXPECT_EQ(static_cast<wchar_t>      (numeric), L'd');
    EXPECT_EQ(static_cast<char>         (numeric),  'd');
    EXPECT_EQ(static_cast<unsigned char>(numeric),  'd');
    
    EXPECT_EQ(static_cast<short>    (numeric), 100);
    EXPECT_EQ(static_cast<int>      (numeric), 100);
    EXPECT_EQ(static_cast<long>     (numeric), 100l);
    EXPECT_EQ(static_cast<long long>(numeric), 100ll);
    
    EXPECT_EQ(static_cast<unsigned short>    (numeric), 100u);
    EXPECT_EQ(static_cast<unsigned int>      (numeric), 100u);
    EXPECT_EQ(static_cast<unsigned long>     (numeric), 100ul);
    EXPECT_EQ(static_cast<unsigned long long>(numeric), 100ull);
    
    EXPECT_FLOAT_EQ (static_cast<float>      (numeric), 100.0f);
    EXPECT_DOUBLE_EQ(static_cast<double>     (numeric), 100);
    EXPECT_DOUBLE_EQ(static_cast<long double>(numeric), 100.0l);
    
    // Type casting ===========================
    if constexpr(std::is_unsigned_v<TypeParam>)
    {
        EXPECT_ASSIGN_OVERFLOW(wchar_t);
        EXPECT_ASSIGN_OVERFLOW(char);
        EXPECT_ASSIGN_OVERFLOW(short);
        EXPECT_ASSIGN_OVERFLOW(int);
        EXPECT_ASSIGN_OVERFLOW(long);
        EXPECT_ASSIGN_OVERFLOW(long long);
        EXPECT_ASSIGN_OVERFLOW(unsigned char);
        EXPECT_ASSIGN_OVERFLOW(unsigned short);
        EXPECT_ASSIGN_OVERFLOW(unsigned int);
        EXPECT_ASSIGN_OVERFLOW(unsigned long);
        EXPECT_ASSIGN_OVERFLOW(unsigned long long);
        EXPECT_ASSIGN_OVERFLOW(float);
        EXPECT_ASSIGN_OVERFLOW(double);
        EXPECT_ASSIGN_OVERFLOW(long double);
        
        EXPECT_ASSIGN_UNDERFLOW(wchar_t);
        EXPECT_ASSIGN_UNDERFLOW(char);
        EXPECT_ASSIGN_UNDERFLOW(short);
        EXPECT_ASSIGN_UNDERFLOW(int);
        EXPECT_ASSIGN_UNDERFLOW(long);
        EXPECT_ASSIGN_UNDERFLOW(long long);
        EXPECT_ASSIGN_UNDERFLOW(unsigned char);
        EXPECT_ASSIGN_UNDERFLOW(unsigned short);
        EXPECT_ASSIGN_UNDERFLOW(unsigned int);
        EXPECT_ASSIGN_UNDERFLOW(unsigned long);
        EXPECT_ASSIGN_UNDERFLOW(unsigned long long);
        EXPECT_ASSIGN_UNDERFLOW(float);
        EXPECT_ASSIGN_UNDERFLOW(double);
        EXPECT_ASSIGN_UNDERFLOW(long double);
    }
    else
    {
        ASSIGN_AND_TEST(wchar_t,       L'a');
        ASSIGN_AND_TEST(char,           'a');
        ASSIGN_AND_TEST(unsigned char,  'a');
        
        ASSIGN_AND_TEST(short,     100);
        ASSIGN_AND_TEST(int,       100);
        ASSIGN_AND_TEST(long,      100l);
        ASSIGN_AND_TEST(long long, 100ll);
        
        ASSIGN_AND_TEST(unsigned short,     100u);
        ASSIGN_AND_TEST(unsigned int,       100u);
        ASSIGN_AND_TEST(unsigned long,      100ul);
        ASSIGN_AND_TEST(unsigned long long, 100ull);
        
        numeric = 100.0f;
        EXPECT_FLOAT_EQ(numeric.numericValue, 100.0f);
        
        numeric = 100.0;
        EXPECT_DOUBLE_EQ(numeric.numericValue, 100.0);
        
        numeric = 100.0l;
        EXPECT_DOUBLE_EQ(numeric.numericValue, 100.0l);
    }
}

TYPED_TEST_P(NumericFixture, TestArithmeticError)
{
    using NumericT = jaut::Numeric<TypeParam, jaut::NumericCheck::ArithmeticError, jaut::NumericCheck::Change>;
    
    const TypeParam init     = std::numeric_limits<TypeParam>::max();
    bool has_overflowed      = false;
    bool was_divided_by_zero = false;
    
    auto on_overflow = [&has_overflowed](const NumericT&) mutable
    {
        has_overflowed = true;
    };
    
    auto on_zero_div = [&was_divided_by_zero](const NumericT&) mutable
    {
        was_divided_by_zero = true;
    };
    
    NumericT &numeric = this->numericChecked;
    numeric.ValueOverflowed += jaut::makeHandler(on_overflow);
    numeric.DividedByZero   += jaut::makeHandler(on_zero_div);
    numeric = init;
    
    if constexpr (!std::is_floating_point_v<TypeParam>)
    {
        ++numeric;
    }
    else
    {
        numeric += init;
    }
    
    EXPECT_TYPE_BASED(init);
    ASSERT_TRUE(has_overflowed);
    
    numeric /= 0;
    
    EXPECT_TYPE_BASED(init);
    ASSERT_TRUE(has_overflowed);
}

TYPED_TEST_P(NumericFixture, TestChangeDetector)
{
    using NumericT = jaut::Numeric<TypeParam, jaut::NumericCheck::ArithmeticError, jaut::NumericCheck::Change>;

    bool change_toggle = false;
    auto on_change   = [&change_toggle](const NumericT&, TypeParam) mutable
    {
        change_toggle = !change_toggle;
    };
    
    NumericT &numeric = this->numericChecked;
    numeric.ValueChanged += jaut::makeHandler(on_change);
    
    numeric = TypeParam(12);
    EXPECT_TRUE(change_toggle);
    
    numeric += 2;
    EXPECT_FALSE(change_toggle);
    
    ++numeric;
    EXPECT_TRUE(change_toggle);
    
    numeric++;
    EXPECT_FALSE(change_toggle);
    
    numeric.cancelEvents();
    EXPECT_FALSE(change_toggle);
}

//======================================================================================================================
REGISTER_TYPED_TEST_SUITE_P(NumericFixture, TestArithmetics, TestComparisons, TestCasts, TestArithmeticError,
                                            TestChangeDetector);
INSTANTIATE_TYPED_TEST_SUITE_P(NumericTests, NumericFixture, ::TestedTypes,);
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
