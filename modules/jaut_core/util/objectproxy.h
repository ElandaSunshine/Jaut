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
    @file   constproxy.h
    @date   12, June 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  The ObjectProxy class encapsulates any object inside and
 *  @tparam T
 */
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
    template<class U = T>
    typename std::enable_if<std::is_class_v<U>, U>::type* operator->() const noexcept
    {
        return std::addressof(object);
    }
    
    T &operator*() const noexcept
    {
        return object;
    }
    
    //==================================================================================================================
#if JAUT_PROXY_OBJECT_OVERLOAD_ADDRESS_OF_OPERATOR
    T *operator&() const noexcept
    {
        DBG("Note: Your object proxy is overloading the address-of operator, this will give you the address of"
            " the underlying object, not the ObjectProxy wrapper instance.");
        return &object;
    }
#else
    T *addressOf() const noexcept
    {
        return &object;
    }
#endif
    
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
