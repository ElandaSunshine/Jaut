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
#include <unordered_map>

namespace jaut
{

class JAUT_API MetadataHelper final
{
public:
    using t_meta_map = std::unordered_map<juce::String, juce::var>;

    static t_meta_map readMetaToMap(InputStream &input) noexcept;
    static NamedValueSet readMetaToNamedValueSet(InputStream &input) noexcept;
    static void setPlaceholder(const String &name, const String &value) noexcept;
};
}
