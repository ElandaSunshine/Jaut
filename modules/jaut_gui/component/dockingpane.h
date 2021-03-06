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
    @file   dockingpane.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
    class JAUT_API DockingPane : public juce::Component
    {
    public:
        enum class JAUT_API Anchor
        {
            Left,
            Top,
            Right,
            Bottom
        };
    
        //==============================================================================================================
        explicit DockingPane(jaut::SplitContainer::Style style = {}, jaut::SplitContainer::Options options = {});
        
        //==============================================================================================================
        void dockComponent(juce::OptionalScopedPointer<juce::Component> component, Anchor anchor);
        juce::OptionalScopedPointer<juce::Component> undockComponent(Anchor anchor);
        
        //==============================================================================================================
        void resized() override;
        
    private:
        SplitContainer container;
    };
}
