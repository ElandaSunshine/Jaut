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
    @file   jaut_JsonParser.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut_provider/config/parser/jaut_JsonParser.h>
#include <jaut_provider/config/parser/detail/jaut_ParserUtil.h>

#include <regex>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    using ContentGenerator = std::function<void(juce::String &output, jaut::detail::Indent indent)>;
    
    //==================================================================================================================
    // GENERATION ============================
    juce::String jsonGenerateEntity(const juce::String &id, ContentGenerator generator, jaut::detail::Indent indent)
    {        
        juce::String output;
        generator(output, indent);
        return indent.toString() + '"' + id + "\": " + output;
    }
    
    juce::String jsonGenerateComment(const juce::String &comment, const juce::String &indent)
    {
        const juce::String trimmed = comment.trim();
        juce::String       output;
        
        if (!trimmed.isEmpty())
        {            
            if (trimmed.containsChar('\n'))
            {
                const juce::StringArray lines = juce::StringArray::fromLines(trimmed.replace("*/", "* /"));
                const juce::String      space = indent + "   ";
                output << indent << "/* " << lines.getReference(0) << '\n';
                
                for (auto it = (lines.begin() + 1); it != lines.end(); ++it)
                {
                    output << space << *it;
                    
                    if (std::next(it) != lines.end())
                    {
                        output << '\n';
                    }
                }
                
                output << indent << " */";
            }
            else
            {
                output << indent << "// " << trimmed;
            }
        }
        
        return output;
    }
    
    // NOLINTNEXTLINE
    juce::String jsonGenerateChildren(const jaut::Config::Property &propertyObj, jaut::detail::Indent indent)
    {
        juce::String output;
        
        for (auto it = propertyObj.begin(); it != propertyObj.end(); ++it)
        {
            const auto         &[child_id, child_obj] = *it;
            const juce::String comment                = ::jsonGenerateComment(child_obj.getComment(), indent.toString());
            
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
            
            ContentGenerator generator;
            
            if (child_obj.isMapProperty())
            {
                generator = [&prop = child_obj](juce::String &lOutput, jaut::detail::Indent lIndent)
                {
                    lOutput << "{\n"
                            << ::jsonGenerateChildren(prop, lIndent.next()) << '\n'
                            << lIndent.toString() << '}';
                };
            }
            else
            {                
                generator = [&prop = child_obj](juce::String &lOutput, jaut::detail::Indent lIndent)
                {
                    lOutput << jaut::detail::serialiseJson(prop.getValue(), lIndent);
                };
            }
            
            output << ::jsonGenerateEntity(child_id, generator, indent);
            
            if (std::next(it) != propertyObj.end())
            {
                output << ",\n";
            }
        }
        
        return output;
    }
    
    juce::String jsonGenerateCategory(const jaut::Config::Property    &categoryObj,
                                      const jaut::JsonParser::Options &options)
    {
        const jaut::detail::Indent indent{ 1, options.indentationWidth };
        const juce::String         indent_p0 = indent.toString();
        juce::String               output;
        
        const juce::String comment = ::jsonGenerateComment(categoryObj.getComment(), indent_p0);
        
        if (!comment.isEmpty())
        {
            output << comment << '\n';
        }
        
        auto child_generator = [&categoryObj](juce::String &lOutput, jaut::detail::Indent lIndent)
        {
            if (categoryObj.hasChildren())
            {
                lOutput << "{\n"
                        << ::jsonGenerateChildren(categoryObj, lIndent.next()) << '\n'
                        << lIndent.toString() << '}';
            }
            else
            {
                lOutput << "{}";
            }
        };
        
        output << ::jsonGenerateEntity(categoryObj.getId(), child_generator, indent);
        return output;
    }
    
    //==================================================================================================================
    // PARSING ============================
    // NOLINTNEXTLINE
    void jsonReadProperty(const juce::var &json, jaut::Config::Property &propertyObj)
    {
        if (propertyObj.isValueProperty())
        {
            if (!json.isUndefined())
            {
                propertyObj.setValue(json, juce::dontSendNotification);
            }
        }
        else if (juce::DynamicObject *const json_obj = json.getDynamicObject())
        {
            for (const auto &[json_sub_id, json_sub_obj] : json_obj->getProperties())
            {
                if (jaut::Config::Property *const prop = propertyObj.getProperty(json_sub_id.toString()))
                {
                    ::jsonReadProperty(json_sub_obj, *prop);
                }
            }
        }
    }
    
    juce::String jsonStripComments(const juce::String &jsonIn)
    {
        const std::regex pattern(R"((?!.*"(\\[\s\S]|[^"])*")(\/\/.*|\/\*[\s\S]*?\*\/))");
        
        std::string input  = jsonIn.toStdString();
        std::smatch match;
                
        while (std::regex_search(input, match, pattern))
        {
            input = match.prefix().str() + match.suffix().str();
        }
                
        return input;
    }
    
    juce::var jsonParseDocument(const juce::File &configFile)
    {
        juce::var root_var;
        
        {
            const juce::String json_text = ::jsonStripComments(configFile.loadFileAsString());
            const juce::Result result    = juce::JSON::parse(json_text, root_var);
                        
            if (result.failed())
            {
                throw jaut::ConfigParseException("config couldn't be parsed, " + result.getErrorMessage());
            }
        }
        
        return root_var;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region JsonParser
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    JsonParser::JsonParser(Options parOptions) noexcept
        : options(parOptions)
    {}
    
    //==================================================================================================================
    void JsonParser::parseConfig(ParseAttributes<Config::CategoryMap> parAttributes)
    {
        const juce::var document = ::jsonParseDocument(parAttributes.configFile);
        juce::DynamicObject *const root_json = document.getDynamicObject();

        if (!root_json)
        {
            throw ConfigParseException("config couldn't be parsed, invalid json document");
        }
        
        Config::CategoryMap &categories = parAttributes.categories;
        
        for (const auto &[category_id, category_var] : root_json->getProperties())
        {
            juce::DynamicObject *const category_json = category_var.getDynamicObject();
            auto category_obj_it = categories.find(category_id.toString());
            
            if (!category_json || category_obj_it == categories.end())
            {
                continue;
            }
            
            // NOLINTNEXTLINE
            for (const auto &[property_id, property_json] : category_json->getProperties())
            {
                Config::Property *const property_obj = category_obj_it->second.getProperty(property_id.toString());
                
                if (!property_obj)
                {
                    continue;
                }
                
                ::jsonReadProperty(property_json, *property_obj);
            }
        }
    }
    
    void JsonParser::writeConfig(ParseAttributes<const Config::CategoryMap> parAttributes) const
    {
        const detail::Indent indent{ 0, options.indentationWidth };
        const juce::String   indent_p0 = indent.toString();
        const juce::String   notice    = ::jsonGenerateComment(parAttributes.options.configNotice, indent_p0);
        juce::String         output;
        
        if (!notice.isEmpty())
        {
            output << notice << '\n';
        }
        
        output << "{\n";
                
        const juce::String &cat_default = parAttributes.options.defaultCategory;
        
        std::vector<std::reference_wrapper<const Config::CategoryMap::value_type>> categories;
        detail::transformCategories(parAttributes.categories, categories, cat_default);
                
        const jaut::Config::Property &prop_cat_default = parAttributes.categories.at(cat_default);
        output << ::jsonGenerateCategory(prop_cat_default, options) << (!categories.empty() ? "," : "") << '\n';
        
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
            
            output << ::jsonGenerateCategory(it->get().second, options);
            
            if (std::next(it) != categories.end())
            {
                output << ",";
            }
            
            output << '\n';
        }
        
        output << '}';
        
        if (!parAttributes.configFile.replaceWithText(output))
        {
            detail::throwWriteError(parAttributes.configFile);
        }
    }
}
//======================================================================================================================
// endregion JsonParser
//**********************************************************************************************************************
