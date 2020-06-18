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
    @file   iconfigparser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{

/**
 *  An interfaces to provide the bones for a new parser class to use in the Config class.
 *  You can also use the parser your own way, this is just an interfaces.
 *
 *  @tparam Property      The property type of the config to write to
 *  @tparam ConstProperty The property type of the config to read from
 */
template<class Property, class ConstProperty>
class JAUT_API IConfigParser
{
public:
    /** The type of the property to write to. */
    using PropertyType = Property;
    
    /** The type of the property to read from. */
    using ConstPropertyType = ConstProperty;
    
    //==================================================================================================================
    virtual ~IConfigParser() = default;
    
    /**
     *  Parse the config from a config file.
     *
     *  @param configFile The path to the config file
     *  @param root       A property containing all the data of the config file
     *  @return The result of this operation
     */
    virtual OperationResult parseConfig(const juce::File &configFile, Property root) const = 0;

    /**
     *  Writes the config to a file.
     *
     *  @param configFile The path to the config file
     *  @param root       A property containing all the data of the config file
     *  @return The result of this operation
     */
    virtual OperationResult writeConfig(const juce::File &configFile, ConstProperty root) const = 0;
};
}
