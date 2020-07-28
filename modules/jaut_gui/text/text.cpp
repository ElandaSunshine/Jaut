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
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   text.h
    @date   30, May 2019
    
    ===============================================================
 */

//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
int getColourCode(juce::juce_wchar charCode) noexcept
{
    charCode = std::tolower(charCode);
    
    if(std::isdigit(charCode))
    {
        return jaut::CharFormat::ColourIds::ColourFormat0Id + static_cast<int>(charCode - '0');
    }
    else if(charCode >= 'a' && charCode <= 'f')
    {
        return jaut::CharFormat::ColourIds::ColourFormatAId + static_cast<int>(charCode - 'a');
    }
    
    return 0;
}

bool isCharValid(juce::juce_wchar charCode) noexcept
{
    charCode = std::tolower(charCode);
    
    return charCode == 'r' || (charCode >= 'x' && charCode <= 'z') || isdigit(charCode) ||
           (charCode >= 'a' && charCode <= 'f');
}

void editFont(juce::juce_wchar charCode, juce::Font &font, juce::Colour &colour, juce::LookAndFeel &lookAndFeel,
              const juce::Colour &defaultColour) noexcept
{
    charCode = std::tolower(charCode);
    
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

jaut::FormatAttributes formatInput(const juce::String &input, const juce::Font &font, juce::LookAndFeel &lookAndFeel,
                                   const jaut::CharFormat::Options &options) noexcept
{
    const juce::Colour default_colour = options.defaultColour == juce::Colour(0) ||
                                        options.defaultColour == juce::Colour(0x00ffffff)
                                                             ? lookAndFeel.findColour(jaut::CharFormat::ColourFormat0Id)
                                                             : options.defaultColour;
    
    juce::Colour current_colour = default_colour;
    juce::Font   current_font   = font;
    juce::String::CharPointerType char_ptr = input.getCharPointer();
    juce::String text;
    jaut::FormatAttributes attributes;
    
    for (;;)
    {
        juce::juce_wchar current_char = char_ptr.getAndAdvance();
        
        if (current_char == 0)
        {
            break;
        }
        
        juce::juce_wchar next_char = *char_ptr;
        
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
            ++char_ptr;
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
                ++char_ptr;
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
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************

namespace jaut
{
//**********************************************************************************************************************
// region CharFormat
//======================================================================================================================
CharFormat::CharFormat() noexcept
{
    setLookAndFeel(nullptr);
}

CharFormat::CharFormat(Options options) noexcept
    : options(std::move(options))
{
    setLookAndFeel(nullptr);
}

//======================================================================================================================
void CharFormat::drawText(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> destRectangle,
                          juce::Justification justification) const noexcept
{
    juce::Graphics::ScopedSaveState save_state (g);
    
    jaut::FormatLayout text_layout;
    auto attributes          = ::formatInput(text, g.getCurrentFont(), *lookAndFeel, options);
    const juce::Point origin = justification.appliedToRectangle({destRectangle.getWidth(), destRectangle.getHeight()},
                                                                destRectangle).getPosition();
    juce::LowLevelGraphicsContext &context = g.getInternalContext();
    const juce::Rectangle<int> clip        = context.getClipBounds();
    const float clip_top                   = clip.getY() - origin.y;
    const float clip_bottom                = clip.getBottom() - origin.y;
    
    attributes.setJustification(justification);
    text_layout.createLayout(attributes, destRectangle.getWidth());
    
    for (int i = 0; i < text_layout.getNumLines(); ++i)
    {
        const jaut::FormatLayout::Line &line  = text_layout.getLine(i);
        const juce::Range<float> line_range_y = line.getLineBoundsY();
        
        if (line_range_y.getEnd() < clip_top)
        {
            continue;
        }
        
        if (line_range_y.getStart() > clip_bottom)
        {
            break;
        }
        
        const juce::Point<float> line_origin = origin + line.lineOrigin;
        
        for (int j = 0; j < line.runs.size(); ++j)
        {
            jaut::FormatLayout::Run const * const run = line.runs.getUnchecked(j);
            context.setFont(run->font);
            context.setFill(run->colour);
            
            juce::String text;
            
            for (auto &glyph : run->glyphs)
            {
                context.drawGlyph(glyph.glyphCode, juce::AffineTransform::translation(line_origin.x + glyph.anchor.x,
                                                                                      line_origin.y + glyph.anchor.y));
                text += static_cast<juce::juce_wchar>(glyph.glyphCode);
            }
            
            if (run->isUnderlined)
            {
                const juce::Range<float> run_extent = run->getRunBoundsX();
                const float line_thickness          = run->font.getDescent() * 0.3f;
                
                context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                  run_extent.getLength(), line_thickness});
            }
        }
    }
}

void CharFormat::drawText(juce::Graphics &g, const juce::String &text, float x, float y, float width, float height,
                          juce::Justification justification) const noexcept
{
    drawText(g, text, {x, y, width, height}, justification);
}

void CharFormat::drawText(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                          juce::Justification justification) const noexcept
{
    drawText(g, text, juce::Rectangle<int>(x, y, width, height).toFloat(), justification);
}

//======================================================================================================================
void CharFormat::setLookAndFeel(LookAndFeel_Jaut *laf) noexcept
{
    if (laf)
    {
        lookAndFeel = laf;
        return;
    }
    
    if (lookAndFeel = dynamic_cast<LookAndFeel_Jaut*>(&juce::Desktop::getInstance().getDefaultLookAndFeel());
       !lookAndFeel)
    {
        lookAndFeel = &LookAndFeel_Jaut::getDefaultLaf();
    }
}

juce::LookAndFeel &CharFormat::getLookAndFeel() const noexcept
{
    return *lookAndFeel;
}

void CharFormat::setColour(const juce::Colour &colour) noexcept
{
    options.defaultColour = colour;
}

juce::Colour CharFormat::getColour(juce::juce_wchar colourCode) const noexcept
{
    const int colour_id = ::getColourCode(colourCode);
    return colour_id && lookAndFeel ? lookAndFeel->findColour(colour_id) : options.defaultColour;
}

juce::juce_wchar CharFormat::getFormattingCharacter() const noexcept
{
    return options.terminator;
}

const CharFormat::Options &CharFormat::getOptions() const noexcept
{
    return options;
}
//======================================================================================================================
// endregion CharFormat
//**********************************************************************************************************************
// region FontFormat
//======================================================================================================================
void FontFormat::drawText(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                          juce::Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    if ((formats & None) == None && (text.isEmpty() || g.clipRegionIntersects(area.getSmallestIntegerContainer())))
    {
        return;
    }
    
    juce::Graphics::ScopedSaveState save_state(g);
    
    jaut::FormatLayout text_layout;
    CharFormat format;
    const bool format_char  = (formats & Format)    == Format;
    const bool format_caps  = (formats & SmallCaps) == SmallCaps;
    CharFormat *char_format = format_char ? (charFormat ? charFormat : &format) : 0;
    auto attributes         = char_format ? FormatAttributes(formatInput(text, g.getCurrentFont(),
                                                                         char_format->getLookAndFeel(),
                                                                         char_format->getOptions()))
                                          : jaut::FormatAttributes(text);
    const juce::Point origin = justification.appliedToRectangle({area.getWidth(), area.getHeight()}, area)
                                            .getPosition();
    juce::LowLevelGraphicsContext &context = g.getInternalContext();
    const juce::Rectangle clip             = context.getClipBounds();
    const float clip_top                   = clip.getY()      - origin.y;
    const float clip_bottom                = clip.getBottom() - origin.y;
    
    
    int last_char      = 0;
    float fixed_indent = 0.0f;
    
    attributes.setJustification(justification);
    text_layout.createLayout(attributes, area.getWidth());
    
    for (int i = 0; i < text_layout.getNumLines(); ++i)
    {
        const jaut::FormatLayout::Line &line = text_layout.getLine(i);
        const juce::Range line_range_y       = line.getLineBoundsY();
        
        if (line_range_y.getEnd() < clip_top)
        {
            continue;
        }
        
        if (line_range_y.getStart() > clip_bottom)
        {
            break;
        }
        
        const juce::Point line_origin = origin + line.lineOrigin;
        
        for (auto *run : line.runs)
        {
            const juce::Font font_normal = run->font;
            const juce::Font font_caps   = run->font.withHeight(font_normal.getHeight() * 0.8f);
            
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
                                  juce::AffineTransform::translation(line_origin.x + glyph.anchor.x + fixed_indent,
                                                                     line_origin.y + glyph.anchor.y));
            }
            
            if (!format_char)
            {
                if (run->font.isUnderlined())
                {
                    const juce::Range run_extent = run->getRunBoundsX();
                    const float line_thickness   = run->font.getDescent() * 0.3f;
                    
                    context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                      run_extent.getLength(), line_thickness});
                }
                
                continue;
            }
            
            if (run->isUnderlined)
            {
                const juce::Range run_extent = run->getRunBoundsX();
                const float line_thickness   = run->font.getDescent() * 0.3f;
                
                context.fillRect({run_extent.getStart() + line_origin.x, line_origin.y + line_thickness * 2.0f,
                                  run_extent.getLength(), line_thickness});
            }
        }
    }
}

void FontFormat::drawText(juce::Graphics &g, const juce::String &text, float x, float y, float width, float height,
                          juce::Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    drawText(g, text, {x, y, width, height}, justification, formats, charFormat);
}

void FontFormat::drawText(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                          juce::Justification justification, Formats formats, CharFormat *charFormat) noexcept
{
    drawText(g, text, juce::Rectangle<int>(x, y, width, height).toFloat(), justification, formats, charFormat);
}

void FontFormat::drawSmallCaps(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                               juce::Justification justification) noexcept
{
    if (text.isEmpty() || !g.clipRegionIntersects(area.getSmallestIntegerContainer()))
    {
        return;
    }
    
    juce::Graphics::ScopedSaveState save_state(g);
    
    const juce::Font font_default              = g.getCurrentFont();
    const juce::Font font_small_caps           = font_default.withHeight(font_default.getHeight() * 0.8f);
    juce::String::CharPointerType current_char = text.getCharPointer();
    juce::String::CharPointerType last_char    = current_char;
    float fixed_indent = 0.0f;
    juce::GlyphArrangement output;
    juce::Array<int> glyphs;
    juce::Array<float> offsets;
    
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
        
        const float next_x            = offsets.getUnchecked(i + 1);
        const float this_x            = offsets.getUnchecked(i) + fixed_indent;
        const bool is_whitespace      = current_char.isWhitespace();
        const int glyph               = glyphs.getUnchecked(i);
        const juce::Font &font_to_use = current_char.isUpperCase() ? font_default : font_small_caps;
        
        output.addGlyph(juce::PositionedGlyph(font_to_use, current_char.getAndAdvance(), glyph,
                                              this_x, 0.0f, next_x - this_x, is_whitespace));
    }
    
    output.justifyGlyphs(0, output.getNumGlyphs(), area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                         justification);
    
    output.draw(g);
}

void FontFormat::drawSmallCaps(juce::Graphics &g, const juce::String &text, float x, float y, float width, float height,
                               juce::Justification justification) noexcept
{
    drawSmallCaps(g, text, {x, y, width, height}, justification);
}

void FontFormat::drawSmallCaps(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                               juce::Justification justification) noexcept
{
    drawSmallCaps(g, text, juce::Rectangle<int>(x, y, width, height).toFloat(), justification);
}

void FontFormat::drawFormattedString(juce::Graphics &g, const juce::String &text, juce::Rectangle<float> area,
                                     juce::Colour colour, juce::Justification justification,
                                     CharFormat *charFormat) noexcept
{
    CharFormat::Options options;
    
    if(!charFormat)
    {
        options.defaultColour = colour;
    }
    
    CharFormat formatter = charFormat ? *charFormat : CharFormat(options);
    formatter.drawText(g, text, area, justification);
}

void FontFormat::drawFormattedString(juce::Graphics &g, const juce::String &text, float x, float y, float width,
                                     float height, juce::Colour colour, juce::Justification justification,
                                     CharFormat *charFormat) noexcept
{
    drawFormattedString(g, text, {x, y, width, height}, colour, justification, charFormat);
}

void FontFormat::drawFormattedString(juce::Graphics &g, const juce::String &text, int x, int y, int width, int height,
                                     juce::Colour colour, juce::Justification justification,
                                     CharFormat *charFormat) noexcept
{
    drawFormattedString(g, text, juce::Rectangle<int>(x, y, width, height).toFloat(), colour, justification,
                        charFormat);
}
//======================================================================================================================
// endregion FontFormat
//**********************************************************************************************************************
}
