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
        /** The orientation of the panels. */
        enum class JAUT_API Orientation
        {
            /** The panels will be split across the x-axis. */
            Vertical,
    
            /** The panels will be split across the y-axis. */
            Horizontal
        };
        
        enum JAUT_API ColourIds
        {
            ColourBackgroundId = nextColourId<20>,
            ColourSeparatorId  = nextColourId<21>,
            ColourBorderId     = nextColourId<22>
        };
        
        /** The side of the component to be inserted in. */
        enum class JAUT_API PanelId
        {
            Panel1,
            Panel2
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
             *  Draws the border of the SplitContainer.
             *
             *  @param g         The graphics context
             *  @param thickness The border thickness
             */
            virtual void drawSplitContainerBorder(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                  Thickness<int> thickness) = 0;
            
            /**
             *  Draws the seperator of the SplitContainer.
             *
             *  @param g           The graphics context
             *  @param bounds      The relative bounds of the separator.
             *  @param orientation The orientation of the separator
             */
            virtual void drawSplitContainerSeparator(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                     Orientation orientation) = 0;
        };
        
        enum class JAUT_API ResizeBehaviour
        {
            /** Move the separator evenly in relation to its previous position. */
            Centred,
            
            /** First resize the first panel then the second. */
            Panel1,
            
            /** First resize the second panel then the first. */
            Panel2,
            
            /** Resize the panels depending on the side the parent component has been resized at. */
            Directional
        };
        
        /** Specifies the mode of this SplitContainer if one or no panel is set. */
        enum class JAUT_API CollapseMode : int
        {
            /** Act as if both components were present. */
            AlwaysShow,
            
            /** Collapse the empty panel and push the seperator to the end. */
            Collapse,
            
            /** Collapse the empty panel and hide the seperator. */
            Hide
        };
        
        struct JAUT_API Style
        {
            /** The padding between border and content. */
            Padding contentPadding;
        
            /** The thickness of the border. */
            Thickness<int> borderThickness;
            
            /** The thickness of the border of the panels. */
            Thickness<int> panelBorderThickness;
        
            /** The thickness of the separator. */
            int separatorThickness { 5 };
        };
        
        struct JAUT_API Options
        {
            /** The resizing behaviour of the individual panels when the SplitContainer is getting resised. */
            ResizeBehaviour resizeBehaviour { ResizeBehaviour::Centred };
            
            /** Specifies what happens if only one or no panel is set. */
            CollapseMode collapseMode { CollapseMode::AlwaysShow };
            
            /** The orientation of the SplitContainer. */
            Orientation orientation { Orientation::Vertical };
            
            /** The minimum size of the left-hand panel when being resized. */
            int panel1MinimumSize { 30 };
    
            /** The minimum size of the right-hand panel when being resized. */
            int panel2MinimumSize { 30 };
        };
        
        //==============================================================================================================
        using ComponentChangedHandler = EventHandler<juce::Component*, PanelId>;
        
        /** Dispatched when a component changes on either side. */
        Event<ComponentChangedHandler> ComponentChanged;
        
        //==============================================================================================================
        /**
         *  Constructs a new SplitContainer object.
         *  @param style   The initial style
         *  @param options The initial options
         */
        SplitContainer(Style style, Options options);
    
        /**
         *  Constructs a new SplitContainer object.
         *  @param style   The initial style
         */
        explicit SplitContainer(Style style);
    
        /**
         *  Constructs a new SplitContainer object.
         *  @param options The initial options
         */
        explicit SplitContainer(Options options);
    
        /** Constructs a new SplitContainer object. */
        SplitContainer();
        
        //==============================================================================================================
        void paint(juce::Graphics &g) override;
        void paintOverChildren(juce::Graphics &g) override;
        void resized() override;
    
        //==============================================================================================================
        /**
         *  Resets the contained component on the specified side and empties the pane.
         *
         *  @param panelId The side of the component
         *  @return True if the component was successfully removed or false if it was null already
         */
        void resetComponent(PanelId panelId);
        
        /**
         *  Sets a component to either one side.
         *
         *  @param component The component to be set
         *  @param panelId   The side of the component
         *  @param limits    The size limits of the component
         */
        void setComponent(juce::OptionalScopedPointer<juce::Component> component, PanelId panelId);
        
        /**
         *  Removes a component from either side
         *  @param panelId The side of the component
         */
        void setComponent(std::nullptr_t, PanelId order);
        
        /**
         *  Sets a component to either one side.
         *
         *  @param component The component to be set
         *  @param panelId   The side of the component
         *  @param limits    The size limits of the component
         */
        void setComponent(juce::Component &component, PanelId panelId);
    
        //==============================================================================================================
        /**
         *  Gets the component inside the specified panel.
         *
         *  @param panelId The panel
         *  @return The component inside the panel or nullptr if there is no component currently
         */
        juce::Component* getComponent(PanelId panelId) noexcept
        {
            return contentContainer.components[static_cast<int>(panelId)].get();
        }
    
        /**
         *  Gets the component inside the specified panel.
         *
         *  @param panelId The panel
         *  @return The component inside the panel or nullptr if there is no component
         */
        const juce::Component* getComponent(PanelId panelId) const noexcept
        {
            return contentContainer.components[static_cast<int>(panelId)].get();
        }
    
        /**
         *  Gets and releases the component inside the specified panel.
         *
         *  @param panelId The panel
         *  @return A smart pointer to the component inside the panel or nullptr if there is no component
         */
        juce::OptionalScopedPointer<juce::Component> releaseComponent(PanelId panelId)
        {
            contentContainer.containers[static_cast<int>(panelId)].setViewedComponent(nullptr, false);
            return std::move(contentContainer.components[static_cast<int>(panelId)]);
        }
    
        //==============================================================================================================
        /**
         *  Gets whether the component in the specified panel is owned by this SplitContainer or not.
         *  This will return false if there is no component in the panel.
         *
         *  @param panelId The panel
         *  @return True if the component inside the panel is exists and is owned, false if not
         */
        bool isOwned(PanelId panelId) const noexcept
        {
            const auto &comp = contentContainer.components[static_cast<int>(panelId)];
            return comp && comp.willDeleteObject();
        }
        
        //==============================================================================================================
        /**
         *  Sets the content padding of this component.
         *  @param padding The new padding
         */
        void setContentPadding(Padding padding);
        
        /**
         *  Sets the thickness of the component border.
         *  @param thickness The new thickness
         */
        void setBorderThickness(Thickness<int> thickness);
    
        /**
         *  Sets the thickness of the panels borders.
         *  @param thickness The new thickness
         */
        void setPanelBorderThickness(Thickness<int> thickness);
        
        /**
         *  Sets the thickness of the seperator.
         *  @param thickness The thickness in pixels
         */
        void setSeperatorThickness(int thickness);
        
        //==============================================================================================================
        /**
         *  Sets the resize behaviour.
         *  @param newBehaviour The new resize behaviour
         */
        void setResizeBehaviour(ResizeBehaviour resizeBehaviour) noexcept { options.resizeBehaviour = resizeBehaviour; }
        
        /**
         *  Sets the collapse mode.
         *  @param collapseMode The new collapse mode
         */
        void setCollapseMode(CollapseMode collapseMode) noexcept;
        
        /**
         *  Sets the orientation and changes the components arrangement.
         *  @param orientation The new orientation
         */
        void setOrientation(Orientation orientation);
        
        /**
         *  Sets the minimum size of the first panel.
         *  @param minimum The new size
         */
        void setPanel1Minimum(int minimum);
    
        /**
         *  Sets the minimum size of the second panel.
         *  @param minimum The new size
         */
        void setPanel2Minimum(int minimum);
        
        //==============================================================================================================
        /**
         *  Sets the style of this component.
         *  @param style The new style
         */
        void setStyle(Style style);
        
        /**
         *  Gets the currently used style of this component.
         *  @return The current style
         */
        const Style& getStyle() const noexcept { return style; }
    
        /**
         *  Sets the options of this component.
         *  @param options The new options
         */
        void setOptions(Options options);
    
        /**
         *  Gets the currently used options of this component.
         *  @return The current options
         */
        const Options& getOptions() const noexcept { return options; }

    private:
        class LockedAxisConstrainer : public juce::ComponentBoundsConstrainer
        {
        public:
            SplitContainer &container;
            
            //==========================================================================================================
            explicit LockedAxisConstrainer(SplitContainer &parContainer) noexcept : container(parContainer) {}
            
            //==========================================================================================================
            void applyBoundsToComponent(Component &component, juce::Rectangle<int> bounds) override
            {
                auto parent_bounds = component.getParentComponent()->getLocalBounds();
                
                if (container.options.collapseMode != CollapseMode::AlwaysShow)
                {
                    if (!container.contentContainer.components[0].get()
                        || !container.contentContainer.components[1].get())
                    {
                        return;
                    }
                }
                
                if (container.options.orientation == Orientation::Vertical)
                {
                    if (parent_bounds.getWidth() > (container.options.panel1MinimumSize
                                                    + container.options.panel2MinimumSize
                                                    + bounds.getWidth()))
                    {
                        const int x = std::clamp(bounds.getX(), container.options.panel1MinimumSize,
                                                 parent_bounds.getWidth() - bounds.getWidth()
                                                                          - container.options.panel2MinimumSize);
                        component.setBounds(bounds.withX(x).withY(0));
                    }
                }
                else if (parent_bounds.getHeight() > (container.options.panel1MinimumSize
                                                      + container.options.panel2MinimumSize
                                                      + bounds.getHeight()))
                {
                    const int y = std::clamp(bounds.getY(), container.options.panel1MinimumSize,
                                             parent_bounds.getHeight() - bounds.getHeight()
                                                                       - container.options.panel2MinimumSize);
                    component.setBounds(bounds.withX(0).withY(y));
                }
            }
        };
        
        class ContentContainer : public juce::Component
        {
        public:
            SplitContainer &parent;
    
            std::array<juce::Viewport, 2> containers;
            std::array<juce::OptionalScopedPointer<juce::Component>, 2> components;
            
            juce::Component        separator;
            LockedAxisConstrainer  constrainer;
            juce::ComponentDragger dragger;
            juce::Rectangle<int>   prevSize;
            
            bool initialised { false };
            
            //==========================================================================================================
            explicit ContentContainer(SplitContainer &container);
            
            //==========================================================================================================
            void paint(juce::Graphics &g) override;
            void resized() override;
    
            //==========================================================================================================
            void mouseDown(const juce::MouseEvent &event) override;
            void mouseDrag(const juce::MouseEvent &event) override;
            
            //==========================================================================================================
            void childBoundsChanged(Component *child) override;
        };
        
        //==============================================================================================================
        Style            style;
        Options          options;
        ContentContainer contentContainer;
    
        //==============================================================================================================
        JAUT_CREATE_LAF()
    };
}
