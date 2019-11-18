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
    @file   lookandfeel.h
    @date   16, October 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/componentoptionlist.h>

namespace jaut
{
class JAUT_API LookAndFeel : public LookAndFeel_V4, public OptionList::LookAndFeelMethods
{
public:
    LookAndFeel() noexcept;
    virtual ~LookAndFeel() override {}

    //==================================================================================================================
    // OptionList methods
    virtual int getOptionListLabelWidth(const String &label) override;
    virtual int getOptionListSpaceBetweenLabelAndBox() override;
    virtual void drawOptionListBackground(Graphics &g, int x, int y, int width, int height) override;
    virtual void drawOptionListOptionBox(Graphics &g, Rectangle<int> bounds, bool isCheckBox, bool checked,
                                         bool enabled, bool isMouseOver, bool isMouseDown) override;
    virtual void drawOptionListOptionLabel(Graphics &g, const String &label, Rectangle<int> bounds, bool isCheckBox,
                                           bool isRightAligned, bool checked, bool enabled,  bool isMouseOver,
                                           bool isMouseDown) override;
};
}
