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
    @file   SplitPane.h
    @date   09, March 2023

    ===============================================================
 */

#pragma once

#include <jaut_gui/component/jaut_SplitPanel.h>
#include <jaut_gui/lookandfeel/jaut_LookAndFeel.h>

#include <juce_gui_basics/juce_gui_basics.h>



class GuiExamples : public juce::Component
{
public:
    GuiExamples();
    ~GuiExamples() override;
    
    //==================================================================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    class ExampleModel;
    
    //==================================================================================================================
    static std::vector<std::unique_ptr<jaut::ExtraComponent>> createExamples();
    
    //==================================================================================================================
    jaut::LookAndFeel style;
    
    std::unique_ptr<ExampleModel> exampleModel;
    juce::ListBox                 exampleList;
    
    juce::PropertyPanel propertyPanel;
    
    juce::Component viewBox;
    
    //==================================================================================================================
    void rowSelected(int row);
    
    //==================================================================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiExamples)
};
