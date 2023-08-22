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
    @file   lookandfeel_jaut.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_gui/component/jaut_ContentPane.h>
#include <jaut_gui/component/jaut_ExtraComponent.h>
#include <jaut_gui/component/jaut_MultiPagePanel.h>
#include <jaut_gui/component/jaut_SplitPanel.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_gui_basics/juce_gui_basics.h>



namespace jaut
{
    //==================================================================================================================
    class JAUT_API LookAndFeel : public juce::LookAndFeel_V4,
                                 public ExtraComponent::LookAndFeelMethods,
                                 public ContentPane::LookAndFeelMethods,
                                 public SplitPanel::LookAndFeelMethods,
                                 public MultiPagePanel::LookAndFeelMethods
                                 // public MultiPagePane::Window::LookAndFeelMethods
    {
    public:
        LookAndFeel() noexcept;
        
        //==============================================================================================================
        void drawExtraComponentBackground(juce::Graphics       &g,
                                          const ExtraComponent &component,
                                          juce::Colour         colour) override;
        void drawExtraComponentBorder(juce::Graphics &g, const ExtraComponent &component, juce::Colour colour) override;
        
        //==============================================================================================================
        void drawContentPaneBackground(juce::Graphics &g, const ContentPane &pane) override;
        void drawContentPaneBorder    (juce::Graphics &g, const ContentPane &pane) override;
        
        //==============================================================================================================
        void drawSplitPanelBackground(juce::Graphics &g, const SplitPanel      &splitPanel) override;
        void drawSplitPanelBorder    (juce::Graphics &g, const SplitPanel      &splitPanel) override;
        void drawSplitPanelSeparator (juce::Graphics &g, const juce::Component &separator) override;
        
        //==============================================================================================================
        void drawMultiPagePanelBackground   (juce::Graphics &g, const MultiPagePanel &panel) = 0;
        void drawMultiPagePanelBorder       (juce::Graphics &g, const MultiPagePanel &panel) = 0;
        void drawMultiPagePanelTabBackground(juce::Graphics                  &g,
                                             const MultiPagePanel::TabButton &tabButton,
                                             bool                            isMouseOver,
                                             bool                            isMouseDown) = 0;
        
        //==============================================================================================================
        //int getWindowStyleFlags() const override;
    };
}
