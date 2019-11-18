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
 * File: jaut_format_layout.cpp
 * Time: 12, Septembre 2019
 *
 * ===============================================================
 */

#include "jaut_format_layout.h"
#include "jaut_format_attributes.h"

namespace jaut
{
static String substring (const String& text, Range<int> range)
{
    return text.substring (range.getStart(), range.getEnd());
}

FormatLayout::Glyph::Glyph (int glyph, Point<float> anch, float w) noexcept
    : glyphCode (glyph), anchor (anch), width (w)
{}

FormatLayout::Glyph::Glyph (const Glyph& other) noexcept
    : glyphCode (other.glyphCode), anchor (other.anchor), width (other.width)
{}

FormatLayout::Glyph& FormatLayout::Glyph::operator= (const Glyph& other) noexcept
{
    glyphCode = other.glyphCode;
    anchor = other.anchor;
    width = other.width;
    return *this;
}

FormatLayout::Glyph::~Glyph() noexcept {}

//==============================================================================
FormatLayout::Run::Run() noexcept
    : colour (0xff000000),
      isUnderlined(false)
{}

FormatLayout::Run::Run (Range<int> range, int numGlyphsToPreallocate, bool isUnderlined)
    : colour (0xff000000), stringRange (range), isUnderlined(isUnderlined)
{
    glyphs.ensureStorageAllocated (numGlyphsToPreallocate);
}

FormatLayout::Run::Run (const Run& other)
    : font (other.font),
      colour (other.colour),
      glyphs (other.glyphs),
      stringRange (other.stringRange),
      isUnderlined(other.isUnderlined)
{}

FormatLayout::Run::~Run() noexcept {}

Range<float> FormatLayout::Run::getRunBoundsX() const noexcept
{
    Range<float> range;
    bool isFirst = true;

    for (auto& glyph : glyphs)
    {
        Range<float> r (glyph.anchor.x, glyph.anchor.x + glyph.width);

        if (isFirst)
        {
            isFirst = false;
            range = r;
        }
        else
        {
            range = range.getUnionWith (r);
        }
    }

    return range;
}

//==============================================================================
FormatLayout::Line::Line() noexcept
    : ascent (0.0f), descent (0.0f), leading (0.0f)
{}

FormatLayout::Line::Line (Range<int> range, Point<float> o, float asc, float desc,
                        float lead, int numRunsToPreallocate)
    : stringRange (range), lineOrigin (o),
      ascent (asc), descent (desc), leading (lead)
{
    runs.ensureStorageAllocated (numRunsToPreallocate);
}

FormatLayout::Line::Line (const Line& other)
    : stringRange (other.stringRange), lineOrigin (other.lineOrigin),
      ascent (other.ascent), descent (other.descent), leading (other.leading)
{
    runs.addCopiesOf (other.runs);
}

FormatLayout::Line::~Line() noexcept
{}

Range<float> FormatLayout::Line::getLineBoundsX() const noexcept
{
    Range<float> range;
    bool isFirst = true;

    for (auto* run : runs)
    {
        auto runRange = run->getRunBoundsX();

        if (isFirst)
        {
            isFirst = false;
            range = runRange;
        }
        else
        {
            range = range.getUnionWith (runRange);
        }
    }

    return range + lineOrigin.x;
}

Range<float> FormatLayout::Line::getLineBoundsY() const noexcept
{
    return { lineOrigin.y - ascent,
             lineOrigin.y + descent };
}

Rectangle<float> FormatLayout::Line::getLineBounds() const noexcept
{
    auto x = getLineBoundsX();
    auto y = getLineBoundsY();

    return { x.getStart(), y.getStart(), x.getLength(), y.getLength() };
}

//==============================================================================
FormatLayout::FormatLayout()
    : width (0), height (0), justification (Justification::topLeft)
{}

FormatLayout::FormatLayout (const FormatLayout& other)
    : width (other.width), height (other.height),
      justification (other.justification)
{
    lines.addCopiesOf (other.lines);
}

FormatLayout::FormatLayout (FormatLayout&& other) noexcept
    : lines (std::move (other.lines)),
      width (other.width), height (other.height),
      justification (other.justification)
{}

FormatLayout& FormatLayout::operator= (FormatLayout&& other) noexcept
{
    lines = std::move (other.lines);
    width = other.width;
    height = other.height;
    justification = other.justification;
    return *this;
}

FormatLayout& FormatLayout::operator= (const FormatLayout& other)
{
    width = other.width;
    height = other.height;
    justification = other.justification;
    lines.clear();
    lines.addCopiesOf (other.lines);
    return *this;
}

FormatLayout::~FormatLayout()
{}

FormatLayout::Line& FormatLayout::getLine (int index) const noexcept
{
    return *lines.getUnchecked (index);
}

void FormatLayout::ensureStorageAllocated (int numLinesNeeded)
{
    lines.ensureStorageAllocated (numLinesNeeded);
}

void FormatLayout::addLine (std::unique_ptr<Line> line)
{
    lines.add (line.release());
}

void FormatLayout::draw (Graphics& g, Rectangle<float> area) const
{
    auto origin = justification.appliedToRectangle (Rectangle<float> (width, getHeight()), area).getPosition();

    auto& context   = g.getInternalContext();
    auto clip       = context.getClipBounds();
    auto clipTop    = clip.getY()      - origin.y;
    auto clipBottom = clip.getBottom() - origin.y;

    for (auto& line : *this)
    {
        auto lineRangeY = line.getLineBoundsY();

        if (lineRangeY.getEnd() < clipTop)
            continue;

        if (lineRangeY.getStart() > clipBottom)
            break;

        auto lineOrigin = origin + line.lineOrigin;

        for (auto* run : line.runs)
        {
            context.setFont (run->font);
            context.setFill (run->colour);

            for (auto& glyph : run->glyphs)
                context.drawGlyph (glyph.glyphCode, AffineTransform::translation (lineOrigin.x + glyph.anchor.x,
                                                                                  lineOrigin.y + glyph.anchor.y));

            if (run->isUnderlined)
            {
                auto runExtent = run->getRunBoundsX();
                auto lineThickness = run->font.getDescent() * 0.3f;

                context.fillRect ({ runExtent.getStart() + lineOrigin.x, lineOrigin.y + lineThickness * 2.0f,
                                    runExtent.getLength(), lineThickness });
            }
        }
    }
}

void FormatLayout::createLayout (const FormatAttributes& text, float maxWidth)
{
    createLayout (text, maxWidth, 1.0e7f);
}

void FormatLayout::createLayout (const FormatAttributes& text, float maxWidth, float maxHeight)
{
    lines.clear();
    width = maxWidth;
    height = maxHeight;
    justification = text.getJustification();

    if (! createNativeLayout (text))
        createStandardLayout (text);

    recalculateSize();
}

void FormatLayout::createLayoutWithBalancedLineLengths (const FormatAttributes& text, float maxWidth)
{
    createLayoutWithBalancedLineLengths (text, maxWidth, 1.0e7f);
}

void FormatLayout::createLayoutWithBalancedLineLengths (const FormatAttributes& text, float maxWidth, float maxHeight)
{
    auto minimumWidth = maxWidth / 2.0f;
    auto bestWidth = maxWidth;
    float bestLineProportion = 0.0f;

    while (maxWidth > minimumWidth)
    {
        createLayout (text, maxWidth, maxHeight);

        if (getNumLines() < 2)
            return;

        auto line1 = lines.getUnchecked (lines.size() - 1)->getLineBoundsX().getLength();
        auto line2 = lines.getUnchecked (lines.size() - 2)->getLineBoundsX().getLength();
        auto shortest = jmin (line1, line2);
        auto longest  = jmax (line1, line2);
        auto prop = shortest > 0 ? longest / shortest : 1.0f;

        if (prop > 0.9f && prop < 1.1f)
            return;

        if (prop > bestLineProportion)
        {
            bestLineProportion = prop;
            bestWidth = maxWidth;
        }

        maxWidth -= 10.0f;
    }

    if (bestWidth != maxWidth)
        createLayout (text, bestWidth, maxHeight);
}

//==============================================================================
namespace FormatLayoutHelpers
{
    struct Token
    {
        Token (const String& t, const Font& f, Colour c, bool whitespace, bool underlined)
            : text (t), font (f), colour (c),
              area (font.getStringWidthFloat (t), f.getHeight()),
              isWhitespace (whitespace),
              isNewLine (t.containsChar ('\n') || t.containsChar ('\r')),
              isUnderlined(underlined)
        {}

        const String text;
        const Font font;
        const Colour colour;
        Rectangle<float> area;
        int line;
        float lineHeight;
        const bool isWhitespace, isNewLine;
        const bool isUnderlined;

        Token& operator= (const Token&) = delete;
    };

    struct TokenList
    {
        TokenList() noexcept {}

        void createLayout (const FormatAttributes& text, FormatLayout& layout)
        {
            layout.ensureStorageAllocated (totalLines);

            addTextRuns (text);
            layoutRuns (layout.getWidth(), text.getLineSpacing(), text.getWordWrap());

            int charPosition = 0;
            int lineStartPosition = 0;
            int runStartPosition = 0;

            std::unique_ptr<FormatLayout::Line> currentLine;
            std::unique_ptr<FormatLayout::Run> currentRun;

            bool needToSetLineOrigin = true;

            for (int i = 0; i < tokens.size(); ++i)
            {
                auto& t = *tokens.getUnchecked (i);

                Array<int> newGlyphs;
                Array<float> xOffsets;
                t.font.getGlyphPositions (getTrimmedEndIfNotAllWhitespace (t.text), newGlyphs, xOffsets);

                if (currentRun == nullptr)  currentRun  = std::make_unique<FormatLayout::Run>();
                if (currentLine == nullptr) currentLine = std::make_unique<FormatLayout::Line>();

                if (newGlyphs.size() > 0)
                {
                    currentRun->glyphs.ensureStorageAllocated (currentRun->glyphs.size() + newGlyphs.size());
                    auto tokenOrigin = t.area.getPosition().translated (0, t.font.getAscent());

                    if (needToSetLineOrigin)
                    {
                        needToSetLineOrigin = false;
                        currentLine->lineOrigin = tokenOrigin;
                    }

                    auto glyphOffset = tokenOrigin - currentLine->lineOrigin;

                    for (int j = 0; j < newGlyphs.size(); ++j)
                    {
                        auto x = xOffsets.getUnchecked (j);
                        currentRun->glyphs.add (FormatLayout::Glyph (newGlyphs.getUnchecked(j),
                                                                   glyphOffset.translated (x, 0),
                                                                   xOffsets.getUnchecked (j + 1) - x));
                    }

                    charPosition += newGlyphs.size();
                }
                else if (t.isWhitespace || t.isNewLine)
                {
                    ++charPosition;
                }

                if (auto* nextToken = tokens[i + 1])
                {
                    if (t.font != nextToken->font || t.colour != nextToken->colour)
                    {
                        addRun (*currentLine, currentRun.release(), t, runStartPosition, charPosition);
                        runStartPosition = charPosition;
                    }

                    if (t.line != nextToken->line)
                    {
                        if (currentRun == nullptr)
                            currentRun = std::make_unique<FormatLayout::Run>();

                        addRun (*currentLine, currentRun.release(), t, runStartPosition, charPosition);
                        currentLine->stringRange = { lineStartPosition, charPosition };

                        if (! needToSetLineOrigin)
                            layout.addLine (std::move (currentLine));

                        runStartPosition = charPosition;
                        lineStartPosition = charPosition;
                        needToSetLineOrigin = true;
                    }
                }
                else
                {
                    addRun (*currentLine, currentRun.release(), t, runStartPosition, charPosition);
                    currentLine->stringRange = { lineStartPosition, charPosition };

                    if (! needToSetLineOrigin)
                        layout.addLine (std::move (currentLine));

                    needToSetLineOrigin = true;
                }
            }

            if ((text.getJustification().getFlags() & (Justification::right | Justification::horizontallyCentred)) != 0)
            {
                auto totalW = layout.getWidth();
                bool isCentred = (text.getJustification().getFlags() & Justification::horizontallyCentred) != 0;

                for (auto& line : layout)
                {
                    auto dx = totalW - line.getLineBoundsX().getLength();

                    if (isCentred)
                        dx /= 2.0f;

                    line.lineOrigin.x += dx;
                }
            }
        }

    private:
        static void addRun (FormatLayout::Line& glyphLine, FormatLayout::Run* glyphRun,
                            const Token& t, int start, int end)
        {
            glyphRun->stringRange  = { start, end };
            glyphRun->font         = t.font;
            glyphRun->colour       = t.colour;
            glyphRun->isUnderlined = t.isUnderlined;
            glyphLine.ascent       = jmax (glyphLine.ascent,  t.font.getAscent());
            glyphLine.descent      = jmax (glyphLine.descent, t.font.getDescent());
            
            glyphLine.runs.add (glyphRun);
        }

        static int getCharacterType (juce_wchar c) noexcept
        {
            if (c == '\r' || c == '\n')
                return 0;

            return CharacterFunctions::isWhitespace (c) ? 2 : 1;
        }

        void appendText (const String& stringText, const Font& font, Colour colour, bool underline)
        {
            auto t = stringText.getCharPointer();
            String currentString;
            int lastCharType = 0;

            for (;;)
            {
                auto c = t.getAndAdvance();

                if (c == 0)
                    break;

                auto charType = getCharacterType (c);

                if (charType == 0 || charType != lastCharType)
                {
                    if (currentString.isNotEmpty())
                        tokens.add (new Token (currentString, font, colour,
                                               lastCharType == 2 || lastCharType == 0, underline));

                    currentString = String::charToString (c);

                    if (c == '\r' && *t == '\n')
                        currentString += t.getAndAdvance();
                }
                else
                {
                    currentString += c;
                }

                lastCharType = charType;
            }

            if (currentString.isNotEmpty())
                tokens.add (new Token (currentString, font, colour, lastCharType == 2, underline));
        }

        void layoutRuns (float maxWidth, float extraLineSpacing, FormatAttributes::WordWrap wordWrap)
        {
            float x = 0, y = 0, h = 0;
            int i;

            for (i = 0; i < tokens.size(); ++i)
            {
                auto& t = *tokens.getUnchecked(i);
                t.area.setPosition (x, y);
                t.line = totalLines;
                x += t.area.getWidth();
                h = jmax (h, t.area.getHeight() + extraLineSpacing);

                auto* nextTok = tokens[i + 1];

                if (nextTok == nullptr)
                    break;

                bool tokenTooLarge = (x + nextTok->area.getWidth() > maxWidth);

                if (t.isNewLine || ((! nextTok->isWhitespace) && (tokenTooLarge && wordWrap != FormatAttributes::None)))
                {
                    setLastLineHeight (i + 1, h);
                    x = 0;
                    y += h;
                    h = 0;
                    ++totalLines;
                }
            }

            setLastLineHeight (jmin (i + 1, tokens.size()), h);
            ++totalLines;
        }

        void setLastLineHeight (int i, float height) noexcept
        {
            while (--i >= 0)
            {
                auto& tok = *tokens.getUnchecked (i);

                if (tok.line == totalLines)
                    tok.lineHeight = height;
                else
                    break;
            }
        }

        void addTextRuns (const FormatAttributes& text)
        {
            auto numAttributes = text.getNumAttributes();
            tokens.ensureStorageAllocated (jmax (64, numAttributes));

            for (int i = 0; i < numAttributes; ++i)
            {
                auto& attr = text.getAttribute (i);

                appendText (substring (text.getText(), attr.range),
                            attr.font, attr.colour, attr.isUnderlined);
            }
        }

        static String getTrimmedEndIfNotAllWhitespace (const String& s)
        {
            auto trimmed = s.trimEnd();

            if (trimmed.isEmpty() && s.isNotEmpty())
                trimmed = s.replaceCharacters ("\r\n\t", "   ");

            return trimmed;
        }

        OwnedArray<Token> tokens;
        int totalLines = 0;

        JUCE_DECLARE_NON_COPYABLE (TokenList)
    };
}

//==============================================================================
void FormatLayout::createStandardLayout (const FormatAttributes& text)
{
    FormatLayoutHelpers::TokenList l;
    l.createLayout (text, *this);
}

bool FormatLayout::createNativeLayout(const FormatAttributes &text)
{
    return false;
}

void FormatLayout::recalculateSize()
{
    if (! lines.isEmpty())
    {
        auto bounds = lines.getFirst()->getLineBounds();

        for (auto* line : lines)
            bounds = bounds.getUnion (line->getLineBounds());

        for (auto* line : lines)
            line->lineOrigin.x -= bounds.getX();

        width  = bounds.getWidth();
        height = bounds.getHeight();
    }
    else
    {
        width = 0;
        height = 0;
    }
}
}
