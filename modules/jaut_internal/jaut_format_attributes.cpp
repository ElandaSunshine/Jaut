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
    @file   jaut_format_attributes.cpp
    @date   11, September 2019
    
    ===============================================================
 */

#include "jaut_format_attributes.h"
#include "jaut_format_layout.h"

namespace jaut
{

namespace
{

int getLength (const Array<FormatAttributes::Attribute>& atts) noexcept
{
    return atts.size() != 0 ? atts.getReference (atts.size() - 1).range.getEnd() : 0;
}

void splitAttributeRanges (Array<FormatAttributes::Attribute>& atts, int position)
{
    for (int i = atts.size(); --i >= 0;)
    {
        const auto& att = atts.getUnchecked (i);
        auto offset = position - att.range.getStart();

        if (offset >= 0)
        {
            if (offset > 0 && position < att.range.getEnd())
            {
                atts.insert (i + 1, FormatAttributes::Attribute (att));
                atts.getReference (i).range.setEnd (position);
                atts.getReference (i + 1).range.setStart (position);
            }

            break;
        }
    }
}

Range<int> splitAttributeRanges (Array<FormatAttributes::Attribute>& atts, Range<int> newRange)
{
    newRange = newRange.getIntersectionWith ({ 0, getLength (atts) });

    if (! newRange.isEmpty())
    {
        splitAttributeRanges (atts, newRange.getStart());
        splitAttributeRanges (atts, newRange.getEnd());
    }

    return newRange;
}

void appendRange (Array<FormatAttributes::Attribute>& atts,
                    int length, const Font* f, const Colour* c)
{
    if (atts.size() == 0)
    {
        atts.add ({ Range<int> (0, length), f != nullptr ? *f : Font(), c != nullptr ? *c : Colour (0xff000000) });
    }
    else
    {
        auto start = getLength (atts);
        atts.add ({ Range<int> (start, start + length),
                    f != nullptr ? *f : atts.getReference (atts.size() - 1).font,
                    c != nullptr ? *c : atts.getReference (atts.size() - 1).colour });
    }
}

void applyFontAndColour (Array<FormatAttributes::Attribute>& atts,
                            Range<int> range, const Font* f, const Colour* c)
{
    range = splitAttributeRanges (atts, range);

    for (auto& att : atts)
    {
        if (range.getStart() < att.range.getEnd())
        {
            if (range.getEnd() <= att.range.getStart())
                break;

            if (c != nullptr) att.colour = *c;
            if (f != nullptr) att.font = *f;
        }
    }
}

void truncate (Array<FormatAttributes::Attribute>& atts, int newLength)
{
    splitAttributeRanges (atts, newLength);

    for (int i = atts.size(); --i >= 0;)
        if (atts.getReference (i).range.getStart() >= newLength)
            atts.remove (i);
}

}

//======================================================================================================================
FormatAttributes::Attribute::Attribute (Range<int> r, const Font& f, Colour c) noexcept
    : range (r), font (f), colour (c), isUnderlined(f.isUnderlined())
{}

//======================================================================================================================
void FormatAttributes::setText (const String& newText)
{
    auto newLength = newText.length();
    auto oldLength = getLength (attributes);

    if (newLength > oldLength)
        appendRange (attributes, newLength - oldLength, nullptr, nullptr);
    else if (newLength < oldLength)
        truncate (attributes, newLength);

    text = newText;
}

void FormatAttributes::append (const String& textToAppend)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), nullptr, nullptr);
}

void FormatAttributes::append (const String& textToAppend, const Font& font)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), &font, nullptr);
}

void FormatAttributes::append (const String& textToAppend, Colour colour)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), nullptr, &colour);
}

void FormatAttributes::append (const String& textToAppend, const Font& font, Colour colour)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), &font, &colour);
}

void FormatAttributes::append (const FormatAttributes& other)
{
    auto originalLength = getLength (attributes);
    auto originalNumAtts = attributes.size();
    text += other.text;
    attributes.addArray (other.attributes);

    for (auto i = originalNumAtts; i < attributes.size(); ++i)
    {
        attributes.getReference (i).range += originalLength;
    }
}

void FormatAttributes::clear()
{
    text.clear();
    attributes.clear();
}

//======================================================================================================================
void FormatAttributes::draw(Graphics& g, const Rectangle<float>& area) const
{
    ignoreUnused(g, area);
}

//======================================================================================================================

void FormatAttributes::setJustification (Justification newJustification) noexcept
{
    justification = newJustification;
}

void FormatAttributes::setWordWrap (WordWrap newWordWrap) noexcept
{
    wordWrap = newWordWrap;
}

void FormatAttributes::setReadingDirection (ReadingDirection newReadingDirection) noexcept
{
    readingDirection = newReadingDirection;
}

void FormatAttributes::setLineSpacing (const float newLineSpacing) noexcept
{
    lineSpacing = newLineSpacing;
}
}
