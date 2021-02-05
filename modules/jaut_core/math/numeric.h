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
    struct JAUT_API NumericEvents
    {
        enum { OnChange = 1, OnOverflow = 2 };
    };
    
    /**
     *  An numeric base class for classes that need to be arithmetically operable.
     *  @tparam NumericType The numeric type this class is managing
     */
    template<class T, int VEvents = NumericEvents::OnChange>
    class JAUT_API Numeric
    {
    private:
        template<class = void> struct OverflowDetector;
    
        template<>
        struct OverflowDetector<std::enable_if_t<std::is_integral_v<T>>>
        {
            juce::BigInteger value;
        
            //==========================================================================================================
            static int checkIfOverflowed(juce::BigInteger value) noexcept
            {
                return value > std::numeric_limits<T>::max() ?  1 :
                       value < std::numeric_limits<T>::min() ? -1 : 0;
            }
        };
    
        //==============================================================================================================
        using Detector = OverflowDetector<>;
        using OfType   = std::conditional_t<(VEvents & NumericEvents::OnOverflow) == NumericEvents::OnOverflow,
                                            juce::BigInteger, T>;
        
    public:
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      JAUT_ASSERT_NUMERIC_TYPE_NOT_NUMERIC);
    
        //==============================================================================================================
        enum class JAUT_API OverflowLimit { Up, Down };
        
        //==============================================================================================================
        using ValueChangedHandler    = EventHandler<Numeric&, T>;
        using ValueOverflowedHandler = EventHandler<Numeric&, T, juce::BigInteger, OverflowLimit>;
        
        /** Dispatched whenever the value has changed. */
        Event<ValueChangedHandler> ValueChanged;
        
        /** Dispatched whenever the number overflowed. */
        Event<ValueOverflowedHandler> ValueOverflowed;
        
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
        Numeric& operator+=(Numeric value) { return modify(OfType(numericValue) + value.numericValue); }
        Numeric& operator-=(Numeric value) { return modify(OfType(numericValue) - value.numericValue); }
        Numeric& operator/=(Numeric value) { return modify(OfType(numericValue) / value.numericValue); }
        Numeric& operator*=(Numeric value) { return modify(OfType(numericValue) * value.numericValue); }
        Numeric& operator%=(Numeric value) { return modify(OfType(numericValue) % value.numericValue); }
        
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
            if constexpr ((VEvents & NumericEvents::OnChange) == NumericEvents::OnChange)
            {
                if (const T old_value = std::exchange(numericValue, newValue); old_value != newValue)
                {
                    ValueChanged.invoke(*this, old_value);
                }
            }
        }
        
        inline Numeric& modify(OfType newValue)
        {
            const bool is_diff = newValue != numericValue;
            T old_value = numericValue;
    
            if constexpr (std::is_same_v<OfType, juce::BigInteger>)
            {
                numericValue = static_cast<T>(newValue.toInt64());
            }
            else
            {
                numericValue = newValue;
            }
            
            if (is_diff && !dontSend)
            {
                if constexpr ((VEvents & NumericEvents::OnChange) == NumericEvents::OnChange)
                {
                    ValueChanged.invoke(*this, old_value);
                }
    
                if constexpr ((VEvents & NumericEvents::OnOverflow) == NumericEvents::OnOverflow)
                {
                    if (const int res = Detector::checkIfOverflowed(newValue))
                    {
                        OverflowLimit limit;
                        
                        if (res < 0)
                        {
                            limit     = OverflowLimit::Down;
                            newValue -= std::numeric_limits<T>::min();
                            newValue.setNegative(false);
                        }
                        else
                        {
                            limit     = OverflowLimit::Up;
                            newValue -= std::numeric_limits<T>::max();
                        }
                        
                        ValueOverflowed.invoke(*this, old_value, newValue, limit);
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
