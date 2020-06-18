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
    @file   configparser.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#ifdef YAML_CPP_API
#   include <regex>
#endif

namespace jaut
{
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
juce::String getCommaOrNot(jaut::Config::Property::ConstIterator begin,
                           const jaut::Config::Property::ConstIterator &end)
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

bool exceedsMaxLength(juce::String::CharPointerType charPtr, int currentLength, int maxLength)
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

juce::String prepareCommentJson(const juce::String &comment, const juce::String &indentation)
{
    const int max_length = 80;
    juce::String::CharPointerType char_ptr = comment.getCharPointer();
    int counter = 1;
    bool is_current_next_line = false;
    juce::String result;
    
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

juce::String getVarVal(const juce::var &value) noexcept
{
    if (value.isVoid())
    {
        return "\"\"";
    }
    
    return !value.isString() ? (value.isBool() ? (value ? "true" : "false") : value.toString())
                             : "\"" + value.toString() + "\"";
}

juce::String appendSubPropertiesXml(const jaut::Config::Property &parent, int level,
                                    const juce::String &tagSetting) noexcept
{
    juce::String output;
    
    for (const auto &[name, setting_property] : parent)
    {
        const juce::var value = setting_property.getValue();
        const bool has_valid_sub_properties = setting_property.hasValid();
        
        if (setting_property.isValid() && (!value.isVoid() || has_valid_sub_properties))
        {
            const juce::String comment = setting_property.getComment();
            juce::String indentation;
            
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

bool readPropertiesXml(const juce::XmlElement *const xml, jaut::Config::Property parent,
                       const juce::String &tagSetting) noexcept
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

juce::String appendSubPropertiesJson(const jaut::Config::Property &parent, int level) noexcept
{
    juce::String output;
    
    for (auto it = parent.begin(); it != parent.end(); ++it)
    {
        const jaut::Config::Property property = it->second;
        const juce::var &value                = property.getValue();
        
        if (property.isValid())
        {
            const juce::String comment = property.getComment();
            juce::String indentation;
            
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

bool readPropertiesJson(const juce::var &json, jaut::Config::Property parent) noexcept
{
    juce::DynamicObject *const setting_obj = json.getDynamicObject();
    
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
        
        jaut::Config::Property property = parent.getProperty(name.toString());
        
        if (property.isValid())
        {
            (void) readPropertiesJson(value, property);
        }
    }
    
    return true;
}

#ifdef YAML_CPP_API
juce::String getVarValYaml(const juce::var &value, const juce::String &indentation) noexcept
{
    if (value.isDouble())
    {
        juce::String double_value = value.toString();
        
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
        juce::String output;
        
        for (const juce::var &entry : *value.getArray())
        {
            output << "\n" << indentation << "- " + entry.toString();
        }
        
        return output;
    }
    else if (value.isString())
    {
        juce::String output = value.toString();
        
        if (output.containsChar('\n'))
        {
            juce::String::CharPointerType char_ptr = output.getCharPointer();
            juce::String result = "|\n" + indentation + "  ";
            juce::juce_wchar current_char;
            
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
            juce::String::CharPointerType char_ptr = output.getCharPointer();
            juce::String result = ">\n" + indentation + "  ";
            int counter         = 1;
            juce::juce_wchar current_char;
            
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

juce::String prepareCommentYaml(const juce::String &comment, const juce::String &indentation)
{
    constexpr int max_length = 80;
    juce::String::CharPointerType char_ptr = comment.getCharPointer();
    bool is_current_next_line = false;
    int counter               = 1;
    juce::String result       = "";
    
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
            if(juce::CharacterFunctions::isWhitespace(result.getLastCharacter()))
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

juce::String appendSubPropertiesYaml(const jaut::Config::Property &parent, int level) noexcept
{
    juce::String output;
    bool is_first = true;
    
    for (const auto &it : parent)
    {
        const jaut::Config::Property property = it.second;
        const juce::var &value                = property.getValue();
        
        if (property.isValid())
        {
            juce::String comment = property.getComment();
            juce::String indentation;
            
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

bool readPropertiesYaml(YAML::Node node, jaut::Config::Property parent) noexcept
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
        juce::var value = parent.getValue();
        
        if (value.isArray())
        {
            if (!node.IsSequence())
            {
                return false;
            }
    
            juce::Array<juce::var> node_array;
            
            for (auto &&i : node)
            {
                node_array.add(juce::String(i.as<std::string>()));
            }
            
            value = node_array;
        }
        else if (value.isDouble() || value.isInt() || value.isInt64())
        {
            if (!node.IsScalar())
            {
                return false;
            }
            
            const juce::String data = node.as<std::string>();
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
            
            const juce::String data = node.as<std::string>();
            
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
            
            const juce::String data = node.as<std::string>();
            
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
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region XmlParser
//======================================================================================================================
XmlParser::XmlParser(const juce::String &tagSettingId, const juce::String &tagIntroId,
                     const juce::String &tagCategoryId) noexcept
    : tagSetting(tagSettingId.trim()),
      tagIntro(tagIntroId.trim()),
      tagCategory(tagCategoryId.trim())
{}

//======================================================================================================================
OperationResult XmlParser::parseConfig(const juce::File &configFile, PropertyType root) const
{
    if (!root.isValid())
    {
        DBG("Root node is invalid.");
        jassertfalse;
        return ErrorCodes::InvalidRootNote;
    }

    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }
    
    const std::unique_ptr<juce::XmlElement> config = juce::XmlDocument(configFile).getDocumentElement();
    
    if (!config || !config->hasTagName(tagIntro))
    {
        DBG("Config is missing root tag.");
        return ErrorCodes::InvalidConfig;
    }
    
    forEachXmlChildElement(*config, xml_category)
    {
        if (xml_category->hasTagName(tagCategory))
        {
            const juce::String category_name     = xml_category->getStringAttribute("name");
            const Config::Property prop_category = root.getProperty(category_name);
            
            if (prop_category.isValid())
            {
                forEachXmlChildElementWithTagName(*xml_category, xml_setting, tagSetting)
                {
                    const juce::String setting_name = xml_setting->getStringAttribute("name");
                    (void) jaut::readPropertiesXml(xml_setting, prop_category.getProperty(setting_name).getObject(),
                                               tagSetting);
                }
            }
        }
    }

    return true;
}

OperationResult XmlParser::writeConfig(const juce::File &configFile, ConstPropertyType root) const
{
    const PropertyType &root_prop = root.getObject();
    
    if (!root_prop.isValid())
    {
        DBG("Root node is invalid.");
        jassertfalse;
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    juce::String comment = root_prop.getComment();
    juce::String tag;
    
    configFile.replaceWithText("");
    
    if (!comment.isEmpty())
    {
        tag = "<!-- " + comment + " -->\n\n";
    }
    
    tag << "<" << tagIntro << ">\n";
    
    for (const auto &[name, property] : root_prop)
    {
        if (property.hasValid())
        {
            const juce::String category_comment = property.getComment();
            
            if (!category_comment.isEmpty())
            {
                // <!-- Comment -->
                tag << "\n    <!-- " << category_comment << " -->\n";
            }
            
            
            tag /* <Category>  */ << "    <" << tagCategory << " Name=\"" << property.getName() << "\">\n"
                /*     <Setting>   */ << jaut::appendSubPropertiesXml(property, 2, tagSetting)
                /* </Category> */ << "    </" << tagCategory << ">\n";
        }
    }

    tag << "</" << tagIntro << ">\n";
    configFile.replaceWithText(tag);

    return true;
}
//======================================================================================================================
// endregion XmlParser
//**********************************************************************************************************************
// region JsonParser
//======================================================================================================================
JsonParser::JsonParser(const juce::String &noticeId) noexcept
    : noticeId(noticeId.trim())
{}

//======================================================================================================================
OperationResult JsonParser::parseConfig(const juce::File &configFile, PropertyType root) const
{
    if (!root.isValid())
    {
        DBG("Root node is invalid.");
        jassertfalse;
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    juce::var json;

    if (juce::JSON::parse(configFile.loadFileAsString(), json).wasOk())
    {
        juce::DynamicObject *const obj_root = json.getDynamicObject();

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
    
            juce::DynamicObject *const obj_category = json_category.getDynamicObject();
            Config::Property property_category      = root.getProperty(category_name.toString());

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

                (void) jaut::readPropertiesJson(json_settings, property_setting);
            }
        }
    }

    return true;
}

OperationResult JsonParser::writeConfig(const juce::File &configFile, ConstPropertyType root) const
{
    const Config::Property &root_prop = root.getObject();
    
    if (!root_prop.isValid())
    {
        DBG("Root node is invalid.");
        jassertfalse;
        return ErrorCodes::InvalidRootNote;
    }
    
    if (!configFile.exists())
    {
        DBG("File not found: " << configFile.getFullPathName());
        return ErrorCodes::FileNotFound;
    }

    const juce::String comment = root_prop.getComment();
    juce::String tag = "{\n";
    
    configFile.replaceWithText("");
    
    if (!comment.isEmpty())
    {
        juce::String::CharPointerType char_ptr = comment.getCharPointer();
        int counter = 1;
        juce::String new_text = "";
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

    for (auto it = root_prop.begin(); it != root_prop.end(); ++it)
    {
        const Config::Property property_category = it->second;

        if (property_category.hasValid())
        {
            const juce::String category_comment = property_category.getComment();
            
            if (!category_comment.isEmpty())
            {
                tag << "\n    \"//\": \"" << jaut::prepareCommentJson(category_comment, "    ") << "\",\n";
            }

            tag << "    \"" << property_category.getName() << "\": {\n"
                << jaut::appendSubPropertiesJson(property_category, 2)
                << "\n    }" << jaut::getCommaOrNot(it, root_prop.end());
        }
    }

    tag << "\n}";
    configFile.replaceWithText(tag);

    return true;
}
//======================================================================================================================
// endregion JsonParser
//**********************************************************************************************************************
// region YamlParser
//======================================================================================================================
#ifdef YAML_CPP_API
OperationResult YamlParser::parseConfig(const juce::File &configFile, PropertyType root) const
{
    if (!root.isValid())
    {
        DBG("Root node is invalid.");
        jassertfalse;
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

                if (jaut::readPropertiesYaml(node_setting, property_setting))
                {
                    error_code = 0;
                }
            }
        }
    }

    return error_code;
}

OperationResult YamlParser::writeConfig(const juce::File &configFile, ConstPropertyType root) const
{
    const Config::Property &root_prop = root.getObject();
    
    if (!root_prop.isValid())
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

    const juce::String comment = root_prop.getComment();
    juce::String tag;
    
    configFile.replaceWithText("");

    if (!comment.isEmpty())
    {
        const int max_length = 80;
        juce::String::CharPointerType char_ptr = comment.getCharPointer();
        int counter = 1;
        juce::String result = "";
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

            if (counter == max_length || jaut::exceedsMaxLength(char_ptr, counter, max_length) || is_current_next_line)
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
    
    for (auto it = root_prop.begin(); it != root_prop.end(); ++it)
    {
        const Config::Property property_category = it->second;
        
        if (property_category.hasValid())
        {
            const juce::String category_comment = property_category.getComment();

            if (!category_comment.isEmpty())
            {
                tag << jaut::prepareCommentYaml(category_comment, "");
            }

            tag << property_category.getName() << ":\n" << jaut::appendSubPropertiesYaml(property_category, 1);

            if (std::distance(root_prop.begin(), it) < (root_prop.size() - 1))
            {
                tag << "\n";
            }
        }
    }

    configFile.replaceWithText(tag);
    return true;
}
#endif
//======================================================================================================================
// endregion YamlParser
//**********************************************************************************************************************
}
