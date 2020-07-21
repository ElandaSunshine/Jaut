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
class JAUT_API SplitContainer : public juce::Component
{
public:
    enum class JAUT_API Direction
    {
        Vertical,
        Horizontal
    };
    
    enum JAUT_API ColourIds
    {
        ColourBackgroundId = nextColourId<17>,
        ColourSplitterId   = nextColourId<18>
    };
    
    enum class JAUT_API ComponentOrder
    {
        LeftOrTop,
        RightOrBottom
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        virtual void drawSplitContainerBackground(juce::Graphics &g, SplitContainer &splitContainer,
                                                  int width, int height) = 0;
        
        virtual void drawSplitContainerSplitter(juce::Graphics &g, SplitContainer &splitContainer,
                                                int x, int y, int width, int height) = 0;
    };
    
    //==================================================================================================================
    explicit SplitContainer(Direction direction = Direction::Horizontal) noexcept;
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //==================================================================================================================
    void setComponent(juce::Component *component, ComponentOrder order);
    
    //==================================================================================================================
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    
    //==================================================================================================================
    void childBoundsChanged(Component *child) override;
    
    //==================================================================================================================
    void setSplitterThickness(int thickness);
    
private:
    struct LockedAxisConstrainer : juce::ComponentBoundsConstrainer
    {
        //==============================================================================================================
        Direction direction;
        
        //==============================================================================================================
        explicit LockedAxisConstrainer(Direction direction) noexcept
            : direction(direction)
        {}
        
        //==============================================================================================================
        void applyBoundsToComponent (Component &component, juce::Rectangle<int> bounds) override
        {
            auto parent_bounds = component.getParentComponent()->getLocalBounds();
            component.setBounds(direction == Direction::Horizontal
                     ? bounds.withX(std::clamp(bounds.getX(), 0, parent_bounds.getWidth() - bounds.getWidth())).withY(0)
                     : bounds.withX(0)
                             .withY(std::clamp(bounds.getY(), 0, parent_bounds.getHeight() - bounds.getHeight())));
        }
    };
    
    //==================================================================================================================
    juce::ComponentDragger dragger;
    juce::Component splitter;
    
    LockedAxisConstrainer constrainer;
    
    ContentPane firstComponent;
    ContentPane secondComponent;
    
    juce::Rectangle<int> prevSize;
    
    int thickness { 5 };
    Direction direction;
    bool initialised { false };
    
    JAUT_CREATE_LAF()
};
}
