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
    @file   configparser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#if __has_include(<yaml-cpp/yaml.h>)
#   include <yaml-cpp/yaml.h>
#endif

namespace jaut
{
/**
    The XmlParser is an in-built parser for the Config class used for parsing Xml style config files.
    You may use it for yourself too though.
 */
class JAUT_API XmlParser final : public jaut::Config::ConfigParserType
{
public:
    struct JAUT_API ErrorCodes final
    {
        /**
         *  The root node to read from was invalid.
         */
        static constexpr int InvalidRootNote = 0x0000;
        
        /**
         *  Could not find the config file.
         */
        static constexpr int FileNotFound = 0x0001;
        
        /**
         *  There was a problem reading the config file.
         */
        static constexpr int InvalidConfig = 0x0002;
    };
    
    //=================================================================================================================
    explicit XmlParser(const juce::String &tagSettingId  = "Setting",
                       const juce::String &tagIntroId    = "Config",
                       const juce::String &tagCategoryId = "Category") noexcept;

    //=================================================================================================================
    OperationResult parseConfig(const juce::File &configFile, PropertyType root) const override;
    OperationResult writeConfig(const juce::File &configFile, ConstPropertyType root) const override;

private:
    juce::String tagSetting;
    juce::String tagIntro;
    juce::String tagCategory;
};

/**
    The JsonParser is an in-built parser for the Config class used for parsing Json style config files.
    You may use it for yourself too though.
 */
class JAUT_API JsonParser final : public Config::ConfigParserType
{
public:
    struct JAUT_API ErrorCodes final
    {
        /**
         *  The root node to read from was invalid.
         */
        static constexpr int InvalidRootNote = 0x0000;
        
        /**
         *  Could not find the config file.
         */
        static constexpr int FileNotFound = 0x0001;
        
        /**
         *  There was a problem reading the config file.
         */
        static constexpr int InvalidConfig = 0x0002;
    };
    
    //==================================================================================================================
    explicit JsonParser(const juce::String &noticeId = "NOTICE") noexcept;

    //==================================================================================================================
    OperationResult parseConfig(const juce::File &configFile, PropertyType root) const override;
    OperationResult writeConfig(const juce::File &configFile, ConstPropertyType root) const override;

private:
    juce::String noticeId;
};

#ifdef YAML_CPP_API
/**
    The YamlParser is an in-built parser for the Config class used for parsing Yaml style config files.
    You may use it for yourself too though.
 */
class JAUT_API YamlParser final : public Config::ConfigParserType
{
public:
    struct JAUT_API ErrorCodes final
    {
        /**
         *  The root node to read from was invalid.
         */
        static constexpr int InvalidRootNote = 0x0000;
        
        /**
         *  Could not find the config file.
         */
        static constexpr int FileNotFound = 0x0001;
        
        /**
         *  There was a problem reading the config file.
         */
        static constexpr int InvalidConfig = 0x0002;
    };
    
    //==================================================================================================================
    OperationResult parseConfig(const juce::File &configFile, PropertyType root) const override;
    OperationResult writeConfig(const juce::File &configFile, ConstPropertyType root) const override;
};
#endif
}
