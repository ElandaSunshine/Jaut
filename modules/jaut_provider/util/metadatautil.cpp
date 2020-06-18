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
    @file   metadatahelper.cpp
    @date   20, November 2019
    
    ===============================================================
 */

namespace jaut
{
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
static std::unordered_map<juce::String, juce::String> placeholderMap;

inline juce::String findPlaceholderAndReplace(const juce::String &input)
{
    const std::regex pattern(R"(\{\s*([a-zA-Z0-9_-]+)\s*\})");
    const std::string regex_input   = input.toStdString();
    const std::regex_iterator begin = std::sregex_iterator(regex_input.begin(), regex_input.end(), pattern);
    const std::regex_iterator end   = std::sregex_iterator();
    juce::String output = input.toStdString();
    
    for(auto i = begin; i != end; ++i)
    {
        std::smatch match = *i;
        auto iterator     = placeholderMap.find(juce::String(match.str(1)).toLowerCase());
        
        if(iterator != placeholderMap.end())
        {
            output = output.replace(match.str(0), iterator->second, true);
            continue;
        }
    }
    
    return output;
}

inline bool checkIfIsVersionString(const juce::String &version)
{
    return std::regex_match(version.toStdString(), std::regex(R"(^[0-9]+\.[0-9]+$)"));
}
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region MetadataHelper
//======================================================================================================================
MetadataHelper::MetadataMap MetadataHelper::readMetaToMap(juce::InputStream &input) noexcept
{
    MetadataMap metadata
    {
        {"name",           "Unnamed Theme"},
        {"version",        "0.1.0"},
        {"description",    "A custom theme pack."},
        {"author",         "Unknown"},
        {"authors",        juce::Array<juce::var>()},
        {"license",        juce::Array<juce::var> {"", ""}},
        {"website",        ""},
        {"excludedImages", juce::Array<juce::var>()},
        {"screenshots",    juce::Array<juce::var>()}
    };

    if (!input.isExhausted())
    {
        juce::var json;

        if (juce::JSON::parse(input.readEntireStreamAsString(), json).wasOk())
        {
            juce::DynamicObject *const json_root = json.getDynamicObject();

            if (!json_root)
            {
                return metadata;
            }

            if(json_root->hasProperty("name"))
            {
                metadata["name"] = json_root->getProperty("name");
            }

            if(json_root->hasProperty("version"))
            {
                const juce::String version_string = jaut::findPlaceholderAndReplace(json_root->getProperty("version"));
                metadata["version"] = jaut::checkIfIsVersionString(version_string) ? version_string : "1.0";
            }

            if(json_root->hasProperty("description"))
            {
                metadata["description"] = jaut::findPlaceholderAndReplace(json_root->getProperty("description"));
            }

            if(json_root->hasProperty("author"))
            {
                metadata["author"] = jaut::findPlaceholderAndReplace(json_root->getProperty("author"));
            }

            if(json_root->hasProperty("authors"))
            {
                const juce::var &authors = json_root->getProperty("authors");
                metadata["authors"]      = authors.isArray() ? authors : juce::var(juce::Array<juce::var>());
            }

            if (json_root->hasProperty("license"))
            {
                const juce::var &license = json_root->getProperty("license");

                if(license.isObject())
                {
                    juce::Array<juce::var> license_array {
                        jaut::findPlaceholderAndReplace(license.getProperty("type", "")),
                        jaut::findPlaceholderAndReplace(license.getProperty("url",  ""))
                    };
                    
                    metadata["license"] = juce::var(license_array);
                }
                else if(license.isString())
                {
                    metadata["license"] = juce::var(juce::Array<juce::var>
                                                        (jaut::findPlaceholderAndReplace(license),""));
                }
            }

            if(json_root->hasProperty("website"))
            {
                metadata["website"] = jaut::findPlaceholderAndReplace(json_root->getProperty("website"));
            }

            if(json_root->hasProperty("excludedImages"))
            {
                const juce::var &excluded_images = json_root->getProperty("excludedImages");
                metadata["excludedImages"]       = excluded_images.isArray() ? excluded_images :
                                                                               juce::var(juce::Array<juce::var>());
            }

            if(json_root->hasProperty("excludedImages"))
            {
                const juce::var &screenshots = json_root->getProperty("screenshots");
                metadata["screenshots"]      = screenshots.isArray() ? screenshots :
                                                                       juce::var(juce::Array<juce::var>());
            }
        }
    }

    return metadata;
}

juce::NamedValueSet MetadataHelper::readMetaToNamedValueSet(juce::InputStream &input) noexcept
{
    juce::NamedValueSet metadata
    {
        {"name",           "Unnamed Theme"},
        {"version",        "0.1.0"},
        {"description",    "A custom theme pack."},
        {"author",         "Unknown"},
        {"authors",        juce::Array<juce::var>()},
        {"license",        juce::Array<juce::var> {"", ""}},
        {"website",        ""},
        {"excludedImages", juce::Array<juce::var>()},
        {"screenshots",    juce::Array<juce::var>()}
    };

    if (!input.isExhausted())
    {
        juce::var json;

        if (juce::JSON::parse(input.readEntireStreamAsString(), json).wasOk())
        {
            juce::DynamicObject *const json_root = json.getDynamicObject();

            if (!json_root)
            {
                return metadata;
            }

            if(json_root->hasProperty("name"))
            {
                metadata.set("name", json_root->getProperty("name"));
            }

            if(json_root->hasProperty("version"))
            {
                const juce::String version_string = jaut::findPlaceholderAndReplace(json_root->getProperty("version"));
                metadata.set("version", jaut::checkIfIsVersionString(version_string) ? version_string : "1.0");
            }

            if(json_root->hasProperty("description"))
            {
                metadata.set("description", jaut::findPlaceholderAndReplace(json_root->getProperty("description")));
            }

            if(json_root->hasProperty("author"))
            {
                metadata.set("author", jaut::findPlaceholderAndReplace(json_root->getProperty("author")));
            }

            if(json_root->hasProperty("authors"))
            {
                const juce::var &authors = json_root->getProperty("authors");
                metadata.set("authors", authors.isArray() ? authors : juce::var(juce::Array<juce::var>()));
            }

            if (json_root->hasProperty("license"))
            {
                const juce::var &license = json_root->getProperty("license");

                if(license.isObject())
                {
                    juce::Array<juce::var> license_array {
                        jaut::findPlaceholderAndReplace(license.getProperty("type", "")),
                        jaut::findPlaceholderAndReplace(license.getProperty("url",  ""))
                    };
                    
                    metadata.set("license", juce::var(license_array));
                }
                else if(license.isString())
                {
                    metadata.set("license",
                                 juce::var(juce::Array<juce::var>(jaut::findPlaceholderAndReplace(license), "")));
                }
            }

            if(json_root->hasProperty("website"))
            {
                metadata.set("website", jaut::findPlaceholderAndReplace(json_root->getProperty("website")));
            }

            if(json_root->hasProperty("excludedImages"))
            {
                const juce::var &excluded_images = json_root->getProperty("excludedImages");
                metadata.set("excludedImages", excluded_images.isArray() ? excluded_images :
                                                                           juce::var(juce::Array<juce::var>()));
            }

            if(json_root->hasProperty("excludedImages"))
            {
                const juce::var &screenshots = json_root->getProperty("screenshots");
                metadata.set("screenshots", screenshots.isArray() ? screenshots :
                                                                    juce::var(juce::Array<juce::var>()));
            }
        }
    }

    return metadata;
}

void MetadataHelper::setPlaceholder(const juce::String &name, const juce::String &value) noexcept
{
    jaut::placeholderMap[name.toLowerCase().removeCharacters(" ")] = value;
}
//======================================================================================================================
// endregion MetadataHelper
//**********************************************************************************************************************
}
