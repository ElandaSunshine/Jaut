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
    @file   lookandfeel.cpp
    @date   16, October 2019
    
    ===============================================================
 */

#include <jaut/lookandfeel.h>
#include <jaut/fontformat.h>

namespace jaut
{
LookAndFeel::LookAndFeel() noexcept
{
    // OptionList
    setColour(OptionList::ColourListBackgroundId,    Colours::transparentBlack);
    setColour(OptionList::ColourOptionBoxEnabledId,  Colours::white);
    setColour(OptionList::ColourOptionBoxDisabledId, Colours::grey);
    setColour(OptionList::ColourOptionLabelId,       Colours::white);

    // CharFormat
    setColour(CharFormat::ColourFormat0Id,           Colours::white);
    setColour(CharFormat::ColourFormat1Id,           Colours::red);
    setColour(CharFormat::ColourFormat2Id,           Colours::orange);
    setColour(CharFormat::ColourFormat3Id,           Colours::yellow);
    setColour(CharFormat::ColourFormat4Id,           Colours::green);
    setColour(CharFormat::ColourFormat5Id,           Colours::blue);
    setColour(CharFormat::ColourFormat6Id,           Colours::indigo);
    setColour(CharFormat::ColourFormat7Id,           Colours::violet);
    setColour(CharFormat::ColourFormat8Id,           Colours::purple);
    setColour(CharFormat::ColourFormat9Id,           Colours::pink);
    setColour(CharFormat::ColourFormatAId,           Colours::silver);
    setColour(CharFormat::ColourFormatBId,           Colours::gold);
    setColour(CharFormat::ColourFormatCId,           Colours::beige);
    setColour(CharFormat::ColourFormatDId,           Colours::brown);
    setColour(CharFormat::ColourFormatEId,           Colours::grey);
    setColour(CharFormat::ColourFormatFId,           Colours::black);
}

int LookAndFeel::getOptionListLabelWidth(const String &label)
{
    return Font().getStringWidth(label);
}

int LookAndFeel::getOptionListSpaceBetweenLabelAndBox()
{
    return 4;
}

void LookAndFeel::drawOptionListBackground(Graphics &g, int x, int y, int width, int height)
{}

void LookAndFeel::drawOptionListOptionBox(Graphics &g, Rectangle<int> bounds, bool isCheckBox, bool checked,
                                          bool enabled, bool isMouseOver, bool isMouseDown)
{
    g.setColour(findColour(enabled ? OptionList::ColourOptionBoxEnabledId : OptionList::ColourOptionBoxDisabledId));

    if(isCheckBox)
    {
        g.drawRect(bounds);

        if(checked)
        {
            const int w = bounds.getWidth();
            const int h = bounds.getHeight();

            Path p;
            p.addLineSegment({w * 0.2f, h * 0.6f, w * 0.4f, h * 0.8f}, 1.0f);
            p.addLineSegment({w * 0.4f, h * 0.8f, w * 0.8f, h * 0.2f}, 1.0f);

            g.fillPath(p);
        }
        else if(isMouseOver)
        {
            g.setColour(findColour(OptionList::ColourOptionBoxEnabledId).withAlpha(0.3f));
            const int w = bounds.getWidth();
            const int h = bounds.getHeight();

            Path p;
            p.addLineSegment({w * 0.2f, h * 0.6f, w * 0.4f, h * 0.8f}, 1.0f);
            p.addLineSegment({w * 0.4f, h * 0.8f, w * 0.8f, h * 0.2f}, 1.0f);

            g.fillPath(p);
        }
    }
    else
    {
        const float ellipsemul = 0.3f;
        g.drawEllipse(bounds.toFloat().reduced(bounds.getWidth() * (ellipsemul / 3),
                                               bounds.getHeight() * (ellipsemul / 3)), 1.0f);

        if(checked)
        {
            g.fillEllipse(bounds.toFloat().reduced(bounds.getWidth() * ellipsemul, bounds.getHeight() * ellipsemul));
        }
        else if(isMouseOver)
        {
            g.setColour(findColour(OptionList::ColourOptionBoxEnabledId).withAlpha(0.3f));
            g.fillEllipse(bounds.toFloat().reduced(bounds.getWidth() * ellipsemul, bounds.getHeight() * ellipsemul));
        }
    }
}

void LookAndFeel::drawOptionListOptionLabel(Graphics &g, const String &label, Rectangle<int> bounds, bool isCheckBox,
                                            bool isRightAligned, bool checked, bool enabled,  bool isMouseOver,
                                            bool isMouseDown)
{
    const Justification align      = isRightAligned ? Justification::centredLeft : Justification::centredRight;
    const Rectangle<int> newbounds = isRightAligned ? bounds.withX(getOptionListSpaceBetweenLabelAndBox())
                                   : bounds.withWidth(bounds.getWidth() - getOptionListSpaceBetweenLabelAndBox());

    g.setColour(findColour(OptionList::ColourOptionLabelId));
    g.drawText(label, newbounds, align);
}
}
