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
    @file   components.cpp
    @date   21, July 2020
    
    ===============================================================
 */

//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
void setComponent(juce::Component &component, jaut::ContentPane &pane)
{
    pane.resetComponent(component);
}

void unsetComponent(jaut::ContentPane &pane)
{
    pane.resetComponent();
}
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************

namespace jaut
{
//**********************************************************************************************************************
// region ContentPane
//======================================================================================================================
ContentPane::ContentPane(juce::Component &content) noexcept
    : content(content)
{}

ContentPane::ContentPane(juce::Component *content, bool owned)
    : content(content, owned)
{}

//======================================================================================================================
void ContentPane::paint(juce::Graphics &g)
{
    if (!content)
    {
        lookAndFeel->drawContentPaneBackground(g, *this, getWidth(), getHeight());
    }
}

void ContentPane::resized()
{
    if (content)
    {
        content->setBounds(getLocalBounds());
    }
}

//======================================================================================================================
void ContentPane::resetComponent(juce::Component &cnt)
{
    content.set(&cnt, false);
}

void ContentPane::resetComponent(juce::Component *cnt, bool owned)
{
    content.set(cnt, owned);
    
    if (cnt)
    {
        addAndMakeVisible(cnt);
    }
    else
    {
        removeAllChildren();
    }
    
    resized();
}
//======================================================================================================================
// endregion ContentPane
//**********************************************************************************************************************
// region SplitContainer
//======================================================================================================================
SplitContainer::SplitContainer(SplitContainer::Direction direction) noexcept
    : constrainer(direction), direction(direction)
{
    addAndMakeVisible(firstComponent);
    addAndMakeVisible(secondComponent);
    addAndMakeVisible(splitter);
    splitter.addMouseListener(this, false);
}

//======================================================================================================================
void SplitContainer::paint(juce::Graphics &g)
{
    lookAndFeel->drawSplitContainerBackground(g, *this, getWidth(), getHeight());
    lookAndFeel->drawSplitContainerSplitter  (g, *this, splitter.getX(), splitter.getY(),
                                              splitter.getWidth(), splitter.getHeight());
}

void SplitContainer::resized()
{
    if (getLocalBounds().getWidth() == 0 && getLocalBounds().getHeight() == 0)
    {
        return;
    }
    
    if (!initialised)
    {
        const float h_thickness = static_cast<float>(thickness) / 2.0f;
    
        if (direction == Direction::Horizontal)
        {
            splitter.setBounds(std::ceil(static_cast<float>(getWidth()) / 2.0f - h_thickness), 0,
                               thickness, getHeight());
        }
        else
        {
            splitter.setBounds(0, std::ceil(static_cast<float>(getHeight()) / 2.0f - h_thickness),
                               getWidth(), thickness);
        }
    
        initialised = true;
    }
    else
    {
        if (direction == Direction::Horizontal)
        {
            const int new_x = static_cast<int>(std::ceil(static_cast<float>(getWidth() - prevSize.getWidth()) / 2.0f));
            constrainer.setBoundsForComponent(&splitter, { splitter.getX() + new_x, 0, thickness, getHeight() },
                                              false, false, false, false);
        }
        else
        {
            const int new_y = static_cast<int>(std::ceil(static_cast<float>(getHeight() - prevSize.getHeight())
                                                         / 2.0f));
            constrainer.setBoundsForComponent(&splitter, { 0, splitter.getY() + new_y, getWidth(), thickness },
                                              false, false, false, false);
        }
    }
    
    prevSize = getLocalBounds();
}

//======================================================================================================================
void SplitContainer::setComponent(juce::Component *component, ComponentOrder order)
{
    auto &pane = order == ComponentOrder::LeftOrTop ? firstComponent : secondComponent;
    pane.resetComponent(component, false);
}

//======================================================================================================================
void SplitContainer::mouseDown(const juce::MouseEvent &event)
{
    dragger.startDraggingComponent(&splitter, event);
}

void SplitContainer::mouseDrag(const juce::MouseEvent &event)
{
    dragger.dragComponent(&splitter, event, &constrainer);
}

//======================================================================================================================
void SplitContainer::childBoundsChanged(juce::Component *child)
{
    if (child == &splitter)
    {
        if (direction == Direction::Horizontal)
        {
            firstComponent .setBounds(0, 0, splitter.getX(), getHeight());
            secondComponent.setBounds(splitter.getRight(), 0, getWidth() - splitter.getRight(), getHeight());
        }
        else
        {
            firstComponent .setBounds(0, 0, getWidth(), splitter.getY());
            secondComponent.setBounds(0, splitter.getBottom(), getWidth(), getHeight() - splitter.getBottom());
        }
    }
}

//======================================================================================================================
void SplitContainer::setSplitterThickness(int val)
{
    if (const int prev = std::exchange(thickness, val); prev != val)
    {
        const int  diff = static_cast<int>(std::ceil(static_cast<float>(val - prev) / 2.0f));
        const auto rect = splitter.getBounds();
        
        if (direction == Direction::Horizontal)
        {
            const int new_x = splitter.getX() - diff;
            splitter.setBounds(rect.withWidth(val).withX(new_x));
        }
        else
        {
            const int new_y = splitter.getY() - diff;
            splitter.setBounds(rect.withHeight(val).withY(new_y));
        }
    }
}
//======================================================================================================================
// endregion SplitContainer
//**********************************************************************************************************************
// region LookAndFeel_Jaut
//======================================================================================================================
LookAndFeel_Jaut::LookAndFeel_Jaut()
{
    // ContentPane
    setColour(ContentPane::ColourBackgroundId, DefaultColours::Colour_Background);
    
    // SplitContainer
    setColour(SplitContainer::ColourBackgroundId, DefaultColours::Colour_Background);
    setColour(SplitContainer::ColourSplitterId,   DefaultColours::Colour_Foreground);
}

//======================================================================================================================
void LookAndFeel_Jaut::drawContentPaneBackground(juce::Graphics &g, ContentPane&, int, int)
{
    g.setColour(findColour(ContentPane::ColourBackgroundId));
    g.fillAll();
}

//======================================================================================================================
void LookAndFeel_Jaut::drawSplitContainerBackground(juce::Graphics &g, SplitContainer&, int, int)
{
    g.setColour(findColour(SplitContainer::ColourBackgroundId));
    g.fillAll();
}

void LookAndFeel_Jaut::drawSplitContainerSplitter(juce::Graphics &g, SplitContainer&, int x, int y, int w, int h)
{
    g.setColour(findColour(SplitContainer::ColourSplitterId));
    g.fillRect(x, y, w, h);
}

//======================================================================================================================
JAUT_IMPL_LAF(ContentPane)
JAUT_IMPL_LAF(SplitContainer)
//======================================================================================================================
// endregion LookAndFeel_Jaut
//**********************************************************************************************************************
}
