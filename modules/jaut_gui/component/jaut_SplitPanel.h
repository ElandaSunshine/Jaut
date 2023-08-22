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
    @file   jaut_SplitPanel.h
    @date   20, July 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_gui/jaut_gui_define.h>
#include <jaut_gui/component/jaut_ContentPane.h>
#include <jaut_gui/component/jaut_ExtraComponent.h>
#include <jaut_gui/lookandfeel/jaut_CidNamespace.h>
#include <jaut_gui/structure/jaut_SizeStructs.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/signal/event/jaut_Event.h>

#include <juce_gui_basics/juce_gui_basics.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  A component that contains two components that can be resized with a separator in-between.<br>
     *  You can pick between two types of orientation, horizontal and vertical, which specify the orientation of
     *  the separator.
     */
    class JAUT_API SplitPanel : public ExtraComponent
    {
    public:
        /** The orientation of the panes. */
        enum class Orientation
        {
            Vertical,
            Horizontal
        };
        
        /** The pane to target. */
        enum class PaneId
        {
            None,
            Pane1,
            Pane2
        };
        
        //==============================================================================================================
        enum ColourId
        {
            ColourBackgroundId = CidnsJaut::getId < 20 >,
            ColourBorderId     = CidnsJaut::getId < 21 >,
            ColourSeparatorId  = CidnsJaut::getId < 22 >,
        };
        
        struct LookAndFeelMethods
        {
            /**
             *  Draws the background of the SplitPane.
             *  
             *  @param g     The graphics context
             *  @param panel The SplitPanel
             */
            virtual void drawSplitPanelBackground(juce::Graphics &g, const SplitPanel &panel) = 0;
            
            /**
             *  Draws the border of the SplitPanel.
             *  
             *  @param g     The graphics context
             *  @param panel The SplitPanel
             */
            virtual void drawSplitPanelBorder(juce::Graphics &g, const SplitPanel &panel) = 0;
            
            /**
             *  Draws the separator of the SplitPane.
             *  
             *  @param g         The graphics context
             *  @param separator The separator component
             */
            virtual void drawSplitPanelSeparator(juce::Graphics &g, const juce::Component &separator) = 0;
        };
        
        //==============================================================================================================
        struct Style
        {
            /** The thickness of the separator. */
            int separatorThickness = 5;
        };
        
        /** The style options for constructing this panel. */
        struct InitStyle : public Style, public ContainerStyle {};
        
        struct Options
        {
            /**
             *  The minimum size of the first pane when being resized.
             *  <br><br>
             *  Set this to 0 to allow any size.
             */
            Size<int> minSizePane1 = 0;
            
            /**
             *  The minimum size of the second pane when being resized.
             *  <br><br>
             *  Set this to 0 to allow any size.
             */
            Size<int> minSizePane2 = 0;
            
            /** The orientation of the SplitPane. */
            Orientation orientation = Orientation::Vertical;
            
            /**
             *  Determines the pane that will get the extra space when resizing the content area.<br>
             *  A value of 0.0 means pane 2 will get all the new area, 1.0 means pane 1 will get all the new space.<br>
             *  Anything in-between will distribute the change for both panes depending on the pivot of 0.5 which will
             *  evenly distribute the size for both panels. (<separator position> += (+/-<size diff> * <distribution>))
             */
            double resizeDistributionFactor = 0;
            
            /** 
             *  Whether pane 1 should be collapsed or not.<br>
             *  If a pane is collapsed, only the other will be shown as sole component, and if both are collapsed you
             *  will just have an empty area.
             */
            bool collapsePane1 = false;
            
            /** 
             *  Whether pane 2 should be collapsed or not.<br>
             *  If a pane is collapsed, only the other will be shown as sole component, and if both are collapsed you
             *  will just have an empty area.
             */
            bool collapsePane2 = false;
        };
        
        //==============================================================================================================
        using ComponentChangedHandler = EventHandler<const juce::Component&, PaneId>;
        using SeparatorMovedHandler   = EventHandler<int, int>;
        
        /** 
         *  Dispatched when a component changes in either of the two panes.
         *  
         *  @param par1 The pane that changed
         *  @param par2 The pane type
         */
        Event<ComponentChangedHandler> eventPaneChanged;
        
        /**
         *  Dispatched whenever the separator moved to a different position.
         *  
         *  @param par1 The new position of the separator, relative to the content area
         *  @param par2 The old position of the separator, relative to the content area
         */
        Event<SeparatorMovedHandler> eventSeparatorMoved;
        
        //==============================================================================================================
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param name    The name of the component
         *  @param style   The initial style
         *  @param options The initial options
         */
        SplitPanel(const juce::String &name, InitStyle style, Options options);
        
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param style   The initial style
         *  @param options The initial options
         */
        SplitPanel(InitStyle style, Options options);
        
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param name  The name of the component
         *  @param style The initial style
         */
        explicit SplitPanel(const juce::String &name, InitStyle style);
        
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param style The initial style
         */
        explicit SplitPanel(InitStyle style);
        
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param name    The name of the component
         *  @param options The initial options
         */
        explicit SplitPanel(const juce::String &name, Options options);
        
        /**
         *  Constructs a new SplitPanel object.
         *  
         *  @param options The initial options
         */
        explicit SplitPanel(Options options);
        
        /** 
         *  Constructs a new default SplitPanel object.
         *  
         *  @param name The name of the component
         */
        explicit SplitPanel(const juce::String &name);
        
        /** Constructs a new default SplitPanel object. */
        SplitPanel();
        
        ~SplitPanel() override;
        
        //==============================================================================================================
        void paintContents(juce::Graphics &g) override;
        void resizeContents() override;
        
        //==============================================================================================================
        /**
         *  Gets the current separator position relative to its content bounds.
         *  
         *  @return The pixel starting from the left/top
         */
        JAUT_NODISCARD
        int getSeparatorPos() const noexcept;
        
        /**
         *  Gets the current separator position relative to its content bounds in normalised coordinates,
         *  where 0.0 is the start of the content and 1.0 is the end.
         *  
         *  @return The normalised position of the separator
         */
        JAUT_NODISCARD
        double getSeparatorPosNormalised() const noexcept;
        
        //==============================================================================================================
        /**
         *  Set the position of the separator relative to the content area.
         *  
         *  @param pos       The new position
         *  @param totalArea If true, the value of "pos" represents a position inside the whole content area,
         *                   otherwise just the portion between the minimum of both panes
         */
        void setSeparatorPos(int pos);
        
        /**
         *  Set the position of the separator relative to the two panel's minimum requirements.<br>
         *  Values range from 0.0 for full left/top to 1.0 for full right/bottom,
         *  the position will be rounded to snap to the nearest available pixel.
         *
         *  @param normalisedPos The normalised position to move the separator to
         */
        void setSeparatorPosNormalised(double normalisedPos);
        
        //==============================================================================================================
        /**
         *  Moves the separator the specified amount.
         *  
         *  @param value The amount to move the resizer
         */
        void moveSeparator(int amount);
        
        /**
         *  Moves the separator the specified amount relative to the entire width of the container.<br>
         *  Values range from -1.0 to 1.0.<br>
         *  The calculated position will be rounded to snap to the nearest available pixel.
         *
         *  @param normalisedAmount The normalised amount to move the separator
         */
        void moveSeparatorNormalised(double normalisedAmount);
        
        //==============================================================================================================
        /**
         *  Gets a reference to the left or top pane.
         *  @return The pane component
         */
        JAUT_NODISCARD
        ContentPane& getPane1() noexcept;
        
        /**
         *  Gets a reference to the left or top pane.
         *  @return The pane component
         */
        JAUT_NODISCARD
        const ContentPane& getPane1() const noexcept;
        
        /**
         *  Gets a reference to the right or bottom pane.
         *  @return The pane component
         */
        JAUT_NODISCARD
        ContentPane& getPane2() noexcept;
        
        /**
         *  Gets a reference to the right or bottom pane.
         *  @return The pane component
         */
        JAUT_NODISCARD
        const ContentPane& getPane2() const noexcept;
        
        //==============================================================================================================
        /** @see Style::separatorThickness */
        JAUT_NODISCARD
        int getSeparatorThickness() const noexcept;
        
        //==============================================================================================================
        /** @see Options::minSizePane1 */
        JAUT_NODISCARD
        Size<int> getMinSizePane1() const noexcept;
        
        /** @see Options::minSizePane2 */
        JAUT_NODISCARD
        Size<int> getMinSizePane2() const noexcept;
        
        /** @see Options::orientation */
        JAUT_NODISCARD
        Orientation getOrientation() const noexcept;
        
        /** @see Options::resizeDistributionFactor */
        JAUT_NODISCARD
        double getResizeDistributionFactor() const noexcept;
        
        /** @see Options::collapsePane1 */
        JAUT_NODISCARD
        bool getIsPane1Collapsed() const noexcept;
        
        /** @see Options::collapsePane2 */
        JAUT_NODISCARD
        bool getIsPane2Collapsed() const noexcept;
        
        //==============================================================================================================
        /** @see Style::separatorThickness */
        void setSeparatorThickness(int thickness);
        
        //==============================================================================================================
        /** @see Options::minSizePane1 */
        void setMinSizePane1(Size<int> minSize);
        
        /** @see Options::minSizePane2 */
        void setMinSizePane2(Size<int> minSize);
        
        /** @see Options::orientation */
        void setOrientation(Orientation orientation);
        
        /** @see Options::resizeDistributionFactor */
        void setResizeDistributionFactor(double resizeDistributionFactor) noexcept;
        
        /** @see Options::collapsePane1 */
        void setPane1Collapsed(bool shouldCollapse);
        
        /** @see Options::collapsePane2 */
        void setPane2Collapsed(bool shouldCollapse);
        
        //==============================================================================================================
        /**
         *  Gets the currently used style of this component.
         *  
         *  @return The current style
         */
        JAUT_NODISCARD
        const Style& getStyle() const noexcept;
        
        /**
         *  Gets the currently used options of this component.
         *  
         *  @return The current options
         */
        JAUT_NODISCARD
        const Options& getOptions() const noexcept;
        
        //==============================================================================================================
        void childBoundsChanged(juce::Component *child) override;
        
    private:
        class Pimpl;
        
        //==============================================================================================================
        Style   style;
        Options options;
        
        std::unique_ptr<Pimpl> pimpl;
        
        //==============================================================================================================
        void drawBackground(juce::Graphics &g) override;
        void drawBorder    (juce::Graphics &g) override;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplitPanel)
        
        //==============================================================================================================
        JAUT_INTERN_CREATE_LAF()
    };
}
