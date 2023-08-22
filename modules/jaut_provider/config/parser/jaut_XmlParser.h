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
    @file   jaut_XmlParser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <bitset>
#include <jaut_provider/config/jaut_Config.h>
#include <jaut_provider/jaut_provider_define.h>
#include <jaut_provider/config/jaut_IConfigParser.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The XmlParser is an in-built parser for the Config class used for parsing Xml style config files.
     *  You may use it for yourself too though.
     */
    class JAUT_API XmlParser final : public IConfigParser
    {
    public:        
        //==============================================================================================================
        struct Options
        {
            //==========================================================================================================
            struct TypeTag
            {
                /** The tag-name of the root tag. */
                juce::String tagNameRoot = "Config";
                
                /** The tag-name of categories. */
                juce::String tagNameCategory = "Category";
                
                juce::String tagNameStringLine = "Line";
                
                juce::String tagTypeInt        = "Int";
                juce::String tagTypeInt64      = "Long";
                juce::String tagTypeString     = "String";
                juce::String tagTypeBool       = "Bool";
                juce::String tagTypeDouble     = "Double";
                juce::String tagTypeArray      = "Array";
                juce::String tagTypeJson       = "Json";
                juce::String tagTypeTree       = "Tree";
                juce::String tagTypeArrayVoid  = "Void";
            };
            
            //==========================================================================================================
            /** The tag-names of certain property types */
            TypeTag typeTags;
            
            /** The amount of spaces different levels of indentation should consist of. */
            int indentationWidth = 4;
            
            /**
             *  Determines whether the parser should throw an exception when it encounters a tag-name it doesn't
             *  understand.
             */
            bool throwOnInvalidTagName = true;
            
            //==========================================================================================================
            Options() noexcept {} // NOLINT
        };
        
        //==============================================================================================================
        explicit XmlParser(Options options = {}) noexcept;
        
        //==============================================================================================================
        void parseConfig(ParseAttributes<Config::CategoryMap>       attributes)       override;
        void writeConfig(ParseAttributes<const Config::CategoryMap> attributes) const override;
        
    private:
        Options options;
    };
}
