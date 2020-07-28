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
    @file   lookandfeel_jaut.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class JAUT_API LookAndFeel_Jaut : public juce::LookAndFeel_V4,
                                  public ContentPane   ::LookAndFeelMethods,
                                  public SplitContainer::LookAndFeelMethods,
                                  public MultiPagePane  ::LookAndFeelMethods
{
public:
    static LookAndFeel_Jaut& getDefaultLaf()
    {
        static LookAndFeel_Jaut defaultLaf;
        return defaultLaf;
    }
    
    //==================================================================================================================
    LookAndFeel_Jaut();
    
    //==================================================================================================================
    void drawContentPaneBackground(juce::Graphics &g, juce::Rectangle<int> bounds) override;
    
    //==================================================================================================================
    void drawSplitContainerBackground(juce::Graphics &g, juce::Rectangle<int> bounds) override;
    void drawSplitContainerSeperator(juce::Graphics &g, juce::Rectangle<int> bounds,
                                     SplitContainer::Orientation orientation) override;
    
    //==================================================================================================================
    juce::Font getMultiTabPaneFont() override;
    void drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane &multiTabPane) override;
    
    void drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                        bool isMouseOver, bool isMouseDown) override;
    
    void drawMultiTabPaneTabBackground(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                       bool isMouseOver, bool isMouseDown) override;
    void drawMultiTabPaneTabText(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                 const juce::Label &label, bool isMouseOver, bool isMouseDown) override;
    void drawMultiTabPaneTabCloseButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                        const juce::TextButton &button, bool isMouseOver, bool isMouseDown) override;
    void drawMultiTabPaneTabPinButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                      const juce::ToggleButton &button, bool isMouseOver, bool isMouseDown) override;
};
}
