/**
    ===============================================================
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
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   imetadata.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  The IMetadata interfaces contains useful methods providing a full description of a theme package.
 *  This interfaces was intended to be used in combination with the IThemeDefinition class but can
 *  be used independently.
 */
class JAUT_API IMetadata
{
public:
    using License = std::pair<juce::String, juce::String>;

    //==================================================================================================================
    virtual ~IMetadata() = default;
    
    //==================================================================================================================
    /**
     *  The author of the theme package.
     *  @return The author name
     */
    virtual juce::String getAuthor() const = 0;

    /**
     *  A list of additional authors also working on this theme package.
     *  @return The other authors' names
     */
    virtual juce::StringArray getAuthors() const = 0;

    /**
     *  A description of the theme package.
     *  @return The description text
     */
    virtual juce::String getDescription() const = 0;

    /**
     *  The relative paths of images which shouldn't be picked up by the theme pack.
     *  @return The array image paths
     */
    virtual juce::StringArray getExcludedImages() const = 0;

    /**
     *  A pair of strings containing the license name and url.
     *  @return A pair of url and name
     */
    virtual License getLicense() const = 0;

    /**
     *  The name of the them package.
     *  @return The name
     */
    virtual juce::String getName() const = 0;

    /**
     *  An array of relative paths to images used as screenshots if supported.
     *  @param An array of paths to the images
     */
    virtual juce::StringArray getScreenshots() const = 0;

    /**
     *  The version of this theme package.
     *  @return The version
     */
    virtual jaut::Version getVersion() const = 0;

    /**
     *  A link to the author's website if available.
     *  @return An url to the website
     */
    virtual juce::String getWebsite() const = 0;
};
}
