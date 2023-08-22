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
    @file   GuiBase.cpp
    @date   09, March 2023

    ===============================================================
 */

#include "GuiBase.h"



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
std::unique_ptr<juce::Component> getMainGuiComponent();
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region GuiBase
//**********************************************************************************************************************
GuiBase::GuiBase(const juce::String &name)
    : DocumentWindow(name,
                     juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                 .findColour(ResizableWindow::backgroundColourId),
                     allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(getMainGuiComponent().release(), true);
    
    #if JUCE_IOS || JUCE_ANDROID
        setFullScreen(true);
    #else
        setResizable(true, true);
        setResizeLimits(426, 240, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        centreWithSize(getWidth(), getHeight());
    #endif
}

void GuiBase::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
//**********************************************************************************************************************
// endregion GuiBase
//**********************************************************************************************************************
// region Main
//======================================================================================================================
class GuiApplication : public juce::JUCEApplication
{
public:
    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<GuiBase>(getApplicationName());
        mainWindow->setVisible(true);
    }

    void shutdown() override
    {
        mainWindow.reset();
    }

    void systemRequestedQuit() override
    {
        quit();
    }
    
    void anotherInstanceStarted(const juce::String&) override {}
    
    //==================================================================================================================
    const juce::String getApplicationName() override
    {
        return APP_NAME;
    }
    
    const juce::String getApplicationVersion() override
    {
        return APP_VERSION;
    }
    
    //==================================================================================================================
    bool moreThanOneInstanceAllowed() override
    {
        return true;
    }

private:
    std::unique_ptr<GuiBase> mainWindow;
};

START_JUCE_APPLICATION(GuiApplication)
//======================================================================================================================
// endregion Main
//**********************************************************************************************************************
