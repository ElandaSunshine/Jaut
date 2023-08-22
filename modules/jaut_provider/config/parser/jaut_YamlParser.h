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
    @file   jaut_YamlParser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut_provider/config/jaut_IConfigParser.h>
#include <jaut_provider/util/jaut_YamlCppCompatibility.h>



namespace jaut
{
    //==================================================================================================================
    #if JAUT_OPTLIB_YAML_CPP
    /**
     *  The YamlParser is an in-built parser for the Config class used for parsing Yaml style config files.
     *  You may use it for yourself too though.
     */
    class JAUT_API YamlParser final : public IConfigParser
    {
    public:
        struct Options
        {
            //==========================================================================================================
            /**
             *  If this is set to true, strings that contain \\n will be wrapped over several lines via the
             *  '|' (pipe) YAML modifier.
             */
            bool breakStringsWithNewLinesCharacters = true;
            
            /**
             *  If this is above 0, longer strings that do not contain any new line characters will be broken over
             *  several lines with the '<' (smaller than) YAML modifier.
             *  This determines the size at which these strings should be broken.
             */
            int breakLongStringsAfter = 80;
            
            /** The amount of spaces different levels of indentation should consist of. */
            int indentationWidth = 2;
            
            //==========================================================================================================
            Options() noexcept {} // NOLINT
        };
        
        //==============================================================================================================
        explicit YamlParser(Options options = {}) noexcept;
        
        //==============================================================================================================
        void parseConfig(ParseAttributes<Config::CategoryMap>       attributes)       override;
        void writeConfig(ParseAttributes<const Config::CategoryMap> attributes) const override;
        
    private:
        Options options;
    };
    #endif
}
