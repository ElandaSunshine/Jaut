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
    
    @author Elanda
    @file   numeric.h
    @date   23, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
// region NumericDecl
/**
 *  An numeric base class for classes that need to be arithmetically operable.
 *  @tparam NumericType The numeric type this class is managing
 */
template<class NumericType, juce::NotificationType DispatchEvent = juce::dontSendNotification>
struct JAUT_API Numeric
{
    //==================================================================================================================
    static_assert(std::is_integral_v<NumericType> || std::is_floating_point_v<NumericType>,
                  JAUT_ASSERT_NUMERIC_TYPE_NOT_NUMERIC);
    
    //==================================================================================================================
    using ValueChangedHandler = EventHandler<NumericType>;
    Event<ValueChangedHandler> ValueChanged;
    
    //==================================================================================================================
    NumericType numericValue { };
    
    //==================================================================================================================
    constexpr Numeric() noexcept = default;
    
    constexpr Numeric(NumericType value)
        : numericValue(std::move(value))
    {}
    
    Numeric(const Numeric &other) noexcept
        : numericValue(other.numericValue)
    {}
    
    Numeric(Numeric &&other) noexcept
        : numericValue(other.numericValue)
    {}
    
    virtual ~Numeric() = default;
    
    //==================================================================================================================
    Numeric &operator=(const Numeric &right) noexcept
    {
        auto temp(right.numericValue);
        std::swap(numericValue, temp);
        postChangeEvent();
        return *this;
    }
    
    Numeric &operator=(Numeric &&right) noexcept
    {
        std::swap(numericValue, right.numericValue);
        postChangeEvent();
        return *this;
    }
    
    //==================================================================================================================
    Numeric &operator=(NumericType value) noexcept
    {
        modifyValue(value);
        return *this;
    }
    
    explicit operator NumericType() const noexcept
    {
        return numericValue;
    }
    
    //==================================================================================================================
    Numeric operator+(Numeric value) const noexcept { return numericValue + value.numericValue; }
    Numeric operator-(Numeric value) const noexcept { return numericValue - value.numericValue; }
    Numeric operator/(Numeric value) const noexcept { return numericValue / value.numericValue; }
    Numeric operator*(Numeric value) const noexcept { return numericValue * value.numericValue; }
    Numeric operator%(Numeric value) const noexcept { return numericValue % value.numericValue; }
    
    Numeric operator+(NumericType value) const noexcept { return numericValue + value; }
    Numeric operator-(NumericType value) const noexcept { return numericValue - value; }
    Numeric operator/(NumericType value) const noexcept { return numericValue / value; }
    Numeric operator*(NumericType value) const noexcept { return numericValue * value; }
    Numeric operator%(NumericType value) const noexcept { return numericValue % value; }
    
    //==================================================================================================================
    Numeric& operator+=(Numeric value) { return modifyValue(numericValue + value.numericValue); }
    Numeric& operator-=(Numeric value) { return modifyValue(numericValue - value.numericValue); }
    Numeric& operator/=(Numeric value) { return modifyValue(numericValue / value.numericValue); }
    Numeric& operator*=(Numeric value) { return modifyValue(numericValue * value.numericValue); }
    Numeric& operator%=(Numeric value) { return modifyValue(numericValue % value.numericValue); }
    
    Numeric& operator+=(NumericType value) { return modifyValue(numericValue + value); }
    Numeric& operator-=(NumericType value) { return modifyValue(numericValue - value); }
    Numeric& operator/=(NumericType value) { return modifyValue(numericValue / value); }
    Numeric& operator*=(NumericType value) { return modifyValue(numericValue * value); }
    Numeric& operator%=(NumericType value) { return modifyValue(numericValue % value); }
    
    //==================================================================================================================
    Numeric& operator++()
    {
        ++numericValue;
        postChangeEvent();
        return *this;
    }
    
    Numeric operator++(int)
    {
        const Numeric old = *this;
        ++numericValue;
        postChangeEvent();
        return old;
    }
    
    Numeric& operator--()
    {
        --numericValue;
        postChangeEvent();
        return *this;
    }
    
    Numeric operator--(int)
    {
        const Numeric old = *this;
        --numericValue;
        postChangeEvent();
        return old;
    }
    
    //==================================================================================================================
    bool operator==(const Numeric &right) const noexcept { return numericValue == right.numericValue; }
    bool operator!=(const Numeric &right) const noexcept { return numericValue != right.numericValue; }
    bool operator> (const Numeric &right) const noexcept { return numericValue > right.numericValue; }
    bool operator< (const Numeric &right) const noexcept { return numericValue < right.numericValue; }
    bool operator>=(const Numeric &right) const noexcept { return (*this) > right || (*this) == right; }
    bool operator<=(const Numeric &right) const noexcept { return (*this) < right || (*this) == right; }
    
private:
    inline void postChangeEvent()
    {
        if constexpr (DispatchEvent != juce::dontSendNotification)
        {
            ValueChanged(numericValue);
        }
    }
    
    inline Numeric &modifyValue(NumericType newValue)
    {
        numericValue = newValue;
        postChangeEvent();
        return *this;
    }
};
// endregion NumericDecl
// region GlobalOps
// Value checks
//======================================================================================================================
#define JAUT_INTERN_NUMERIC_TYPE_LIST bool, char16_t, char32_t, wchar_t, \
                                      char, short, int, long, long long, \
                                      unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, \
                                      float, double, long double

#define JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, OP) \
        template<class NumericType> \
        JAUT_API inline bool operator OP(TYPE value, const Numeric<NumericType> &numeric) noexcept \
        { return value OP numeric.numeric; }

//======================================================================================================================
#define JAUT_INTERN_NUMERIC_OP_SIG_IS(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, ==)
#define JAUT_INTERN_NUMERIC_OP_SIG_NOT(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, !=)
#define JAUT_INTERN_NUMERIC_OP_SIG_LT(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE,  <)
#define JAUT_INTERN_NUMERIC_OP_SIG_GT(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE,  >)
#define JAUT_INTERN_NUMERIC_OP_SIG_IOL(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, <=)
#define JAUT_INTERN_NUMERIC_OP_SIG_IOG(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, >=)

//======================================================================================================================
#define JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(OP) \
        JAUT_UPP_ARGS_UNCOMMATISE(JAUT_UPP_ARGS_FOR_EACH(JAUT_INTERN_NUMERIC_OP_SIG_##OP, JAUT_INTERN_NUMERIC_TYPE_LIST))

//======================================================================================================================
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IS)  // operator==
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(NOT) // operator!=
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(LT)  // operator<
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(GT)  // operator>
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IOL) // operator<=
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IOG) // operator>=

//======================================================================================================================
#undef JAUT_INTERN_NUMERIC_FUNC_BODY
#undef JAUT_INTERN_NUMERIC_OP_SIG_IS
#undef JAUT_INTERN_NUMERIC_OP_SIG_NOT
#undef JAUT_INTERN_NUMERIC_OP_SIG_LT
#undef JAUT_INTERN_NUMERIC_OP_SIG_GT
#undef JAUT_INTERN_NUMERIC_OP_SIG_IOL
#undef JAUT_INTERN_NUMERIC_OP_SIG_IOG



// Value mods
//======================================================================================================================
#define JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, OP) \
        template<class NumericType> \
        JAUT_API inline NumericType operator OP(TYPE value, const Numeric<NumericType> &numeric) noexcept \
        { return value OP numeric.numeric; }

//======================================================================================================================
#define JAUT_INTERN_NUMERIC_OP_SIG_ADD(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, +)
#define JAUT_INTERN_NUMERIC_OP_SIG_SUB(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, -)
#define JAUT_INTERN_NUMERIC_OP_SIG_DIV(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, /)
#define JAUT_INTERN_NUMERIC_OP_SIG_MUL(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, *)
#define JAUT_INTERN_NUMERIC_OP_SIG_MOD(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, %)

//======================================================================================================================
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(ADD) // operator+
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(SUB) // operator-
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(DIV) // operator/
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(MUL) // operator*
JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(MOD) // operator%

//======================================================================================================================
#undef JAUT_INTERN_NUMERIC_FUNC_BODY
#undef JAUT_INTERN_NUMERIC_OP_SIG_ADD
#undef JAUT_INTERN_NUMERIC_OP_SIG_SUB
#undef JAUT_INTERN_NUMERIC_OP_SIG_DIV
#undef JAUT_INTERN_NUMERIC_OP_SIG_MUL
#undef JAUT_INTERN_NUMERIC_OP_SIG_MOD

//======================================================================================================================
#undef JAUT_INTERN_NUMERIC_TYPE_LIST
#undef JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS
// endregion GlobalOps
}
