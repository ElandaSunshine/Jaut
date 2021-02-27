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
    class CoordinatePane : public juce::Component
    {
    public:
        CoordinatePane();
        
        //==============================================================================================================
        void paint(juce::Graphics &g) override;
        
        void resized() override;
        
        //==============================================================================================================
        void setXLabel(const juce::String &text)
        { labelX.setText(text, juce::NotificationType::dontSendNotification); }
        
        void setYLabel(const juce::String &text)
        { labelY.setText(text, juce::NotificationType::dontSendNotification); }
        
        //==============================================================================================================
        void setXRange(juce::Range<double> range);
        
        void setYRange(juce::Range<double> range);
        
        //==============================================================================================================
        void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;
        
        void mouseDown(const juce::MouseEvent &event) override;
        
        void mouseUp(const juce::MouseEvent &event) override;
        
        void mouseDrag(const juce::MouseEvent &event) override;
        
        //==============================================================================================================
        double getXValueAt(int x);
        
        double getYValueAt(int y);
    
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
            
            //==========================================================================================================
            ValueStrip();
            
            //==========================================================================================================
            bool setRange(juce::Range<double>);
            
            bool setBounds(juce::Rectangle<int>);
            
            void draw(juce::Graphics &, int);
            
            //==========================================================================================================
            int getTickStart() const noexcept;
            
            int getZeroLinePos() const;
        };
        
        //==============================================================================================================
        juce::Label labelX;
        juce::Label labelY;
        
        ValueStrip<false> stripX;
        ValueStrip<true>  stripY;
        
        juce::Point<int>  prevPos;
        juce::MouseCursor prevCursor;
        
        juce::Font font_text { 11.0f };
        
        //==============================================================================================================
        void updateLabelPosition();
    };
}
