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
    @file   configparser.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut/provider/config/configparser.h>

#ifdef YAML_CPP_API
#include <regex>
#endif

namespace jaut
{
namespace
{
const String getCommaOrNot(Config::Property::t_Iterator it, Config::Property::t_Iterator &end)
{
    for (auto _it = ++it; _it != end; ++_it)
    {
        if (_it->second.isValid())
        {
            return ",\n";
        }
    }

    return "";
}

const bool exceedsMaxLength(String::CharPointerType charPtr, int currentLength, int maxLength)
{
    for (;;)
    {
        if (*charPtr == 0)
        {
            break;
        }

        if (!charPtr.isWhitespace())
        {
            ++currentLength;
            ++charPtr;
        }
        else
        {
            break;
        }

        if (currentLength > maxLength)
        {
            return true;
        }
    }

    return false;
}

const String prepareCommentJson(const String &comment, const String &indentation)
{
    const int maxlength = 80;
    String::CharPointerType charptr = comment.getCharPointer();
    int counter = 1;
    String result = "";
    bool iscurrentnextline = false;

    for (;;)
    {
        if (*charptr == 0)
        {
            break;
        }

        if (*charptr == '\n')
        {
            iscurrentnextline = true;
        }

        if (counter == maxlength || exceedsMaxLength(charptr, counter, maxlength) || iscurrentnextline)
        {
            result += " \\\n       " + indentation;
            counter = 0;
        }

        if (!iscurrentnextline)
        {
            result += *charptr;
        }

        ++counter;
        ++charptr;
        iscurrentnextline = false;
    }

    return result;
}

const String getVarVal(const var &value) noexcept
{
    if (value.isVoid())
    {
        return "\"\"";
    }

    return !value.isString() ? (value.isBool() ? (value ? "true" : "false") : value.toString())
                             : "\"" + value.toString() + "\"";
}

const bool appendSubPropertiesXml(const Config::Property propParent, String &fileOutput, int level,
                                  const String &tagSetting) noexcept
{
    for (auto &[name, propsetting] : propParent)
    {
        var value = propsetting.getValue();
        bool hasvalidsubproperties = propsetting.hasValid();

        if (propsetting.isValid() && (!value.isVoid() || hasvalidsubproperties))
        {
            String comment = propsetting.getComment();
            String indentation;

            for (int i = 0; i < level; ++i)
            {
                indentation += "    ";
            }

            if (!comment.isEmpty())
            {
                fileOutput += "\n" + indentation + "<!-- " + comment + " -->\n";
            }

            fileOutput += indentation + "<" + tagSetting + " Name=\"" + propsetting.getName();

            if (!value.isVoid())
            {
                fileOutput += "\" Value=\"" + value.toString() + "\"";
            }

            if (hasvalidsubproperties)
            {
                fileOutput += ">\n";
                (void)appendSubPropertiesXml(propsetting, fileOutput, level + 1, tagSetting); // @noret
                fileOutput += indentation + "</" + tagSetting + ">\n";
            }
            else
            {
                fileOutput += "/>\n";
            }
        }
    }

    return true;
}

const bool readPropertiesXml(const XmlElement *xml, Config::Property propParent, const String &tagSetting) noexcept
{
    int childsize = xml->getNumChildElements();

    if (!propParent.isValid() || (childsize == 0 && xml->getStringAttribute("value").isEmpty()))
    {
        return false;
    }

    propParent.setValue(xml->getStringAttribute("value"));

    if (childsize > 0)
    {
        forEachXmlChildElementWithTagName(*xml, xmlsubsetting, tagSetting)
        {
            (void)readPropertiesXml(xmlsubsetting, propParent.getProperty(xmlsubsetting->getStringAttribute("name")),
                                    tagSetting);
        }
    }

    return true;
}

const bool appendSubPropertiesJson(const Config::Property propParent, String &fileOutput, int level) noexcept
{
    for (auto pairsetting = propParent.begin(); pairsetting != propParent.end(); ++pairsetting)
    {
        auto propsetting = pairsetting->second;
        var value = propsetting.getValue();
        bool hasvalidsubproperties = propsetting.hasValid();

        if (propsetting.isValid())
        {
            String comment = propsetting.getComment();
            String indentation;

            for (int i = 0; i < level; ++i)
            {
                indentation += "    ";
            }

            if (!comment.isEmpty())
            {
                fileOutput += "\n" + indentation + "\"//\": \"" + prepareCommentJson(comment, indentation) + "\",\n";
            }

            fileOutput += indentation + "\"" + propsetting.getName() + "\": ";

            auto end_iterator = propParent.end();

            if (hasvalidsubproperties)
            {
                fileOutput += "{\n";

                if (!value.isVoid())
                {
                    fileOutput += "    " + indentation + "\"value\": " + getVarVal(value) + ",\n";
                }

                (void)appendSubPropertiesJson(propsetting, fileOutput, level + 1);
                fileOutput += "\n" + indentation + "}" + getCommaOrNot(pairsetting, end_iterator);
            }
            else
            {
                fileOutput += getVarVal(value) + getCommaOrNot(pairsetting, end_iterator);
            }
        }
    }

    return true;
}

const bool readPropertiesJson(const var &json, Config::Property parentProp) noexcept
{
    DynamicObject *objsubsetting = json.getDynamicObject();

    if (!objsubsetting)
    {
        parentProp.setValue(json);
        return !json.isVoid();
    }

    for (auto &pairobjsubsetting : objsubsetting->getProperties())
    {
        if (pairobjsubsetting.name.toString().equalsIgnoreCase("//"))
        {
            continue;
        }

        if (pairobjsubsetting.name.toString().equalsIgnoreCase("value"))
        {
            parentProp.setValue(pairobjsubsetting.value);
            continue;
        }

        Config::Property propsubsetting = parentProp.getProperty(pairobjsubsetting.name.toString());

        if (propsubsetting.isValid())
        {
            readPropertiesJson(pairobjsubsetting.value, propsubsetting);
        }
    }

    return true;
}

#ifdef YAML_CPP_API
const String getVarValYaml(const var &value, const String &indentation) noexcept
{
    if (value.isDouble())
    {
        String val = value.toString();

        if (!val.containsChar('.'))
        {
            val += ".0";
        }

        return value.toString();
    }
    else if (value.isBool())
    {
        if (!static_cast<bool>(value))
        {
            return "false";
        }

        return "true";
    }
    else if (value.isArray())
    {
        String outputstring;

        for (var entry : *value.getArray())
        {
            outputstring += "\n" + indentation + "- " + entry.toString();
        }

        return outputstring;
    }
    else if (value.isString())
    {
        String output = value.toString();

        if (output.containsChar('\n'))
        {
            String::CharPointerType charptr = output.getCharPointer();
            juce_wchar currentchar;
            String result = "|\n" + indentation + "  ";

            for (;;)
            {
                currentchar = charptr.getAndAdvance();

                if (currentchar == 0)
                {
                    break;
                }

                if (currentchar == '\n')
                {
                    result += currentchar;
                    result += indentation + "  ";
                    continue;
                }

                result += currentchar;
            }

            output = result;
        }
        else if(output.length() > 80 && output.containsChar(' '))
        {
            String::CharPointerType charptr = output.getCharPointer();
            juce_wchar currentchar;
            String result = ">\n" + indentation + "  ";
            int counter = 1;

            for (;;)
            {
                currentchar = charptr.getAndAdvance();

                if (currentchar == 0)
                {
                    break;
                }

                if (exceedsMaxLength(charptr - 1, counter, 80))
                {
                    result = result.dropLastCharacters(1);
                    result += "\n" + indentation + "  ";
                    counter = 0;
                }

                result += currentchar;
                ++counter;
            }

            output = result;
        }

        return "\"" + output + "\"";
    }

    return value.toString();
}

const String prepareCommentYaml(const String &comment, const String &indentation)
{
    const int maxlength = 80;
    String::CharPointerType charptr = comment.getCharPointer();
    int counter = 1;
    String result = "";
    bool iscurrentnextline = false;

    for (;;)
    {
        if (*charptr == 0)
        {
            break;
        }

        if (*charptr == '\n')
        {
            iscurrentnextline = true;
        }

        if (counter == maxlength || exceedsMaxLength(charptr, counter, maxlength) || iscurrentnextline)
        {
            if(CharacterFunctions::isWhitespace(result.getLastCharacter()))
            {
                result = result.dropLastCharacters(1);
            }
            
            result += "\n" + indentation + "# ";
            counter = 0;
        }

        if (!iscurrentnextline)
        {
            result += *charptr;
        }

        ++counter;
        ++charptr;
        iscurrentnextline = false;
    }

    return indentation + "# " + result + "\n";
}

const bool appendSubPropertiesYaml(const Config::Property propParent, String &fileOutput, int level) noexcept
{
    bool isfirst = true;

    for (auto pairsetting = propParent.begin(); pairsetting != propParent.end(); ++pairsetting)
    {
        auto propsetting = pairsetting->second;
        var value = propsetting.getValue();
        bool hasvalidsubproperties = propsetting.hasValid();

        if (propsetting.isValid())
        {
            String comment = propsetting.getComment();
            String indentation;

            for (int i = 0; i < level; ++i)
            {
                indentation += "    ";
            }

            if (hasvalidsubproperties)
            {
                if (!comment.isEmpty())
                {
                    if (!isfirst)
                    {
                        fileOutput += "\n";
                    }

                    fileOutput += prepareCommentYaml(comment, indentation);
                }

                fileOutput += indentation + propsetting.getName() + ":\n";
                (void) jaut::appendSubPropertiesYaml(propsetting, fileOutput, level + 1);
            }
            else if (!value.isVoid())
            {
                if (!comment.isEmpty())
                {
                    if (!isfirst)
                    {
                        fileOutput += "\n";
                    }

                    fileOutput += prepareCommentYaml(comment, indentation);
                }

                fileOutput += indentation + propsetting.getName() + ": " + getVarValYaml(value, indentation) + "\n";
            }
        }

        isfirst = false;
    }

    return true;
}

const bool readPropertiesYaml(YAML::Node node, Config::Property parentProp) noexcept
{
    if (!parentProp.isValid() || !node)
    {
        return false;
    }

    if (parentProp.hasValid())
    {
        for (auto &setting_pair : parentProp)
        {
            if (!setting_pair.second.isValid())
            {
                continue;
            }

            YAML::Node node_setting = node[setting_pair.second.getName().toRawUTF8()];

            if (!node_setting)
            {
                continue;
            }

            (void) jaut::readPropertiesYaml(node_setting, setting_pair.second);
        }
    }
    else if(!parentProp.getValue().isVoid())
    {
        var value = parentProp.getValue();

        if (value.isArray())
        {
            if (!node.IsSequence())
            {
                return false;
            }

            Array<var> node_array;

            for (int i = 0; i < node.size(); ++i)
            {
                node_array.add(String(node[i].as<std::string>()));
            }

            value = node_array;
        }
        else if (value.isDouble() || value.isInt() || value.isInt64())
        {
            if (!node.IsScalar())
            {
                return false;
            }

            String data = node.as<std::string>();
            std::regex regex("[-+]?([0-9]*\\.[0-9]+|[0-9]+)");

            if (!data.isEmpty() && std::regex_match(data.toStdString(), regex))
            {
                if (value.isDouble())
                {
                    value = data.getDoubleValue();
                }
                else if (value.isInt())
                {
                    value = data.getIntValue();
                }
                else
                {
                    value = data.getLargeIntValue();
                }
            }
        }
        else if (value.isBool())
        {
            if (!node.IsScalar())
            {
                return false;
            }

            String data = node.as<std::string>();

            if (!data.isEmpty())
            {
                if (data.equalsIgnoreCase("yes") || data.equalsIgnoreCase("true") || data.equalsIgnoreCase("1"))
                {
                    value = true;
                }
                else if (data.equalsIgnoreCase("no") || data.equalsIgnoreCase("false") || data.equalsIgnoreCase("0"))
                {
                    value = false;
                }
            }
        }
        else if (value.isString())
        {
            if (!node.IsScalar())
            {
                return false;
            }

            String data = node.as<std::string>();

            if (!data.isEmpty())
            {
                value = data;
            }
        }

        parentProp.setValue(value);
    }

    return true;
}
#endif
}



/* ==================================================================================
 * ================================== XmlParser =====================================
 * ================================================================================== */

XmlParser::XmlParser(const String &defaultCategory, const String &tagSettingId, const String &tagIntroId,
                     const String &tagCategoryId) noexcept
    : defaultCategory(defaultCategory.trim()),
      tagSetting(tagSettingId.trim()),
      tagIntro(tagIntroId.trim()),
      tagCategory(tagCategoryId.trim())
{}

const bool XmlParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    XmlDocument document(configFile);
    std::unique_ptr<XmlElement> config(document.getDocumentElement());
    Config::Property propdefaultcategory = root.getProperty(defaultCategory);

    if (!config || !config->hasTagName(tagIntro))
    {
        return false;
    }

    forEachXmlChildElement(*config, xmlcategory)
    {
        if (xmlcategory->hasTagName(tagSetting))
        {
            String settingname = xmlcategory->getStringAttribute("name");
            (void) jaut::readPropertiesXml(xmlcategory, propdefaultcategory.getProperty(settingname), tagSetting); // @noret
        }
        else if (xmlcategory->hasTagName(tagCategory))
        {
            String categoryname   = xmlcategory->getStringAttribute("name");
            Config::Property propcategory = root.getProperty(categoryname);

            if (propcategory.isValid())
            {
                forEachXmlChildElementWithTagName(*xmlcategory, xmlsetting, tagSetting)
                {
                    String settingname = xmlsetting->getStringAttribute("name");
                    (void) jaut::readPropertiesXml(xmlsetting, propcategory.getProperty(settingname), tagSetting); // @noret
                }
            }
        }
    }

    return true;
}

const bool XmlParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    Config::Property propdefaultcategory = root.getProperty(defaultCategory);
    String comment               = root.getComment();
    String tag;
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        tag = "<!-- " + comment + " -->\n\n";
    }

    tag += "<" + tagIntro + ">\n";

    if (propdefaultcategory.isValid())
    {
        (void) jaut::appendSubPropertiesXml(propdefaultcategory, tag, 1, tagSetting); // @noret
    }

    for (auto &[name, prop] : root)
    {
        if (prop.getName().trim().equalsIgnoreCase(defaultCategory))
        {
            continue;
        }

        if (prop.hasValid())
        {
            String categorycomment = prop.getComment();

            if (!categorycomment.isEmpty())
            {
                tag += "\n    <!-- " + categorycomment + " -->\n";
            }

            tag += "    <" + tagCategory + " Name=\"" + prop.getName() + "\">\n";

            (void) jaut::appendSubPropertiesXml(prop, tag, 2, tagSetting);

            tag += "    </" + tagCategory + ">\n";
        }
    }

    tag += "</" + tagIntro + ">\n";
    configFile.replaceWithText(tag);

    return true;
}



/* ==================================================================================
 * ================================= JsonParser =====================================
 * ================================================================================== */

JsonParser::JsonParser(const String &noticeId) noexcept
    : noticeId(noticeId.trim())
{}

const bool JsonParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    var json;

    if (JSON::parse(configFile.loadFileAsString(), json).wasOk())
    {
        DynamicObject *jsonroot = json.getDynamicObject();

        if (!jsonroot)
        {
            return false;
        }

        for (auto &pairobjcategory : jsonroot->getProperties())
        {
            if (pairobjcategory.name.toString().equalsIgnoreCase("//"))
            {
                continue;
            }

            Config::Property propcategory = root.getProperty(pairobjcategory.name.toString());
            DynamicObject *objcategory = pairobjcategory.value.getDynamicObject();

            if (!propcategory.hasValid() || !objcategory)
            {
                continue;
            }

            for (auto &pairobjsetting : objcategory->getProperties())
            {
                if (pairobjsetting.name.toString().equalsIgnoreCase("//"))
                {
                    continue;
                }

                Config::Property propsetting = propcategory.getProperty(pairobjsetting.name.toString());

                if (!propsetting.isValid())
                {
                    continue;
                }

                (void) jaut::readPropertiesJson(pairobjsetting.value, propsetting); // @noret
            }
        }
    }

    return true;
}

const bool JsonParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    String comment = root.getComment();
    String tag = "{\n";
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        String::CharPointerType charptr = comment.getCharPointer();
        int counter = 1;
        String newtext = "";
        bool iscurrentnextline = false;

        for (;;)
        {
            if (*charptr == 0)
            {
                break;
            }

            if (*charptr == '\n')
            {
                iscurrentnextline = true;
            }

            if (counter == 80 || iscurrentnextline)
            {
                newtext += " \\\n               ";
                counter = 0;
            }

            if (!iscurrentnextline)
            {
                newtext += *charptr;
            }

            ++counter;
            ++charptr;
            iscurrentnextline = false;
        }

        tag += "    \"" + noticeId + "\": \"" + newtext + "\",\n\n\n";
    }

    for (auto i = root.begin(); i != root.end(); ++i)
    {
        auto propcategory = i->second;

        if (propcategory.hasValid())
        {
            String categorycomment = propcategory.getComment();
            auto iterator = i;

            if (!categorycomment.isEmpty())
            {
                tag += "\n    \"//\": \"" + jaut::prepareCommentJson(categorycomment, "    ") + "\",\n";
            }

            tag += "    \"" + propcategory.getName() + "\": {\n";

            (void) jaut::appendSubPropertiesJson(propcategory, tag, 2); // @noret

            auto end_iterator = root.end();
            tag += "\n    }" + jaut::getCommaOrNot(iterator, end_iterator);
        }
    }

    tag += "\n}";
    configFile.replaceWithText(tag);

    return true;
}



#ifdef YAML_CPP_API
/* ==================================================================================
 * ================================= YamlParser =====================================
 * ================================================================================== */
const bool YamlParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    bool was_successful = false;
    std::string yaml    = configFile.loadFileAsString().toStdString();
    YAML::Node node_root;

    try
    {
        node_root = YAML::Load(yaml);
    }
    catch (std::exception&)
    {
        return false;
    }

    if (node_root)
    {
        for (auto &category_pair : root)
        {
            if (!category_pair.second.hasValid())
            {
                continue;
            }

            YAML::Node node_category = node_root[category_pair.first.toRawUTF8()];

            if (!node_category)
            {
                continue;
            }

            for (auto &setting_pair : category_pair.second)
            {
                if (!setting_pair.second.isValid())
                {
                    continue;
                }

                YAML::Node node_setting = node_category[setting_pair.second.getName().toRawUTF8()];

                if (!node_setting)
                {
                    continue;
                }

                if (jaut::readPropertiesYaml(node_setting, setting_pair.second))
                {
                    was_successful = true;
                }
            }
        }
    }

    return was_successful;
}

const bool YamlParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        throw std::invalid_argument("The config root property can't be invalid and must define a clear structure.");
    }

    if (!configFile.exists())
    {
        return false;
    }

    String comment = root.getComment();
    String tag;
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        const int maxlength = 80;
        String::CharPointerType charptr = comment.getCharPointer();
        int counter = 1;
        String result = "";
        bool iscurrentnextline = false;

        for (;;)
        {
            if (*charptr == 0)
            {
                break;
            }

            if (*charptr == '\n')
            {
                iscurrentnextline = true;
            }

            if (counter == maxlength || jaut::exceedsMaxLength(charptr, counter, maxlength) || iscurrentnextline)
            {
                result += "\n# ";
                counter = 0;
            }

            if (!iscurrentnextline)
            {
                result += *charptr;
            }

            ++counter;
            ++charptr;
            iscurrentnextline = false;
        }

        tag += "# " + result + "\n\n";
    }

    int count = 0;

    for (auto it = root.begin(); it != root.end(); ++it)
    {
        Config::Property propcategory = it->second;

        if (propcategory.hasValid())
        {
            String categorycomment = propcategory.getComment();

            if (!categorycomment.isEmpty())
            {
                tag += jaut::prepareCommentYaml(categorycomment, "");
            }

            tag += propcategory.getName() + ":\n";

            (void) jaut::appendSubPropertiesYaml(propcategory, tag, 1);

            if (count < (root.size() - 1))
            {
                tag += "\n";
            }
        }

        ++count;
    }

    configFile.replaceWithText(tag);
    return true;
}
#endif
}
