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
    
    @author Elanda
    @file   jaut_Numeric.h
    @date   23, March 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/signal/event/jaut_Event.h>
#include <jaut_core/math/jaut_SafeInteger.h>
#include <jaut_core/math/jaut_SafeFloat.h>
#include <jaut_core/util/jaut_TypeTraits.h>

#include <bitset>
#include <cfenv>
#include <stdexcept>
#include <string>
#include <type_traits>



//**********************************************************************************************************************
// region Header
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    namespace detail
    {
        struct NumericUncheckedOp
        {
            //==========================================================================================================
            template<class T>
            struct Result
            {
                //======================================================================================================
                int code{};
                T value;

                //======================================================================================================
                constexpr Result(T val) noexcept // NOLINT
                    : value(val)
                {}
            };

            //==========================================================================================================
            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> cast(U value)
            {
                return static_cast<T>(value);
            }

            //==========================================================================================================
            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> add(T left, U right)
            {
                return (left + right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> sub(T left, U right)
            {
                return (left - right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> mul(T left, U right)
            {
                return (left * right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> div(T left, U right)
            {
                return (left / right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> mod(T left, U right)
            {
                if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>)
                {
                    return std::fmod(left, right);
                }
                else
                {
                    return (left % right);
                }
            }

            //==========================================================================================================
            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> shiftLeft(T left, U right)
            {
                return (left << right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> shiftRight(T left, U right)
            {
                return (left >> right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> bitAnd(T left, U right)
            {
                return (left & right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> bitOr(T left, U right)
            {
                return (left | right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static Result<T> bitXor(T left, U right)
            {
                return (left ^ right);
            }

            //==========================================================================================================
            template<class T>
            JAUT_NODISCARD
            static Result<T&> preInc(T &value)
            {
                return ++value;
            }

            template<class T>
            JAUT_NODISCARD
            static Result<T> postInc(T &value)
            {
                return value++;
            }

            template<class T>
            JAUT_NODISCARD
            static Result<T&> preDec(T &value)
            {
                return --value;
            }

            template<class T>
            JAUT_NODISCARD
            static Result<T> postDec(T &value)
            {
                return value--;
            }

            //==========================================================================================================
            template<class T, class U>
            JAUT_NODISCARD
            static bool equals(T left, U right)
            {
                return (left == right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static bool notEquals(T left, U right)
            {
                return (left != right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static bool greaterThan(T left, U right)
            {
                return (left > right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static bool smallerThan(T left, U right)
            {
                return (left < right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static bool greaterThanOrEquals(T left, U right)
            {
                return (left >= right);
            }

            template<class T, class U>
            JAUT_NODISCARD
            static bool smallerThanOrEquals(T left, U right)
            {
                return (left <= right);
            }
        };
    }
    
    //==================================================================================================================
    /**
     *  This enum provides a list of flags that can be passed to jaut::Numeric as a switch to enable
     *  certain value-modification checks and preventions.
     */
    enum class JAUT_API NumericCheck
    {
        /** This determines whether the numeric should check for value changes. */
        Change,

        /** This determines whether any modification to the numeric should be error checked. (overflow and zero div) */
        ArithmeticError
    };

    //==================================================================================================================
    /**
     *  Checks whether T is a valid type for this wrapper.
     *  Valid types are any integral (non-boolean) and floating point types.
     *
     *  @return True if the type is valid, false otherwise
     */
    template<class T>
    struct JAUT_API isValidNumeric
    {
        static constexpr bool value = (((std::is_integral_v<T> && !std::is_same_v<T, bool>)
                                        || std::is_floating_point_v<T>) );
    };

    /**
     *  Checks whether T and U are valid operations for a bit-wise expression.
     *  This will be true if none of both types is a floating point number.
     *
     *  @return True if the type is valid, false otherwise
     */
    template<class T, class U>
    struct JAUT_API allowsBitOperations
    {
        static constexpr bool value = std::is_integral_v<T> && std::is_integral_v<U>;
    };

    /**
     *  Checks whether T is a valid type for the jaut::Numeric class.
     *  Valid types are any integral (non-boolean) and floating point types.
     *
     *  @return True if the type is valid, false otherwise
     */
    template<class T>
    JAUT_API inline constexpr bool isValidNumeric_v = isValidNumeric<T>::value;

    /**
     *  Checks whether T and U are valid operations for a bit-wise expression.
     *  This will be true if none of both types is a floating point number.
     *
     *  @return True if the type is valid, false otherwise
     */
    template<class T, class U>
    JAUT_API inline constexpr bool allowsBitOperations_v = allowsBitOperations<T, U>::value;

    //==================================================================================================================
    /**
     *  An numeric base class for classes that need to be arithmetically operable.
     *  It can also be used as a direct wrapper for numeric types.
     *
     *  Note that, if for integral types overflow-checking is enabled, this will incur a heavy runtime-cost.
     *  As every modification to this object needs to be first converted to a juce::BigInteger object which has to
     *  dynamically allocate memory for it to be able to store bigger numbers.
     *
     *  @tparam NumericType The numeric type this class is managing
     *  @tparam CheckFlags  A list of flags determining the value checks to make before and after any modification
     *
     *  @see jaut::NumericCheck
     */
    template<class T, NumericCheck... CheckFlags>
    class JAUT_API Numeric
    {
    public:
        using ValueHandler = EventHandler<const Numeric&, T>;
        using ErrorHandler = EventHandler<const Numeric&>;

        //==============================================================================================================
        /**
         *  Dispatched whenever the value has changed.
         *  @param par1 Reference to the numeric object raising the event
         *  @param par2 The old value before the modification
         */
        Event<ValueHandler> ValueChanged;

        /**
         *  Dispatched whenever the number overflowed.
         *  @param par Reference to the numeric object raising the event
         */
        Event<ErrorHandler> ValueOverflowed;

        /**
         *  Dispatched whenever the user tried to divide by zero.
         *  @param par Reference to the numeric object raising the event
         */
        Event<ErrorHandler> DividedByZero;

        //==============================================================================================================
        /** The underlying value of the Numeric wrapper. */
        T numericValue{};

        //==============================================================================================================
        /** Constructs a new instance with numericValue default initialised. */
        constexpr Numeric() noexcept = default;

        /**
         *  Tries to construct this numeric object from the given numerical value.
         *  If the value didn't fit and error checking is enabled, it will be initialised with T().
         *  
         *  @param value The initial value
         */
        template<class U>
        constexpr Numeric(U value);// NOLINT

        constexpr Numeric(const Numeric &other) noexcept;
        constexpr Numeric(Numeric &&other) noexcept;

        virtual ~Numeric() = default;

        //==============================================================================================================
        Numeric &operator=(Numeric right);

        /**
         *  Tries to assign this numeric object from the given numerical value.
         *  If the value didn't fit and error checking is enabled, it will be left unchanged.
         *  
         *  @param value The initial value
         */
        template<class U>
        Numeric &operator=(U right);

        //==============================================================================================================
        /**
         *  Tries to return this numeric's value as a wchar_t,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return wchar_t The converted value
         */
        JAUT_NODISCARD
        explicit operator wchar_t() const;

        /**
         *  Tries to return this numeric's value as a char,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator char() const;

        /**
         *  Tries to return this numeric's value as an unsigned char,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator unsigned char() const;

        /**
         *  Tries to return this numeric's value as a short,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator short() const;

        /**
         *  Tries to return this numeric's value as an int,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator int() const;

        /**
         *  Tries to return this numeric's value as a long,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator long() const;

        /**
         *  Tries to return this numeric's value as a long long,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator long long() const;

        /**
         *  Tries to return this numeric's value as an unsigned short,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator unsigned short() const;

        /**
         *  Tries to return this numeric's value as an unsigned int,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator unsigned int() const;

        /**
         *  Tries to return this numeric's value as an unsigned long,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator unsigned long() const;

        /**
         *  Tries to return this numeric's value as an unsigned long long,
         *  or 0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator unsigned long long() const;

        /**
         *  Tries to return this numeric's value as a float,
         *  or 0.0f if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator float() const;

        /**
         *  Tries to return this numeric's value as a double,
         *  or 0.0 if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator double() const;

        /**
         *  Tries to return this numeric's value as a long double,
         *  or 0.0L if the value didn't fit and error checking is enabled.
         *  
         *  @return The converted value
         */
        JAUT_NODISCARD
        explicit operator long double() const;

        /**
         *  Returns the numeric's value as a bool.
         *  @return True if numericValue == T(), false otherwise
         */
        JAUT_NODISCARD
        explicit operator bool() const noexcept;

        //==============================================================================================================
        /**
         *  Adds value to this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to add
         *  @return The sum as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator+(U value) const;

        /**
         *  Subtracts value from this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to subtract
         *  @return The difference as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator-(U value) const;

        /**
         *  Multiplies value with this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to multiply
         *  @return The product as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator*(U value) const;

        /**
         *  Divides value by this numeric's value and returns a new jaut::Numeric.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be the same as this'.
         *  
         *  @param value The value to divide
         *  @return The quotient as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator/(U value) const;

        /**
         *  Divides value by this numeric's value and returns a new jaut::Numeric.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be the same as this'.
         *  
         *  @param value The value to divide
         *  @return The remainder as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator%(U value) const;

        /**
         *  Left shifts this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The amount to shift left
         *  @return The value as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator<<(U value) const;

        /**
         *  Right shifts this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The amount to shift right
         *  @return The value as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator>>(U value) const;

        /**
         *  Does an arithmetic AND operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator&(U value) const;

        /**
         *  Does an arithmetic OR operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator|(U value) const;

        /**
         *  Does an arithmetic XOR operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U>
        JAUT_NODISCARD
        Numeric operator^(U value) const;

        /**
         *  Adds value to this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to add
         *  @return The sum as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator+(const Numeric<U, Flags...> &value) const;

        /**
         *  Subtracts value from this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to subtract
         *  @return The difference as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator-(const Numeric<U, Flags...> &value) const;

        /**
         *  Multiplies value with this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The value to multiply
         *  @return The product as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator*(const Numeric<U, Flags...> &value) const;

        /**
         *  Divides value by this numeric's value and returns a new jaut::Numeric.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be the same as this'.
         *  
         *  @param value The value to divide
         *  @return The quotient as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator/(const Numeric<U, Flags...> &value) const;

        /**
         *  Divides value by this numeric's value and returns a new jaut::Numeric.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be the same as this'.
         *  
         *  @param value The value to divide
         *  @return The remainder as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator%(const Numeric<U, Flags...> &value) const;

        /**
         *  Left shifts this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The amount to shift left
         *  @return The value as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator<<(const Numeric<U, Flags...> &value) const;

        /**
         *  Right shifts this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The amount to shift right
         *  @return The value as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator>>(const Numeric<U, Flags...> &value) const;

        /**
         *  Does an arithmetic AND operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator&(const Numeric<U, Flags...> &value) const;

        /**
         *  Does an arithmetic OR operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator|(const Numeric<U, Flags...> &value) const;

        /**
         *  Does an arithmetic XOR operation on this numeric's value and returns a new jaut::Numeric.
         *  If the new value overflows and error checking is enabled, its contents will be the same as this'.
         *  
         *  @param value The bitmask to operate on
         *  @return The value as new object
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        Numeric operator^(const Numeric<U, Flags...> &value) const;

        //==============================================================================================================
        /**
         *  Adds right to this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to add
         *  @return This object
         */
        template<class U>
        Numeric &operator+=(U right);

        /**
         *  Subtracts right from this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to subtract
         *  @return This object
         */
        template<class U>
        Numeric &operator-=(U right);

        /**
         *  Multiplies right with this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to multiply
         *  @return This object
         */
        template<class U>
        Numeric &operator*=(U right);

        /**
         *  Divides numeric's value by right.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be left unchanged.
         *  
         *  @param right The value to divide by
         *  @return This object
         */
        template<class U>
        Numeric &operator/=(U right);

        /**
         *  Divides numeric's value by right and sets this numeric object to the remainder.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be left unchanged.
         *  
         *  @param right The value to divide by
         *  @return This object
         */
        template<class U>
        Numeric &operator%=(U right);

        /**
         *  Left shifts this numeric's by right amount of time.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The amount to shift left
         *  @return This object
         */
        template<class U>
        Numeric &operator<<=(U right);

        /**
         *  Right shifts this numeric's by right amount of time.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The amount to shift right
         *  @return This object
         */
        template<class U>
        Numeric &operator>>=(U right);

        /**
         *  Does an arithmetic AND operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U>
        Numeric &operator&=(U right);

        /**
         *  Does an arithmetic OR operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U>
        Numeric &operator|=(U right);

        /**
         *  Does an arithmetic XOR operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U>
        Numeric &operator^=(U right);

        /**
         *  Adds right to this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to add
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator+=(const Numeric<U, Flags...> &right);

        /**
         *  Subtracts right from this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to subtract
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator-=(const Numeric<U, Flags...> &right);

        /**
         *  Multiplies right with this numeric's value.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The value to multiply
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator*=(const Numeric<U, Flags...> &right);

        /**
         *  Divides numeric's value by right.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be left unchanged.
         *  
         *  @param right The value to divide by
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator/=(const Numeric<U, Flags...> &right);

        /**
         *  Divides numeric's value by right and sets this numeric object to the remainder.
         *  
         *  If the new value overflows or was divided by zero and error checking is enabled,
         *  its contents will be left unchanged.
         *  
         *  @param right The value to divide by
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator%=(const Numeric<U, Flags...> &right);

        /**
         *  Left shifts this numeric's by right amount of time.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The amount to shift left
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator<<=(const Numeric<U, Flags...> &right);

        /**
         *  Right shifts this numeric's by right amount of time.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The amount to shift right
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator>>=(const Numeric<U, Flags...> &right);

        /**
         *  Does an arithmetic AND operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator&=(const Numeric<U, Flags...> &right);

        /**
         *  Does an arithmetic OR operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator|=(const Numeric<U, Flags...> &right);

        /**
         *  Does an arithmetic XOR operation on this numeric's value with right.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @param right The bitmask to operate on
         *  @return This object
         */
        template<class U, NumericCheck... Flags>
        Numeric &operator^=(const Numeric<U, Flags...> &right);

        //==============================================================================================================
        /**
         *  Increments this numeric by 1.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @return This object
         */
        Numeric &operator++();

        /**
         *  Decrements this numeric by 1.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @return This object
         */
        Numeric &operator--();

        /**
         *  Increments this numeric by 1 and returns the old value as new numeric object.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @return A new numeric object containing the value before the increment
         */
        Numeric operator++(int);// NOLINT

        /**
         *  Decrements this numeric by 1 and returns the old value as new numeric object.
         *  If the new value overflows and error checking is enabled, its contents will be left unchanged.
         *  
         *  @return A new numeric object containing the value before the decrement
         */
        Numeric operator--(int);// NOLINT

        //==============================================================================================================
        /**
         *  Compares this numeric's value with the right hand side for equality.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if both are equal 
         */
        template<class U>
        JAUT_NODISCARD
        bool operator==(U right) const;

        /**
         *  Compares this numeric's value with the right hand side for non-equality.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if both are not equal 
         */
        template<class U>
        JAUT_NODISCARD
        bool operator!=(U right) const;

        /**
         *  Determines whether this numeric's value is greater than right.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is greater than right's
         */
        template<class U>
        JAUT_NODISCARD
        bool operator>(U right) const;

        /**
         *  Determines whether this numeric's value is smaller than right.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is smaller than right's
         */
        template<class U>
        JAUT_NODISCARD
        bool operator<(U right) const;

        /**
         *  Determines whether this numeric's value is greater than right or equal.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is greater than right's or equal
         */
        template<class U>
        JAUT_NODISCARD
        bool operator>=(U right) const;

        /**
         *  Determines whether this numeric's value is smaller than right or equal.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is smaller than right's or equal
         */
        template<class U>
        JAUT_NODISCARD
        bool operator<=(U right) const;

        /**
         *  Compares this numeric's value with the right hand side for equality.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if both are equal 
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator==(const Numeric<U, Flags...> &right) const;

        /**
         *  Compares this numeric's value with the right hand side for non-equality.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if both are not equal 
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator!=(const Numeric<U, Flags...> &right) const;

        /**
         *  Determines whether this numeric's value is greater than right.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is greater than right's
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator>(const Numeric<U, Flags...> &right) const;

        /**
         *  Determines whether this numeric's value is smaller than right.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is smaller than right's
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator<(const Numeric<U, Flags...> &right) const;

        /**
         *  Determines whether this numeric's value is greater than right or equal.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is greater than right's or equal
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator>=(const Numeric<U, Flags...> &right) const;

        /**
         *  Determines whether this numeric's value is smaller than right or equal.
         *  Note, if error checking is not enabled, this will not compare UB-friendly.
         *  
         *  @param right The right value to compare against
         *  @return True if this numeric's value is smaller than right's or equal
         */
        template<class U, NumericCheck... Flags>
        JAUT_NODISCARD
        bool operator<=(const Numeric<U, Flags...> &right) const;
        
        //==============================================================================================================
        /**
         *  Tries to get the negative version of this numeric's content.
         *  @return The negative value 
         */
        JAUT_NODISCARD
        Numeric operator-() const;
        
        /**
         *  Returns this numeric's value.
         *  @return The value 
         */
        JAUT_NODISCARD
        Numeric operator+() const;
        
        /**
         *  Gets the boolean representation of this numeric's value and negates it.
         *  @return The negation of this numeric's boolean value
         */
        JAUT_NODISCARD
        bool operator!() const;
        
        //==============================================================================================================
        /**
         *  This will stop the next modification to this value to be announced.
         *  It will reset after each event, so be sure to call it everytime you need the next event to cancel.
         */
        void cancelEvents() noexcept;
        
        //==============================================================================================================
        friend void swap(Numeric &left, Numeric &right) noexcept
        {
            using std::swap;
        
            swap(left.ValueChanged,    right.ValueChanged);
            swap(left.ValueOverflowed, right.ValueOverflowed);
            swap(left.DividedByZero,   right.DividedByZero);
            swap(left.numericValue,    right.numericValue);
            swap(left.dontSend,        right.dontSend);
        }
        
    private:
        static_assert(isValidNumeric_v<T>, JAUT_ASSERT_NUMERIC_TYPE_NOT_NUMERIC);
        
        //==============================================================================================================
        static constexpr bool checksChanges = ((CheckFlags == NumericCheck::Change) || ...);
        static constexpr bool checksErrors  = ((CheckFlags == NumericCheck::ArithmeticError) || ...);
        
        //==============================================================================================================
        template<class U>
        JAUT_NODISCARD
        static constexpr bool useSafeInt() noexcept;
        
        template<class U>
        JAUT_NODISCARD
        static constexpr bool useSafeFloat() noexcept;
        
        //==============================================================================================================
        template<class OperandType>
        using SafeType = TypeLadder_t
        <
            TypeCase<useSafeInt<OperandType>(), SafeInteger>,
            TypeCase<useSafeFloat<OperandType>(), SafeFloat>,
            DefaultCase<detail::NumericUncheckedOp>
        >;
        
        template<class U, class V>
        using ResultType = typename SafeType<U>::template Result<V>;
        
        //==============================================================================================================
        static_assert((!std::is_floating_point_v<T> || std::numeric_limits<T>::is_iec559 || !checksErrors),
                      JAUT_ASSERT_NUMERIC_IEEE_NOT_SUPPORTED);
        
        //==============================================================================================================
        template<class U> JAUT_NODISCARD static ResultType<U, T> add(T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> sub(T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> mul(T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> div(T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> mod(T left, U right);
        
        //==============================================================================================================
        template<class U> JAUT_NODISCARD static ResultType<U, T> shiftLeft (T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> shiftRight(T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> bitAnd    (T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> bitOr     (T left, U right);
        template<class U> JAUT_NODISCARD static ResultType<U, T> bitXor    (T left, U right);
        
        //==============================================================================================================
        template<class U> JAUT_NODISCARD static bool eq (T left, U right);
        template<class U> JAUT_NODISCARD static bool neq(T left, U right);
        template<class U> JAUT_NODISCARD static bool gt (T left, U right);
        template<class U> JAUT_NODISCARD static bool st (T left, U right);
        template<class U> JAUT_NODISCARD static bool gte(T left, U right);
        template<class U> JAUT_NODISCARD static bool ste(T left, U right);
        
        //==============================================================================================================
        mutable bool dontSend{false};
        
        //==============================================================================================================
        template<class U, class V>
        JAUT_NODISCARD
        std::pair<bool, U> cast(V value) const;
        
        template<class U>
        JAUT_NODISCARD
        Numeric& modify(U right, ResultType<U, T> (*func)(T, U));
    };

    //==================================================================================================================
    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator<(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric > value);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator>(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric < value);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator==(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric == value);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator!=(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric != value);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator<=(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric >= value);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline bool operator>=(T value, const Numeric<U, Checks...> &numeric) noexcept
    {
        return (numeric <= value);
    }

    //==================================================================================================================
    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator+(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) += numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator-(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) -= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator*(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) *= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator/(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) /= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<isValidNumeric_v<T>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator%(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) %= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<allowsBitOperations_v<T, U>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator<<(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) <<= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<allowsBitOperations_v<T, U>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator>>(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) >>= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<allowsBitOperations_v<T, U>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator&(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) &= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<allowsBitOperations_v<T, U>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator|(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) |= numeric);
    }

    template<class T, class U, NumericCheck... Checks, class = std::enable_if_t<allowsBitOperations_v<T, U>>>
    JAUT_NODISCARD
    JAUT_API inline Numeric<U, Checks...> operator^(T value, const Numeric<U, Checks...> &numeric)
    {
        return (Numeric<U, Checks...>(value) ^= numeric);
    }
}
//======================================================================================================================
// endregion Header
//**********************************************************************************************************************
// region Implementation
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline constexpr Numeric<T, CheckFlags...>::Numeric(U parValue)
        : numericValue(cast<T>(parValue).second)
    {}
    
    template<class T, NumericCheck ...CheckFlags>
    inline constexpr Numeric<T, CheckFlags...>::Numeric(const Numeric &parOther) noexcept
        : numericValue(parOther.numericValue)
    {}
    
    template<class T, NumericCheck ...CheckFlags>
    inline constexpr Numeric<T, CheckFlags...>::Numeric(Numeric &&parOther) noexcept
    {
        swap(*this, parOther);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator=(Numeric parRight)
    {
        Numeric temp(std::move(parRight));
        
        const T old_value = numericValue;
        swap(*this, temp);
        
        if constexpr (checksChanges)
        {
            if (!dontSend && *this != numericValue)
            {
                ValueChanged.invoke(*this, old_value);
            }
        }
        
        dontSend = false;
        return *this;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator=(U parRight)
    {
        const T old = numericValue;
        const std::pair<bool, T> result = cast<T>(parRight);
        
        if (result.first)
        {
            numericValue = result.second;
        }
        
        if constexpr (checksChanges)
        {
            if (!dontSend && *this != old)
            {
                ValueChanged.invoke(*this, old);
            }
        }
        
        dontSend = false;
        return *this;
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator wchar_t() const
    {
        return cast<wchar_t>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator char() const
    {
        return cast<char>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator unsigned char() const
    {
        return cast<unsigned char>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator short() const
    {
        return cast<short>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator int() const
    {
        return cast<int>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator long() const
    {
        return cast<long>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator long long() const
    {
        return cast<long long>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator unsigned short() const
    {
        return cast<unsigned short>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator unsigned int() const
    {
        return cast<unsigned int>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator unsigned long() const
    {
        return cast<unsigned long>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator unsigned long long() const
    {
        return cast<unsigned long long>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator float() const
    {
        return cast<float>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator double() const
    {
        return cast<double>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator long double() const
    {
        return cast<long double>(numericValue).second;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>::operator bool() const noexcept
    {
        return static_cast<bool>(numericValue);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator+(U parValue) const
    {
        return (Numeric(numericValue) += parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator-(U parValue) const
    {
        return (Numeric(numericValue) -= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator*(U parValue) const
    {
        return (Numeric(numericValue) *= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator/(U parValue) const
    {
        return (Numeric(numericValue) /= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator%(U parValue) const
    {
        return (Numeric(numericValue) %= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator<<(U parValue) const
    {
        return (Numeric(numericValue) <<= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator>>(U parValue) const
    {
        return (Numeric(numericValue) >>= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator&(U parValue) const
    {
        return (Numeric(numericValue) &= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator|(U parValue) const
    {
        return (Numeric(numericValue) |= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator^(U parValue) const
    {
        return (Numeric(numericValue) ^= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator+(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) += parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator-(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) -= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator*(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) *= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator/(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) /= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator%(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) %= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator<<(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) <<= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator>>(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) >>= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator&(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) &= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator|(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) |= parValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator^(const Numeric<U, Flags...> &parValue) const
    {
        return (Numeric(numericValue) ^= parValue);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator+=(U parRight)
    {
        return modify(parRight, Numeric::add);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator-=(U parRight)
    {
        return modify(parRight, Numeric::sub);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator*=(U parRight)
    {
        return modify(parRight, Numeric::mul);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator/=(U parRight)
    {
        return modify(parRight, Numeric::div);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator%=(U parRight)
    {
        return modify(parRight, Numeric::mod);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator<<=(U parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight, Numeric::shiftLeft);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator>>=(U parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight, Numeric::shiftRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator&=(U parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight, Numeric::bitAnd);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator|=(U parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight, Numeric::bitOr);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator^=(U parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight, Numeric::bitXor);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator+=(const Numeric<U, Flags...> &parRight)
    {
        return modify(parRight.numericValue, Numeric::add);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator-=(const Numeric<U, Flags...> &parRight)
    {
        return modify(parRight.numericValue, Numeric::sub);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator*=(const Numeric<U, Flags...> &parRight)
    {
        return modify(parRight.numericValue, Numeric::mul);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator/=(const Numeric<U, Flags...> &parRight)
    {
        return modify(parRight.numericValue, Numeric::div);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator%=(const Numeric<U, Flags...> &parRight)
    {
        return modify(parRight.numericValue, Numeric::mod);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator<<=(const Numeric<U, Flags...> &parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight.numericValue, Numeric::shiftLeft);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator>>=(const Numeric<U, Flags...> &parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight.numericValue, Numeric::shiftRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator&=(const Numeric<U, Flags...> &parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight.numericValue, Numeric::bitAnd);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator|=(const Numeric<U, Flags...> &parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight.numericValue, Numeric::bitOr);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator^=(const Numeric<U, Flags...> &parRight)
    {
        static_assert(!std::is_floating_point_v<T> && !std::is_floating_point_v<U>);
        return modify(parRight.numericValue, Numeric::bitXor);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator++()
    {
        return modify(1, Numeric::add);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::operator--()
    {
        return modify(1, Numeric::sub);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator++(int) // NOLINT
    {
        const T old = numericValue;
        (void) modify(1, Numeric::add);
        return { old };
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator--(int) // NOLINT
    {
        const T old = numericValue;
        (void) modify(1, Numeric::sub);
        return { old };
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator==(U parRight) const
    {
        return eq(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator!=(U parRight) const
    {
        return neq(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator>(U parRight) const
    {
        return gt(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator<(U parRight) const
    {
        return st(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator>=(U parRight) const
    {
        return gte(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::operator<=(U parRight) const
    {
        return ste(numericValue, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator==(const Numeric<U, Flags...> &parRight) const
    {
        return eq(numericValue, parRight.numericValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator!=(const Numeric<U, Flags...> &parRight) const
    {
        return neq(numericValue, parRight.numericValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator>(const Numeric<U, Flags...> &parRight) const
    {
        return gt(numericValue, parRight.numericValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator<(const Numeric<U, Flags...> &parRight) const
    {
        return st(numericValue, parRight.numericValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator>=(const Numeric<U, Flags...> &parRight) const
    {
        return gte(numericValue, parRight.numericValue);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U, NumericCheck ...Flags>
    inline bool Numeric<T, CheckFlags...>::operator<=(const Numeric<U, Flags...> &parRight) const
    {
        return ste(numericValue, parRight.numericValue);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator-() const
    {
        return (Numeric(*this) * -1);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline Numeric<T, CheckFlags...> Numeric<T, CheckFlags...>::operator+() const
    {
        return *this;
    }
    
    template<class T, NumericCheck ...CheckFlags>
    inline bool Numeric<T, CheckFlags...>::operator!() const
    {
        return (!static_cast<bool>(numericValue));
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    inline void Numeric<T, CheckFlags...>::cancelEvents() noexcept
    {
        dontSend = true;
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline constexpr bool Numeric<T, CheckFlags...>::useSafeInt() noexcept
    {
        return (std::is_integral_v<T> && std::is_integral_v<U> && checksErrors);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline constexpr bool Numeric<T, CheckFlags...>::useSafeFloat() noexcept
    {
        return ((std::is_floating_point_v<T> || std::is_floating_point_v<U>) && checksErrors);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::add(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::add(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::sub(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::sub(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::mul(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::mul(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::div(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::div(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::mod(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::mod(parLeft, parRight);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::shiftLeft(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::shiftLeft(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::shiftRight(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::shiftRight(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::bitAnd(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::bitAnd(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::bitOr(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::bitOr(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline auto Numeric<T, CheckFlags...>::bitXor(T parLeft, U parRight)
        -> typename Numeric<T, CheckFlags...>::template ResultType<U, T>
    {
        return SafeType<U>::bitXor(parLeft, parRight);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::eq(T parLeft, U parRight)
    {
        return SafeType<U>::equals(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::neq(T parLeft, U parRight)
    {
        return SafeType<U>::notEquals(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::gt(T parLeft, U parRight)
    {
        return SafeType<U>::greaterThan(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::st(T parLeft, U parRight)
    {
        return SafeType<U>::smallerThan(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::gte(T parLeft, U parRight)
    {
        return SafeType<U>::greaterThanOrEquals(parLeft, parRight);
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline bool Numeric<T, CheckFlags...>::ste(T parLeft, U parRight)
    {
        return SafeType<U>::smallerThanOrEquals(parLeft, parRight);
    }
    
    //==================================================================================================================
    template<class T, NumericCheck ...CheckFlags>
    template<class U, class V>
    inline std::pair<bool, U> Numeric<T, CheckFlags...>::cast(V parValue) const
    {
        using CastType = TypeLadder_t
        <
            TypeCase<std::is_integral_v<V>       && checksErrors, SafeInteger>,
            TypeCase<std::is_floating_point_v<V> && checksErrors, SafeFloat>,
            DefaultCase<detail::NumericUncheckedOp>
        >;
            
        if constexpr (std::is_same_v<U, V>)
        {
            return { true, parValue };
        }
        
        const auto result  = CastType::template cast<U>(parValue);
        bool       success = true;
        
        if constexpr (checksErrors)
        {
            if (result.code == 1)
            {
                success = false;
                
                if (!dontSend)
                {
                    ValueOverflowed.invoke(*this);
                }
            }
        }
        
        dontSend = false;
        return { success, result.value };
    }
    
    template<class T, NumericCheck ...CheckFlags>
    template<class U>
    inline Numeric<T, CheckFlags...>& Numeric<T, CheckFlags...>::modify(U parRight, ResultType<U, T>(*parFunc)(T, U))
    {
        const T old = numericValue;
        const ResultType<U, T> result = (parFunc)(numericValue, parRight);
        numericValue = result.value;
        
        if constexpr (checksErrors)
        {
            if (!dontSend)
            {
                if (result.code == 1)
                {
                    ValueOverflowed.invoke(*this);
                }
                else if (result.code == 2)
                {
                    DividedByZero.invoke(*this);
                }
            }
        }
        
        if constexpr (checksChanges)
        {
            if (!dontSend && (*this != old))
            {
                ValueChanged.invoke(*this, old);
            }
        }
        
        dontSend = false;
        return *this;
    }
}
//======================================================================================================================
// endregion Implementation
//**********************************************************************************************************************
