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

#include <jaut/util/fontformat.h>
#include <set>

#include "jaut_format_attributes.h"
#include "jaut_format_layout.h"

namespace
{
const int getColourCode(juce_wchar charCode) noexcept
{
    charCode = tolower(charCode);

    if(isdigit(charCode))
    {
        return jaut::JAUT_COLOUR_ID<4> + static_cast<int>(charCode - '0');
    }
    else if(charCode >= 'a' && charCode <= 'f')
    {
        return jaut::JAUT_COLOUR_ID<14> + static_cast<int>(charCode - 'a');
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
    else if (int colour_id = getColourCode(charCode))
    {
        colour = lookAndFeel.findColour(colour_id);
    }
    else if (charCode >= 'x' && charCode <= 'z')
    {
        font.setStyleFlags(font.getStyleFlags() | (1 << (charCode - 'x')));
    }
}

jaut::FormatAttributes formatInput(const String &input, const Font &font, LookAndFeel &lookAndFeel,
                                   const jaut::CharFormat::Options &options) noexcept
{
    const Colour default_colour = options.defaultColour == Colour(0) || options.defaultColour == Colour(0x00ffffff)
                                  ? lookAndFeel.findColour(jaut::CharFormat::ColourFormat0Id) : options.defaultColour;
    Colour current_colour = default_colour;
    Font current_font     = font;
    String::CharPointerType charptr = input.getCharPointer();
    String text;
    jaut::FormatAttributes attributes;
    juce_wchar current_char;
    juce_wchar next_char;

    for (;;)
    {
        current_char = charptr.getAndAdvance();

        if (current_char == 0)
        {
            break;
        }

        next_char = *charptr;

        if (current_char == '\n')
        {
            if(!text.isEmpty())
            {
                attributes.append(text, current_font, current_colour);
                text.clear();
            }

            attributes.append("\n", current_font, current_colour);
        }
        else if (current_char == '\\' && next_char == options.terminator)
        {
            text += options.terminator;
            ++charptr;
        }
        else if (current_char == options.terminator)
        {
            if(::isCharValid(next_char))
            {
                if (!text.isEmpty())
                {
                    attributes.append(text, current_font, current_colour);
                    text.clear();
                }

                ::editFont(next_char, current_font, current_colour, lookAndFeel, default_colour);
                ++charptr;
            }
        }
        else
        {
            text += current_char;
        }
    }

    if (!text.isEmpty())
    {
        attributes.append(text, current_font, current_colour);
    }

    return attributes;
}
}

namespace jaut
{
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
    Graphics::ScopedSaveState save_state (g);

    FormatLayout text_layout;
    auto attributes    = ::formatInput(text, g.getCurrentFont(), *lookAndFeel, options);
    const Point origin = justification.appliedToRectangle({destRectangle.getWidth(), destRectangle.getHeight()},
                                                          destRectangle).getPosition();
    LowLevelGraphicsContext &context = g.getInternalContext();
    const Rectangle<int> clip        = context.getClipBounds();
    const float clip_top             = clip.getY() - origin.y;
    const float clip_bottom          = clip.getBottom() - origin.y;

    attributes.setJustification(justification);
    text_layout.createLayout(attributes, destRectangle.getWidth());

    for (int i = 0; i < text_layout.getNumLines(); ++i)
    {
        const FormatLayout::Line &line  = text_layout.getLine(i);
        const Range<float> line_range_y = line.getLineBoundsY();

        if (line_range_y.getEnd() < clip_top)
        {
            continue;
        }

        if (line_range_y.getStart() > clip_bottom)
        {
            break;
        }

        const Point<float> line_origin = origin + line.lineOrigin;

        for (int j = 0; j < line.runs.size(); ++j)
        {
            FormatLayout::Run const * const run = line.runs.getUnchecked(j);
            context.setFont(run->font);
            context.setFill(run->colour);

            String text;

            for (auto &glyph : run->glyphs)
            {
                context.drawGlyph(glyph.glyphCode, AffineTransform::translation(line_origin.x + glyph.anchor.x,
                                                                                line_origin.y + glyph.anchor.y));
                text += static_cast<juce_wchar>(glyph.glyphCode);
            }

            if (run->isUnderlined)
            {
                const Range<float> run_extent = run->getRunBoundsX();
                const float line_thickness    = run->font.getDescent() * 0.3f;

                context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                  run_extent.getLength(), line_thickness});
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
    const int colour_id = ::getColourCode(colourCode);
    return colour_id && lookAndFeel ? lookAndFeel->findColour(colour_id) : options.defaultColour;
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
    if ((formats & None) == None && (text.isEmpty() || g.clipRegionIntersects(area.getSmallestIntegerContainer())))
    {
        return;
    }

    Graphics::ScopedSaveState save_state(g);

    FormatLayout text_layout;
    CharFormat format;
    const bool format_char  = (formats & Format)    == Format;
    const bool format_caps  = (formats & SmallCaps) == SmallCaps;
    CharFormat *char_format = format_char ? (charFormat ? charFormat : &format) : 0;
    auto attributes         = char_format ? FormatAttributes(formatInput(text, g.getCurrentFont(),
                                                             char_format->getLookAndFeel(), char_format->getOptions()))
                                          : FormatAttributes(text);
    const Point origin = justification.appliedToRectangle({area.getWidth(), area.getHeight()}, area).getPosition();
    LowLevelGraphicsContext &context = g.getInternalContext();
    const Rectangle clip             = context.getClipBounds();
    const float clip_top             = clip.getY()      - origin.y;
    const float clip_bottom          = clip.getBottom() - origin.y;


    int last_char      = 0;
    float fixed_indent = 0.0f;

    attributes.setJustification(justification);
    text_layout.createLayout(attributes, area.getWidth());

    for (int i = 0; i < text_layout.getNumLines(); ++i)
    {
        const FormatLayout::Line &line = text_layout.getLine(i);
        const Range line_range_y       = line.getLineBoundsY();

        if (line_range_y.getEnd() < clip_top)
        {
            continue;
        }

        if (line_range_y.getStart() > clip_bottom)
        {
            break;
        }

        const Point line_origin = origin + line.lineOrigin;

        for (auto *run : line.runs)
        {
            const Font font_normal = run->font;
            const Font font_caps   = run->font.withHeight(font_normal.getHeight() * 0.8f);

            context.setFont(font_normal);
            context.setFill(run->colour);

            for (auto &glyph : run->glyphs)
            {
                if (format_caps)
                {
                    if (isupper(last_char) && islower(glyph.glyphCode))
                    {
                        fixed_indent += 1.5f;
                        context.setFont(font_caps);
                    }
                    else if (isupper(glyph.glyphCode) && islower(glyph.glyphCode))
                    {
                        context.setFont(font_normal);
                    }
                }

                last_char = glyph.glyphCode;
                context.drawGlyph(format_caps ? toupper(glyph.glyphCode) : glyph.glyphCode,
                                  AffineTransform::translation(line_origin.x + glyph.anchor.x + fixed_indent,
                                                               line_origin.y + glyph.anchor.y));
            }

            if (!format_char)
            {
                if (run->font.isUnderlined())
                {
                    const Range run_extent     = run->getRunBoundsX();
                    const float line_thickness = run->font.getDescent() * 0.3f;

                    context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                      run_extent.getLength(), line_thickness});
                }

                continue;
            }

            if (run->isUnderlined)
            {
                const Range run_extent     = run->getRunBoundsX();
                const float line_thickness = run->font.getDescent() * 0.3f;

                context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                  run_extent.getLength(), line_thickness});
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

    Graphics::ScopedSaveState save_state(g);

    const Font font_default              = g.getCurrentFont();
    const Font font_small_caps           = font_default.withHeight(font_default.getHeight() * 0.8f);
    String::CharPointerType current_char = text.getCharPointer();
    String::CharPointerType last_char    = current_char;
    float fixed_indent = 0.0f;
    GlyphArrangement output;
    Array<int> glyphs;
    Array<float> offsets;

    font_default.getGlyphPositions(text.toUpperCase(), glyphs, offsets);

    for (int i = 0; i < glyphs.size(); ++i)
    {
        if (i > 0)
        {
            if (last_char.isUpperCase() && current_char.isLowerCase())
            {
                fixed_indent += 1.5f;
            }

            last_char = current_char;
        }

        const float next_x       = offsets.getUnchecked(i + 1);
        const float this_x       = offsets.getUnchecked(i) + fixed_indent;
        const bool is_whitespace = current_char.isWhitespace();
        const int glyph          = glyphs.getUnchecked(i);
        const Font &font_to_use  = current_char.isUpperCase() ? font_default : font_small_caps;

        output.addGlyph(PositionedGlyph(font_to_use, current_char.getAndAdvance(), glyph,
                                        this_x, 0.0f, next_x - this_x, is_whitespace));
    }

    output.justifyGlyphs(0, output.getNumGlyphs(), area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                         justification);
    
    output.draw(g);
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
    CharFormat::Options options;

    if(!charFormat)
    {
        options.defaultColour = colour;
    }

    CharFormat formatter = charFormat ? *charFormat : CharFormat(options);
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
