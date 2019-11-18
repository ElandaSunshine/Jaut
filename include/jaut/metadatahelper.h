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
    @file   metadatahelper.h
    @date   30, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/imetadata.h>
#include <unordered_map>

namespace jaut
{
namespace
{
inline void addToMapIfExists(const String &name, std::unordered_map<String, var> &map, const DynamicObject *dno) noexcept
{
    map[name] = dno->getProperty(name);
}

inline void addToNvsIfExists(const String &name, NamedValueSet &map, const DynamicObject *dno) noexcept
{
    map.set(name, dno->getProperty(name));
}
}

class JAUT_API MetadataHelper final
{
public:
    using t_meta_map = std::unordered_map<juce::String, juce::var>;

    static t_meta_map readMetaToMap(InputStream &input) noexcept
    {
        t_meta_map metadata;
        metadata["name"]           = "Unnamed Theme";
        metadata["version"]        = "1.0.0";
        metadata["description"]    = "A theme for ESAC Cossin.";
        metadata["author"]         = "n/a";
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

                addToMapIfExists("name", metadata, jsonroot);
                addToMapIfExists("version", metadata, jsonroot);
                addToMapIfExists("description", metadata, jsonroot);
                addToMapIfExists("author", metadata, jsonroot);
                addToMapIfExists("authors", metadata, jsonroot);

                if (jsonroot->hasProperty("license"))
                {
                    Array<var> license {jsonroot->getProperty("license").getProperty("type", ""),
                                        jsonroot->getProperty("license").getProperty("url",  "")};
                    metadata["license"] = var(license);
                }

                addToMapIfExists("website", metadata, jsonroot);
                addToMapIfExists("excludedImages", metadata, jsonroot);
                addToMapIfExists("screenshots", metadata, jsonroot);
            }
        }

        return metadata;
    }

    static NamedValueSet readMetaToNamedValueSet(InputStream &input) noexcept
    {
        NamedValueSet metadata;
        metadata.set("name", "Unnamed Theme");
        metadata.set("version", "1.0.0");
        metadata.set("description", "A theme for ESAC Cossin.");
        metadata.set("author", "n/a");
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

                addToNvsIfExists("name", metadata, jsonroot);
                addToNvsIfExists("version", metadata, jsonroot);
                addToNvsIfExists("description", metadata, jsonroot);
                addToNvsIfExists("author", metadata, jsonroot);
                addToNvsIfExists("authors", metadata, jsonroot);

                if (jsonroot->hasProperty("license"))
                {
                    Array<var> license {jsonroot->getProperty("license").getProperty("type", ""),
                                        jsonroot->getProperty("license").getProperty("url",  "")};
                    metadata.set("license", license);
                }

                addToNvsIfExists("website", metadata, jsonroot);
                addToNvsIfExists("excludedImages", metadata, jsonroot);
                addToNvsIfExists("screenshots", metadata, jsonroot);
            }
        }

        return metadata;
    }
};
}
