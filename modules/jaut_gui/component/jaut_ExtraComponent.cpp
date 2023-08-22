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
    @file   jaut_ExtraComponent.cpp
    @date   17, March 2023

    ===============================================================
 */

#include <jaut_gui/component/jaut_ExtraComponent.h>

#include <jaut_gui/lookandfeel/jaut_LookAndFeel.h>

#include <jaut_core/define/jaut_DefUtils.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    enum class ResizeAction
    {
        Call,
        BoundsChanged,
        
    };
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region StyleableComponent
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    ExtraComponent::ExtraComponent(const juce::String &parName, ContainerStyle parStyle)
        : juce::Component(parName),
          style          (parStyle)
    {
        JAUT_INTERN_INIT_LAF();
        addComponentListener(this);
    }
    
    ExtraComponent::ExtraComponent(ContainerStyle parStyle)
        : ExtraComponent("", parStyle)
    {}
    
    ExtraComponent::ExtraComponent(const juce::String &parName)
        : ExtraComponent(parName, {})
    {}
    
    ExtraComponent::ExtraComponent()
        : ExtraComponent("", {})
    {}
    
    //==================================================================================================================
    void ExtraComponent::paintOverChildren(juce::Graphics &parG)
    {
        drawBorder(parG);
    }
    
    void ExtraComponent::paint(juce::Graphics &parG)
    {
        drawBackground(parG);
        paintContents(parG);
    }
    
    void ExtraComponent::resized()
    {
        if (!internalResizeCall)
        {
            return;
        }
        
        const juce::Rectangle old_bounds = contentBounds;
        contentBounds = style.borderThickness.trimRectangle(style.padding.trimRectangle(getLocalBounds()));
        
        if (old_bounds != contentBounds)
        {
            resizeContents();
            eventContentBoundsChanged.invoke(*this, old_bounds);
        }
    }
    
    //==================================================================================================================
    juce::Rectangle<int> ExtraComponent::getContentBounds() const noexcept
    {
        return contentBounds;
    }
    
    juce::Rectangle<int> ExtraComponent::getActualBounds() const noexcept
    {
        return actualBounds;
    }
    
    //==================================================================================================================
    Thickness<int> ExtraComponent::getBorderThickness() const noexcept
    {
        return style.borderThickness;
    }
    
    Margin<int> ExtraComponent::getMargin() const noexcept
    {
        return style.margin;
    }
    
    Padding<int> ExtraComponent::getPadding() const noexcept
    {
        return style.padding;
    }
    
    //==================================================================================================================
    void ExtraComponent::setBorderThickness(Thickness<int> parBorderThickness)
    {
        // Can't have negative border size, can ya?
        // Use padding instead, which allows negative values
        jassert(parBorderThickness.left >= 0 && parBorderThickness.top >= 0
                && parBorderThickness.right >= 0 && parBorderThickness.bottom >= 0);

        std::swap(style.borderThickness, parBorderThickness);
        
        if (parBorderThickness != style.borderThickness)
        {
            internalResize();
            eventBorderThicknessChanged.invoke(*this, parBorderThickness);
        }
    }
    
    void ExtraComponent::setMargin(Margin<int> parMargin)
    {
        std::swap(style.margin, parMargin);
        
        if (parMargin != style.margin)
        {
            componentMovedOrResized(*this, false, false);
            eventMarginChanged.invoke(*this, parMargin);
        }
    }
    
    void ExtraComponent::setPadding(Padding<int> parPadding)
    {
        std::swap(style.padding, parPadding);
        
        if (parPadding != style.padding)
        {
            internalResize();
            eventPaddingChanged.invoke(*this, parPadding);
        }
    }
    
    //==================================================================================================================
    const ExtraComponent::ContainerStyle& ExtraComponent::getContainerStyle() const noexcept
    {
        return style;
    }
    
    void ExtraComponent::setContainerStyle(ContainerStyle parStyle)
    {
        std::swap(style, parStyle);
        
        if (parStyle.margin != style.margin)
        {
            setBounds(actualBounds);
        }
        else if (parStyle.borderThickness != style.borderThickness || parStyle.padding != style.padding)
        {
            internalResize();
        }
    }
    
    //==================================================================================================================
    void ExtraComponent::drawBackground(juce::Graphics &parG)
    {
        lookAndFeel->drawExtraComponentBackground(parG, *this, findColour(ColourBorderId, true));
    }
    
    void ExtraComponent::drawBorder(juce::Graphics &parG)
    {
        lookAndFeel->drawExtraComponentBorder(parG, *this, findColour(ColourBackgroundId, true));
    }
    
    //==================================================================================================================
    void ExtraComponent::componentMovedOrResized(juce::Component &component, bool wasMoved, bool wasResized)
    {
        if (&component != this || internalResizeCall)
        {
            return;
        }
        
        const juce::Rectangle<int> pre_bounds = component.getBounds();
        
        if (wasResized)
        {
            actualBounds.setSize(pre_bounds.getWidth(), pre_bounds.getHeight());
        }
        
        if (wasMoved)
        {
            actualBounds.setPosition(pre_bounds.getPosition());
        }
        
        {
            jdscoped juce::ScopedValueSetter(internalResizeCall, true);
            const juce::Rectangle new_bounds = style.margin.trimRectangle(actualBounds);
            
            if (new_bounds != component.getBounds())
            {
                component.setBounds(new_bounds);
            }
            else
            {
                resized();
            }
        }
    }
    
    //==================================================================================================================
    void ExtraComponent::internalResize()
    {
        {
            jdscoped juce::ScopedValueSetter(internalResizeCall, true);
            resized();
        }
        
        repaint();
    }
}
//======================================================================================================================
// endregion StyleableComponent
//**********************************************************************************************************************
//**********************************************************************************************************************
// region LookAndFeel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    void LookAndFeel::drawExtraComponentBackground(juce::Graphics &parG, const jaut::ExtraComponent&,
                                                   juce::Colour parColour)
    {
        parG.setColour(parColour);
        parG.fillAll();
    }
    
    void LookAndFeel::drawExtraComponentBorder(juce::Graphics             &parG,
                                               const jaut::ExtraComponent &parComponent,
                                               juce::Colour               parColour)
    {
        parG.setColour(parColour);
        parG.fillPath(DistanceUtil::borderPathFromThickness(parComponent.getLocalBounds(),
                                                            parComponent.getBorderThickness()));
    }
    
    //==================================================================================================================
    JAUT_INTERN_IMPL_LAF(ExtraComponent)
}
//======================================================================================================================
// endregion LookAndFeel
//**********************************************************************************************************************
