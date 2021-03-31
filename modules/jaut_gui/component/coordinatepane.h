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
    
    @author Elanda & Sk0bel
    @file   coordinatepane.h
    @date   27, February 2021
    
    ===============================================================
 */

#pragma once

namespace jaut
{
    class JAUT_API CoordinatePane : public juce::Component
    {
    public:
        enum JAUT_API ColourIds
        {
            ColourBackgroundId      = nextColourId<34>,
            ColourBorderId          = nextColourId<35>,
            ColourValueNormalId     = nextColourId<36>,
            ColourValueOutOfRangeId = nextColourId<37>,
            ColourValueLineId       = nextColourId<38>,
            ColourValueLineInterId  = nextColourId<39>,
            ColourValueLineZeroId   = nextColourId<40>,
            ColourLabelId           = nextColourId<41>
        };
        
        struct JAUT_API Style
        {
            /** The padding between border and content. */
            Padding contentPadding;
            
            /** The thickness of the border. */
            Thickness<int> borderThickness;
    
            /** The cursor to be used when the mouse is over the coordinate area. */
            juce::MouseCursor cursorNormal { juce::MouseCursor::CrosshairCursor };
    
            /** The cursor to be used when the mouse is moving the coordinate area. */
            juce::MouseCursor cursorMove { juce::MouseCursor::UpDownLeftRightResizeCursor };
            
            /** The cursor to be used when the mouse is over a coordinate point. */
            juce::MouseCursor cursorEdge { juce::MouseCursor::PointingHandCursor };
        };
        
        struct JAUT_API Options
        {
            /**
             *  The minimum tile size of the different values in pixels.
             *  Once "values * minTileSize" exceeds this' components width, the values will be subdivided.
             *  This only concerns integer ranges.
             */
            int minTileSize { 80 };
            
            /**
             *  The maximum tile size of the different values in pixels.
             *  As long as "values * maxTileSize" lies beneath this' components width, the values will be subdivided.
             *  This only concerns floating point ranges.
             */
            int maxTileSize { 180 };
            
            /** The amount of lines between each full value. */
            int numIntermediateLines { 4 };
            
            /**
             *  If this is true, the coordinate system will automatically scale the minimum and maximum tile
             *  size depending on the reference value of minTileSize and maxTileSize.
             *  If this is false, minTileSize and maxTileSize will be absolute for every size.
             */
            bool autoScale { false };
            
            /**
             *  Determines whether it should be allowed to move the canvas by holding the left mouse button down
             *  and dragging around.
             */
            bool allowDragMove { true };
            
            /** Determines whether it should be allowed to resize the canvas by rotating the mouse wheel up and down. */
            bool allowWheelResize { true };
        };
        
        struct JAUT_API LookAndFeelMethods
        {
            /**
             *  Draws the background of the CoordinatePane.
             *
             *  @param g              The graphics context
             *  @param coordinatePane The coordinate pane
             */
            virtual void drawCoordinatePaneBackground(juce::Graphics &g, const CoordinatePane &coordinatePane) = 0;
    
            /**
             *  Draws the border of the CoordinatePane.
             *
             *  @param g              The graphics context
             *  @param coordinatePane The coordinate pane
             */
            virtual void drawCoordinatePaneBorder(juce::Graphics &g, const CoordinatePane &coordinatePane) = 0;
            
            /**
             *  Draws the labels for the 2 axises.
             *
             *  @param g              The graphics context
             *  @param coordinatePane The coordinate pane
             *  @param bounds         The bounds of the label
             *  @param text           The text of the label
             *  @param isXLabel       Whether this label is the x or y label
             */
            virtual void drawCoordinatePaneLabel(juce::Graphics &g, const CoordinatePane &coordinatePane,
                                                 juce::Rectangle<int> area, juce::String text, bool isXLabel) = 0;
            
            /**
             *  Draws the value and intermediate lines.
             *
             *  @param g              The graphics context
             *  @param coordinatePane The coordinate pane
             *  @param area           The bounds of the coordinate area
             *  @param lines          The list of the lines x or y position
             *  @param zeroLineIndex  The index of the line that represents the zero value
             *  @param intermediate   Whether the lines to be drawn are value or intermediate lines
             *  @param isXAxis        Whether the lines are vertical or horizontal
             */
            virtual void drawCoordinatePaneLines(juce::Graphics &g, const CoordinatePane &coordinatePane,
                                                 juce::Rectangle<int> area, const std::vector<int> &lines,
                                                 int zeroLineIndex, bool intermediate, bool isXAxis) = 0;
    
            /**
             *  Draws the values of the pane
             *
             *  Tip: If you want to draw the 0, or whatever the centre value will be, in a different way
             *  than the other values, you might want to check whether the value is almost equal to zero, which
             *  you can do by utilizing juce::approximatelyEqual(point, 0.0).
             *
             *  @param g              The graphics context
             *  @param coordinatePane The coordinate pane
             *  @param area           The bounds of the coordinate area
             *  @param lines          The list of the lines x or y position
             *  @param values         The list of values to be drawn
             *  @param zeroValIndex   The index of the value that represents the zero value
             *  @param linePos        The x or y position of the line to attach the values to
             *                        If isXAxis is true, this will be the y coordinate and vice-versa
             *  @param isXAxis        Whether the values are vertical or horizontal
             */
            virtual void drawCoordinatePaneValues(juce::Graphics &g, const CoordinatePane &coordinatePane,
                                                  juce::Rectangle<int> area, const std::vector<int> &lines,
                                                  const std::vector<double> &values,
                                                  int zeroValIndex, int linePos, bool isXAxis) = 0;
            
            /**
             *  Gets the font used for drawing the values and labels.
             *  @return The font to be used
             */
            virtual juce::Font getCoordinatePaneFont() = 0;
        };
        
        //==============================================================================================================
        CoordinatePane(Style style, Options options);
        CoordinatePane(Style style);
        CoordinatePane(Options options);
        CoordinatePane();
        
        //==============================================================================================================
        void paint(juce::Graphics &g) override;
        void paintOverChildren(juce::Graphics &g) override;
        void resized() override;
        
        //==============================================================================================================
        void setXLabel(juce::String text) { std::swap(txtLabelX, text); repaint(); }
        void setYLabel(juce::String text) { std::swap(txtLabelY, text); repaint(); }
        
        //==============================================================================================================
        void setXRange(juce::Range<double> range);
        void setYRange(juce::Range<double> range);
        
        //==============================================================================================================
        void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
        void mouseDrag     (const juce::MouseEvent&)                                 override;
    
        //==============================================================================================================
        void mouseDown(const juce::MouseEvent&) override;
        void mouseUp  (const juce::MouseEvent&) override;
        void mouseMove(const juce::MouseEvent&) override;
        
        //==============================================================================================================
        double getXValueAt(int x);
        double getYValueAt(int y);
    
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
        template<bool Vertical>
        class ValueStrip
        {
        public:
            juce::Range<double> range { -1.0, 1.0 };
            juce::Rectangle<int> area;
            std::vector<double> points;
            
            double tickDistance { 0.0 };
            double tail { 0.0 };
            double head { 0.0 };
            double mult { 1.0 };
            
            int centreIndex { -1 };
            int zeroLinePos {  0 };
            
            //==========================================================================================================
            explicit ValueStrip(CoordinatePane&);
            
            //==========================================================================================================
            bool setRange (juce::Range<double> );
            bool setBounds(juce::Rectangle<int>);
            void draw     (juce::Graphics&, int);
            
            //==========================================================================================================
            int getTickStart()   const noexcept;

        private:
            CoordinatePane &pane;
            
            //==========================================================================================================
            void calculateValues();
            int  calculateZeroLinePos() const;
        };
        
        struct Attributes
        {
            enum { Initialised, Dragging };
        };
        
        //==============================================================================================================
        const juce::MouseCursor defaultCursor;
        
        ValueStrip<false> stripX;
        ValueStrip<true>  stripY;
        
        juce::Point<int>  prevPos;
        juce::MouseCursor prevCursor;
        
        Style   style;
        Options options;
        
        juce::String txtLabelX;
        juce::String txtLabelY;
        
        std::bitset<8> attributes;
        
        //==============================================================================================================
        void drawLabels(juce::Graphics&, const juce::Rectangle<int>&);
    
        //==============================================================================================================
        JAUT_CREATE_LAF()
    };
}
