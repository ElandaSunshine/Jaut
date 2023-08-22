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
    @file   components.cpp
    @date   21, July 2020

    ===============================================================
 */

//**********************************************************************************************************************
// region Anonymous
//======================================================================================================================
#include "pseudocomponent.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
namespace
{
    /** The amount of difference on the x-axis between two tabs before swapping them. */
    inline constexpr int Const_TabSwapSensitivity = 25;
    
    //==================================================================================================================
    
    
    //==================================================================================================================
    bool hasPinFlag(const jaut::MultiPagePane::Options &options, jaut::MultiPagePane::PinBehaviour flag)
    {
        return (options.pinnedTabBehaviour & flag) == flag;
    }
    
    
}
//======================================================================================================================
// endregion Anonymous
//**********************************************************************************************************************

namespace jaut
{
    //******************************************************************************************************************
    // region MultiPagePane
    //==================================================================================================================
    //==================================================================================================================
    
    //==================================================================================================================
    //==================================================================================================================
    // endregion MultiPagePane
    //******************************************************************************************************************
    // region DockingPane
    //==================================================================================================================
    DockingPane::DockingPane(jaut::SplitPane::Style style, jaut::SplitPane::Options options)
        : SplitPane(style, options),
          OverlayTarget(*this, OverlayTarget::allBlocks)
    {
        setCollapseMode(SplitPane::CollapseMode::Hide);
    }
    
    //==================================================================================================================
    void DockingPane::dockComponent(juce::OptionalScopedPointer<juce::Component> component, Anchor anchor)
    {
        /*
        if (!component)
        {
            return;
        }
        
        const auto &panel1 = getComponent(SplitPane::PanelId::Panel1);
        
        if (!panel1)
        {
            setComponent(std::move(component), SplitPane::PanelId::Panel1);
        }
        else
        {
            if (panel1 == component.get())
            {
                return;
            }
            
            if (anchor == Anchor::Fill)
            {
                if (auto *target = dynamic_cast<OverlayTarget*>(panel1))
                {
                    auto root_obj = std::make_unique<juce::DynamicObject>();
                    juce::var var = ;
                    
                    const juce::DragAndDropTarget::SourceDetails details {
                        var, component.release(), { 0, 0 }
                    };
                    
                    target->itemDropped(details);
                }
            }
            else if (!getComponent(SplitPane::PanelId::Panel2))
            {
                auto dock_1 = std::make_unique<DockingPane>(getStyle(), getOptions());
                auto dock_2 = std::make_unique<DockingPane>(getStyle(), getOptions());
                dock_1->dockComponent(releaseComponent(SplitPane::PanelId::Panel1), Anchor::Left);
                dock_2->dockComponent(std::move(component),                         Anchor::Left);
    
                setComponent(juce::OptionalScopedPointer<juce::Component>(std::move(dock_1)), PanelId::Panel1);
                setComponent(juce::OptionalScopedPointer<juce::Component>(std::move(dock_2)), PanelId::Panel2);
    
                switch (anchor)
                {
                    case Anchor::Left:
                        setOrientation(SplitPane::Orientation::Vertical);
                        swapPanes();
                        break;
                    case Anchor::Right:
                        setOrientation(SplitPane::Orientation::Vertical);
                        break;
                    case Anchor::Top:
                        setOrientation(SplitPane::Orientation::Horizontal);
                        swapPanes();
                        break;
                    case Anchor::Bottom:
                        setOrientation(SplitPane::Orientation::Horizontal);
                        break;
                    default: break;
                }
            }
        }
         */
    }
    
    juce::OptionalScopedPointer<juce::Component> DockingPane::undockComponent(SplitPane::PanelId panelId)
    {
        return {}; //return splitPane.releaseComponent(panelId);
    }
    
    //==================================================================================================================
    // endregion DockingPane
    //******************************************************************************************************************
    // region LookAndFeel_Jaut
    //==================================================================================================================
    
    
    //==================================================================================================================
    
    
    //==================================================================================================================
    
    
    //==================================================================================================================
    juce::Font LookAndFeel_Jaut::getMultiTabPaneFont()
    {
        return juce::Font().withHeight(13.0f);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane &multiPagePane)
    {
        g.setColour(multiPagePane.findColour(MultiPagePane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBorder(juce::Graphics &g, const MultiPagePane &multiTabPane)
    {
        g.setColour(multiTabPane.findColour(MultiPagePane::ColourBorderId));
        g.fillPath(::getBorderForComponent(multiTabPane.getLocalBounds(), multiTabPane.getStyle().borderThickness));
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                                          bool isMouseOver, bool isMouseDown)
    {
        constexpr float w      = 8.0f;
        constexpr float h      = 5.0f;
        constexpr float half_w = w / 2.0f;
    
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
    
        g.setColour(isMouseOver ? button.findColour(MultiPagePane::ColourTabHighlightId, true)
                                : isMouseDown ? button.findColour(MultiPagePane::ColourTabActiveId, true)
                                              : juce::Colours::transparentBlack);
        g.fillRoundedRectangle(bounds.toFloat(), 2.0f);
    
        juce::Path path_down;
        path_down.addLineSegment({ 0.0f,   0.0f, half_w, h    }, 1.5f);
        path_down.addLineSegment({ half_w, h,    w,      0.0f }, 1.5f);
    
        g.setColour(button.findColour(MultiPagePane::ColourTabTextId, true));
        g.addTransform(juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - half_w,
                                                          bounds.getHeight() / 2.0f - h / 2.0f));
        g.fillPath(path_down);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabBackground(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                         bool isMouseOver, bool)
    {
        constexpr int bottom_line = 2;
    
        const bool is_active      = tabButton.isActive();
        const juce::Colour colour = is_active ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true)
                                              : (isMouseOver
                                                     ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true)
                                                     : tabButton.findColour(MultiPagePane::ColourTabId,          true));
        g.setColour(is_active && isMouseOver ? colour.darker() : colour);
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabText(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                   const juce::Label &label, bool, bool)
    {
        g.setColour(tabButton.findColour(MultiPagePane::ColourTabTextId, true));
        g.setFont(getMultiTabPaneFont());
        g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centredLeft);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabCloseButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                          const juce::TextButton &button, bool isMouseOver, bool)
    {
        constexpr float s =     7.0f;
        constexpr float c = s * 1.8f;
        
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Colour main_colour      = tabButton.findColour(MultiPagePane::ColourTabCloseButtonId, true);
        
        if (isMouseOver)
        {
            g.setColour(main_colour);
            g.fillEllipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f, c, c);
            
            const juce::Colour colour = tabButton.isActive()
                ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true)
                : (isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true)
                               : tabButton.findColour(MultiPagePane::ColourTabId,          true));
            
            g.setColour(colour);
        }
        else
        {
            g.setColour(main_colour);
        }
    
        const float p_comp = isMouseOver / 2.0f;
        const float s_less = s           - p_comp;
        
        juce::Path path_x;
        path_x.addLineSegment({ p_comp, p_comp, s_less, s_less }, 1.0f + p_comp);
        path_x.addLineSegment({ s_less, p_comp, p_comp, s_less }, 1.0f + p_comp);
    
        g.fillPath(path_x, juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - s / 2.0f,
                                                              bounds.getHeight() / 2.0f - s / 2.0f));
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabPinButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                        const juce::ToggleButton &button, bool isMouseOver, bool)
    {
        constexpr float s =     7.0f;
        constexpr float c = s * 1.8f;
    
        const juce::Colour &colour_text     = tabButton.findColour(MultiPagePane::ColourTabTextId, true);
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Rectangle<float> ellipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f,
                                             c, c);
        
        if (tabButton.isActive())
        {
            g.setColour(isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true) : colour_text);
        }
        else
        {
            g.setColour(isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true) : colour_text);
        }
        
        if (tabButton.isPinned())
        {
            g.fillEllipse(ellipse.reduced(2.0f));
        }
        else
        {
            g.drawEllipse(ellipse.reduced(2.0f), 1.5f);
        }
    }
    
    //==================================================================================================================
    int LookAndFeel_Jaut::getWindowStyleFlags() const
    {
        using Flags = juce::ComponentPeer::StyleFlags;
        return Flags::windowHasTitleBar | Flags::windowHasCloseButton;
    }
    
    //==================================================================================================================
    JAUT_IMPL_LAF(ContentPane)
    JAUT_IMPL_LAF(SplitPane)
    JAUT_IMPL_LAF(MultiPagePane)
    JAUT_IMPL_LAF(MultiPagePane::TabButton)
    JAUT_IMPL_LAF(MultiPagePane::Window)
    //==================================================================================================================
    // endregion LookAndFeel_Jaut
    //******************************************************************************************************************
}
