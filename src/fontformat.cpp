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
    @file   fontformat.cpp
    @date   30, May 2019
    
    ===============================================================
 */

#include <jaut/fontformat.h>
#include <set>

#include "jaut_format_attributes.h"
#include "jaut_format_layout.h"

namespace jaut
{
namespace
{

const int getColourCode(juce_wchar charCode) noexcept
{
    charCode = tolower(charCode);

    if(isdigit(charCode))
    {
        return JAUT_COLOUR_ID<4> + static_cast<int>(charCode - '0');
    }
    else if(charCode >= 'a' && charCode <= 'f')
    {
        return JAUT_COLOUR_ID<14> + static_cast<int>(charCode - 'a');
    }

    return 0;
}

const bool isCharValid(juce_wchar charCode) noexcept
{
    charCode = tolower(charCode);

    return charCode == 'r' || (charCode >= 'x' && charCode <= 'z') || isdigit(charCode) ||
           (charCode >= 'a' && charCode <= 'f');
}

void editFont(juce_wchar charCode, Font &font, Colour &colour, LookAndFeel &lookAndFeel,
              const Colour &defaultColour) noexcept
{
    charCode = tolower(charCode);

    if (charCode == 'r')
    {
        font.setStyleFlags(0);
        colour = defaultColour;
    }
    else if (uint32 colourid = getColourCode(charCode))
    {
        colour = lookAndFeel.findColour(colourid);
    }
    else if (charCode >= 'x' && charCode <= 'z')
    {
        font.setStyleFlags(font.getStyleFlags() | (1 << (charCode - 'x')));
    }
}

FormatAttributes formatInput(const String &input, const Font &font, LookAndFeel &lookAndFeel,
                             const CharFormat::Options &options) noexcept
{
    String::CharPointerType charptr = input.getCharPointer();
    Font cfont = font;
    Colour dcol(options.defaultColour == Colours::transparentBlack || options.defaultColour == Colours::transparentWhite
                ? lookAndFeel.findColour(CharFormat::ColourFormat0Id) : options.defaultColour);
    Colour ccol(dcol);
    String text;
    FormatAttributes as;
    juce_wchar currentchar;
    juce_wchar nextchar;

    for (;;)
    {
        currentchar = charptr.getAndAdvance();

        if (currentchar == 0)
        {
            break;
        }

        nextchar = *charptr;

        if (currentchar == '\n')
        {
            if(!text.isEmpty())
            {
                as.append(text, cfont, ccol);
                text.clear();
            }

            as.append("\n", cfont, ccol);
        }
        else if (currentchar == '\\' && nextchar == options.terminator)
        {
            text += options.terminator;
            ++charptr;
        }
        else if (currentchar == options.terminator)
        {
            if(isCharValid(nextchar))
            {
                if (!text.isEmpty())
                {
                    as.append(text, cfont, ccol);
                    text.clear();
                }

                editFont(nextchar, cfont, ccol, lookAndFeel, dcol);
                ++charptr;
            }
        }
        else
        {
            text += currentchar;
        }
    }

    if (!text.isEmpty())
    {
        as.append(text, cfont, ccol);
    }

    return as;
}
}



/* ==================================================================================
 * ================================= CharFormat =====================================
 * ================================================================================== */

CharFormat::CharFormat() noexcept
    : lookAndFeel(&Desktop::getInstance().getDefaultLookAndFeel())
{}

CharFormat::CharFormat(const Options &options) noexcept
    : lookAndFeel(&Desktop::getInstance().getDefaultLookAndFeel()),
      options(options)
{}

//=====================================================================================================================
void CharFormat::drawText(Graphics &g, const String &text, Rectangle<float> destRectangle,
                          Justification justification) const noexcept
{
    Graphics::ScopedSaveState sss(g);

    FormatLayout textLayout;
    FormatAttributes as(formatInput(text, g.getCurrentFont(), *lookAndFeel, options));
    Point<float> origin = justification.appliedToRectangle(Rectangle<float>(destRectangle.getWidth(),
                                                                            destRectangle.getHeight()),
                                                           destRectangle).getPosition();
    LowLevelGraphicsContext &context = g.getInternalContext();
    Rectangle<int> clip              = context.getClipBounds();
    float clipTop                    = clip.getY() - origin.y;
    float clipBottom                 = clip.getBottom() - origin.y;

    as.setJustification(justification);
    textLayout.createLayout(as, destRectangle.getWidth());

    for (int i = 0; i < textLayout.getNumLines(); ++i)
    {
        FormatLayout::Line &line  = textLayout.getLine(i);
        Range<float> lineRangeY = line.getLineBoundsY();

        if (lineRangeY.getEnd() < clipTop)
        {
            continue;
        }

        if (lineRangeY.getStart() > clipBottom)
        {
            break;
        }

        Point<float> lineOrigin = origin + line.lineOrigin;

        for (int j = 0; j < line.runs.size(); ++j)
        {
            FormatLayout::Run *run = line.runs.getUnchecked(j);
            context.setFont(run->font);
            context.setFill(run->colour);

            String text;

            for (auto &glyph : run->glyphs)
            {
                context.drawGlyph(glyph.glyphCode, AffineTransform::translation(lineOrigin.x + glyph.anchor.x,
                                                                                lineOrigin.y + glyph.anchor.y));
                text += static_cast<juce_wchar>(glyph.glyphCode);
            }

            if (run->isUnderlined)
            {
                Range<float> runExtent = run->getRunBoundsX();
                float lineThickness    = run->font.getDescent() * 0.3f;

                context.fillRect({runExtent.getStart() + lineOrigin.x, lineOrigin.y + lineThickness * 2.0f,
                                  runExtent.getLength(), lineThickness});
            }
        }
    }
}

void CharFormat::drawText(Graphics &g, const String &text, float x, float y, float width, float height,
                          Justification justification) const noexcept
{
    drawText(g, text, {x, y, width, height}, justification);
}

void CharFormat::drawText(Graphics &g, const String &text, int x, int y, int width, int height,
                          Justification justification) const noexcept
{
    drawText(g, text, Rectangle<int>(x, y, width, height).toFloat(), justification);
}

//=====================================================================================================================
void CharFormat::setLookAndFeel(LookAndFeel *lookAndFeel) noexcept
{
    if(lookAndFeel)
    {
        this->lookAndFeel = lookAndFeel;
        return;
    }

    lookAndFeel = &Desktop::getInstance().getDefaultLookAndFeel();
}

LookAndFeel &CharFormat::getLookAndFeel() const noexcept
{
    return *lookAndFeel;
}

void CharFormat::setColour(const Colour &colour) noexcept
{
    options.defaultColour = colour;
}

Colour CharFormat::getColour(juce_wchar colourCode) const noexcept
{
    const int colourid = jaut::getColourCode(colourCode);
    return colourid && lookAndFeel ? lookAndFeel->findColour(colourid) : options.defaultColour;
}

const juce_wchar CharFormat::getFormattingCharacter() const noexcept
{
    return options.terminator;
}

const CharFormat::Options &CharFormat::getOptions() const noexcept
{
    return options;
}



/* ==================================================================================
 * ================================= FontFormat =====================================
 * ================================================================================== */

void FontFormat::drawText(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                          Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    if ((formats & NONE ) == NONE && (text.isEmpty() || g.clipRegionIntersects(area.getSmallestIntegerContainer())))
    {
        return;
    }

    Graphics::ScopedSaveState sss(g);

    FormatLayout textlayout;
    CharFormat format;
    bool flagformat        = (formats & FORMAT)     == FORMAT;
    bool flagcaps          = (formats & SMALL_CAPS) == SMALL_CAPS;
    CharFormat *charformat = flagformat ? (charFormat ? charFormat : &format) : 0;
    FormatAttributes input = charformat ? FormatAttributes(formatInput(text, g.getCurrentFont(),
                                                           charformat->getLookAndFeel(), charformat->getOptions()))
                                        : FormatAttributes(text);
    auto origin            = justification.appliedToRectangle(Rectangle<float>(area.getWidth(), area.getHeight()),
                                                                               area).getPosition();
    LowLevelGraphicsContext &context = g.getInternalContext();
    Rectangle<int> clip              = context.getClipBounds();
    float cliptop                    = clip.getY() - origin.y;
    float clipbottom                 = clip.getBottom() - origin.y;
    int lastcharacter                = 0;
    float fixedindent                = 0.0f;

    input.setJustification(justification);
    textlayout.createLayout(input, area.getWidth());

    for (int i = 0; i < textlayout.getNumLines(); ++i)
    {
        FormatLayout::Line &line = textlayout.getLine(i);
        auto lineRangeY = line.getLineBoundsY();

        if (lineRangeY.getEnd() < cliptop)
        {
            continue;
        }

        if (lineRangeY.getStart() > clipbottom)
        {
            break;
        }

        auto lineOrigin = origin + line.lineOrigin;

        for (auto *run : line.runs)
        {
            Font normalfont = run->font;
            Font capsfont   = run->font.withHeight(normalfont.getHeight() * 0.8f);

            context.setFont(normalfont);
            context.setFill(run->colour);

            for (auto &glyph : run->glyphs)
            {
                if (flagcaps)
                {
                    if (isupper(lastcharacter) && islower(glyph.glyphCode))
                    {
                        fixedindent += 1.5f;
                        context.setFont(capsfont);
                    }
                    else if (isupper(glyph.glyphCode) && islower(glyph.glyphCode))
                    {
                        context.setFont(normalfont);
                    }
                }

                lastcharacter = glyph.glyphCode;
                context.drawGlyph(flagcaps ? toupper(glyph.glyphCode) : glyph.glyphCode,
                                  AffineTransform::translation(lineOrigin.x + glyph.anchor.x + fixedindent,
                                                               lineOrigin.y + glyph.anchor.y));
            }

            if (!charformat)
            {
                if (run->font.isUnderlined())
                {
                    auto runExtent = run->getRunBoundsX();
                    auto lineThickness = run->font.getDescent() * 0.3f;

                    context.fillRect({runExtent.getStart() + lineOrigin.x, lineOrigin.y + lineThickness * 2.0f,
                                      runExtent.getLength(), lineThickness});
                }

                continue;
            }

            if (run->isUnderlined)
            {
                Range<float> runExtent = run->getRunBoundsX();
                float lineThickness = run->font.getDescent() * 0.3f;

                context.fillRect({runExtent.getStart() + lineOrigin.x, lineOrigin.y + lineThickness * 2.0f,
                                  runExtent.getLength(), lineThickness});
            }
        }
    }
}

void FontFormat::drawText(Graphics &g, const String &text, float x, float y, float width, float height, Colour colour,
                          Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    drawText(g, text, {x, y, width, height}, colour, justification, formats, charFormat);
}

void FontFormat::drawText(Graphics &g, const String &text, int x, int y, int width, int height, Colour colour,
                          Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    drawText(g, text, Rectangle<int>(x, y, width, height).toFloat(), colour, justification, formats, charFormat);
}

void FontFormat::drawSmallCaps(Graphics &g, const String &text, Rectangle<float> area,
                               Justification justification) noexcept
{
    if (text.isEmpty() || !g.clipRegionIntersects(area.getSmallestIntegerContainer()))
    {
        return;
    }

    Graphics::ScopedSaveState sss(g);

    Font font   = g.getCurrentFont();
    Font fontSc = font.withHeight(font.getHeight() * 0.8f);
    String::CharPointerType t     = text.getCharPointer();
    String::CharPointerType lastT = t;
    float fixedIndent = 0.0f;
    GlyphArrangement arr;
    Array<int> newGlyphs;
    Array<float> xOffsets;
    font.getGlyphPositions(text.toUpperCase(), newGlyphs, xOffsets);

    for (int i = 0; i < newGlyphs.size(); ++i)
    {
        if (i > 0)
        {
            if (lastT.isUpperCase() && t.isLowerCase())
            {
                fixedIndent += 1.5f;
            }

            lastT = t;
        }

        float nextX       = xOffsets.getUnchecked(i + 1);
        float thisX       = xOffsets.getUnchecked(i) + fixedIndent;
        bool isWhitespace = t.isWhitespace();
        int  glyph        = newGlyphs.getUnchecked(i);
        Font &fontToUse   = t.isUpperCase() ? font : fontSc;

        arr.addGlyph(PositionedGlyph(fontToUse, t.getAndAdvance(), glyph, thisX, 0.0f, nextX - thisX, isWhitespace));
    }

    arr.justifyGlyphs(0, arr.getNumGlyphs(), area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                      justification);
    
    arr.draw(g);
}

void FontFormat::drawSmallCaps(Graphics &g, const String &text, float x, float y, float width, float height,
                               Justification justification) noexcept
{
    drawSmallCaps(g, text, {x, y, width, height}, justification);
}

void FontFormat::drawSmallCaps(Graphics &g, const String &text, int x, int y, int width, int height,
                               Justification justification) noexcept
{
    drawSmallCaps(g, text, Rectangle<int>(x, y, width, height).toFloat(), justification);
}

void FontFormat::drawFormattedString(Graphics &g, const String &text, Rectangle<float> area, Colour colour,
                                     Justification justification, CharFormat *charFormat) noexcept
{
    CharFormat::Options opts;

    if(!charFormat)
    {
        opts.defaultColour = colour;
    }

    CharFormat formatter = charFormat ? *charFormat : CharFormat(opts);
    formatter.drawText(g, text, area, justification);
}

void FontFormat::drawFormattedString(Graphics &g, const String &text, float x, float y, float width, float height,
                                     Colour colour, Justification justification, CharFormat *charFormat) noexcept
{
    drawFormattedString(g, text, {x, y, width, height}, colour, justification, charFormat);
}

void FontFormat::drawFormattedString(Graphics &g, const String &text, int x, int y, int width, int height,
                                     Colour colour, Justification justification, CharFormat *charFormat) noexcept
{
    drawFormattedString(g, text, Rectangle<int>(x, y, width, height).toFloat(), colour, justification, charFormat);
}
}
