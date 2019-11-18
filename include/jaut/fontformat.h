/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: fontformat.h
 * Time: 30, Mai 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{
constexpr int MAX_COLOURS_SIZE = 16;

/**
    The ChatFormat class helps you to format text directly through text instead of creating a new font
    over and over again.
    You can change the colour and font style, like making it bold or italic.

    The default key is '$' followed by a number or character to identify the format applied to the text.
 */

class JAUT_API CharFormat final
{
public:
    class JAUT_API ColourCompound final
    {
    public:
        ColourCompound() noexcept;
        ColourCompound(Colour colour0, Colour colour1, Colour colour2, Colour colour3, Colour colour4,
                       Colour colour5, Colour colour6, Colour colour7, Colour colour8, Colour colour9,
                       Colour colourA, Colour colourB, Colour colourC, Colour colourD, Colour colourE,
                       Colour colourF) noexcept;

        //=========================================================================================================
        const Colour &operator[](juce_wchar characterId) const noexcept;

        //=========================================================================================================
        const Colour &getColourFromIndex(uint32 index) const noexcept;

    private:
        Colour colours[MAX_COLOURS_SIZE];
    };

    struct Options final
    {
        juce_wchar terminator = '$';
        Colour defaultColour = Colours::transparentBlack;
    };

    /**
        Creates a new default instance of the CharFormat class.
     */
    CharFormat() = default;

    /**
        Creates a new instance of the CharFormat class.

        @param colourComp The colour compound containing all colours used for the different colour codes
        @param terminator The character determining when a new formatting sequence begins
     */
    explicit CharFormat(const ColourCompound &colourComp, const Options &options) noexcept;

    /**
        Creates a new instance of the CharFormat class with all colours white.

        @param terminator The character determining when a new formatting sequence begins
     */
    explicit CharFormat(const Options &options) noexcept;

    //==================================================================================================================
    /**
        Draws the formatted text to screen.

        @param g             The graphics context
        @param text          The text to draw
        @param destRectangle The destination rectangle
        @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, Rectangle<float> destRectangle,
                  Justification justification) const noexcept;

    /**
        Draws the formatted text to screen.

        @param g             The graphics context
        @param text          The text to draw
        @param x             The x coordinate of the text
        @param y             The y coordinate of the text
        @param width         The max width the text should be able to occupy
        @param height        The max height the text should be able to occupy
        @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, float x, float y, float width, float height,
                  Justification justification) const noexcept;

    /**
        Draws the formatted text to screen.

        @param g             The graphics context
        @param text          The text to draw
        @param x             The x coordinate of the text
        @param y             The y coordinate of the text
        @param width         The max width the text should be able to occupy
        @param height        The max height the text should be able to occupy
        @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, int x, int y, int width, int height,
                  Justification justification) const noexcept;

    //=================================================================================================================
    /**
        Sets a new colour collection to be used for the various formatting codes.

        @param colourComp The colour compound containing all colours used for the different colour codes
     */
    void setColours(const ColourCompound &colourComp) noexcept;

    /**
        Sets the default and fallback colour for the renderer.

        @param colour The colour
     */
    void setColour(const Colour &colour) noexcept;
    
    /**
        Gets the current colour collection which is used for the various formatting codes.
        
        @return const The colour compound containing all colours used for the different colour codes
     */
    const ColourCompound &getColours() const noexcept;
    
    /**
        Gets a specific colour for a particular colour code.

        @param colourCode The colour code ranging from 0-9 and A-F
        @return The colour to that colour code or else a default colour
     */
    const Colour &getColour(juce_wchar colourCode) const noexcept;

    /**
        Gets the current formatting character.

        @return The character
     */
    const juce_wchar getFormattingCharacter() const noexcept;

    /**
        Gets this' formatters options.
        
        @return The Options object
     */
    const Options &getOptions() const noexcept;

private:
    ColourCompound colours;
    Options options;
};

class JAUT_API FontFormat final
{
public:
    enum JAUT_API Formats
    {
        NONE,
        SMALL_CAPS,
        FORMAT
    };

    //=================================================================================================================
    static void drawText(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                         Justification justification, Formats formats = NONE,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawText(Graphics &g, const String &text, float x, float y, float width, float height,
                         Colour colour,  Justification justification, Formats formats = NONE,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawText(Graphics &g, const String &text, int x, int y, int width, int height,
                         Colour colour,  Justification justification, Formats formats = NONE,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                              Justification justification) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, float x, float y, float width, float height,
                              Colour colour, Justification justification) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, int x, int y, int width, int height,
                              Colour colour, Justification justification) noexcept;
    static void drawFormattedString(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                                    Justification justification, CharFormat *charFormat = nullptr) noexcept;
    static void drawFormattedString(Graphics &g, const String &text, float x, float y, float width, float height,
                                    Colour colour, Justification justification,
                                    CharFormat *charFormat = nullptr) noexcept;
    static void drawFormattedString(Graphics &g, const String &text, int x, int y, int width, int height,
                                    Colour colour, Justification justification,
                                    CharFormat *charFormat = nullptr) noexcept;
};
}
