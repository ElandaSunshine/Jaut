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
    @file   configparser.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
    The XmlParser is an in-built parser for the Config class used for parsing Xml style config files.
    You may use it for yourself too though.
 */
class JAUT_API XmlParser final : public IConfigParser
{
public:
    explicit XmlParser(const String &defaultCategory, const String &tagSettingId = "Setting",
                       const String &tagIntroId = "Config", const String &tagCategoryId = "Category") noexcept;

    //=================================================================================================================
    const bool parseConfig(const File &configFile, Config::Property root) const override;
    const bool writeConfig(const File &configFile, const Config::Property root) const override;

private:
    String defaultCategory;
    String tagSetting;
    String tagIntro;
    String tagCategory;
};

/**
    The JsonParser is an in-built parser for the Config class used for parsing Json style config files.
    You may use it for yourself too though.
 */
class JAUT_API JsonParser final : public IConfigParser
{
public:
    explicit JsonParser(const String &noticeId = "NOTICE") noexcept;

    //==================================================================================================================
    const bool parseConfig(const File &configFile, Config::Property root) const override;
    const bool writeConfig(const File &configFile, const Config::Property root) const override;

private:
    String noticeId;
};

#include <yaml-cpp/yaml.h>
#ifdef YAML_CPP_API
/**
    The YamlParser is an in-built parser for the Config class used for parsing Yaml style config files.
    You may use it for yourself too though.
 */
class JAUT_API YamlParser final : public IConfigParser
{
public:
    const bool parseConfig(const File &configFile, Config::Property root) const override;
    const bool writeConfig(const File &configFile, const Config::Property root) const override;
};
#endif
}
