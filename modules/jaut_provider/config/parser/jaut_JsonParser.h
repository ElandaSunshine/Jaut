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
    @file   jaut_JsonParser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut_provider/jaut_provider_define.h>
#include <jaut_provider/config/jaut_IConfigParser.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The JsonParser is an in-built parser for the Config class used for parsing Json style config files.
     *  You may use it for yourself too though.
     */
    class JAUT_API JsonParser final : public IConfigParser
    {
    public:
        struct Options
        {
            //==========================================================================================================
            /** The amount of spaces different levels of indentation should consist of. */
            int indentationWidth = 4;
    
            //==========================================================================================================
            Options() {} // NOLINT
        };
        
        //==============================================================================================================
        explicit JsonParser(Options options = {}) noexcept;
    
        //==============================================================================================================
        void parseConfig(ParseAttributes<Config::CategoryMap>       attributes)       override;
        void writeConfig(ParseAttributes<const Config::CategoryMap> attributes) const override;
        
    private:
        Options options;
    };
}
