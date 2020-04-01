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

#ifdef YAML_CPP_API
#include <regex>
#endif

// region Namespace
namespace
{
using namespace jaut;
String getCommaOrNot(Config::Property::ConstIterator begin, const Config::Property::ConstIterator &end)
{
    for (++begin; begin != end; ++begin)
    {
        if (begin->second.isValid())
        {
            return ",\n";
        }
    }
    
    return "";
}

bool exceedsMaxLength(String::CharPointerType charPtr, int currentLength, int maxLength)
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

String prepareCommentJson(const String &comment, const String &indentation)
{
    const int max_length = 80;
    String::CharPointerType char_ptr = comment.getCharPointer();
    int counter = 1;
    bool is_current_next_line = false;
    String result;
    
    for (;;)
    {
        if (*char_ptr == 0)
        {
            break;
        }
        
        if (*char_ptr == '\n')
        {
            is_current_next_line = true;
        }
        
        if (counter == max_length || exceedsMaxLength(char_ptr, counter, max_length) || is_current_next_line)
        {
            result += " \\\n       " + indentation;
            counter = 0;
        }
        
        if (!is_current_next_line)
        {
            result += *char_ptr;
        }
        
        ++counter;
        ++char_ptr;
        is_current_next_line = false;
    }
    
    return result;
}

String getVarVal(const var &value) noexcept
{
    if (value.isVoid())
    {
        return "\"\"";
    }
    
    return !value.isString() ? (value.isBool() ? (value ? "true" : "false") : value.toString())
                             : "\"" + value.toString() + "\"";
}

String appendSubPropertiesXml(const Config::Property parent, int level, const String &tagSetting) noexcept
{
    String output;
    
    for (const auto &[name, setting_property] : parent)
    {
        const var value = setting_property.getValue();
        const bool has_valid_sub_properties = setting_property.hasValid();
        
        if (setting_property.isValid() && (!value.isVoid() || has_valid_sub_properties))
        {
            const String comment = setting_property.getComment();
            String indentation;
            
            for (int i = 0; i < level; ++i)
            {
                indentation += "    ";
            }
            
            if (!comment.isEmpty())
            {
                output << "\n" << indentation << "<!-- " << comment << " -->\n";
            }
            
            output << indentation << "<" << tagSetting << " Name=\"" << setting_property.getName();
            
            if (!value.isVoid())
            {
                output << "\" Value=\"" << value.toString() << "\"";
            }
            
            if (has_valid_sub_properties)
            {
                output << ">\n" << appendSubPropertiesXml(setting_property, level + 1, tagSetting)
                       << indentation << "</" << tagSetting << ">\n";
            }
            else
            {
                output << "/>\n";
            }
        }
    }
    
    return output;
}

bool readPropertiesXml(const XmlElement *const xml, Config::Property parent, const String &tagSetting) noexcept
{
    const int num_childs = xml->getNumChildElements();
    
    if (!parent.isValid() || (num_childs == 0 && xml->getStringAttribute("value").isEmpty()))
    {
        return false;
    }
    
    parent.setValue(xml->getStringAttribute("value"));
    
    if (num_childs > 0)
    {
        forEachXmlChildElementWithTagName(*xml, setting, tagSetting)
        {
            (void) readPropertiesXml(setting, parent.getProperty(setting->getStringAttribute("name")), tagSetting);
        }
    }
    
    return true;
}

String appendSubPropertiesJson(const Config::Property parent, int level) noexcept
{
    String output;
    
    for (auto it = parent.begin(); it != parent.end(); ++it)
    {
        const Config::Property property = it->second;
        const var &value                = property.getValue();
        
        if (property.isValid())
        {
            const String comment = property.getComment();
            String indentation;
            
            for (int i = 0; i < level; ++i)
            {
                indentation += "    ";
            }
            
            if (!comment.isEmpty())
            {
                output << "\n" << indentation << R"("//": ")" << prepareCommentJson(comment, indentation) << "\",\n";
            }
            
            output << indentation << "\"" << property.getName() << "\": ";
            
            if (property.hasValid())
            {
                output << "{\n";
                
                if (!value.isVoid())
                {
                    output << "    " << indentation << "\"value\": " << getVarVal(value) << ",\n";
                }
                
                output << appendSubPropertiesJson(property, level + 1) << "\n" << indentation << "}"
                       << getCommaOrNot(it, property.end());
            }
            else
            {
                output << getVarVal(value) << getCommaOrNot(it, property.end());
            }
        }
    }
    
    return output;
}

bool readPropertiesJson(const var &json, Config::Property parent) noexcept
{
    DynamicObject *const setting_obj = json.getDynamicObject();
    
    if (!setting_obj)
    {
        parent.setValue(json);
        return !json.isVoid();
    }
    
    for (const auto &[name, value] : setting_obj->getProperties())
    {
        if (name.toString().equalsIgnoreCase("//"))
        {
            continue;
        }
        
        if (name.toString().equalsIgnoreCase("value"))
        {
            parent.setValue(value);
            continue;
        }
        
        Config::Property property = parent.getProperty(name.toString());
        
        if (property.isValid())
        {
            (void) readPropertiesJson(value, property);
        }
    }
    
    return true;
}

#ifdef YAML_CPP_API
String getVarValYaml(const var &value, const String &indentation) noexcept
{
    if (value.isDouble())
    {
        String double_value = value.toString();
        
        if (!double_value.containsChar('.'))
        {
            double_value += ".0";
        }
        
        return double_value;
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
        String output;
        
        for (const var &entry : *value.getArray())
        {
            output << "\n" << indentation << "- " + entry.toString();
        }
        
        return output;
    }
    else if (value.isString())
    {
        String output = value.toString();
        
        if (output.containsChar('\n'))
        {
            String::CharPointerType char_ptr = output.getCharPointer();
            String result = "|\n" + indentation + "  ";
            juce_wchar current_char;
            
            for (;;)
            {
                current_char = char_ptr.getAndAdvance();
                
                if (current_char == 0)
                {
                    break;
                }
                
                if (current_char == '\n')
                {
                    result << current_char << indentation << "  ";
                    continue;
                }
                
                result << current_char;
            }
            
            output = result;
        }
        else if(output.length() > 80 && output.containsChar(' '))
        {
            String::CharPointerType char_ptr = output.getCharPointer();
            String result = ">\n" + indentation + "  ";
            int counter   = 1;
            juce_wchar current_char;
            
            for (;;)
            {
                current_char = char_ptr.getAndAdvance();
                
                if (current_char == 0)
                {
                    break;
                }
                
                if (exceedsMaxLength((char_ptr - 1), counter, 80))
                {
                    result = result.dropLastCharacters(1);
                    result << "\n" << indentation << "  ";
                    counter = 0;
                }
                
                result << current_char;
                ++counter;
            }
            
            output = result;
        }
        
        return "\"" + output + "\"";
    }
    
    return value.toString();
}

String prepareCommentYaml(const String &comment, const String &indentation)
{
    const int max_length = 80;
    String::CharPointerType char_ptr = comment.getCharPointer();
    bool is_current_next_line        = false;
    int counter   = 1;
    String result = "";
    
    for (;;)
    {
        if (*char_ptr == 0)
        {
            break;
        }
        
        if (*char_ptr == '\n')
        {
            is_current_next_line = true;
        }
        
        if (counter == max_length || exceedsMaxLength(char_ptr, counter, max_length) || is_current_next_line)
        {
            if(CharacterFunctions::isWhitespace(result.getLastCharacter()))
            {
                result = result.dropLastCharacters(1);
            }
            
            result << "\n" << indentation << "# ";
            counter = 0;
        }
        
        if (!is_current_next_line)
        {
            result << *char_ptr;
        }
        
        ++counter;
        ++char_ptr;
        is_current_next_line = false;
    }
    
    return indentation + "# " + result + "\n";
}

String appendSubPropertiesYaml(const Config::Property parent, int level) noexcept
{
    String output;
    bool is_first = true;
    
    for (const auto &it : parent)
    {
        const Config::Property property = it.second;
        const var &value                = property.getValue();
        
        if (property.isValid())
        {
            String comment = property.getComment();
            String indentation;
            
            for (int i = 0; i < level; ++i)
            {
                indentation << "    ";
            }
            
            if (property.hasValid())
            {
                if (!comment.isEmpty())
                {
                    if (!is_first)
                    {
                        output << "\n";
                    }
                    
                    output << prepareCommentYaml(comment, indentation);
                }
                
                output << indentation << property.getName() << ":\n"
                       << appendSubPropertiesYaml(property, level + 1);
            }
            else if (!value.isVoid())
            {
                if (!comment.isEmpty())
                {
                    if (!is_first)
                    {
                        output << "\n";
                    }
                    
                    output << prepareCommentYaml(comment, indentation);
                }
                
                output << indentation << property.getName() << ": " << getVarValYaml(value, indentation) << "\n";
            }
        }
        
        is_first = false;
    }
    
    return output;
}

bool readPropertiesYaml(YAML::Node node, Config::Property parent) noexcept
{
    if (!parent.isValid() || !node)
    {
        return false;
    }
    
    if (parent.hasValid())
    {
        for (auto &[_, property] : parent)
        {
            if (!property.isValid())
            {
                continue;
            }
            
            const YAML::Node node_setting = node[property.getName().toRawUTF8()];
            
            if (!node_setting)
            {
                continue;
            }
            
            (void) readPropertiesYaml(node_setting, property);
        }
    }
    else if(!parent.getValue().isVoid())
    {
        var value = parent.getValue();
        
        if (value.isArray())
        {
            if (!node.IsSequence())
            {
                return false;
            }
            
            Array<var> node_array;
            
            for (auto &&i : node)
            {
                node_array.add(String(i.as<std::string>()));
            }
            
            value = node_array;
        }
        else if (value.isDouble() || value.isInt() || value.isInt64())
        {
            if (!node.IsScalar())
            {
                return false;
            }
            
            const String data = node.as<std::string>();
            const std::regex regex("[-+]?([0-9]*\\.[0-9]+|[0-9]+)");
            
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
            
            const String data = node.as<std::string>();
            
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
            
            const String data = node.as<std::string>();
            
            if (!data.isEmpty())
            {
                value = data;
            }
        }
        
        parent.setValue(value);
    }
    
    return true;
}
#endif
}
// endregion Namespace

namespace jaut
{
// region XmlParser
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

//======================================================================================================================
OperationResult XmlParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }

    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }
    
    XmlDocument document(configFile);
    std::unique_ptr<XmlElement> config(document.getDocumentElement());
    
    if (!config || !config->hasTagName(tagIntro))
    {
        DBG("Config is missing root tag.");
        return ErrorCodes::InvalidConfig;
    }
    
    Config::Property default_category_property = root.getProperty(defaultCategory);
    
    forEachXmlChildElement(*config, xml_category)
    {
        if (xml_category->hasTagName(tagSetting))
        {
            const String category_name = xml_category->getStringAttribute("name");
            (void) ::readPropertiesXml(xml_category, default_category_property.getProperty(category_name),
                                           tagSetting);
        }
        else if (xml_category->hasTagName(tagCategory))
        {
            const String category_name     = xml_category->getStringAttribute("name");
            Config::Property prop_category = root.getProperty(category_name);
            
            if (prop_category.isValid())
            {
                forEachXmlChildElementWithTagName(*xml_category, xml_setting, tagSetting)
                {
                    const String setting_name = xml_setting->getStringAttribute("name");
                    (void) ::readPropertiesXml(xml_setting, prop_category.getProperty(setting_name), tagSetting);
                }
            }
        }
    }

    return true;
}

OperationResult XmlParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    const Config::Property default_category_property = root.getProperty(defaultCategory);
    String comment = root.getComment();
    String tag;
    
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        tag = "<!-- " + comment + " -->\n\n";
    }

    tag << "<" << tagIntro << ">\n";

    if (default_category_property.isValid())
    {
        tag << ::appendSubPropertiesXml(default_category_property, 1, tagSetting);
    }

    for (const auto &[name, property] : root)
    {
        if (name.equalsIgnoreCase(defaultCategory))
        {
            continue;
        }

        if (property.hasValid())
        {
            const String category_comment = property.getComment();

            if (!category_comment.isEmpty())
            {
                tag << "\n    <!-- " << category_comment << " -->\n";
            }

            tag << "    <" << tagCategory << " Name=\"" << property.getName() << "\">\n"
                << ::appendSubPropertiesXml(property, 2, tagSetting)
                << "    </" << tagCategory << ">\n";
        }
    }

    tag << "</" << tagIntro << ">\n";
    configFile.replaceWithText(tag);

    return true;
}
// endregion XmlParser
// region JsonParser
/* ==================================================================================
 * ================================= JsonParser =====================================
 * ================================================================================== */
JsonParser::JsonParser(const String &noticeId) noexcept
    : noticeId(noticeId.trim())
{}

OperationResult JsonParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    var json;

    if (JSON::parse(configFile.loadFileAsString(), json).wasOk())
    {
        DynamicObject *const obj_root = json.getDynamicObject();

        if (!obj_root)
        {
            DBG("Json file is invalid.");
            return ErrorCodes::InvalidConfig;
        }

        for (const auto &[category_name, json_category] : obj_root->getProperties())
        {
            if (category_name.toString().equalsIgnoreCase("//"))
            {
                continue;
            }
    
            DynamicObject *const obj_category  = json_category.getDynamicObject();
            Config::Property property_category = root.getProperty(category_name.toString());

            if (!property_category.hasValid() || !obj_category)
            {
                continue;
            }

            for (const auto &[setting_name, json_settings] : obj_category->getProperties())
            {
                if (setting_name.toString().equalsIgnoreCase("//"))
                {
                    continue;
                }

                Config::Property property_setting = property_category.getProperty(setting_name.toString());

                if (!property_setting.isValid())
                {
                    continue;
                }

                (void) ::readPropertiesJson(json_settings, property_setting);
            }
        }
    }

    return true;
}

OperationResult JsonParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    const String comment = root.getComment();
    String tag = "{\n";
    
    configFile.replaceWithText("");
    
    if (!comment.isEmpty())
    {
        String::CharPointerType char_ptr = comment.getCharPointer();
        int counter = 1;
        String new_text = "";
        bool is_current_next_line = false;

        for (;;)
        {
            if (*char_ptr == 0)
            {
                break;
            }

            if (*char_ptr == '\n')
            {
                is_current_next_line = true;
            }

            if (counter == 80 || is_current_next_line)
            {
                new_text += " \\\n               ";
                counter = 0;
            }

            if (!is_current_next_line)
            {
                new_text += *char_ptr;
            }

            ++counter;
            ++char_ptr;
            is_current_next_line = false;
        }

        tag << "    \"" << noticeId << "\": \"" << new_text << "\",\n\n\n";
    }

    for (auto it = root.begin(); it != root.end(); ++it)
    {
        const Config::Property property_category = it->second;

        if (property_category.hasValid())
        {
            const String category_comment = property_category.getComment();
            
            if (!category_comment.isEmpty())
            {
                tag << "\n    \"//\": \"" << ::prepareCommentJson(category_comment, "    ") << "\",\n";
            }

            tag << "    \"" << property_category.getName() << "\": {\n"
                << ::appendSubPropertiesJson(property_category, 2)
                << "\n    }" << ::getCommaOrNot(it, root.end());
        }
    }

    tag << "\n}";
    configFile.replaceWithText(tag);

    return true;
}
// endregion JsonParser
// region YamlParser
#ifdef YAML_CPP_API
/* ==================================================================================
 * ================================= YamlParser =====================================
 * ================================================================================== */
OperationResult YamlParser::parseConfig(const File &configFile, Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }
    
    const std::string yaml = configFile.loadFileAsString().toStdString();
    YAML::Node node_root;

    try
    {
        node_root = YAML::Load(yaml);
    }
    catch (std::exception&)
    {
        DBG("YAML file is invalid.");
        return ErrorCodes::InvalidConfig;
    }
    
    int error_code = ErrorCodes::InvalidConfig;
    
    if (node_root)
    {
        for (auto &[category_name, property_category] : root)
        {
            if (!property_category.hasValid())
            {
                continue;
            }

            const YAML::Node node_category = node_root[category_name.toRawUTF8()];

            if (!node_category)
            {
                continue;
            }

            for (auto &[setting_name, property_setting] : property_category)
            {
                if (!property_setting.isValid())
                {
                    continue;
                }

                YAML::Node node_setting = node_category[property_setting.getName().toRawUTF8()];
                
                if (!node_setting)
                {
                    continue;
                }

                if (::readPropertiesYaml(node_setting, property_setting))
                {
                    error_code = 0;
                }
            }
        }
    }

    return error_code;
}

OperationResult YamlParser::writeConfig(const File &configFile, const Config::Property root) const
{
    if (!root.isValid())
    {
        jassertfalse;
        DBG("Root node is invalid.");
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    const String comment = root.getComment();
    String tag;
    
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        const int max_length = 80;
        String::CharPointerType char_ptr = comment.getCharPointer();
        int counter = 1;
        String result = "";
        bool is_current_next_line = false;

        for (;;)
        {
            if (*char_ptr == 0)
            {
                break;
            }

            if (*char_ptr == '\n')
            {
                is_current_next_line = true;
            }

            if (counter == max_length || ::exceedsMaxLength(char_ptr, counter, max_length) || is_current_next_line)
            {
                result << "\n# ";
                counter = 0;
            }

            if (!is_current_next_line)
            {
                result << *char_ptr;
            }

            ++counter;
            ++char_ptr;
            is_current_next_line = false;
        }

        tag << "# " << result << "\n\n";
    }
    
    for (auto it = root.begin(); it != root.end(); ++it)
    {
        const Config::Property property_category = it->second;
        
        if (property_category.hasValid())
        {
            const String category_comment = property_category.getComment();

            if (!category_comment.isEmpty())
            {
                tag << ::prepareCommentYaml(category_comment, "");
            }

            tag << property_category.getName() << ":\n"
                << ::appendSubPropertiesYaml(property_category, 1);

            if (std::distance(root.begin(), it) < (root.size() - 1))
            {
                tag << "\n";
            }
        }
    }

    configFile.replaceWithText(tag);
    
    return true;
}
#endif
// endregion YamlParser
}
