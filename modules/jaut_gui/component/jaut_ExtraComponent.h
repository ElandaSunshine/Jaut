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
    @file   jaut_ExtraComponent.h
    @date   17, March 2023

    ===============================================================
 */

#pragma once

#include <jaut_gui/structure/jaut_SizeStructs.h>
#include <jaut_gui/lookandfeel/jaut_CidNamespace.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/signal/event/jaut_Event.h>

#include <juce_gui_basics/juce_gui_basics.h>



namespace jaut
{
    /**
     *  A juce::Component implementation that allows basic styling of a component,
     *  such as borders margin and padding, and much more.
     */
    class JAUT_API ExtraComponent : public juce::Component,
                                    private juce::ComponentListener
    {
    public:
        /**
         *  The container style options for any component inheriting from this component.<br>
         *  These just concern basic component properties.
         */
        struct ContainerStyle
        {
            /**
             *  The size of the outline of this component.<br>
             *  This will affect the position and size of the content area.
             */
            Thickness<int> borderThickness { 0 };
            
            /**
             *  The space between this component and any other component.<br>
             *  This will essentially make this component smaller.
             */
            Margin<int> margin { 0 };

            /**
             *  The space between border and content area.<br>
             *  This will affect the position and size of the content area.
             */
            Padding<int> padding { 0 };
        };
        
        //==============================================================================================================
        enum ColourId
        {
            ColourBackgroundId = CidnsJaut::getId<16>,
            ColourBorderId     = CidnsJaut::getId<17>
        };
        
        struct LookAndFeelMethods
        {
            /**
             *  Draws the background for this component.
             *  
             *  @param g         The graphics context
             *  @param component The component to draw in
             *  @param colour    The background colour
             */
            virtual void drawExtraComponentBackground(juce::Graphics       &g,
                                                      const ExtraComponent &component,
                                                      juce::Colour         colour) = 0;
            
            /**
             *  Draws the border of this component.
             *  
             *  @param g         The graphics context
             *  @param component The component to draw in
             *  @param colour    The border colour
             */
            virtual void drawExtraComponentBorder(juce::Graphics       &g,
                                                  const ExtraComponent &component,
                                                  juce::Colour         colour) = 0;
        };
        
        //==============================================================================================================
        using StyleChangedHandler         = EventHandler<ExtraComponent&, Distance<int>>;
        using ContentBoundsChangedHandler = EventHandler<ExtraComponent&, juce::Rectangle<int>>;
        
        /**
         *  Dispatched whenever the margin has changed.
         *  
         *  @param component The component the change has occurred for
         *  @param oldValue  The old margin value
         */
        Event<StyleChangedHandler> eventMarginChanged;
        
        /**
         *  Dispatched whenever the border thickness has changed.
         *  
         *  @param component The component the change has occurred for
         *  @param oldValue  The old margin value
         */
        Event<StyleChangedHandler> eventBorderThicknessChanged;
        
        /**
         *  Dispatched whenever the border thickness has changed.
         *  
         *  @param component The component the change has occurred for
         *  @param oldValue  The old margin value
         */
        Event<StyleChangedHandler> eventPaddingChanged;
        
        /**
         *  Dispatched whenever the content bounds have changed.<br>
         *  This might happen during a resize or when any of the style properties were changed.
         *  
         *  @param par1 The component that has had the update
         *  @param par2 The previous bounds before the update
         */
        Event<ContentBoundsChangedHandler> eventContentBoundsChanged;
        
        //==============================================================================================================
        ExtraComponent(const juce::String &name, ContainerStyle style);
        explicit ExtraComponent(const juce::String &name);
        explicit ExtraComponent(ContainerStyle style);
        
        ExtraComponent();
        
        //==============================================================================================================
        void paintOverChildren(juce::Graphics &g) override;
        void paint(juce::Graphics &g) override;
        void resized() final;
        
        //==============================================================================================================
        /**
         *  Gets the bounds of the content area of this styleable component.<br>
         *  This will be the area that was determined after the border and padding has been applied.
         *  
         *  @return The content bounds
         */
        JAUT_NODISCARD
        juce::Rectangle<int> getContentBounds() const noexcept;
        
        /**
         *  Gets the theoretical actual bounds of this component minus the applied margin.
         *  
         *  @return The actual bounds
         */
        JAUT_NODISCARD
        juce::Rectangle<int> getActualBounds() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets the thickness of the border of the component.<br>
         *  This will be the outline of the component on all edges, and will push in the content of this component.
         *  
         *  @return The thickness of the outline/border of this component on all sides
         */
        JAUT_NODISCARD
        Thickness<int> getBorderThickness() const noexcept;
        
        /**
         *  Gets the margin of this component, which will be the distance between this components real bounds and
         *  the adjusted bounds.
         *  
         *  @return The margin on all sides
         */
        JAUT_NODISCARD
        Margin<int> getMargin() const noexcept;
        
        /**
         *  Gets the padding that should be applied to this component.<br>
         *  The padding is the distance between border and content in pixels.
         *  
         *  @return The padding on all sides
         */
        JAUT_NODISCARD
        Padding<int> getPadding() const noexcept;
        
        //==============================================================================================================
        /**
         *  Sets the border thickness and updates the bounds if necessary.
         *  
         *  @param borderThickness The new border thickness
         */
        void setBorderThickness(Thickness<int> borderThickness);
        
        /**
         *  Sets the margin and updates the bounds if necessary.
         *  
         *  @param margin The new margin
         */
        void setMargin(Margin<int> margin);
        
        /**
         *  Sets the content padding and updates the bounds if necessary.
         *  
         *  @param padding The new content padding
         */
        void setPadding(Padding<int> padding);
        
        //==============================================================================================================
        /** 
         *  Gets the current style of this component.
         *  
         *  @return The style options
         */
        JAUT_NODISCARD
        const ContainerStyle& getContainerStyle() const noexcept;
        
        /**
         *  Sets the new style struct and updates the component if needed.
         *  
         *  @param style The new style options
         */
        void setContainerStyle(ContainerStyle style);
        
    protected:
        /**
         *  Paints the content area of this styleable component.<br>
         *  To get the bounds of the content area, you can fetch them with getContentBounds().
         *  
         *  @param g The graphics context
         */
        virtual void paintContents(JAUT_MUNUSED juce::Graphics &g) {}
        
        /**
         *  Resizes the content inside the content area.<br>
         *  To get the bounds of the content area, you can fetch them with getContentBounds().
         */
        virtual void resizeContents() {}
        
        //==============================================================================================================
        virtual void drawBorder(juce::Graphics &g);
        virtual void drawBackground(juce::Graphics &g);
        
    private:
        ContainerStyle       style;
        juce::Rectangle<int> contentBounds;
        juce::Rectangle<int> actualBounds;
        
        bool internalResizeCall { false };
        
        //==============================================================================================================
        void componentMovedOrResized(juce::Component &component, bool wasMoved, bool wasResized) override;
        
        //==============================================================================================================
        void internalResize();
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtraComponent)
        
        //==============================================================================================================
        JAUT_INTERN_CREATE_LAF()
    };
}
