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
 * File: iconfigparser.h
 * Time: 4, May 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/config.h>

namespace jaut
{
/**
    An interface to provide the bones for a new parser class to use in the Config class.
    You can also use the parser your own way, this is just an interface.
 */
class JAUT_API IConfigParser
{
public:
    //=================================================================================================================
    virtual ~IConfigParser() {}

    /**
        Parse the config from a config file.

        @param configFile The path to the config file
        @param root       A property containing the full config structure
     */
    virtual const bool parseConfig(const File &configFile, Config::Property root) const = 0;

    /**
        Writes the config to a file.

        @param configFile The path to the config file
        @param root       A property containing all the data of the config file
     */
    virtual const bool writeConfig(const File &configFile, const Config::Property root) const = 0;
};
}