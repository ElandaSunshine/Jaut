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
    @file   jaut_SafeInteger.cpp
    @date   06, April 2022

    ===============================================================
 */

#include <jaut_core/math/jaut_SafeInteger.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/detail/SafeInt.hpp>
#include <jaut_core/preprocessor/jaut_UppMisc.h>



//**********************************************************************************************************************
// region Preprocessor
//======================================================================================================================
#define JAUT_FUNC_OVERLOAD(NAME, RET, T, THROW)                   \
    template RET SafeInteger:: NAME(T, wchar_t) THROW;            \
    template RET SafeInteger:: NAME(T, char) THROW;               \
    template RET SafeInteger:: NAME(T, short) THROW;              \
    template RET SafeInteger:: NAME(T, int) THROW;                \
    template RET SafeInteger:: NAME(T, long) THROW;               \
    template RET SafeInteger:: NAME(T, long long) THROW;          \
    template RET SafeInteger:: NAME(T, unsigned char) THROW;      \
    template RET SafeInteger:: NAME(T, unsigned short) THROW;     \
    template RET SafeInteger:: NAME(T, unsigned int) THROW;       \
    template RET SafeInteger:: NAME(T, unsigned long) THROW;      \
    template RET SafeInteger:: NAME(T, unsigned long long) THROW;

#define JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, T) JAUT_FUNC_OVERLOAD(NAME, SafeInteger::Result<T>, T,)
#define JAUT_FUNC_OVERLOAD_COMPARE(NAME, T)    JAUT_FUNC_OVERLOAD(NAME, bool, T, noexcept)

#define JAUT_FUNC_OVERLOAD_CASTING(T)                                      \
    template SafeInteger::Result<T> SafeInteger::cast(wchar_t);            \
    template SafeInteger::Result<T> SafeInteger::cast(char);               \
    template SafeInteger::Result<T> SafeInteger::cast(short);              \
    template SafeInteger::Result<T> SafeInteger::cast(int);                \
    template SafeInteger::Result<T> SafeInteger::cast(long);               \
    template SafeInteger::Result<T> SafeInteger::cast(long long);          \
    template SafeInteger::Result<T> SafeInteger::cast(unsigned char);      \
    template SafeInteger::Result<T> SafeInteger::cast(unsigned short);     \
    template SafeInteger::Result<T> SafeInteger::cast(unsigned int);       \
    template SafeInteger::Result<T> SafeInteger::cast(unsigned long);      \
    template SafeInteger::Result<T> SafeInteger::cast(unsigned long long);

#define JAUT_FUNC_OVERLOAD_ALL(NAME)                        \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, wchar_t)            \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, char)               \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, short)              \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, int)                \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, long)               \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, long long)          \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, unsigned char)      \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, unsigned short)     \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, unsigned int)       \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, unsigned long)      \
    JAUT_FUNC_OVERLOAD_ARITHMETIC(NAME, unsigned long long)

#define JAUT_FUNC_OVERLOAD_CMP(NAME)                 \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, wchar_t)        \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, char)           \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, short)          \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, int)            \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, long)           \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, long long)      \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, unsigned char)  \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, unsigned short) \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, unsigned int)   \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, unsigned long)  \
    JAUT_FUNC_OVERLOAD_COMPARE(NAME, unsigned long long)

#define JAUT_FUNC_OVERLOAD_CST()                   \
    JAUT_FUNC_OVERLOAD_CASTING(wchar_t)            \
    JAUT_FUNC_OVERLOAD_CASTING(char)               \
    JAUT_FUNC_OVERLOAD_CASTING(short)              \
    JAUT_FUNC_OVERLOAD_CASTING(int)                \
    JAUT_FUNC_OVERLOAD_CASTING(long)               \
    JAUT_FUNC_OVERLOAD_CASTING(long long)          \
    JAUT_FUNC_OVERLOAD_CASTING(unsigned char)      \
    JAUT_FUNC_OVERLOAD_CASTING(unsigned short)     \
    JAUT_FUNC_OVERLOAD_CASTING(unsigned int)       \
    JAUT_FUNC_OVERLOAD_CASTING(unsigned long)      \
    JAUT_FUNC_OVERLOAD_CASTING(unsigned long long) \
    JAUT_FUNC_OVERLOAD_CASTING(float)              \
    JAUT_FUNC_OVERLOAD_CASTING(double)             \
    JAUT_FUNC_OVERLOAD_CASTING(long double)

//======================================================================================================================
// endregion Preprocessor
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    template<class T, class U>
    jaut::SafeInteger::Result<T> calculate(SafeInt<T>&(SafeInt<T>::*func)(U), T left, U right)
    {
        SafeInt<T> result { left };
        int        code {};
        
        try
        {
            (result.*func)(right);
        }
        catch (const SafeIntException &ex)
        {
            code = static_cast<int>(ex.m_code);
        }
        
        return { code, static_cast<T>(result) };
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region SafeInteger
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::cast(U value)
    {
        T   result {};
        int code   {};
        
        try
        {
            result = static_cast<T>(SafeInt(value));
        }
        catch (const SafeIntException &ex)
        {
            code = static_cast<int>(ex.m_code);
        }
        
        return { code, result };
    }
    JAUT_FUNC_OVERLOAD_CST()
    
    //==================================================================================================================
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::add(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator+=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(add)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::sub(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator-=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(sub)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::mul(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator*=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(mul)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::div(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator/=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(div)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::mod(T left, U right)
    {        
        return ::calculate(&SafeInt<T>::operator%=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(mod)
    
    //==================================================================================================================
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::shiftLeft(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator<<=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(shiftLeft)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::shiftRight(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator>>=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(shiftRight)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::bitAnd(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator&=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(bitAnd)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::bitOr(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator|=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(bitOr)
    
    template<class T, class U>
    SafeInteger::Result<T> SafeInteger::bitXor(T left, U right)
    {
        return ::calculate(&SafeInt<T>::operator^=, left, right);
    }
    JAUT_FUNC_OVERLOAD_ALL(bitXor)
    
    //==================================================================================================================
    template<class T, class U>
    bool SafeInteger::equals(T left, U right) noexcept
    {
        return SafeInt<T>(left) == right;
    }
    JAUT_FUNC_OVERLOAD_CMP(equals)
    
    template<class T, class U>
    bool SafeInteger::notEquals(T left, U right) noexcept
    {
        return SafeInt<T>(left) != right;
    }
    JAUT_FUNC_OVERLOAD_CMP(notEquals)
    
    template<class T, class U>
    bool SafeInteger::greaterThan(T left, U right) noexcept
    {
        return SafeInt<T>(left) > right;
    }
    JAUT_FUNC_OVERLOAD_CMP(greaterThan)
    
    template<class T, class U>
    bool SafeInteger::smallerThan(T left, U right) noexcept
    {
        return SafeInt<T>(left) < right;
    }
    JAUT_FUNC_OVERLOAD_CMP(smallerThan)
    
    template<class T, class U>
    bool SafeInteger::greaterThanOrEquals(T left, U right) noexcept
    {
        return SafeInt<T>(left) >= right;
    }
    JAUT_FUNC_OVERLOAD_CMP(greaterThanOrEquals)
    
    template<class T, class U>
    bool SafeInteger::smallerThanOrEquals(T left, U right) noexcept
    {
        return SafeInt<T>(left) <= right;
    }
    JAUT_FUNC_OVERLOAD_CMP(smallerThanOrEquals)
}
//======================================================================================================================
// endregion SafeInteger
//**********************************************************************************************************************



#undef JAUT_FUNC_OVERLOAD
#undef JAUT_FUNC_OVERLOAD_ARITHMETIC
#undef JAUT_FUNC_OVERLOAD_COMPARE
#undef JAUT_FUNC_OVERLOAD_CASTING
#undef JAUT_FUNC_OVERLOAD_ALL
#undef JAUT_FUNC_OVERLOAD_CMP
#undef JAUT_FUNC_OVERLOAD_CST
