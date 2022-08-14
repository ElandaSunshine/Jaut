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
    @file   dockingpane.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
    class JAUT_API DockingPane : public SplitPane, public OverlayTarget
    {
    public:
        enum class JAUT_API Anchor
        {
            Left,
            Top,
            Right,
            Bottom,
            Fill
        };
    
        //==============================================================================================================
        explicit DockingPane(jaut::SplitPane::Style style = {}, jaut::SplitPane::Options options = {});
        
        //==============================================================================================================
        void dockComponent(juce::OptionalScopedPointer<juce::Component> component, Anchor anchor);
        juce::OptionalScopedPointer<juce::Component> undockComponent(SplitPane::PanelId panelId);
        
    private:
        using SplitPane::resetComponent;
        using SplitPane::setComponent;
        using SplitPane::swapPanes;
        using SplitPane::releaseComponent;
        using SplitPane::getComponent;
        using SplitPane::isOwned;
        using SplitPane::setContentPadding;
        using SplitPane::setBorderThickness;
        using SplitPane::setPanelBorderThickness;
        using SplitPane::setSeperatorThickness;
        using SplitPane::setResizeBehaviour;
        using SplitPane::setCollapseMode;
        using SplitPane::setOrientation;
        using SplitPane::setPanel1Minimum;
        using SplitPane::setPanel2Minimum;
        using SplitPane::setStyle;
        using SplitPane::setOptions;
        using SplitPane::getStyle;
        using SplitPane::getOptions;
        
        //==============================================================================================================
        void onItemDropped(const SourceDetails &details) override {}
        bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override {}
    };
}
