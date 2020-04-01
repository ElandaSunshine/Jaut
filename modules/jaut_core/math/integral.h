/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   integral.h
    @date   23, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  An integral base class for classes that need to be arithmetically operable.
 *  @tparam NumericType The numeric type this class is managing
 */
template<class NumericType>
struct JAUT_API Integral
{
    //==================================================================================================================
    static_assert(std::is_integral_v<NumericType>, JAUT_ASSERT_INTEGRAL_TYPE_NOT_NUMERIC);
    
    //==================================================================================================================
    constexpr Integral() noexcept = default;
    constexpr Integral(NumericType value)
        : integral(value)
    {}
    
    virtual ~Integral() = default;
    
    //==================================================================================================================
    virtual Integral &operator=(NumericType value) noexcept
    {
        integral = value;
        return *this;
    }
    
    virtual operator NumericType() const noexcept
    {
        return integral;
    }
    
    //==================================================================================================================
    virtual Integral operator+(Integral value) const noexcept
    {
        return integral + value;
    }
    
    virtual Integral operator-(Integral value) const noexcept
    {
        return integral - value;
    }
    
    //==================================================================================================================
    virtual Integral &operator+=(Integral value) noexcept
    {
        integral += value;
        return *this;
    }
    
    virtual Integral &operator-=(Integral value) noexcept
    {
        integral -= value;
        return *this;
    }
    
    virtual Integral &operator++() noexcept
    {
        ++integral;
        return *this;
    }
    
    virtual Integral operator++(int) noexcept
    {
        const Integral old = *this;
        ++integral;
        return old;
    }
    
    virtual Integral &operator--() noexcept
    {
        --integral;
        return *this;
    }
    
    virtual Integral operator--(int) noexcept
    {
        const Integral old = *this;
        --integral;
        return old;
    }
    
    //==================================================================================================================
    virtual bool operator==(const Integral &right) const noexcept
    {
        return integral == right.integral;
    }
    
    virtual bool operator!=(const Integral &right) const noexcept
    {
        return integral != right.integral;
    }
    
    virtual bool operator>(const Integral &right) const noexcept
    {
        return integral > right.integral;
    }
    
    virtual bool operator<(const Integral &right) const noexcept
    {
        return integral < right.integral;
    }
    
    virtual bool operator>=(const Integral &right) const noexcept
    {
        return (*this) > right || (*this) == right;
    }
    
    virtual bool operator<=(const Integral &right) const noexcept
    {
        return (*this) < right || (*this) == right;
    }

protected:
    NumericType integral {};
};
}
