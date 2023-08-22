/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    @file   jaut_ContentPane.cpp
    @date   21, July 2020

    ===============================================================
 */

#pragma once

#include <jaut_gui/component/jaut_ContentPane.h>
#include <jaut_gui/lookandfeel/jaut_LookAndFeel.h>



//**********************************************************************************************************************
// region ContentPane
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    ContentPane::ContentPane(const juce::String &name, jaut::ExtraComponent::ContainerStyle style)
        : ExtraComponent(name, style)
    {
        JAUT_INTERN_INIT_LAF();
    }
    
    ContentPane::ContentPane(jaut::ExtraComponent::ContainerStyle style)
        : ContentPane("", style)
    {}
    
    ContentPane::ContentPane(const juce::String &name)
        : ContentPane(name, {})
    {}
    
    //==================================================================================================================
    void ContentPane::resizeContents()
    {
        if (content)
        {
            content->setBounds(getContentBounds());
        }
    }
    
    //==================================================================================================================
    void ContentPane::setComponent(std::unique_ptr<juce::Component> component)
    {
        (void) updateComponent(ContentPtr(std::move(component)));
    }
    
    bool ContentPane::setComponent(juce::Component *const component)
    {
        return updateComponent({ component, false });
    }
    
    bool ContentPane::setComponent(juce::Component &component)
    {
        return setComponent(&component);
    }
    
    //==================================================================================================================
    juce::Component* ContentPane::getCurrentComponent() noexcept
    {
        return content.get();
    }
    
    const juce::Component* ContentPane::getCurrentComponent() const noexcept
    {
        return content.get();
    }
    
    juce::OptionalScopedPointer<juce::Component> ContentPane::releaseComponent()
    {
        removeAllChildren();
        eventContentChanged.invoke(nullptr);
        return std::move(content);
    }
    
    //==================================================================================================================
    bool ContentPane::isOwned() const noexcept
    {
        return (content && content.willDeleteObject());
    }
    
    //==================================================================================================================
    void ContentPane::drawBackground(juce::Graphics &parG)
    {
        lookAndFeel->drawContentPaneBackground(parG, *this);
    }
    
    void ContentPane::drawBorder(juce::Graphics &parG)
    {
        lookAndFeel->drawContentPaneBorder(parG, *this);
    }
    
    //==================================================================================================================
    bool ContentPane::updateComponent(ContentPtr parContentPtr)
    {
        if (parContentPtr == content)
        {
            return false;
        }
        
        std::swap(content, parContentPtr);
        removeAllChildren();
        
        if (content)
        {
            addAndMakeVisible(content.get());
            resizeContents();
        }
        
        eventContentChanged.invoke(content.get());
        return true;
    }
}
//======================================================================================================================
// endregion ContentPane
//**********************************************************************************************************************
// region LookAndFeel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    void LookAndFeel::drawContentPaneBackground(juce::Graphics &parG, const ContentPane &parPane)
    {
        drawExtraComponentBackground(parG, parPane, parPane.findColour(ContentPane::ColourBackgroundId, true));
    }
    
    void LookAndFeel::drawContentPaneBorder(juce::Graphics &parG, const ContentPane &parPane)
    {
        drawExtraComponentBorder(parG, parPane, parPane.findColour(ContentPane::ColourBorderId, true));
    }
    
    //==================================================================================================================
    JAUT_INTERN_IMPL_LAF(ContentPane)
}
//======================================================================================================================
// endregion LookAndFeel
//**********************************************************************************************************************
