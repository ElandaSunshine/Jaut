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
    @file   metadatahelper.cpp
    @date   20, November 2019
    
    ===============================================================
 */

#include <jaut/metadatahelper.h>
#include <regex>

namespace jaut
{
namespace
{
static std::unordered_map<String, String> placeholderMap;

inline String findPlaceholderAndReplace(const String &input)
{
    String output           = input.toStdString();
    std::string regex_input = input.toStdString();
    std::smatch match;
    std::regex pattern(R"(\{\s*([a-zA-Z0-9]+)\s*\})");
    int max_attempts = 10;

    while(std::regex_search(regex_input, match, pattern) && max_attempts > 0)
    {
        auto iterator = placeholderMap.find(String(match.str(1)).toLowerCase());

        if(iterator != placeholderMap.end())
        {
            regex_input = (output = output.replace(match.str(0), iterator->second, true)).toStdString();
            continue;
        }

        --max_attempts;
    }

    return output;
}
}



/* ==================================================================================
 * ================================= MetadataHelper =================================
 * ================================================================================== */
#if(1) // MetadataHelper
MetadataHelper::t_meta_map MetadataHelper::readMetaToMap(InputStream &input) noexcept
{
    t_meta_map metadata;
    metadata["name"]           = "Unnamed Theme";
    metadata["version"]        = "1.0.0";
    metadata["description"]    = "A custom theme pack.";
    metadata["author"]         = "Unknown";
    metadata["authors"]        = Array<var>();
    metadata["license"]        = Array<var>("", "");
    metadata["website"]        = "";
    metadata["excludedImages"] = Array<var>();
    metadata["screenshots"]    = Array<var>();

    if (!input.isExhausted())
    {
        var json;

        if (JSON::parse(input.readEntireStreamAsString(), json).wasOk())
        {
            DynamicObject *jsonroot = json.getDynamicObject();

            if (!jsonroot)
            {
                return metadata;
            }

            if(jsonroot->hasProperty("name"))
            {
                metadata["name"] = jaut::findPlaceholderAndReplace(jsonroot->getProperty("name"));
            }

            if(jsonroot->hasProperty("version"))
            {
                metadata["version"] = jaut::findPlaceholderAndReplace(jsonroot->getProperty("version"));
            }

            if(jsonroot->hasProperty("description"))
            {
                metadata["description"] = jaut::findPlaceholderAndReplace(jsonroot->getProperty("description"));
            }

            if(jsonroot->hasProperty("author"))
            {
                metadata["author"] = jaut::findPlaceholderAndReplace(jsonroot->getProperty("author"));
            }

            if(jsonroot->hasProperty("authors"))
            {
                const var &authors  = jsonroot->getProperty("authors");
                metadata["authors"] = authors.isArray() ? authors : var(Array<var>());
            }

            if (jsonroot->hasProperty("license"))
            {
                const var &license  = jsonroot->getProperty("license");

                if(license.isObject())
                {
                    Array<var> license_array {jaut::findPlaceholderAndReplace(license.getProperty("type", "")),
                                              jaut::findPlaceholderAndReplace(license.getProperty("url",  ""))};
                    
                    metadata["license"] = var(license_array);
                }
                else if(license.isString())
                {
                    metadata["license"] = var(Array<var>(jaut::findPlaceholderAndReplace(license), ""));
                }
            }

            if(jsonroot->hasProperty("website"))
            {
                metadata["website"] = jaut::findPlaceholderAndReplace(jsonroot->getProperty("website"));
            }

            if(jsonroot->hasProperty("excludedImages"))
            {
                const var &excluded_images = jsonroot->getProperty("excludedImages");
                metadata["excludedImages"] = excluded_images.isArray() ? excluded_images : var(Array<var>());
            }

            if(jsonroot->hasProperty("excludedImages"))
            {
                const var &screenshots  = jsonroot->getProperty("screenshots");
                metadata["screenshots"] = screenshots.isArray() ? screenshots : var(Array<var>());
            }
        }
    }

    return metadata;
}

NamedValueSet MetadataHelper::readMetaToNamedValueSet(InputStream &input) noexcept
{
    NamedValueSet metadata;
    metadata.set("name", "Unnamed Theme");
    metadata.set("version", "1.0.0");
    metadata.set("description", "A custom theme pack.");
    metadata.set("author", "Unknown");
    metadata.set("authors", Array<var>());
    metadata.set("license", Array<var>("", ""));
    metadata.set("website", "");
    metadata.set("excludedImages", Array<var>());
    metadata.set("screenshots",    Array<var>());

    if (!input.isExhausted())
    {
        var json;

        if (JSON::parse(input.readEntireStreamAsString(), json).wasOk())
        {
            DynamicObject *jsonroot = json.getDynamicObject();

            if (!jsonroot)
            {
                return metadata;
            }

            if(jsonroot->hasProperty("name"))
            {
                metadata.set("name", jaut::findPlaceholderAndReplace(jsonroot->getProperty("name")));
            }

            if(jsonroot->hasProperty("version"))
            {
                metadata.set("version", jaut::findPlaceholderAndReplace(jsonroot->getProperty("version")));
            }

            if(jsonroot->hasProperty("description"))
            {
                metadata.set("description", jaut::findPlaceholderAndReplace(jsonroot->getProperty("description")));
            }

            if(jsonroot->hasProperty("author"))
            {
                metadata.set("author", jaut::findPlaceholderAndReplace(jsonroot->getProperty("author")));
            }

            if(jsonroot->hasProperty("authors"))
            {
                const var &authors  = jsonroot->getProperty("authors");
                metadata.set("authors", authors.isArray() ? authors : var(Array<var>()));
            }

            if (jsonroot->hasProperty("license"))
            {
                const var &license  = jsonroot->getProperty("license");

                if(license.isObject())
                {
                    Array<var> license_array {jaut::findPlaceholderAndReplace(license.getProperty("type", "")),
                                              jaut::findPlaceholderAndReplace(license.getProperty("url",  ""))};
                    
                    metadata.set("license", var(license_array));
                }
                else if(license.isString())
                {
                    metadata.set("license", var(Array<var>(jaut::findPlaceholderAndReplace(license), "")));
                }
            }

            if(jsonroot->hasProperty("website"))
            {
                metadata.set("website", jaut::findPlaceholderAndReplace(jsonroot->getProperty("website")));
            }

            if(jsonroot->hasProperty("excludedImages"))
            {
                const var &excluded_images = jsonroot->getProperty("excludedImages");
                metadata.set("excludedImages", excluded_images.isArray() ? excluded_images : var(Array<var>()));
            }

            if(jsonroot->hasProperty("excludedImages"))
            {
                const var &screenshots  = jsonroot->getProperty("screenshots");
                metadata.set("screenshots", screenshots.isArray() ? screenshots : var(Array<var>()));
            }
        }
    }

    return metadata;
}

void MetadataHelper::setPlaceholder(const String &name, const String &value) noexcept
{
    const String name_adjusted = name.toLowerCase().removeCharacters(" ");
    jaut::placeholderMap[name_adjusted] = value;
}
#endif // MetadataHelper
}
