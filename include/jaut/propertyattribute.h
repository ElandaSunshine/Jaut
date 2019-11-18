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
 * File: propertyattribute.h
 * Time: 4, Mai 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/propertymap.h>

namespace jaut
{
/**
    The property attribute class is a date pipeline with which you can synchronize properties over gui and processor
    threads.

    When trying to obtain a certain value in the gui, you have to pass the exact name of the property since they are
    case-sensitive.
    If the given PropertyMap contains such a property and that property was changed, whether it's in the gui or
    processor thread, it will synchronize that new value to its counterpart.

    Always use this class in the gui class and only one per property.
 */
class JAUT_API PropertyAttribute final
{
public:
    //=================================================================================================================
    PropertyAttribute(PropertyMap &propertyMap, const StringRef propertyName) noexcept;
    ~PropertyAttribute();

    //=================================================================================================================
    /**
        Gets the current synched value of the PropertyMap.
     */
    operator var();

    /**
        Sets a new value to be synched with the PropertyMap.
     */
    void operator=(const var&);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyAttribute)
};
}