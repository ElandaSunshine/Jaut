/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: propertyattribute.cpp
 * Time: 4, Mai 2019
 *
 * ===============================================================
 */

#include <jaut/propertyattribute.h>

namespace jaut
{
struct PropertyAttributeBase : public AsyncUpdater, public PropertyMap::Listener
{

    //=================================================================================================================
    PropertyAttributeBase(PropertyMap &map, const String &name) noexcept
        : map(map),
          name(name),
          lastValue(0)
    {
        map.addListener(this);
    }

    ~PropertyAttributeBase() noexcept
    {
        map.removeListener(this);
    }

    //=================================================================================================================
    void updatePropertyValue(const var &newValue) noexcept
    {
        if (propertyExists())
        {
            map.setProperty(name, newValue);
        }
    }

    void sendInitialUpdate() noexcept
    {
        if (!propertyExists())
        {
            return;
        }

        onValueChanged(name, var(), map.getProperty(name));
    }

    void handleAsyncUpdate() override
    {
        setValue(lastValue);
    }

    const bool propertyExists() noexcept
    {
        return map.contains(Identifier(name));
    }

    void onValueChanged(const String &name, var, var newValue) override
    {
        if (name != this->name)
        {
            return;
        }

        lastValue = newValue;

        if (MessageManager::getInstance()->isThisTheMessageThread())
        {
            cancelPendingUpdate();
            setValue(newValue);
        }
        else
        {
            triggerAsyncUpdate();
        }
    }

    virtual void setValue(const var &) = 0;

    PropertyMap &map;

    String name;
    var lastValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyAttributeBase)

    void onPropertyAdded(const String&, var) override {}
};

//=====================================================================================================================
struct PropertyAttribute::Pimpl : private PropertyAttributeBase
{
    Pimpl(PropertyMap &map, const StringRef name)
        : PropertyAttributeBase(map, name),
          ignoreCallbacks(false)
    {
        sendInitialUpdate();
    }

    //=================================================================================================================
    void setValue(const var &newValue) override
    {
        const ScopedLock selfCallbackLock(selfCallbackMutex);
        {
            ScopedValueSetter<bool> svs(ignoreCallbacks, true);
            currentValue = newValue;
        }
    }

    void updateValue(const var &value)
    {
        const ScopedLock selfCallbackLock(selfCallbackMutex);

        if (!ignoreCallbacks)
        {
            updatePropertyValue(value);
            currentValue = value;
        }
    }

    const var &getValue()
    {
        return currentValue;
    }

private:
    bool ignoreCallbacks;

    CriticalSection selfCallbackMutex;
    var currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pimpl)
};

//=====================================================================================================================
PropertyAttribute::PropertyAttribute(PropertyMap &map, const StringRef name) noexcept
    : pimpl(new Pimpl(map, name))
{}

PropertyAttribute::~PropertyAttribute()
{}

//=====================================================================================================================
PropertyAttribute::operator var()
{
    return pimpl->getValue();
}

void PropertyAttribute::operator=(const var &value)
{
    pimpl->updateValue(value);
}
}