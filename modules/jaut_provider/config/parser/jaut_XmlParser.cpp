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
    @file   jaut_XmlParser.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut_provider/config/parser/jaut_XmlParser.h>
#include <jaut_provider/config/parser/detail/jaut_ParserUtil.h>


//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    using ContentGenerator = std::function<void(juce::String &output, jaut::detail::Indent indent)>;
    
    //==================================================================================================================
    // CONVERSION ============================
    bool xmlTestAndSkipSequence(juce::String::CharPointerType &it)
    {
        auto c_it = it;
        
        for (;;)
        {
            const juce::juce_wchar c = *c_it;
            
            if (c == 0 || c == ' ')
            {
                return false;
            }
            else if (c == ';')
            {
                it = (++c_it);
                return true;
            }
            
            ++c_it;
        }
    }
    
    juce::String encodeString(const juce::String &string)
    {
        juce::String::CharPointerType it = string.getCharPointer();
        juce::String output;
        output.preallocateBytes(string.getNumBytesAsUTF8() * JUCE_STRING_UTF_TYPE);
        
        for (;;)
        {
            const juce::juce_wchar c = *it;
            
            if (c == 0)
            {
                break;
            }
            
            switch (c)
            {
                case '\"': output << "&quot;"; break;
                case '\'': output << "&apos;"; break;
                case '<':  output << "&lt;";   break;
                case '>':  output << "&gt;";   break;
                
                case '&':
                {
                    if (!xmlTestAndSkipSequence(it))
                    {
                        output << "&amp;";
                        break;
                    }
                }
                
                default: output << c;
            }
            
            ++it;
        }
        
        return output.replace("\n", "\\n");
    }
    
    juce::String decodeString(const juce::String &string)
    {
        return string.replace("\\n", "\n");
    }
    
    const juce::String& xmlVarToMod(const juce::var &value, const jaut::XmlParser::Options::TypeTag &tags)
    {
        switch (jaut::VarUtil::getVarType(value))
        {
            // Named types
            case jaut::VarUtil::VarTypeId::Array:         return tags.tagTypeArray;
            case jaut::VarUtil::VarTypeId::Bool:          return tags.tagTypeBool;
            case jaut::VarUtil::VarTypeId::Double:        return tags.tagTypeDouble;
            case jaut::VarUtil::VarTypeId::DynamicObject: return tags.tagTypeJson;
            case jaut::VarUtil::VarTypeId::Int:           return tags.tagTypeInt;
            case jaut::VarUtil::VarTypeId::Int64:         return tags.tagTypeInt64;
            
            // String types (no modifier required)
            case jaut::VarUtil::VarTypeId::String:
            case jaut::VarUtil::VarTypeId::Void:
                return tags.tagTypeString;
            
            // Invalid
            case jaut::VarUtil::VarTypeId::Method:
            case jaut::VarUtil::VarTypeId::OtherObject:
            case jaut::VarUtil::VarTypeId::Undefined:
            case jaut::VarUtil::VarTypeId::Unknown:
            case jaut::VarUtil::VarTypeId::BinaryData:
                break;
        }
        
        static const juce::String invalid;
        return invalid;
    }
        
    juce::String xmlGenerateTag(const juce::String&, ContentGenerator, jaut::detail::Indent,
                             std::vector<std::pair<juce::String, juce::String>> = {});
    juce::String xmlVarToValue(const juce::var&, jaut::detail::Indent, const jaut::XmlParser::Options::TypeTag&);
    
    juce::String xmlVarToArray(const juce::var                         &value,
                               jaut::detail::Indent                    indent,
                               const jaut::XmlParser::Options::TypeTag &tags)
    {
        if (value.getArray()->isEmpty())
        {
            return "";
        }
        
        const juce::String           indent_p0 = indent.toString();
        const juce::Array<juce::var> &array    = *value.getArray();
        
        juce::String output;
        
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            const juce::var &e_value = *it;
                    
            auto element_generator = [&e_value, &tags](juce::String &lOutput, jaut::detail::Indent lIndent) -> bool
            {
                const juce::String content = ::xmlVarToValue(e_value, lIndent, tags);
                lOutput << content;
                
                return content.containsChar('\n');
            };
            
            juce::String tag_name;
            
            if (e_value.isVoid())
            {
                tag_name = tags.tagTypeArrayVoid;
            }
            else
            {
                tag_name = ::xmlVarToMod(e_value, tags);
            }
            
            output << ::xmlGenerateTag(tag_name, element_generator, indent);
            
            if (std::next(it) != array.end())
            {
                output << '\n';
            }
        }
        
        return output;
    }
    
    juce::String xmlVarToValue(const juce::var                         &value,
                               jaut::detail::Indent                    indent,
                               const jaut::XmlParser::Options::TypeTag &tags)
    {
        switch (jaut::VarUtil::getVarType(value))
        {
            case jaut::VarUtil::VarTypeId::Double:
            case jaut::VarUtil::VarTypeId::Int:
            case jaut::VarUtil::VarTypeId::Int64:
                return value.toString();
            
            case jaut::VarUtil::VarTypeId::String:
                return ::encodeString(value.toString());
               
            case jaut::VarUtil::VarTypeId::Bool:
                return (static_cast<bool>(value) ? "true" : "false");
            
            case jaut::VarUtil::VarTypeId::Array:
                return ::xmlVarToArray(value, indent, tags);
            
            case jaut::VarUtil::VarTypeId::DynamicObject:
                 return indent.toString() + "<![CDATA[" + jaut::detail::serialiseJson(value, indent) + "]]>";
            
            case jaut::VarUtil::VarTypeId::Void:
                return "";
            
            case jaut::VarUtil::VarTypeId::OtherObject:
            case jaut::VarUtil::VarTypeId::Method:
            case jaut::VarUtil::VarTypeId::Unknown:
            case jaut::VarUtil::VarTypeId::Undefined:
            case jaut::VarUtil::VarTypeId::BinaryData:
                break;
        }
        
        throw jaut::ConfigIncompatibleTypeException::invalidVarType(value);
    }
    
    juce::var xmlNodeToVar(const juce::XmlElement &element, const jaut::XmlParser::Options &options)
    {
        const juce::String &tag_name = element.getTagName();
        
        if (element.getBoolAttribute("xsi:nil") || tag_name == options.typeTags.tagTypeArrayVoid)
        {
            return {};
        }
        
        if (tag_name == options.typeTags.tagTypeString) return ::decodeString(element.getAllSubText());
        
        const juce::String content = element.getAllSubText().trim();
        
        if (tag_name == options.typeTags.tagTypeInt)    return content.getIntValue();        
        if (tag_name == options.typeTags.tagTypeInt64)  return content.getLargeIntValue();
        if (tag_name == options.typeTags.tagTypeDouble) return content.getDoubleValue();
        if (tag_name == options.typeTags.tagTypeBool)   return static_cast<bool>(juce::var(content));
        if (tag_name == options.typeTags.tagTypeJson)   return juce::JSON::parse(content);
        
        if (tag_name == options.typeTags.tagTypeArray)
        {
            juce::Array<juce::var> array;
            
            for (const auto &array_element : element.getChildIterator())
            {
                juce::var value = ::xmlNodeToVar(*array_element, options);
                
                if (!value.isUndefined())
                {
                    array.add(std::move(value));
                }
            }
            
            return array;
        }
        
        if (options.throwOnInvalidTagName)
        {
            throw jaut::ConfigParseException("Invalid type-tag '" + tag_name + "'");
        }
        
        return juce::var::undefined();
    }
    
    //==================================================================================================================
    // GENERATION ============================
    juce::String xmlGenerateTag(const juce::String                                 &name,
                                ContentGenerator                                   generator,
                                jaut::detail::Indent                               indent,
                                std::vector<std::pair<juce::String, juce::String>> attributes)
    {
        const juce::String indent_p0 = indent.toString();
        juce::String       output    = indent_p0 + '<' + name;
        
        if (!attributes.empty())
        {
            output << ' ';
            
            for (auto it = attributes.begin(); it != attributes.end(); ++it)
            {
                const auto &[att_name, att_value] = *it;
                output << att_name << "=\"" << att_value << '\"';
                
                if (std::next(it) != attributes.end())
                {
                    output << ' ';
                }
            }
        }
        
        if (generator)
        {
            output << '>';
            
            juce::String text;
            generator(text, indent.next());
            
            if (text.containsChar('\n'))
            {
                output << '\n' << text << '\n' << indent_p0;
            }
            else
            {
                output << text;
            }
            
            output << "</" << name << '>';
        }
        else
        {
            output << "/>";
        }
        
        return output;
    }
    
    juce::String xmlGenerateComment(const juce::String &comment, const juce::String &indent)
    {
        const juce::String trimmed = comment.trim();
        juce::String       output;
        
        if (!trimmed.isEmpty())
        {
            const juce::String comment_escaped = trimmed.replace("-->", "- ->");
            
            if (comment_escaped.containsChar('\n'))
            {
                const juce::StringArray lines = juce::StringArray::fromLines(comment_escaped);
                const juce::String      space = indent + "     ";
                output << indent << "<!-- " << lines.getReference(0) << '\n';
                
                for (auto it = (lines.begin() + 1); it != lines.end(); ++it)
                {
                    output << space << *it;
                    
                    if (std::next(it) != lines.end())
                    {
                        output << '\n';
                    }
                }
                
                output << " -->";
            }
            else
            {
                output << indent + "<!-- " + comment_escaped + " -->";
            }
        }
        
        return output;
    }
    
    // NOLINTNEXTLINE
    juce::String xmlGenerateChildren(const jaut::Config::Property            &propertyObj,
                                     jaut::detail::Indent                    indent,
                                     const jaut::XmlParser::Options::TypeTag &tags)
    {
        juce::String output;
        
        for (auto it = propertyObj.begin(); it != propertyObj.end(); ++it)
        {
            const auto &[child_id, child_obj] = *it;
            
            const auto         att_name = std::make_pair("name", child_id);
            const juce::String comment  = ::xmlGenerateComment(child_obj.getComment(), indent.toString());
            
            if (it != propertyObj.begin())
            {
                auto l_it = std::prev(it);
                
                if (!comment.isEmpty() || !l_it->second.getComment().trim().isEmpty())
                {
                    output << '\n';
                }
            }
            
            if (!comment.isEmpty())
            {
                output << comment << '\n';
            }
            
            if (child_obj.isMapProperty())
            {
                auto property_generator = [&prop = child_obj, &tags](juce::String         &lOutput,
                                                                     jaut::detail::Indent lIndent)
                {
                    lOutput << ::xmlGenerateChildren(prop, lIndent, tags);
                };
                
                output << ::xmlGenerateTag(tags.tagTypeTree, property_generator, indent, { att_name });
            }
            else
            {
                const juce::var &value = child_obj.getValue();
                
                if (value.isVoid())
                {
                    juce::String     tag_name;
                    ContentGenerator generator;
                    
                    if (child_obj.getDefaultValue().isVoid())
                    {
                        tag_name = tags.tagTypeString;
                    }
                    else
                    {
                        tag_name = ::xmlVarToMod(child_obj.getDefaultValue(), tags);
                    }
                    
                    const auto att_null = std::make_pair("xsi:nil", "true");
                    output << ::xmlGenerateTag(tag_name, nullptr, indent, { att_name, att_null });
                }
                else
                {
                    auto value_generator = [&value, &tags](juce::String &lOutput, jaut::detail::Indent lIndent)
                    {
                        const juce::String content = ::xmlVarToValue(value, lIndent, tags);
                        lOutput << content;
                    };
                    
                    const juce::String tag_name = ::xmlVarToMod(value, tags);
                    std::vector<std::pair<juce::String, juce::String>> atts {
                        att_name
                    };
                    
                    if (tag_name == tags.tagTypeString)
                    {
                        atts.emplace_back("xml:space", "preserve");
                    }
                    
                    output << ::xmlGenerateTag(tag_name, value_generator, indent, std::move(atts));
                }
            }
            
            if (std::next(it) != propertyObj.end())
            {
                output << '\n';
            }
        }
        
        return output;
    }
    
    juce::String xmlGenerateCategory(const jaut::Config::Property   &categoryObj,
                                     const jaut::XmlParser::Options &options)
    {
        const jaut::detail::Indent indent{ 1, options.indentationWidth };
        const juce::String         indent_p0 = indent.toString();
        juce::String               output;
        
        const juce::String comment = ::xmlGenerateComment(categoryObj.getComment(), indent_p0);
        
        if (!comment.isEmpty())
        {
            output << comment << '\n';
        }
        
        auto child_generator = [&categoryObj, &options](juce::String &lOutput, jaut::detail::Indent lIndent)
        {
            if (categoryObj.hasChildren())
            {
                lOutput << ::xmlGenerateChildren(categoryObj, lIndent, options.typeTags);
            }
        };
        
        const auto att_name = std::make_pair("name", categoryObj.getId());
        output << ::xmlGenerateTag(options.typeTags.tagNameCategory, child_generator, indent, { att_name });
        return output;
    }
    
    //==================================================================================================================
    // PARSING ==========================    
    // NOLINTNEXTLINE
    void xmlReadProperty(const juce::XmlElement         &propertyXml,
                         jaut::Config::Property         &propertyObj,
                         const jaut::XmlParser::Options &options)
    {
        for (const auto &xml_entity : propertyXml.getChildIterator())
        {
            if (!xml_entity->hasAttribute("name"))
            {
                continue;
            }
            
            const juce::String &id = xml_entity->getStringAttribute("name");
            
            if (jaut::Config::Property *const prop = propertyObj.getProperty(id))
            {
                const juce::String &tag_name = xml_entity->getTagName();
                
                if (tag_name == options.typeTags.tagTypeTree)
                {
                    if (prop->isValueProperty())
                    {
                        continue;
                    }
                    
                    ::xmlReadProperty(*xml_entity, *prop, options);
                }
                else
                {
                    if (prop->isMapProperty())
                    {
                        continue;
                    }
                    
                    juce::var value = ::xmlNodeToVar(*xml_entity, options);
                
                    if (!value.isUndefined())
                    {
                        prop->setValue(std::move(value), juce::dontSendNotification);
                    }
                }
            }
        }
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region XmlParser
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    XmlParser::XmlParser(Options parOptions) noexcept
        : options(std::move(parOptions))
    {}
    
    //==================================================================================================================
    void XmlParser::parseConfig(ParseAttributes<Config::CategoryMap> parAttributes)
    {
        juce::XmlDocument                       document(parAttributes.configFile);
        const std::unique_ptr<juce::XmlElement> config = document.getDocumentElement();
        
        if (!config)
        {
            throw ConfigParseException(document.getLastParseError());
        }
        
        if (!config->hasTagName(options.typeTags.tagNameRoot))
        {
            throw ConfigParseException("invalid root tag-name '" + config->getTagName() + "' where there "
                                       "was '" + options.typeTags.tagNameRoot + "' expected");
        }
        
        jaut::Config::CategoryMap &categories = parAttributes.categories;
        
        for (const auto &xml_entity : config->getChildIterator())
        {
            if (!xml_entity->hasTagName(options.typeTags.tagNameCategory))
            {
                if (options.throwOnInvalidTagName)
                {
                    throw jaut::ConfigParseException("invalid tag-name '" + xml_entity->getTagName() + "' where there "
                                                     "was '" + options.typeTags.tagNameCategory + "' expected");
                }
                
                continue;
            }
            
            if (!xml_entity->hasAttribute("name"))
            {
                continue;
            }
            
            const juce::String &cat_id = xml_entity->getStringAttribute("name");
            
            if (const auto cat_prop_it = categories.find(cat_id); cat_prop_it != categories.end())
            {
                ::xmlReadProperty(*xml_entity, cat_prop_it->second, options);
            }
        }
    }
    
    void XmlParser::writeConfig(ParseAttributes<const Config::CategoryMap> parAttributes) const
    {                
        const detail::Indent indent{ 0, options.indentationWidth };
        const juce::String   indent_p0 = indent.toString();
        const juce::String   notice = ::xmlGenerateComment(parAttributes.options.configNotice, indent_p0);
        juce::String         output;
        
        if (!notice.isEmpty())
        {
            output << notice << '\n';
        }
        
        auto category_generator = [this, &parAttributes](juce::String &lOutput, detail::Indent)
        {
            const juce::String &cat_default = parAttributes.options.defaultCategory;
            
            std::vector<std::reference_wrapper<const Config::CategoryMap::value_type>> categories;
            detail::transformCategories(parAttributes.categories, categories, cat_default);
            
            const jaut::Config::Property &prop_cat_default = parAttributes.categories.at(cat_default);
            lOutput << ::xmlGenerateCategory(prop_cat_default, options) << '\n';
            
            for (auto it = categories.begin(); it != categories.end(); ++it)
            {
                if (it != categories.begin())
                {
                    const bool has_comment = !it->get().second.getComment().trim().isEmpty();
                    auto       l_it        = std::prev(it);
                    
                    if (has_comment || !l_it->get().second.getComment().trim().isEmpty())
                    {
                        lOutput << '\n';
                    }
                }
                else if (!prop_cat_default.getComment().trim().isEmpty())
                {
                    lOutput << '\n';
                }
                
                lOutput << ::xmlGenerateCategory(it->get().second, options);
                                
                if (std::next(it) != categories.end())
                {
                    lOutput << '\n';
                }
            }
        };
        
        const auto att_xsi = std::make_pair("xmlns:xsi", "https://www.w3.org/2001/XMLSchema-instance");
        output << ::xmlGenerateTag(options.typeTags.tagNameRoot, category_generator, indent, { att_xsi }) << '\n';
        
        if (!parAttributes.configFile.replaceWithText(output))
        {
            detail::throwWriteError(parAttributes.configFile);
        }
    }
}
//======================================================================================================================
// endregion XmlParser
//**********************************************************************************************************************
