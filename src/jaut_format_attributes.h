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
 * File: jaut_format_attributes.h
 * Time: 11, Septembre 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{
class FormatAttributes final
{
public:
    struct Attribute
    {
        Range<int> range;
        Font font;
        Colour colour { 0xff000000 };
        bool isUnderlined;

        //==============================================================================================================
        Attribute()                     = default;
        Attribute(const Attribute&)     = default;
        Attribute(Attribute&&) noexcept = default;
        Attribute(Range<int> range, const Font &font, Colour colour) noexcept;

        //==============================================================================================================
        Attribute &operator=(const Attribute&)     = default;
        Attribute &operator=(Attribute&&) noexcept = default;


    private:
        JUCE_LEAK_DETECTOR(Attribute)
    };

    enum WordWrap
    {
        None,
        ByWord,
        ByChar
    };

    enum ReadingDirection
    {
        Natural,
        LeftToRight,
        RightToLeft
    };

    //==================================================================================================================
    FormatAttributes()                            = default;
    FormatAttributes(const FormatAttributes&)     = default;
    FormatAttributes(FormatAttributes&&) noexcept = default;
    explicit FormatAttributes (const String& newString)  { setText(newString); }

    //==================================================================================================================
    FormatAttributes &operator=(const FormatAttributes&)     = default;
    FormatAttributes &operator=(FormatAttributes&&) noexcept = default;
    
    //==================================================================================================================
    void append (const String& textToAppend);
    void append (const String& textToAppend, const Font& font);
    void append (const String& textToAppend, Colour colour);
    void append (const String& textToAppend, const Font& font, Colour colour);
    void append (const FormatAttributes& other);
    void clear();

    //==================================================================================================================
    void draw(Graphics &g, const Rectangle<float> &area) const;

    //==================================================================================================================
    const String &getText() const noexcept { return text; }
    Justification getJustification() const noexcept { return justification; }
    WordWrap getWordWrap() const noexcept { return wordWrap; }
    ReadingDirection getReadingDirection() const noexcept { return readingDirection; }
    float getLineSpacing() const noexcept { return lineSpacing; }
    void setText(const String &newText);
    void setJustification(Justification newJustification) noexcept;
    void setWordWrap(WordWrap newWordWrap) noexcept;
    void setReadingDirection(ReadingDirection newReadingDirection) noexcept;
    void setLineSpacing(float newLineSpacing) noexcept;

    //==================================================================================================================
    int getNumAttributes() const noexcept { return attributes.size(); }
    const Attribute &getAttribute(int index) const noexcept { return attributes.getReference(index); }

private:
    String text;
    float lineSpacing = 0.0f;
    Justification justification = Justification::left;
    WordWrap wordWrap = FormatAttributes::ByWord;
    ReadingDirection readingDirection = FormatAttributes::Natural;
    Array<Attribute> attributes;

    JUCE_LEAK_DETECTOR(FormatAttributes)
};
}
