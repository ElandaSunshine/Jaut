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
    @file   metadatahelper.h
    @date   30, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  A helper class for reading metadata into a fitting container.
 *  This is supposed to be used in combination with a custom IMetadata and IMetaReader implementation.
 */
class JAUT_API MetadataHelper final
{
public:
    /** The map containing the metadata. */
    using MetadataMap = std::unordered_map<juce::String, juce::var>;
    
    //==================================================================================================================
    /**
     *  Reads a metadata input stream and returns an unordered map with the fetched data.
     *
     *  @param input The input stream to read from
     *  @return An unordered_map containing the metadata
     */
    static MetadataMap readMetaToMap(juce::InputStream &input) noexcept;
    
    /**
     *  Reads a metadata input stream and returns a NamedValueSet with the fetched data.
     *
     *  @param input The input stream to read from
     *  @return A NamedValueSet containing the metadata
     */
    static juce::NamedValueSet readMetaToNamedValueSet(juce::InputStream &input) noexcept;
    
    /**
     *  Set placeholders to use inside the metadata.
     *  This can by any placeholders that contain a specific value.
     *  For example:
     *  @code
     *  setPlaceholder("author", "Elanda");
     *  @endcode
     *
     *  Inside the metadata, wherever you use {author} will expand to the associated value.
     *
     *  @param name  The name of the placeholder
     *  @param value The associated value of the placeholder
     */
    static void setPlaceholder(const juce::String &name, const juce::String &value) noexcept;
};
}
