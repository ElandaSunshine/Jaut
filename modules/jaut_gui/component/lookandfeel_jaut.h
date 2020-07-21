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
    @file   lookandfeel_jaut.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class JAUT_API LookAndFeel_Jaut : public juce::LookAndFeel_V4,
                                  public ContentPane   ::LookAndFeelMethods,
                                  public SplitContainer::LookAndFeelMethods
{
public:
    static LookAndFeel_Jaut& getDefaultLaf()
    {
        static LookAndFeel_Jaut defaultLaf;
        return defaultLaf;
    }
    
    //==================================================================================================================
    LookAndFeel_Jaut();
    
    //==================================================================================================================
    void drawContentPaneBackground(juce::Graphics &g, ContentPane &pane, int width, int height) override;
    
    //==================================================================================================================
    void drawSplitContainerBackground(juce::Graphics &g, SplitContainer &splitContainer,
                                      int width, int height) override;
    
    void drawSplitContainerSplitter(juce::Graphics &g, SplitContainer &splitContainer,
                                    int x, int y, int width, int height) override;
};
}
