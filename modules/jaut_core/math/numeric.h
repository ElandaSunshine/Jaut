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
    struct JAUT_API NumericChecks
    {
        enum { CheckForChanges = 1, CheckForOverflow = 2, CheckForZeroDivision };
    };
    
    /**
     *  An numeric base class for classes that need to be arithmetically operable.
     *  @tparam NumericType The numeric type this class is managing
     */
    template<class T, int VChecks = NumericChecks::CheckForChanges>
    class JAUT_API Numeric
    {
    private:
        struct OverflowDetector
        {
            static int checkIfOverflowed(const juce::BigInteger &value)
            {
                return value < std::numeric_limits<T>::min() ? -1 : (value > std::numeric_limits<T>::max());
            }
    
            template<class T2>
            static int checkIfOverflowed(T2)
            {
                return std::fetestexcept(FE_UNDERFLOW) ? -1 : (std::fetestexcept(FE_OVERFLOW) > 0);
            }
        };
        
        //==============================================================================================================
        using Detector = OverflowDetector;
        using OfType   = std::conditional_t<(VChecks & NumericChecks::CheckForOverflow)
                                                == NumericChecks::CheckForOverflow
                                            && std::is_integral_v<T>, juce::BigInteger, T>;
        
    public:
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      JAUT_ASSERT_NUMERIC_TYPE_NOT_NUMERIC);
        
        //==============================================================================================================
        enum class JAUT_API OverflowLimit { Down = -1, Up = 1 };
        
        //==============================================================================================================
        using ValueChangedHandler    = EventHandler<Numeric&, T>;
        using ValueOverflowedHandler = std::conditional_t<std::is_integral_v<T>,
                                                          EventHandler<Numeric&, T, juce::BigInteger, OverflowLimit>,
                                                          EventHandler<Numeric&, T, OverflowLimit>>;
        using DividedByZeroHandler   = EventHandler<Numeric&>;
        
        /** Dispatched whenever the value has changed. */
        Event<ValueChangedHandler> ValueChanged;
        
        /** Dispatched whenever the number overflowed. */
        Event<ValueOverflowedHandler> ValueOverflowed;
        
        /** Dispatched whenever the user tried to divide by zero. */
        Event<DividedByZeroHandler> DividedByZero;
        
        //==============================================================================================================
        T numericValue {};
    
        //==============================================================================================================
        constexpr Numeric() noexcept = default;
        
        constexpr Numeric(T value)
            : numericValue(std::move(value))
        {}
        
        Numeric(const Numeric &other) noexcept
            : numericValue(other.numericValue)
        {}
        
        virtual ~Numeric() = default;
    
        //==============================================================================================================
        Numeric& operator=(const Numeric &right) noexcept
        {
            postChangeEvent(right.numericValue);
            return *this;
        }
        
        explicit operator T() const noexcept
        {
            return numericValue;
        }
    
        //==============================================================================================================
        Numeric operator+(Numeric value) const noexcept { return numericValue + value.numericValue; }
        Numeric operator-(Numeric value) const noexcept { return numericValue - value.numericValue; }
        Numeric operator/(Numeric value) const noexcept { return numericValue / value.numericValue; }
        Numeric operator*(Numeric value) const noexcept { return numericValue * value.numericValue; }
        Numeric operator%(Numeric value) const noexcept { return numericValue % value.numericValue; }
        
        Numeric operator+(T value) const noexcept { return numericValue + value; }
        Numeric operator-(T value) const noexcept { return numericValue - value; }
        Numeric operator/(T value) const noexcept { return numericValue / value; }
        Numeric operator*(T value) const noexcept { return numericValue * value; }
        Numeric operator%(T value) const noexcept { return numericValue % value; }
    
        //==============================================================================================================
        Numeric& operator+=(Numeric value)
        {
            enableOverflowBits();
            return modify(OfType(numericValue) + value.numericValue);
        }
        
        Numeric& operator-=(Numeric value)
        {
            enableOverflowBits();
            return modify(OfType(numericValue) - value.numericValue);
        }
        
        Numeric& operator/=(Numeric value)
        {
            const bool dont_send = dontSend;
            
            enableOverflowBits();
            (void) modify(OfType(numericValue) / value.numericValue);
    
            if constexpr ((VChecks & NumericChecks::CheckForZeroDivision) == NumericChecks::CheckForZeroDivision)
            {
                if (value == 0 && !dont_send)
                {
                    DividedByZero.invoke(*this);
                }
            }
            
            return *this;
        }
        
        Numeric& operator*=(Numeric value)
        {
            enableOverflowBits();
            return modify(OfType(numericValue) * value.numericValue);
        }
        
        Numeric& operator%=(Numeric value)
        {
            enableOverflowBits();
            return modify(OfType(numericValue) % value.numericValue);
        }
        
        /*
        Numeric& operator+=(T value) { return modify(static_cast<OfType>(numericValue) + value); }
        Numeric& operator-=(T value) { return modify(static_cast<OfType>(numericValue) - value); }
        Numeric& operator/=(T value) { return modify(static_cast<OfType>(numericValue) / value); }
        Numeric& operator*=(T value) { return modify(static_cast<OfType>(numericValue) * value); }
        Numeric& operator%=(T value) { return modify(static_cast<OfType>(numericValue) % value); }
        */
        
        //==============================================================================================================
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
    
        //==============================================================================================================
        bool operator==(const Numeric &right) const noexcept { return numericValue == right.numericValue;  }
        bool operator!=(const Numeric &right) const noexcept { return numericValue != right.numericValue;  }
        bool operator> (const Numeric &right) const noexcept { return numericValue > right.numericValue;   }
        bool operator< (const Numeric &right) const noexcept { return numericValue < right.numericValue;   }
        bool operator>=(const Numeric &right) const noexcept { return (*this) > right || (*this) == right; }
        bool operator<=(const Numeric &right) const noexcept { return (*this) < right || (*this) == right; }
    
        //==============================================================================================================
        /**
         *  This will stop the next modification to this value to be announced.
         *  It will reset after each event, so be sure to call it everytime you need the next event to cancel.
         */
        void cancelEvents() noexcept { dontSend = true; }
        
    private:
        bool dontSend;
    
        //==============================================================================================================
        void postChangeEvent(T newValue)
        {
            if constexpr ((VChecks & NumericChecks::CheckForChanges) == NumericChecks::CheckForChanges)
            {
                if (const T old_value = std::exchange(numericValue, newValue); old_value != newValue)
                {
                    ValueChanged.invoke(*this, old_value);
                }
            }
        }
        
        void enableOverflowBits()
        {
            if constexpr (std::is_floating_point_v<T>
                          && (VChecks & NumericChecks::CheckForOverflow) == NumericChecks::CheckForOverflow)
            {
                if (!dontSend)
                {
                    std::feclearexcept(FE_OVERFLOW);
                    std::feclearexcept(FE_UNDERFLOW);
                }
            }
        }
        
        Numeric& modify(OfType newValue)
        {
            const bool is_diff = newValue != numericValue;
            T old_value = numericValue;
    
            if constexpr (std::is_same_v<OfType, juce::BigInteger>)
            {
                using CastType = std::conditional_t<std::is_unsigned_v<T>, std::uint64_t, std::int64_t>;
                numericValue   = std::clamp<CastType>(static_cast<CastType>(newValue.toInt64()),
                                                      std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
            }
            else
            {
                numericValue = newValue;
            }
            
            if (is_diff && !dontSend)
            {
                if constexpr ((VChecks & NumericChecks::CheckForChanges) == NumericChecks::CheckForChanges)
                {
                    ValueChanged.invoke(*this, old_value);
                }
    
                if constexpr ((VChecks & NumericChecks::CheckForOverflow) == NumericChecks::CheckForOverflow)
                {
                    if (const int res = Detector::checkIfOverflowed(newValue))
                    {
                        const OverflowLimit limit = static_cast<OverflowLimit>(res);
                        
                        if constexpr (std::is_integral_v<T>)
                        {
                            if (limit == OverflowLimit::Down)
                            {
                                newValue -= std::numeric_limits<T>::min();
                                newValue.setNegative(false);
                            }
                            else
                            {
                                newValue -= std::numeric_limits<T>::max();
                            }
    
                            ValueOverflowed.invoke(*this, old_value, newValue, limit);
                        }
                        else
                        {
                            ValueOverflowed.invoke(*this, old_value, limit);
                        }
                    }
                }
            }
            
            dontSend = false;
            return *this;
        }
    };
    
    //==================================================================================================================
    #define JAUT_INTERN_NUMERIC_TYPE_LIST bool, char16_t, char32_t, wchar_t, \
                                          char, short, int, long, long long, \
                                          unsigned char, unsigned short, unsigned int, unsigned long, \
                                          unsigned long long, float, double, long double
    
    #define JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, OP) \
            template<class T> \
            JAUT_API inline bool operator OP(TYPE value, const Numeric<T> &numeric) noexcept \
            { return value OP numeric.numeric; }
    
    //==================================================================================================================
    #define JAUT_INTERN_NUMERIC_OP_SIG_IS(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, ==)
    #define JAUT_INTERN_NUMERIC_OP_SIG_NOT(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, !=)
    #define JAUT_INTERN_NUMERIC_OP_SIG_LT(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE,  <)
    #define JAUT_INTERN_NUMERIC_OP_SIG_GT(TYPE)  JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE,  >)
    #define JAUT_INTERN_NUMERIC_OP_SIG_IOL(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, <=)
    #define JAUT_INTERN_NUMERIC_OP_SIG_IOG(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, >=)
    
    //==================================================================================================================
    #define JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(OP) \
            JAUT_UPP_ARGS_UNCOMMATISE(JAUT_UPP_ARGS_FOR_EACH(JAUT_INTERN_NUMERIC_OP_SIG_##OP, \
                                                             JAUT_INTERN_NUMERIC_TYPE_LIST))
    
    //==================================================================================================================
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IS)  // operator==
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(NOT) // operator!=
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(LT)  // operator<
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(GT)  // operator>
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IOL) // operator<=
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(IOG) // operator>=
    
    //==================================================================================================================
    #undef JAUT_INTERN_NUMERIC_FUNC_BODY
    #undef JAUT_INTERN_NUMERIC_OP_SIG_IS
    #undef JAUT_INTERN_NUMERIC_OP_SIG_NOT
    #undef JAUT_INTERN_NUMERIC_OP_SIG_LT
    #undef JAUT_INTERN_NUMERIC_OP_SIG_GT
    #undef JAUT_INTERN_NUMERIC_OP_SIG_IOL
    #undef JAUT_INTERN_NUMERIC_OP_SIG_IOG
    
    
    
    // Value mods
    //==================================================================================================================
    #define JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, OP) \
            template<class T> \
            JAUT_API inline T operator OP(TYPE value, const Numeric<T> &numeric) noexcept \
            { return value OP numeric.numeric; }
    
    //==================================================================================================================
    #define JAUT_INTERN_NUMERIC_OP_SIG_ADD(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, +)
    #define JAUT_INTERN_NUMERIC_OP_SIG_SUB(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, -)
    #define JAUT_INTERN_NUMERIC_OP_SIG_DIV(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, /)
    #define JAUT_INTERN_NUMERIC_OP_SIG_MUL(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, *)
    #define JAUT_INTERN_NUMERIC_OP_SIG_MOD(TYPE) JAUT_INTERN_NUMERIC_FUNC_BODY(TYPE, %)
    
    //==================================================================================================================
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(ADD) // operator+
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(SUB) // operator-
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(DIV) // operator/
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(MUL) // operator*
    JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS(MOD) // operator%
    
    //==================================================================================================================
    #undef JAUT_INTERN_NUMERIC_FUNC_BODY
    #undef JAUT_INTERN_NUMERIC_OP_SIG_ADD
    #undef JAUT_INTERN_NUMERIC_OP_SIG_SUB
    #undef JAUT_INTERN_NUMERIC_OP_SIG_DIV
    #undef JAUT_INTERN_NUMERIC_OP_SIG_MUL
    #undef JAUT_INTERN_NUMERIC_OP_SIG_MOD
    
    //==================================================================================================================
    #undef JAUT_INTERN_NUMERIC_TYPE_LIST
    #undef JAUT_INTERN_MAKE_CONDITIONAL_NUMERICS
}
