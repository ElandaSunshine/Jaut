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

    @author Elanda (elanda@elandasunshine.com)
    @file   jaut_SafeInteger.h
    @date   06, April 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_Define.h>

namespace jaut
{
    struct JAUT_API SafeInteger
    {
        //==============================================================================================================
        template<class T>
        struct Result
        {
            int code;
            T   value;
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
         *  Tries to apply a bitwise left shift operation on left shifted by right amount of times.
         *  Returns the shifted value or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> shiftLeft(T left, U right);
        
        /**
         *  Tries to apply a bitwise right shift operation on left shifted by right amount of times.
         *  Returns the shifted value or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> shiftRight(T left, U right);
        
        /**
         *  Tries to apply a bitwise AND operation on left and right.
         *  Returns the calculated value or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> bitAnd(T left, U right);
        
        /**
         *  Tries to apply a bitwise OR operation on left and right.
         *  Returns the calculated value or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> bitOr(T left, U right);
        
        /**
         *  Tries to apply a bitwise XOR operation on left and right.
         *  Returns the calculated value or left if an overflow has happened.
         *  
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return The result of the operation
         */
        template<class T, class U>
        JAUT_NODISCARD
        static Result<T> bitXor(T left, U right);
        
        //==============================================================================================================
        /**
         *  Tries to safely check for equality of left and right.
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
         *  @param left The left-hand operand
         *  @param right The right-hand operand
         *  @return True if left is smaller or equal, otherwise false
         */
        template<class T, class U>
        JAUT_NODISCARD
        static bool smallerThanOrEquals(T left, U right) noexcept;
    };
}