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
    @file   jaut_SafeFloat.h
    @date   17, April 2022

    ===============================================================
 */
 
#pragma once

#include <cmath>
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/preprocessor/arguments/jaut_UppArgs.h>

#include <cfenv>
#include <limits>
#include <type_traits>
#include <utility>



//**********************************************************************************************************************
// region Header
//======================================================================================================================
namespace jaut
{
    struct JAUT_API SafeFloat
    {
        //==============================================================================================================
        template<class T>
        struct Result
        {
            //==========================================================================================================
            int code;
            T value;
        };

        //==============================================================================================================
        /**
         *  Tries to cast U to T.
         *  Returns the casted value or 0 if the cast went wrong.
         *  
         *  @tparam T The type to cast to
         *  @param value The value to cast to T
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> cast(U value);

        //==============================================================================================================
        /**
         *  Tries to add left with right.
         *  Returns the sum or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> add(T left, U right);

        /**
         *  Tries to subtract right from left.
         *  Returns the difference or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> sub(T left, U right);

        /**
         *  Tries to multiply left with right.
         *  Returns the product or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> mul(T left, U right);

        /**
         *  Tries to divide left by right.
         *  Returns the quotient or left if an overflow or a division-by-zero has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> div(T left, U right);

        /**
         *  Tries to divide left by right.
         *  Returns the remainder or left if an overflow or a division-by-zero has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> mod(T left, U right);

        //==============================================================================================================
        /**
         *  Tries to safely check for equality of left and right.
         *  
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if they are equal, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool equals(T left, U right) noexcept;

        /**
         *  Tries to safely check for non-equality of left and right.
         *  
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if they are not equal, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool notEquals(T left, U right) noexcept;

        /**
         *  Tries to safely check if left is greater than right.
         *  
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if left is greater, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool greaterThan(T left, U right) noexcept;

        /**
         *  Tries to safely check if left is smaller than right.
         *
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if left is smaller, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool smallerThan(T left, U right) noexcept;

        /**
         *  Tries to safely check if left is greater or equal to right.
         *  
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if left is greater or equal, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool greaterThanOrEquals(T left, U right) noexcept;

        /**
         *  Tries to safely check if left is smaller or equal to right.
         *  
         *  Note that this only allows comparison of floating-point types with any other type, it does not guarantee
         *  a more accurate comparison result.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if left is smaller or equal, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool smallerThanOrEquals(T left, U right) noexcept;
    };

    //==================================================================================================================
    namespace detail
    {
        template<class T, class U, class Fn>
        JAUT_NODISCARD
        inline SafeFloat::Result<T> safeFloatOperation(T left, U right, Fn &&func)
        {
            static_assert(std::is_floating_point_v<T> || std::is_floating_point_v<U>,
                          JAUT_ASSERT_SAFEFLOAT_NO_FLOAT_TYPE);
            
            std::feclearexcept(FE_OVERFLOW);
            std::feclearexcept(FE_UNDERFLOW);
            
            const T result = (std::forward<Fn>(func))(left, right);
            
            if (std::fetestexcept(FE_OVERFLOW) || std::fetestexcept(FE_UNDERFLOW))
            {
                return {1, left};
            }
            
            return {0, result};
        }
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
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::cast(U value)
    {
        static_assert(std::is_floating_point_v<U>, JAUT_ASSERT_SAFEFLOAT_NO_FLOAT_TYPE);
        
        if (value > std::numeric_limits<T>::max() || value < std::numeric_limits<T>::min())
        {
            return { 1, T{} };
        }
        
        return { 0, static_cast<T>(value) };
    }
    
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::add(T left, U right)
    {
        return detail::safeFloatOperation(left, right, [](T l, U r) -> T
        {
            return (l + r);
        });
    }
    
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::sub(T left, U right)
    {
        return detail::safeFloatOperation(left, right, [](T l, U r) -> T
        {
            return (l - r);
        });
    }
    
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::mul(T left, U right)
    {
        return detail::safeFloatOperation(left, right, [](T l, U r) -> T
        {
            return (l * r);
        });
    }
    
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::div(T left, U right)
    {
        if (right == U{})
        {
            return { 2, left };
        }
        
        return detail::safeFloatOperation(left, right, [](T l, U r) -> T
        {
            return (l / r);
        });
    }
    
    template<class T, class U>
    inline SafeFloat::Result<T> SafeFloat::mod(T left, U right)
    {
        if (right == U{})
        {
            return { 2, left };
        }
        
        return detail::safeFloatOperation(left, right, [](T l, U r) -> T
        {
            return std::fmod(l, r);
        });
    }
    
    //==================================================================================================================
    template<class T, class U>
    inline bool SafeFloat::equals(T left, U right) noexcept
    {
        // TODO safe equal comparison
        return (left == right);
    }
    
    template<class T, class U>
    inline bool SafeFloat::notEquals(T left, U right) noexcept
    {
        // TODO safe non-equal comparison
        return (left != right);
    }
    
    template<class T, class U>
    inline bool SafeFloat::greaterThan(T left, U right) noexcept
    {
        // TODO safe greater than comparison
        return (left > right);
    }
    
    template<class T, class U>
    inline bool SafeFloat::smallerThan(T left, U right) noexcept
    {
        // TODO safe smaller than comparison
        return (left < right);
    }
    
    template<class T, class U>
    inline bool SafeFloat::greaterThanOrEquals(T left, U right) noexcept
    {
        // TODO safe greater than or equals comparison
        return (left >= right);
    }
    
    template<class T, class U>
    inline bool SafeFloat::smallerThanOrEquals(T left, U right) noexcept
    {
        // TODO safe smaller than or equals comparison
        return (left <= right);
    }
}
//======================================================================================================================
// endregion Implementation
//**********************************************************************************************************************
