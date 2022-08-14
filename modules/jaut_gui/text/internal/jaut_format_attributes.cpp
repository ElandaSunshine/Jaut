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
    @file   jaut_format_attributes.cpp
    @date   11, September 2019
    
    ===============================================================
 */

namespace jaut
{
namespace
{
int getLength (const juce::Array<FormatAttributes::Attribute>& atts) noexcept
{
    return atts.size() != 0 ? atts.getReference (atts.size() - 1).range.getEnd() : 0;
}

void splitAttributeRanges (juce::Array<FormatAttributes::Attribute>& atts, int position)
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

juce::Range<int> splitAttributeRanges (juce::Array<FormatAttributes::Attribute>& atts, juce::Range<int> newRange)
{
    newRange = newRange.getIntersectionWith ({ 0, getLength (atts) });

    if (! newRange.isEmpty())
    {
        splitAttributeRanges (atts, newRange.getStart());
        splitAttributeRanges (atts, newRange.getEnd());
    }

    return newRange;
}

void appendRange (juce::Array<FormatAttributes::Attribute>& atts,
                    int length, const juce::Font* f, const juce::Colour* c)
{
    if (atts.size() == 0)
    {
        atts.add ({ juce::Range<int> (0, length), f != nullptr ? *f : juce::Font(), c != nullptr ? *c : juce::Colour (0xff000000) });
    }
    else
    {
        auto start = getLength (atts);
        atts.add ({ juce::Range<int> (start, start + length),
                    f != nullptr ? *f : atts.getReference (atts.size() - 1).font,
                    c != nullptr ? *c : atts.getReference (atts.size() - 1).colour });
    }
}

void applyFontAndColour (juce::Array<FormatAttributes::Attribute>& atts,
                         juce::Range<int> range, const juce::Font* f, const juce::Colour* c)
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

void truncate (juce::Array<FormatAttributes::Attribute>& atts, int newLength)
{
    splitAttributeRanges (atts, newLength);

    for (int i = atts.size(); --i >= 0;)
        if (atts.getReference (i).range.getStart() >= newLength)
            atts.remove (i);
}

}

//======================================================================================================================
FormatAttributes::Attribute::Attribute (juce::Range<int> r, const juce::Font& f, juce::Colour c) noexcept
    : range (r), font (f), colour (c), isUnderlined(f.isUnderlined())
{}

//======================================================================================================================
void FormatAttributes::setText (const juce::String &newText)
{
    auto newLength = newText.length();
    auto oldLength = jaut::getLength(attributes);

    if (newLength > oldLength)
        appendRange (attributes, newLength - oldLength, nullptr, nullptr);
    else if (newLength < oldLength)
        truncate (attributes, newLength);

    text = newText;
}

void FormatAttributes::append (const juce::String& textToAppend)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), nullptr, nullptr);
}

void FormatAttributes::append (const juce::String& textToAppend, const juce::Font& font)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), &font, nullptr);
}

void FormatAttributes::append (const juce::String& textToAppend, juce::Colour colour)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), nullptr, &colour);
}

void FormatAttributes::append (const juce::String& textToAppend, const juce::Font& font, juce::Colour colour)
{
    text += textToAppend;
    appendRange (attributes, textToAppend.length(), &font, &colour);
}

void FormatAttributes::append (const FormatAttributes& other)
{
    auto originalLength = jaut::getLength(attributes);
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
void FormatAttributes::draw(juce::Graphics& g, const juce::Rectangle<float>& area) const
{
    ignoreUnused(g, area);
}

//======================================================================================================================

void FormatAttributes::setJustification (juce::Justification newJustification) noexcept
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
