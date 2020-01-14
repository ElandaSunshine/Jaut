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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   propertymap.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{

/**
    The PropertyMap class is similiar to the AudioProcessorValueTreeState in that,
    that it can synchronize processor data with the gui thread.
    
    In combination with the PropertyAttachment class,
    you can send and receive data to and from the gui thread.

    Note: Property keys are case-sensitive.
 */
class JAUT_API PropertyMap final
{
public:
    //=================================================================================================================
    struct JAUT_API Listener
    {
        virtual void onValueChanged(const String &name, var oldValue, var newValue) = 0;
        virtual void onPropertyAdded(const String &name, var value) {}
    };

    using t_map           = NamedValueSet;
    using t_iterator      = const t_map::NamedValue*;
    using t_listener_list = ListenerList<Listener>;

    //=================================================================================================================
    PropertyMap() = default;

    //=================================================================================================================
    /**
        Creates a new property with the given value and adds it to the property map.

        @param key          The key of the property
        @param defaultValue The initial value this property should have
        @param notifyChange Determines whether changes should be pushed to the event handler
     */
    const bool createProperty(const Identifier &key, const var &defaultValue, bool notifyChange = true) noexcept;

    /**
        Gets a property by its name.

        @param key The key of the property
        @return The property if it exists, else a void var will be returned
     */
    var getProperty(const Identifier &key) const noexcept;

    /**
        Sets a property if it exists else adds it to the map.

        @param key          The key of the property
        @param defaultValue The initial value this property should have
        @param notifyChange Determines whether changes should be pushed to the event handler
     */
    void setProperty(const Identifier &key, const var &defaultValue, bool notifyChange = true) noexcept;

    /**
        Get the pointer of a property object.
        You don't need to delete this pointer since it only points to a value inside the PropertyMap.

        Note that once the PropertyMap was destroyed the pointer may point to an unknown location, so make sure you 
        null it once your PropertyMap was destroyed.

        @param key The key of the property
     */
    const var *getPropertyPtr(const Identifier &key) const noexcept;

    //=================================================================================================================
    /**
        Gets the amount of properties this map contains.

        @return The amount of properties
     */
    const size_t size() const noexcept;

    /**
        Clears the entire map.
     */
    void clear() noexcept;

    /**
        Determines whether the map contains any properties.

        @return False if the map contains any properties, true if not
     */
    const bool isEmpty() const noexcept;

    /**
        Determines whether the map contains a particular key.

        @param The key of the property
        @return True if it contains a given key, false if not
     */
    const bool contains(const Identifier &key) const noexcept;

    /**
        Determines whether the map contains a particular value once or more times.

        @param The value to search for
        @return True if it contains a given value, false if not
     */
    const bool contains(const var &value) const noexcept;
    t_iterator begin() const noexcept;
    t_iterator end() const noexcept;

    //=================================================================================================================
    /**
        Reads the data from a valid ValueTree object and overrides the PropertyMap with the given data.

        @param dataTree     The ValueTree to adopt the data from
        @param shouldNotify Determines whether changes should be pushed to the event handler
     */
    void readFrom(const ValueTree &dataTree, bool shouldNotify = true) noexcept;

    /**
        Gets the data from the PropertyMap and stores it into a ValueTree object.

        @return A ValueTree object with all the data from this map
     */
    ValueTree writeTo(const String &name = "Properties") const noexcept;
    
    //=================================================================================================================
    void addListener(Listener *listener) noexcept;
    void removeListener(Listener *listener) noexcept;

private:
    t_map properties;
    t_listener_list listeners;

    //=================================================================================================================
    void postValueChangedListener(const String &name, var oldValue, var newValue) noexcept;
    void postAddedListener(const String &name, var value) noexcept;
};
}
