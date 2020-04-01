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
    @file   fontformat.h
    @date   30, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{
/**
 * The ChatFormat class helps you to format text directly through text instead of creating a new font
 * over and over again.
 * You can change the colour and font style, like making it bold or italic.
 *
 * The default terminator is '$' followed by a number or character to identify the format applied to the text.
 */

class JAUT_API CharFormat final
{
public:
    struct JAUT_API Options final
    {
        /**
         * The terminator character defines when a formatted sequence should begin.
         * e.g. "$xThis is a test" <- This will make the text bold
         */
        juce::juce_wchar terminator = '$';

        /**
         * The default colour to use when there was a problem finding a colour.
         */
        juce::Colour defaultColour = Colours::transparentBlack;
    };

    /**
     * The list of colours applicable for formatting.
     */
    enum JAUT_API ColourIds
    {
        ColourFormat0Id = JAUT_COLOUR_ID <4>,
        ColourFormat1Id = JAUT_COLOUR_ID <5>,
        ColourFormat2Id = JAUT_COLOUR_ID <6>,
        ColourFormat3Id = JAUT_COLOUR_ID <7>,
        ColourFormat4Id = JAUT_COLOUR_ID <8>,
        ColourFormat5Id = JAUT_COLOUR_ID <9>,
        ColourFormat6Id = JAUT_COLOUR_ID<10>,
        ColourFormat7Id = JAUT_COLOUR_ID<11>,
        ColourFormat8Id = JAUT_COLOUR_ID<12>,
        ColourFormat9Id = JAUT_COLOUR_ID<13>,
        ColourFormatAId = JAUT_COLOUR_ID<14>,
        ColourFormatBId = JAUT_COLOUR_ID<15>,
        ColourFormatCId = JAUT_COLOUR_ID<16>,
        ColourFormatDId = JAUT_COLOUR_ID<17>,
        ColourFormatEId = JAUT_COLOUR_ID<18>,
        ColourFormatFId = JAUT_COLOUR_ID<19>
    };

    //==================================================================================================================
    /**
     * Creates a new default instance of the CharFormat class.
     */
    CharFormat() noexcept;

    /**
     * Creates a new instance of the CharFormat class with all colours white.
     * @param terminator The character determining when a new formatting sequence begins
     */
    explicit CharFormat(const Options &options) noexcept;

    //==================================================================================================================
    /**
     * Draws the formatted text to screen.
     * @param g             The graphics context
     * @param text          The text to draw
     * @param destRectangle The destination rectangle
     * @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, Rectangle<float> destRectangle,
                  Justification justification) const noexcept;

    /**
     * Draws the formatted text to screen.
     * @param g             The graphics context
     * @param text          The text to draw
     * @param x             The x coordinate of the text
     * @param y             The y coordinate of the text
     * @param width         The max width the text should be able to occupy
     * @param height        The max height the text should be able to occupy
     * @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, float x, float y, float width, float height,
                  Justification justification) const noexcept;

    /**
     * Draws the formatted text to screen.
     * @param g             The graphics context
     * @param text          The text to draw
     * @param x             The x coordinate of the text
     * @param y             The y coordinate of the text
     * @param width         The max width the text should be able to occupy
     * @param height        The max height the text should be able to occupy
     * @param justification The justification to apply to the text
     */
    void drawText(Graphics &g, const String &text, int x, int y, int width, int height,
                  Justification justification) const noexcept;

    //=================================================================================================================
    /**
     * Sets this formatters LookAndFeel object.
     * This is majorly used for colours this formatter should use.
     * 
     * If this is nullptr, the default LookAndFeel will be used!
     * @param lookAndFeel A pointer to a LookAndFeel object which should be used
     */
    
    void setLookAndFeel(LookAndFeel *lookAndFeel) noexcept;

    /**
        Gets the current set LookAndFeel object.
        @returns The LookAndFeel object
     */
    LookAndFeel &getLookAndFeel() const noexcept;

    /**
        Sets the default and fallback colour for the renderer.

        @param colour The colour
     */
    void setColour(const Colour &colour) noexcept;
    
    /**
        Gets a specific colour for a particular colour code.

        @param colourCode The colour code ranging from 0-9 and A-F
        @return The colour to that colour code or else a default colour
     */
    Colour getColour(juce_wchar colourCode) const noexcept;

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
    LookAndFeel *lookAndFeel;
    Options options;
};

class JAUT_API FontFormat final
{
public:
    enum JAUT_API Formats
    {
        None,
        SmallCaps,
        Format
    };

    //=================================================================================================================
    static void drawText(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                         Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawText(Graphics &g, const String &text, float x, float y, float width, float height,
                         Colour colour,  Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawText(Graphics &g, const String &text, int x, int y, int width, int height,
                         Colour colour,  Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, Rectangle<float> area,
                              Justification justification) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, float x, float y, float width, float height,
                              Justification justification) noexcept;
    static void drawSmallCaps(Graphics &g, const String &text, int x, int y, int width, int height,
                              Justification justification) noexcept;
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
