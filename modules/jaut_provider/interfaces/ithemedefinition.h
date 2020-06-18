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
    @file   ithemedefinition.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{

/**
 *  The IThemeDefinition interfaces provides a collection of useful methods to implement your own
 *  theme package system.
 *
 *  Optionally, you can use the ResourceManager together with this class if you want.
 */

class JAUT_API IThemeDefinition
{
public:
    virtual ~IThemeDefinition() = default;

    //=================================================================================================================
    /**
     *  Gets a file relative to the root folder of this theme package.
     *
     *  @param filePath The relative path to the file
     *  @return The file
     */
    virtual juce::File getFile(const juce::String &filePath) const = 0;

    /**
     *  Gets an image relative to the root folder of this theme package.
     *
     *  @param imageName The name of the image to search for  (without extension)
     *  @return The image object
     */
    virtual juce::Image getImage(const juce::String &imageName) const = 0;

    /**
     *  Gets the missing image if a certain image wasn't found.
     *  @return The image object
     */
    virtual juce::Image getMissingImage() const = 0;

    /**
     *  Gets a colour from the colour-map map if implemented.
     *
     *  @param The name of the color in the map
     *  @return The colour or a substitute if none such colour was found
     */
    virtual juce::Colour getThemeColour(const juce::String &colorMappingKey) const = 0;

    /**
     *  Gets a colour directly from the colour-map image if implemented.
     *
     *  @param x The x coordinate of the pixel
     *  @param y The y coordinate of the pixel
     *  @return The colour or a substitute if none such colour was found
     */
    virtual juce::Colour getThemeColourFromPixel(int x, int y) const = 0;

    /**
     *  The font to use for the theme package.
     *  @return The font object
     */
    virtual juce::Font getThemeFont() const = 0;

    /**
     *  The thumbnail image of the theme package if implemented.
     *  @return The image object
     */
    virtual juce::Image getThemeThumbnail() const = 0;

    /**
     *  Gets whether a file at a specific relative path starting from the theme package's root exists or not.
     *
     *  @param filePath The path to the file
     *  @return True if the file exists, false if not
     */
    virtual const bool fileExists(const juce::String &filePath) const = 0;

    /**
     *  Gets whether an image exists or not.
     *
     *  @param imageName The name of the image (without extension)
     *  @return True if the image exists, false if not
     */
    virtual const bool imageExists(const juce::String &imageName) const = 0;

    /**
     *  Gets whether the given image is not null and contains image data.
     *
     *  @param image The image to check
     *  @param True if the image is valid, false if not
     */
    virtual const bool isImageValid(const juce::Image &image) const = 0;

    /**
     *  Gets whether this theme package is valid or not.
     *  @return True if the theme package is valid, false if not
     */
    virtual const bool isValid() const = 0;

    /**
     *  Gets the theme package's metadata containing name, description, author ect..
     *  @return A pointer to the metadata from this theme package
     */
    virtual const IMetadata* getThemeMeta() const = 0;

    /**
     *  Gets the file extension of the images used for this application.
     *  @return The extension
     */
    virtual const juce::String getImageExtension() const = 0;

    /**
     *  Gets the absolute path of the root directory of this theme package.
     *  @return The root path of the theme package
     */
    virtual const juce::String getThemeRootPath() const = 0;

};
}
