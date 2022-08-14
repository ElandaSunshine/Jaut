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
    @file   pseudocomponent.h
    @date   16, February 2021
    
    ===============================================================
 */

#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

namespace jaut
{
    class JAUT_API PseudoComponent : private juce::Component, public juce::ComponentListener
    {
    public:
        struct JAUT_API Style
        {
            /** The padding between border and content. */
            Padding contentPadding;
    
            /** The thickness of the border. */
            Thickness<int> borderThickness;
        };
    
        //==============================================================================================================
        using VisibilityChangedHandler = TrivialHandler;
        
        /** Dispatched whenever the visibility of the pseudo component changed. */
        Event<VisibilityChangedHandler> VisibilityChanged;
        
        //==============================================================================================================
        PseudoComponent();
        
        //==============================================================================================================
        using Component::getName;
        using Component::setName;
        using Component::getComponentID;
        using Component::setComponentID;
    
        //==============================================================================================================
        void setVisible(bool shouldBeVisible);
        bool isVisible() const noexcept { return attributes[Attribute::Visibility]; }
        bool isShowing() const;
    
        //==============================================================================================================
        using Component::getX;
        using Component::getY;
        using Component::getWidth;
        using Component::getHeight;
        using Component::getRight;
        using Component::getBottom;
        using Component::getPosition;
        using Component::getBounds;
        using Component::getLocalBounds;
        using Component::getBoundsInParent;
    
        //==============================================================================================================
        int                  getScreenX()        const { return getScreenPosition().getX(); }
        int                  getScreenY()        const { return getScreenPosition().getY(); }
        juce::Point<int>     getScreenPosition() const { return localPointToGlobal(juce::Point<int>()); }
        juce::Rectangle<int> getScreenBounds()   const { return localAreaToGlobal (getLocalBounds()); }
        
        juce::Point<int>       getLocalPoint(const Component       *sourceComponent,
                                             juce::Point<int>       pointRelativeToSourceComponent) const;
        juce::Point<float>     getLocalPoint(const Component       *sourceComponent,
                                             juce::Point<float>     pointRelativeToSourceComponent) const;
        juce::Rectangle<int>   getLocalArea (const Component       *sourceComponent,
                                             juce::Rectangle<int>   areaRelativeToSourceComponent)  const;
        juce::Rectangle<float> getLocalArea (const Component       *sourceComponent,
                                             juce::Rectangle<float> areaRelativeToSourceComponent)  const;
        
        juce::Point<int>       localPointToGlobal(juce::Point<int>       localPoint) const;
        juce::Point<float>     localPointToGlobal(juce::Point<float>     localPoint) const;
        juce::Rectangle<int>   localAreaToGlobal (juce::Rectangle<int>   localArea)  const;
        juce::Rectangle<float> localAreaToGlobal (juce::Rectangle<float> localArea)  const;
    
        //==============================================================================================================
        using Component::setTopLeftPosition;
        using Component::setTopRightPosition;
        using Component::setSize;
        using Component::setBounds;
        using Component::setBoundsRelative;
        using Component::setBoundsInset;
        using Component::setBoundsToFit;
        using Component::setCentrePosition;
        using Component::setCentreRelative;
        using Component::centreWithSize;
    
        //==============================================================================================================
        
    private:
        struct Attribute
        {
            enum { Visibility };
        };
        
        struct ComponentInfo
        {
            juce::Component *component;
            std::bitset<1> properties;
        };
        
        //==============================================================================================================
        juce::Component *parent;
        std::vector<ComponentInfo> childs;
        
        std::bitset<1> attributes;
    
        //==============================================================================================================
        void componentVisibilityChanged(Component &component);
    };
}

// region IMPLEMENTATION
/*
PseudoComponent::PseudoComponent()
{}

//==================================================================================================================
void PseudoComponent::setVisible(bool shouldBeVisible)
{
    auto ref = attributes[Attribute::Visibility];

    if (ref == shouldBeVisible)
    {
        return;
    }

    ref = shouldBeVisible;

    if (shouldBeVisible)
    {
        for (auto &child : childs)
        {
            if (child.properties[Attribute::Visibility])
            {
                child.component->setVisible(true);
            }
        }
    }
    else
    {
        for (auto &child : childs)
        {
            child.component->setVisible(false);
        }
    }
    
    VisibilityChanged.invoke();
}

bool PseudoComponent::isShowing() const { return isVisible() && parent && parent->isShowing(); }

//==================================================================================================================
juce::Point<int> PseudoComponent::getLocalPoint(const Component *sourceComponent,
                                                juce::Point<int> pointRelativeToSourceComponent) const
{
    return parent
               ? parent->getLocalPoint(sourceComponent, pointRelativeToSourceComponent).translated(getX(), getY())
               : juce::Point<int>();
}

juce::Point<float> PseudoComponent::getLocalPoint(const Component *sourceComponent,
                                                  juce::Point<float> pointRelativeToSourceComponent) const
{
    return parent
               ? parent->getLocalPoint(sourceComponent, pointRelativeToSourceComponent).translated(getX(), getY())
               : juce::Point<float>();
}

juce::Rectangle<int> PseudoComponent::getLocalArea(const Component *sourceComponent,
                                                   juce::Rectangle<int> areaRelativeToSourceComponent) const
{
    return parent
               ? parent->getLocalArea(sourceComponent, areaRelativeToSourceComponent).translated(getX(), getY())
               : juce::Rectangle<int>();
}

juce::Rectangle<float> PseudoComponent::getLocalArea(const Component *sourceComponent,
                                                     juce::Rectangle<float> areaRelativeToSourceComponent) const
{
    return parent
               ? parent->getLocalArea(sourceComponent, areaRelativeToSourceComponent).translated(getX(), getY())
               : juce::Rectangle<float>();
}

juce::Point<int> PseudoComponent::localPointToGlobal(juce::Point<int> localPoint) const
{
    return parent
               ? parent->localPointToGlobal(localPoint).translated(getX(), getY())
               : juce::Point<int>();
}

juce::Point<float> PseudoComponent::localPointToGlobal(juce::Point<float> localPoint) const
{
    return parent
               ? parent->localPointToGlobal(localPoint).translated(getX(), getY())
               : juce::Point<float>();
}

juce::Rectangle<int> PseudoComponent::localAreaToGlobal(juce::Rectangle<int> localArea) const
{
    return parent
               ? parent->localAreaToGlobal(localArea).translated(getX(), getY())
               : juce::Rectangle<int>();
}

juce::Rectangle<float> PseudoComponent::localAreaToGlobal(juce::Rectangle<float> localArea) const
{
    return parent
               ? parent->localAreaToGlobal(localArea).translated(getX(), getY())
               : juce::Rectangle<float>();
}

//==================================================================================================================
void PseudoComponent::componentVisibilityChanged(juce::Component &component)
{
    auto it = std::find_if(childs.begin(), childs.end(), [&component](juce::Component *comp)
    {
        return comp == &component;
    });

    it->properties[Attribute::Visibility] = component.isVisible();
    
    if (!isVisible())
    {
        component.setVisible(false);
    }
}
//==================================================================================================================
*/
// endregion Implementation

#pragma clang diagnostic pop
