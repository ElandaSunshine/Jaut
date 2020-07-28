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
    @file   splitcontainer.h
    @date   20, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  A component that contains two components that can be resised with a seperator between those two.
 *  You can pick between two types of orientation, horizontal and vertical, which specify the orientation of
 *  the seperator.
 */
class JAUT_API SplitContainer : public juce::Component
{
public:
    /** The orientation of the seperator. */
    enum class JAUT_API Orientation
    {
        Vertical,
        Horizontal
    };
    
    enum JAUT_API ColourIds
    {
        ColourBackgroundId = nextColourId<17>,
        ColourSeperatorId  = nextColourId<18>
    };
    
    /** The side of the component to be inserted in. */
    enum class JAUT_API ComponentOrder
    {
        LeftOrTop,
        RightOrBottom
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        /**
         *  Draws the background of the SplitContainer.
         *
         *  @param g      The graphics context
         *  @param bounds The local bounds of the SplitContainer
         */
        virtual void drawSplitContainerBackground(juce::Graphics &g, juce::Rectangle<int> bounds) = 0;
        
        /**
         *  Draws the seperator of the SplitContainer.
         *
         *  @param g           The graphics context
         *  @param bounds      The relative bounds of the seperator.
         *  @param orientation The orientation of the seperator
         */
        virtual void drawSplitContainerSeperator(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                 Orientation orientation) = 0;
    };
    
    //==================================================================================================================
    using ComponentChangedHandler = EventHandler<juce::Component*, ComponentOrder>;
    
    /** Dispatched when a component changes on either side. */
    Event<ComponentChangedHandler> ComponentChanged;
    
    //==================================================================================================================
    /**
     *  Constructs a new SplitContainer object with a default horizontal orientation.
     *  @param orientation The orientation to be used
     */
    explicit SplitContainer(Orientation orientation = Orientation::Horizontal) noexcept;
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //==================================================================================================================
    /**
     *  Sets a component to either one side or empties it if nullptr.
     *
     *  @param component The component to be set or null if should be emptied
     *  @param order     The side of the component
     *  @param owned     True if the component should be deleted automatically or false if not
     */
    void setComponent(juce::Component *component, ComponentOrder order, bool owned);
    
    /**
     *  Sets a component to either one side.
     *
     *  @param component The component to be set
     *  @param order     The side of the component
     */
    void setComponent(juce::Component &component, ComponentOrder order);
    
    //==================================================================================================================
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    
    //==================================================================================================================
    void childBoundsChanged(Component *child) override;
    
    //==================================================================================================================
    /**
     *  Sets the thickness of the seperator.
     *  @param thickness The thickness in pixels
     */
    void setSeperatorThickness(int thickness);
    
private:
    struct LockedAxisConstrainer : juce::ComponentBoundsConstrainer
    {
        //==============================================================================================================
        Orientation orientation;
        
        //==============================================================================================================
        explicit LockedAxisConstrainer(Orientation orientation) noexcept
            : orientation(orientation)
        {}
        
        //==============================================================================================================
        void applyBoundsToComponent (Component &component, juce::Rectangle<int> bounds) override
        {
            auto parent_bounds = component.getParentComponent()->getLocalBounds();
            component.setBounds(orientation == Orientation::Horizontal
                     ? bounds.withX(std::clamp(bounds.getX(), 0, parent_bounds.getWidth() - bounds.getWidth())).withY(0)
                     : bounds.withX(0)
                             .withY(std::clamp(bounds.getY(), 0, parent_bounds.getHeight() - bounds.getHeight())));
        }
    };
    
    //==================================================================================================================
    juce::ComponentDragger dragger;
    juce::Component seperator;
    
    LockedAxisConstrainer constrainer;
    
    ContentPane firstComponent;
    ContentPane secondComponent;
    
    juce::Rectangle<int> prevSize;
    
    int thickness { 5 };
    Orientation orientation;
    bool initialised { false };
    
    JAUT_CREATE_LAF()
};
}
