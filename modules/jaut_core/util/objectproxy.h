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
    @file   constproxy.h
    @date   12, June 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
template<class T>
class ObjectProxy
{
public:
    using ObjectType = std::remove_reference_t<T>;
    using ValueType  = std::decay_t<T>;
    
    //==================================================================================================================
    template<class ...Args>
    ObjectProxy(Args &&...args) noexcept
        : object(std::forward<Args>(args)...)
    {}
    
    //==================================================================================================================
    ObjectType& getObject() noexcept
    {
        return object;
    }
    
    const ObjectType& getObject() const noexcept
    {
        return object;
    }
    
private:
    T object;
};

/**
 *  A type helper for objects that need to be returned by-const-val.
 *  @tparam T The const proxy type
 */
template<class T>
using ConstProxy = ObjectProxy<const T>;
}
