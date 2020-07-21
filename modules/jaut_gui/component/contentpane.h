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
    @file   contentpane.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class JAUT_API ContentPane : public juce::Component
{
public:
    enum JAUT_API ColourIds
    {
        ColourBackgroundId = nextColourId<16>
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        virtual void drawContentPaneBackground(juce::Graphics &g, ContentPane &pane, int width, int height) = 0;
    };
    
    //==================================================================================================================
    explicit ContentPane(juce::Component &content) noexcept;
    explicit ContentPane(juce::Component *content = nullptr, bool owned = true);
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //==================================================================================================================
    void resetComponent(juce::Component &content);
    void resetComponent(juce::Component *content = nullptr, bool owned = true);
    
private:
    juce::OptionalScopedPointer<juce::Component> content;
    JAUT_CREATE_LAF()
};
}
