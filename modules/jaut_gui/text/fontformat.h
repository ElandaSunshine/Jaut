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
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   fontformat.h
    @date   30, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
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
    static void drawText(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                         juce::Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr);
    static void drawText(juce::Graphics &g, const juce::String &text, float x, float y, float width, float height,
                         juce::Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr);
    static void drawText(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                         juce::Justification justification, Formats formats = None,
                         CharFormat *charFormat = nullptr);
    static void drawSmallCaps(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                              juce::Justification justification);
    static void drawSmallCaps(juce::Graphics &g, const juce::String &text, float x, float y, float width, float height,
                              juce::Justification justification);
    static void drawSmallCaps(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                              juce::Justification justification);
    static void drawFormattedString(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                                    juce::Colour colour, juce::Justification justification,
                                    CharFormat *charFormat = nullptr);
    static void drawFormattedString(juce::Graphics &g, const juce::String &text, float x, float y, float width,
                                    float height, juce::Colour colour, juce::Justification justification,
                                    CharFormat *charFormat = nullptr);
    static void drawFormattedString(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                                    juce::Colour colour, juce::Justification justification,
                                    CharFormat *charFormat = nullptr);
    static void drawOutlineString(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                                  float outlineThickness, juce::Colour outlineColour,
                                  juce::Justification justification, bool useEllipsesIfTooBig = true);
    static void drawOutlineString(juce::Graphics &g, const juce::String &text, float x, float y, float width,
                                  float height, float outlineThickness, juce::Colour outlineColour,
                                  juce::Justification justification, bool useEllipsesIfTooBig = true);
    static void drawOutlineString(juce::Graphics &g, const juce::String &text, int x, int y, int width,
                                  int height, float outlineThickness, juce::Colour outlineColour,
                                  juce::Justification justification, bool useEllipsesIfTooBig = true);
};
}
