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
    @file   jaut_IConfigParser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include "jaut_core/util/jaut_CommonUtils.h"
#include <jaut_core/define/jaut_Define.h>
#include <jaut_provider/config/jaut_Config.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  An interfaces to provide the bare-bones for a new parser class to use in the jaut::Config class.
     *  You can also use the parser for your very own purposes, this is just an interface after all.
     */
    struct JAUT_API IConfigParser
    {
        //==============================================================================================================
        /**
         *  The object that is sent to the parsing functions, containing all necessary information to parse the config.
         */
        template<class MapType>
        struct ParseAttributes
        {
            //==========================================================================================================
            /** The path to the config file. */
            juce::File configFile;
            
            /** The property map to read/write from/to. */
            MapType &categories;
            
            /** The config's options. */
            const Config::Options &options;
        };
        
        //==============================================================================================================
        virtual ~IConfigParser() = default;
        
        //==============================================================================================================
        /**
         *  Parse the config from a config file.
         *
         *  It is ill-advised to add/remove properties from the map or any of the child properties,
         *  however, if you know what you are doing it should be fine.
         *  This is just a disclaimer that things might not work as expected anymore.
         *
         *  @param attributes The parser attributes
         *
         *  @throws jaut::ConfigParseException If there was a problem parsing the config file
         *  @throws jaut::ConfigIncompatibleTypeException If there was a type mismatch and strict typing is enabled
         */
        virtual void parseConfig(ParseAttributes<Config::CategoryMap> attributes) = 0;
        
        /**
         *  Writes the config to a file.
         *  @param attributes The parser attributes
         *
         *  @throws jaut::ConfigIOException If there was a problem writing to the config file
         */
        virtual void writeConfig(ParseAttributes<const Config::CategoryMap> attributes) const = 0;
    };
}
