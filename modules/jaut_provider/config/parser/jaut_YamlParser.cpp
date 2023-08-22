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
    @file   jaut_YamlParser.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut_provider/config/parser/jaut_YamlParser.h>
#include <jaut_provider/config/parser/detail/jaut_ParserUtil.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
#if JAUT_OPTLIB_YAML_CPP
namespace
{
    //==================================================================================================================
    using ContentGenerator = std::function<void(juce::String &output, jaut::detail::Indent indent)>;
    
    //==================================================================================================================
    // CONVERSION ============================
    juce::String yamlVarToValue(const juce::var&, jaut::detail::Indent, const jaut::YamlParser::Options&);
    juce::String yamlVarToArray(const juce::var                 &value,
                                jaut::detail::Indent            indent,
                                const jaut::YamlParser::Options &options)
    {
        if (value.getArray()->isEmpty())
        {
            return "[]";
        }
        
        const juce::String           indent_p0    = indent.toString();
        const juce::String           entry_indent = juce::String().paddedLeft(' ', indent.width - 1);
        const juce::Array<juce::var> &array       = *value.getArray();
        
        juce::String output;
        output << "-" << entry_indent << ::yamlVarToValue(array.getReference(0), indent.next(), options);
        
        for (auto it = (array.begin() + 1); it != array.end(); ++it)
        {
            const juce::var &e_value = *it;
            output << '\n' << indent_p0 << "-" << entry_indent << ::yamlVarToValue(e_value, indent.next(), options);
        }
        
        return output;
    }
    
    juce::String yamlStringToValue(const juce::String              &text,
                                   jaut::detail::Indent            indent,
                                   const jaut::YamlParser::Options &options)
    {
        const juce::String indent_p0 = indent.toString();
        
        if (text.containsChar('\n') && !text.endsWithChar('\n'))
        {
            if (options.breakStringsWithNewLinesCharacters)
            {
                const juce::StringArray lines = juce::StringArray::fromLines(text);
                return "|-\n" + indent_p0 + lines.joinIntoString("\n" + indent_p0);
            }
        }
        else if (options.breakLongStringsAfter > 0 && text.length() > options.breakLongStringsAfter
                 && !text.endsWithChar('\n'))
        {
            juce::StringArray lines;
            lines.ensureStorageAllocated(static_cast<int>(std::ceil(text.length()
                                         / static_cast<double>(options.breakLongStringsAfter))));
            
            for (int i = 0; i < text.length();)
            {
                const int current_last   = std::min(i + options.breakLongStringsAfter, text.length()) - 1;
                const juce::juce_wchar c = text[current_last];
                
                if (c != ' ')
                {
                    bool matched = false;
                    
                    for (int j = (current_last - 1); j > i; --j)
                    {
                        const juce::juce_wchar cc = text[j];
                        
                        if (cc == ' ')
                        {
                            lines.addLines(text.substring(i, j));
                            matched = true;
                            i       = (j + 1);
                            break;
                        }
                    }
                    
                    if (matched)
                    {
                        continue;
                    }
                    
                    for (int j = (current_last + 1);; ++j)
                    {
                        const juce::juce_wchar cc = text[j];
                        
                        if (cc == ' ' || cc == 0)
                        {
                            lines.addLines(text.substring(i, j));
                            matched = true;
                            i       = (j + 1);
                            break;
                        }
                    }
                    
                    continue;
                }
                
                lines.add(text.substring(i, std::min(i + options.breakLongStringsAfter, text.length()) - 1));
                i += options.breakLongStringsAfter;
            }
            
            return ">-\n" + indent_p0 + lines.joinIntoString("\n" + indent_p0);
        }

        return '"' + jaut::detail::encodeJsonString(text) + '"';
    }
    
    juce::String yamlMapToValue(const juce::NamedValueSet       &props,
                                jaut::detail::Indent            indent,
                                const jaut::YamlParser::Options &options)
    {
        if (props.isEmpty())
        {
            return "{}";
        }
        
        const juce::String indent_p0 = indent.toString();
        const juce::String indent_p1 = indent.toString(+1);
        juce::String       output;
        
        {
            const auto &[first_id, first_obj] = *props.begin();
            const juce::String text           = ::yamlVarToValue(first_obj, indent.next(), options);
            
            if (((first_obj.getArray() || first_obj.getDynamicObject()) && text.containsChar('\n')))
            {
                output << first_id << ":\n" << indent_p1 << text;
            }
            else
            {
                output << first_id << ": " << text;
            }
        }
        
        for (auto it = (props.begin() + 1); it != props.end(); ++it)
        {
            const auto         &[mapped_id, mapped_obj] = *it;
            const juce::String text                     = ::yamlVarToValue(mapped_obj, indent.next(), options);
            
            if (((mapped_obj.getArray() || mapped_obj.getDynamicObject()) && text.containsChar('\n')))
            {
                output << '\n' << indent_p0 << mapped_id << ":\n" << indent_p1 << text;
            }
            else
            {
                output << '\n' << indent_p0 << mapped_id << ": " << text;
            }
        }
        
        return output;
    }
    
    juce::String yamlVarToValue(const juce::var                 &value,
                                jaut::detail::Indent            indent,
                                const jaut::YamlParser::Options &options)
    {
        switch (jaut::VarUtil::getVarType(value))
        {
            case jaut::VarUtil::VarTypeId::Double:
            case jaut::VarUtil::VarTypeId::Int:
            case jaut::VarUtil::VarTypeId::Int64:
                return value.toString();
            
            case jaut::VarUtil::VarTypeId::String:
                return ::yamlStringToValue(value.toString(), indent, options);
            
            case jaut::VarUtil::VarTypeId::Bool:
                return (static_cast<bool>(value) ? "true" : "false");
            
            case jaut::VarUtil::VarTypeId::Array:
                return ::yamlVarToArray(value, indent, options);
            
            case jaut::VarUtil::VarTypeId::DynamicObject:
                 return ::yamlMapToValue(value.getDynamicObject()->getProperties(), indent, options);
            
            case jaut::VarUtil::VarTypeId::Void:
                return "null";
            
            case jaut::VarUtil::VarTypeId::OtherObject:
            case jaut::VarUtil::VarTypeId::Method:
            case jaut::VarUtil::VarTypeId::Unknown:
            case jaut::VarUtil::VarTypeId::Undefined:
            case jaut::VarUtil::VarTypeId::BinaryData:
                break;
        }
        
        throw jaut::ConfigIncompatibleTypeException::invalidVarType(value);
    }
    
    //==================================================================================================================
    // GENERATION ============================
    juce::String yamlGenerateEntity(const juce::String &id, ContentGenerator generator, jaut::detail::Indent indent)
    {        
        juce::String output;
        generator(output, indent.next());
        return indent.toString() + id + ":" + output;
    }
    
    juce::String yamlGenerateComment(const juce::String &comment, const juce::String &indent)
    {
        const juce::String trimmed = comment.trim();
        
        if (!trimmed.isEmpty())
        {
            const juce::StringArray lines = juce::StringArray::fromLines(trimmed);
            return indent + "# " + lines.joinIntoString('\n' + indent + "# ");
        }
        
        return "";
    }
    
    // NOLINTNEXTLINE
    juce::String yamlGenerateChildren(const jaut::Config::Property    &propertyObj,
                                      jaut::detail::Indent            indent,
                                      const jaut::YamlParser::Options &options)
    {
        juce::String output;
        
        for (auto it = propertyObj.begin(); it != propertyObj.end(); ++it)
        {
            const auto &[child_id, child_obj] = *it;
            const juce::String comment = ::yamlGenerateComment(child_obj.getComment(), indent.toString());
            
            if (it != propertyObj.begin())
            {
                const auto l_it = std::prev(it);
                
                if (!comment.isEmpty() || !l_it->second.getComment().trim().isEmpty()
                    || (l_it->second.getValue().getArray() && !l_it->second.getValue().getArray()->isEmpty()))
                {
                    output << '\n';
                }
            }
            
            if (!comment.isEmpty())
            {
                output << comment << '\n';
            }
            
            ContentGenerator generator;
            
            if (child_obj.isMapProperty())
            {
                generator = [&prop = child_obj, &options](juce::String &lOutput, jaut::detail::Indent lIndent)
                {
                    lOutput << "\n"
                            << ::yamlGenerateChildren(prop, lIndent, options);
                };
            }
            else
            {
                generator = [&prop = child_obj, &options](juce::String &lOutput, jaut::detail::Indent lIndent)
                {
                    const juce::var    &value = prop.getValue();
                    const juce::String text   = ::yamlVarToValue(prop.getValue(), lIndent, options);
                    
                    if (((value.getArray() || value.getDynamicObject()) && text.containsChar('\n')))
                    {
                        lOutput << '\n' << lIndent.toString() << text;
                    }
                    else
                    {
                        lOutput << ' ' << text;
                    }
                };
            }
            
            output << ::yamlGenerateEntity(child_id, generator, indent);
            
            if (std::next(it) != propertyObj.end())
            {
                output << "\n";
            }
        }
        
        return output;
    }
    
    juce::String yamlGenerateCategory(const jaut::Config::Property    &categoryObj,
                                      const jaut::YamlParser::Options &options)
    {
        const jaut::detail::Indent indent{ 0, options.indentationWidth };
        const juce::String         indent_p0 = indent.toString();
        juce::String               output;
        
        const juce::String comment = ::yamlGenerateComment(categoryObj.getComment(), indent_p0);
        
        if (!comment.isEmpty())
        {
            output << comment << '\n';
        }
        
        auto child_generator = [&categoryObj, &options](juce::String &lOutput, jaut::detail::Indent lIndent)
        {
            if (categoryObj.hasChildren())
            {
                lOutput << "\n"
                        << ::yamlGenerateChildren(categoryObj, lIndent, options);
            }
            else
            {
                lOutput << "{}";
            }
        };
        
        output << ::yamlGenerateEntity(categoryObj.getId(), child_generator, indent);
        return output;
    }
    
    //==================================================================================================================
    // PARSING ============================
    // NOLINTNEXTLINE
    void yamlReadProperty(const YAML::Node &propertyNode, jaut::Config::Property &propertyObj) noexcept
    {
        if (propertyObj.isValueProperty())
        {
            if (propertyNode && propertyNode.IsDefined())
            {
                propertyObj.setValue(propertyNode.as<juce::var>(), juce::dontSendNotification);
            }
        }
        else if (propertyNode && propertyNode.IsMap())
        {
            for (const auto &sub_property_node : propertyNode)
            {
                const juce::String key       = sub_property_node.first.as<std::string>();
                const YAML::Node   &sub_node = sub_property_node.second;
                
                if (jaut::Config::Property *const sub_property_obj = propertyObj.getProperty(key))
                {
                    ::yamlReadProperty(sub_node, *sub_property_obj);
                }
            }
        }
    }
    
    YAML::Node yamlParseDocument(const juce::File &configFile)
    {    
        try
        {
            return YAML::LoadFile(configFile.getFullPathName().toStdString());
        }
        catch (const YAML::ParserException &ex)
        {
            throw jaut::ConfigParseException("config couldn't be parsed, " + juce::String(ex.what()));
        }
    }
}
#endif
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region YamlParser
//======================================================================================================================
#if JAUT_OPTLIB_YAML_CPP
namespace jaut
{
    //==================================================================================================================
    YamlParser::YamlParser(Options parOptions) noexcept
        : options(parOptions)
    {}
    
    //==================================================================================================================
    void YamlParser::parseConfig(ParseAttributes<Config::CategoryMap> parAttributes)
    {
        const YAML::Node document = ::yamlParseDocument(parAttributes.configFile);
        
        if (!document.IsMap())
        {
            throw ConfigParseException("config couldn't be parsed, invalid yaml document");
        }
        
        Config::CategoryMap &categories = parAttributes.categories;
        
        for (const auto &category_map_node : document)
        {
            const auto       category_key   = category_map_node.first.as<juce::String>();
            const YAML::Node &category_node = category_map_node.second;
            
            const auto category_it = categories.find(category_key);
            
            if (!category_node.IsMap() || category_it == categories.end())
            {
                continue;
            }
            
            for (const auto &property_map_node : category_node)
            {
                const juce::String property_key   = property_map_node.first.as<std::string>();
                const YAML::Node   &property_node = property_map_node.second;
                
                Config::Property *const property_obj = category_it->second.getProperty(property_key);
                
                if (!property_obj)
                {
                    continue;
                }
                
                ::yamlReadProperty(property_node, *property_obj);
            }
        }
    }
    
    void YamlParser::writeConfig(ParseAttributes<const Config::CategoryMap> parAttributes) const
    {
        const detail::Indent indent{ 0, options.indentationWidth };
        const juce::String   indent_p0 = indent.toString();
        const juce::String   notice    = ::yamlGenerateComment(parAttributes.options.configNotice, indent_p0);
        juce::String         output;
        
        if (!notice.isEmpty())
        {
            output << notice << "\n\n";
        }
                        
        const juce::String &cat_default = parAttributes.options.defaultCategory;
        
        std::vector<std::reference_wrapper<const Config::CategoryMap::value_type>> categories;
        detail::transformCategories(parAttributes.categories, categories, cat_default);
                
        const jaut::Config::Property &prop_cat_default = parAttributes.categories.at(cat_default);
        output << ::yamlGenerateCategory(prop_cat_default, options) << '\n';
        
        for (auto it = categories.begin(); it != categories.end(); ++it)
        {
            if (it != categories.begin())
            {
                const bool has_comment = !it->get().second.getComment().trim().isEmpty();
                auto       l_it        = std::prev(it);
                
                if (has_comment || !l_it->get().second.getComment().trim().isEmpty())
                {
                    output << '\n';
                }
            }
            else if (!prop_cat_default.getComment().trim().isEmpty())
            {
                output << '\n';
            }
            
            output << ::yamlGenerateCategory(it->get().second, options) << '\n';
        }
        
        if (!parAttributes.configFile.replaceWithText(output))
        {
            detail::throwWriteError(parAttributes.configFile);
        }
    }
}
#endif
//======================================================================================================================
// endregion YamlParser
//**********************************************************************************************************************
