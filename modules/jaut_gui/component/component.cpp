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
// region Anonymous
//======================================================================================================================
#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
namespace
{
    /** The amount of difference on the x-axis between two tabs before swapping them. */
    inline constexpr int Const_TabSwapSensitivity = 25;
    
    //==================================================================================================================
    enum class SplitContainerSizeState { Full, Semi, Small };
    
    //==================================================================================================================
    bool hasPinFlag(const jaut::MultiPagePane::Options &options, jaut::MultiPagePane::PinBehaviour flag)
    {
        return (options.pinnedTabBehaviour & flag) == flag;
    }
    
    void setPositionOf(juce::Component &comp, int value, bool vertical)
    {
        if (!vertical)
        {
            if (value == comp.getX())
            {
                comp.getParentComponent()->childBoundsChanged(&comp);
                return;
            }
        
            comp.setTopLeftPosition(value, 0);
        }
        else
        {
            if (value == comp.getY())
            {
                comp.getParentComponent()->childBoundsChanged(&comp);
                return;
            }
            
            comp.setTopLeftPosition(0, value);
        }
    }
    
    constexpr SplitContainerSizeState getSizeState(int size, int min1, int min2, int thickness) noexcept
    {
        if (size >= (min1 + min2 + thickness))
        {
            return SplitContainerSizeState::Full;
        }
        else if (size >= (std::min(min1, min2) * 2 + thickness))
        {
            return SplitContainerSizeState::Semi;
        }
        
        return SplitContainerSizeState::Small;
    }
    
    constexpr jaut::SplitPane::ResizeBehaviour getResizeBehaviour(jaut::SplitPane::ResizeBehaviour behaviour,
                                                                  int prevXY, int xy, int prevWidthHeight,
                                                                  int widthHeight) noexcept
    {
        if (behaviour != jaut::SplitPane::ResizeBehaviour::Directional)
        {
            return behaviour;
        }
        
        if (prevXY != xy && (prevXY + prevWidthHeight) != (xy + widthHeight))
        {
            return jaut::SplitPane::ResizeBehaviour::Centred;
        }
        else if (prevXY == xy)
        {
            return jaut::SplitPane::ResizeBehaviour::Panel2;
        }
        
        return jaut::SplitPane::ResizeBehaviour::Panel1;
    }
    
    void resizeSplitContainer(jaut::SplitPane::ResizeBehaviour behaviour, juce::Component &comp,
                              int prevWidthHeight, int prevXY, int xy, int widthHeight, int heightWidth,
                              int separatorXY, int sepHeightWidth, int sepRightBottom, int prevSavedXY,
                              int min1, int min2, int sepThickness, bool withLast, bool vertical)
    {
        if (sepHeightWidth != heightWidth)
        {
            if (!vertical)
            {
                comp.setSize(sepThickness, heightWidth);
            }
            else
            {
                comp.setSize(heightWidth, sepThickness);
            }
        }
        
        const int top_max = widthHeight - (min2 + sepThickness);
    
        const SplitContainerSizeState prev_state = getSizeState(prevWidthHeight, min1, min2, sepThickness);
        const SplitContainerSizeState this_state = getSizeState(widthHeight,     min1, min2, sepThickness);
        
        if (this_state == SplitContainerSizeState::Full)
        {
            if (behaviour == jaut::SplitPane::ResizeBehaviour::Centred)
            {
                if (prev_state == SplitContainerSizeState::Full || prev_state == SplitContainerSizeState::Semi)
                {
                    const int value = std::round((separatorXY / (prevWidthHeight / 100.0)) * (widthHeight / 100.0));
                    setPositionOf(comp, std::clamp(value, min1, top_max), vertical);
                }
                else
                {
                    const int value = std::round((widthHeight / 2.0) - (sepThickness / 2.0f));
                    setPositionOf(comp, std::clamp(value, min1, top_max), vertical);
                }
            }
            else if (behaviour == jaut::SplitPane::ResizeBehaviour::Panel2)
            {
                const int max_pos = widthHeight - sepThickness - prevSavedXY;
                const int size_2  = widthHeight - sepThickness - separatorXY;
                const int diff    = widthHeight - prevWidthHeight;
    
                if (prev_state == SplitContainerSizeState::Semi || prev_state == SplitContainerSizeState::Small)
                {
                    setPositionOf(comp, withLast ? std::min(prevSavedXY, top_max) : min1, vertical);
                }
                else if (diff < 0)
                {
                    if (size_2 >= min2)
                    {
                        comp.getParentComponent()->childBoundsChanged(&comp);
                    }
                    else
                    {
                        setPositionOf(comp, top_max, vertical);
                    }
                }
                else
                {
                    if (withLast && (separatorXY < prevSavedXY))
                    {
                        setPositionOf(comp, std::min(prevSavedXY, separatorXY + diff), vertical);
                        
                        if (separatorXY == prevSavedXY)
                        {
                            comp.getParentComponent()->childBoundsChanged(&comp);
                        }
                    }
                    else
                    {
                        comp.getParentComponent()->childBoundsChanged(&comp);
                    }
                }
            }
            else
            {
                const int max_pos = widthHeight - sepThickness - prevSavedXY;
                const int size_2  = widthHeight - sepThickness - separatorXY;
                const int diff    = widthHeight - prevWidthHeight;
                
                if (prev_state == SplitContainerSizeState::Semi || prev_state == SplitContainerSizeState::Small)
                {
                    setPositionOf(comp, withLast ? std::max(min1, max_pos) : top_max, vertical);
                }
                else if (diff < 0)
                {
                    const int min_pos = separatorXY + diff;
                    
                    if (min_pos >= min1)
                    {
                        setPositionOf(comp, min_pos, vertical);
                    }
                    else
                    {
                        setPositionOf(comp, min1, vertical);
    
                        if (separatorXY == min1)
                        {
                            comp.getParentComponent()->childBoundsChanged(&comp);
                        }
                    }
                }
                else
                {
                    if (withLast && ((size_2 - diff) < prevSavedXY))
                    {
                        setPositionOf(comp, std::max(max_pos, min1), vertical);
                        
                        if (separatorXY == min1)
                        {
                            comp.getParentComponent()->childBoundsChanged(&comp);
                        }
                    }
                    else
                    {
                        setPositionOf(comp, separatorXY + diff, vertical);
                    }
                }
            }
        }
        else if (this_state == SplitContainerSizeState::Semi)
        {
            if (min1 > min2)
            {
                setPositionOf(comp, top_max, vertical);
            }
            else
            {
                comp.getParentComponent()->childBoundsChanged(&comp);
            }
        }
        else
        {
            const int value = std::round((widthHeight / 2.0) - (sepThickness / 2.0f));
            setPositionOf(comp, value, vertical);
        }
    }
    
    auto getBorderForComponent(juce::Rectangle<int> bounds, jaut::Thickness<int> thickness)
    {
        juce::Path path;
        path.addRectangle(bounds.withWidth (thickness.left));
        path.addRectangle(bounds.withHeight(thickness.top));
        path.addRectangle(bounds.withWidth(thickness.right).withX(bounds.getX() + bounds.getWidth() - thickness.right));
        path.addRectangle(bounds.withHeight(thickness.bottom)
                                .withY(bounds.getY() + bounds.getHeight() - thickness.bottom));
        
        return path;
    }
    
    juce::uint32 getArgbFrom(juce::LookAndFeel_V4 *laf, juce::LookAndFeel_V4::ColourScheme::UIColour colour) noexcept
    {
        return laf->getCurrentColourScheme().getUIColour(colour).getARGB();
    }
    
    //==================================================================================================================
    // CoordinatePane
    //==================================================================================================================
    inline constexpr double zeroPivot   = 1e-7;
    inline constexpr int    minTileSize = 90;
    inline constexpr int    maxTileSize = 180;
    inline constexpr int    textPadding = 4;
    
    //==================================================================================================================
    std::vector<double> getPoints(juce::Range<double> range, int size, double &resMult)
    {
        std::vector<double> points;
        const double length = range.getLength();
        
        if ((length * minTileSize) > size)
        {
            int base = 1;
            int mult = 1;
            
            for (;;)
            {
                if (((length / base) * minTileSize) <= size)
                {
                    break;
                }
                else if (((length / (base * 2)) * minTileSize) <= size)
                {
                    mult = 2;
                    break;
                }
                else if (((length / (base * 5)) * minTileSize) <= size)
                {
                    mult = 5;
                    break;
                }
                else
                {
                    base *= 10;
                }
            }
            
            resMult = base * mult;
            
            const int vec_len = length / resMult + 1;
            points.reserve(vec_len);
            
            const int mult_int = resMult;
            int val = range.getStart() + ((mult_int - (static_cast<int>(range.getStart()) % mult_int)) % mult_int);
            
            if (val < range.getStart())
            {
                val += mult_int;
            }
            
            for (int i = 0; i < vec_len; ++i)
            {
                if (val > range.getEnd())
                {
                    break;
                }
                
                points.emplace_back(val);
                val += resMult;
            }
        }
        else
        {
            double base = 1;
            double mult = 1;
            
            for (;;)
            {
                if (((length / base) * maxTileSize) >= size)
                {
                    break;
                }
                else if (((length / (base * 0.5)) * maxTileSize) >= size)
                {
                    mult = 0.5;
                    break;
                }
                else if (((length / (base * 0.2)) * maxTileSize) >= size)
                {
                    mult = 0.2;
                    break;
                }
                else
                {
                    base *= 0.1;
                }
            }
            
            resMult = base * mult;
            
            const int vec_len = length / resMult + 1;
            points.reserve(vec_len);
            
            double val = range.getStart() + std::fmod(resMult - std::fmod(range.getStart(), resMult), resMult);
            
            if (val < range.getStart())
            {
                val += resMult;
            }
            
            for (int i = 0; i < vec_len; ++i)
            {
                if (val > range.getEnd())
                {
                    break;
                }
                
                points.emplace_back(val);
                val += resMult;
            }
        }
        
        points.shrink_to_fit();
        return points;
    }
    
    int getMaxWidth(const juce::Font &font, const std::vector<double> &points)
    {
        juce::String text;
        
        for (const auto point : points)
        {
            if (std::abs(point) < zeroPivot)
            {
                continue;
            }
            
            if (juce::String number(point); number.length() > text.length())
            {
                std::swap(text, number);
            }
        }
        
        return font.getStringWidth(text);
    }
}
//======================================================================================================================
// endregion Anonymous
//**********************************************************************************************************************

namespace jaut
{
    //******************************************************************************************************************
    // region ContentPane
    //==================================================================================================================
    ContentPane::ContentPane(Style parStyle) : style(parStyle)      { JAUT_INIT_LAF() }
    ContentPane::ContentPane()               : ContentPane(Style{}) {}
    
    //==================================================================================================================
    void ContentPane::paint(juce::Graphics &g)
    {
        lookAndFeel->drawContentPaneBackground(g, *this);
    }
    
    void ContentPane::paintOverChildren(juce::Graphics &g)
    {
        lookAndFeel->drawContentPaneBorder(g, *this);
    }
    
    void ContentPane::resized()
    {
        if (content)
        {
            content->setBounds(style.borderThickness
                                    .trimRectangle(style.contentPadding.trimRectangle(getLocalBounds())));
        }
        
        initialised = true;
    }
    
    //==================================================================================================================
    bool ContentPane::resetComponent()
    {
        if (!content)
        {
            return false;
        }
    
        content.reset();
        removeAllChildren();
        ComponentChanged.invoke(nullptr);
    
        return true;
    }
    
    bool ContentPane::setComponent(std::nullptr_t)
    {
        return resetComponent();
    }
    
    bool ContentPane::setComponent(juce::OptionalScopedPointer<juce::Component> component)
    {
        if (component.get() == content.get())
        {
            return false;
        }
    
        content = std::move(component);
        removeAllChildren();
    
        if (content)
        {
            addAndMakeVisible(content.get());
        }
    
        ComponentChanged.invoke(content.get());
        resized();
    
        return true;
    }
    
    bool ContentPane::setComponent(juce::Component &component)
    {
        return setComponent({ &component, false });
    }
    
    //==================================================================================================================
    juce::OptionalScopedPointer<juce::Component> ContentPane::releaseComponent()
    {
        removeAllChildren();
        ComponentChanged.invoke(nullptr);
        return std::move(content);
    }
    
    //==================================================================================================================
    void ContentPane::setContentPadding(Padding padding)
    {
        if (style.contentPadding != std::exchange(style.contentPadding, padding) && initialised)
        {
            resized();
        }
    }
    
    void ContentPane::setBorderThickness(Thickness<int> thickness)
    {
        if (style.borderThickness != std::exchange(style.borderThickness, thickness) && initialised)
        {
            resized();
            repaint();
        }
    }
    
    //==================================================================================================================
    void ContentPane::setStyle(Style newStyle)
    {
        std::swap(style, newStyle);
        
        if (!initialised)
        {
            return;
        }
        
        if (style.borderThickness != newStyle.borderThickness)
        {
            resized();
            repaint();
        }
        else if (style.contentPadding != newStyle.contentPadding)
        {
            resized();
        }
    }
    //==================================================================================================================
    // endregion ContentPane
    //******************************************************************************************************************
    // region SplitPane
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region ContentContainer
    //==================================================================================================================
    SplitPane::ContentContainer::ContentContainer(SplitPane &container)
        : parent(container), constrainer(container),
          lastRememberedPosition(container.options.panel1MinimumSize)
    {
        if (parent.options.orientation == Orientation::Vertical)
        {
            containers[0].setScrollBarsShown(false, true);
            containers[1].setScrollBarsShown(false, true);
            separator.setMouseCursor(container.style.cursorVertical);
        }
        else
        {
            containers[0].setScrollBarsShown(true, false);
            containers[1].setScrollBarsShown(true, false);
            separator.setMouseCursor(container.style.cursorHorizontal);
        }
        
        addAndMakeVisible(containers[0]);
        addAndMakeVisible(containers[1]);
    
        separator.addMouseListener(this, false);
        addAndMakeVisible(separator);
    }
    
    //==================================================================================================================
    void SplitPane::ContentContainer::paint(juce::Graphics &g)
    {
        LookAndFeel_Jaut &laf = *parent.lookAndFeel;
        laf.drawSplitContainerSeparator(g, separator);
        
        if (parent.options.orientation == Orientation::Vertical)
        {
            laf.drawSplitContainerBorder(g, containers[0], getLocalBounds().withWidth(separator.getX()),
                                         parent.style.panelBorderThickness);
            laf.drawSplitContainerBorder(g, containers[1], getLocalBounds().withWidth(getWidth() - separator.getRight())
                                                                          .withX(separator.getRight()),
                                         parent.style.panelBorderThickness);
        }
        else
        {
            laf.drawSplitContainerBorder(g, containers[0], getLocalBounds().withHeight(separator.getY()),
                                         parent.style.panelBorderThickness);
            laf.drawSplitContainerBorder(g, containers[1], getLocalBounds().withHeight(getHeight()
                                                                                        - separator.getBottom())
                                                                           .withY(separator.getBottom()),
                                         parent.style.panelBorderThickness);
        }
    }
    
    void SplitPane::ContentContainer::resized()
    {
        if (getWidth() <= 0 || getHeight() <= 0)
        {
            return;
        }
        
        initialised      = true;
        prevSeparatorPos = separator.getPosition();
    
        const Options &loptions = parent.options;
        const Style   &lstyle   = parent.style;
        
        if (loptions.collapseMode != CollapseMode::AlwaysShow)
        {
            if (!components[0].get())
            {
                const int mul = ((loptions.collapseMode == CollapseMode::Hide) * lstyle.separatorThickness);
        
                if (loptions.orientation == Orientation::Vertical)
                {
                    separator.setTopLeftPosition(-mul, 0);
                }
                else
                {
                    separator.setTopLeftPosition(0, -mul);
                }
                
                prevSize = getLocalBounds().withX(parent.getX()).withY(parent.getY());
                return;
            }
            else if (!components[1].get())
            {
                const int mul = ((loptions.collapseMode == CollapseMode::Collapse) * lstyle.separatorThickness);
        
                if (loptions.orientation == Orientation::Vertical)
                {
                    separator.setTopLeftPosition(getWidth() - mul, 0);
                }
                else
                {
                    separator.setTopLeftPosition(0, getHeight() - mul);
                }
        
                prevSize = getLocalBounds().withX(parent.getX()).withY(parent.getY());
                return;
            }
        }
        
        const bool should_remember = loptions.rememberLastSeparatorPosition
                                         && (   loptions.resizeBehaviour == ResizeBehaviour::Panel1
                                             || loptions.resizeBehaviour == ResizeBehaviour::Panel2);
        
        if (loptions.orientation == Orientation::Vertical)
        {
            const ResizeBehaviour behaviour = getResizeBehaviour(loptions.resizeBehaviour, prevSize.getX(), getX(),
                                                                 prevSize.getWidth(), getWidth());
            
            ::resizeSplitContainer(behaviour, separator, prevSize.getWidth(), prevSize.getX(),
                                   getX(), getWidth(), getHeight(), separator.getX(), separator.getHeight(),
                                   separator.getRight(), lastRememberedPosition, loptions.panel1MinimumSize,
                                   loptions.panel2MinimumSize, lstyle.separatorThickness, should_remember, false);
        }
        else
        {
            const ResizeBehaviour behaviour = getResizeBehaviour(loptions.resizeBehaviour, prevSize.getY(), getY(),
                                                                 prevSize.getHeight(), getHeight());
            
            ::resizeSplitContainer(behaviour, separator, prevSize.getHeight(), prevSize.getY(),
                                   getY(), getHeight(), getWidth(), separator.getY(), separator.getWidth(),
                                   separator.getBottom(), lastRememberedPosition, loptions.panel1MinimumSize,
                                   loptions.panel2MinimumSize, lstyle.separatorThickness, should_remember, true);
        }
        
        prevSize = getLocalBounds().withX(parent.getX()).withY(parent.getY());
    }
    
    //==================================================================================================================
    void SplitPane::ContentContainer::mouseDown(const juce::MouseEvent &event)
    {
        if (event.eventComponent != &separator)
        {
            return;
        }
        
        prevSeparatorPos = separator.getPosition();
        dragger.startDraggingComponent(&separator, event);
    }
    
    void SplitPane::ContentContainer::mouseDrag(const juce::MouseEvent &event)
    {
        if (event.eventComponent != &separator)
        {
            return;
        }
        
        dragger.dragComponent(&separator, event, &constrainer);
        updateLastPosition();
    }
    
    //==================================================================================================================
    void SplitPane::ContentContainer::updateLastPosition() noexcept
    {
        if (parent.options.resizeBehaviour == ResizeBehaviour::Panel2)
        {
            lastRememberedPosition = parent.options.orientation == Orientation::Vertical
                                         ? separator.getX() : separator.getY();
        }
        else
        {
            lastRememberedPosition = parent.options.orientation == Orientation::Vertical
                                         ? getWidth() - separator.getRight() : getHeight() - separator.getBottom();
        }
    }
    
    //==================================================================================================================
    void SplitPane::ContentContainer::childBoundsChanged(Component *child)
    {
        if (child == &separator)
        {
            if (prevSeparatorPos != child->getPosition())
            {
                parent.SeparatorMoved.invoke(prevSeparatorPos, child->getPosition());
            }
            
            const Options &loptions = parent.options;
            const Style   &lstyle   = parent.style;
            
            auto &container_1 = containers[0];
            auto &container_2 = containers[1];
            
            const juce::Rectangle pane1_old = containers[0].getBounds();
            const juce::Rectangle pane2_old = containers[1].getBounds();
            
            if (loptions.orientation == Orientation::Vertical)
            {
                const int new_min_1 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.left
                                                                 - lstyle.panelBorderThickness.right;
                const int new_min_2 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.left
                                                                 - lstyle.panelBorderThickness.right;
    
                container_1.setBounds(lstyle.panelBorderThickness.trimRectangle({ separator.getX(), getHeight() }));
                container_2.setBounds(lstyle.panelBorderThickness.trimRectangle({ separator.getRight(), 0,
                                                                                  getWidth() - separator.getRight(),
                                                                                  getHeight() }));
                
                if (auto *comp = components[0].get())
                {
                    comp->setBounds(0, 0, std::max(container_1.getWidth(), new_min_1), getHeight());
                }
            
                if (auto *comp = components[1].get())
                {
                    comp->setBounds(0, 0, std::max(container_2.getWidth(), new_min_2), getHeight());
                }
            }
            else
            {
                const int new_min_1 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.top
                                                                 - lstyle.panelBorderThickness.bottom;
                const int new_min_2 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.top
                                                                 - lstyle.panelBorderThickness.bottom;
    
                container_1.setBounds(lstyle.panelBorderThickness.trimRectangle({ getWidth(), separator.getY() }));
                container_2.setBounds(lstyle.panelBorderThickness.trimRectangle({ 0, separator.getBottom(), getWidth(),
                                                                                  getHeight()
                                                                                      - separator.getBottom() }));
                
                if (auto *comp = components[0].get())
                {
                    comp->setBounds(0, 0, getWidth(), std::max(container_1.getHeight(), new_min_1));
                }
                
                if (auto *comp = components[1].get())
                {
                    comp->setBounds(0, 0, getWidth(), std::max(container_2.getHeight(), new_min_2));
                }
            }
    
            if (const juce::Rectangle pane1_new = container_1.getBounds(); pane1_new != pane1_old)
            {
                parent.PanelResized.invoke(PanelId::Panel1, pane1_old, pane1_new);
            }
            
            if (const juce::Rectangle pane2_new = container_2.getBounds(); pane2_new != pane2_old)
            {
                parent.PanelResized.invoke(PanelId::Panel2, pane2_old, pane2_new);
            }
        }
    }
    //==================================================================================================================
    // endregion ContentContainer
    //******************************************************************************************************************
    // region SplitPane
    //==================================================================================================================
    SplitPane::SplitPane(Style parStyle, Options parOptions)
        : style(std::move(parStyle)),
          options(std::move(parOptions)),
          contentContainer(*this)
    {
        JAUT_INIT_LAF()
        addAndMakeVisible(contentContainer);
    }
    
    SplitPane::SplitPane(Style   parStyle)   : SplitPane(std::move(parStyle), Options{}) {}
    SplitPane::SplitPane(Options parOptions) : SplitPane(Style{}, std::move(parOptions)) {}
    SplitPane::SplitPane()                   : SplitPane(Style{}, Options{})             {}
    
    //==================================================================================================================
    void SplitPane::paint(juce::Graphics &g)
    {
        lookAndFeel->drawSplitContainerBackground(g, *this);
    }
    
    void SplitPane::paintOverChildren(juce::Graphics &g)
    {
        lookAndFeel->drawSplitContainerBorder(g, *this, getLocalBounds(), style.borderThickness);
    }
    
    void SplitPane::resized()
    {
        contentContainer.setBounds(style.borderThickness.trimRectangle(style.contentPadding
                                                                            .trimRectangle(getLocalBounds())));
    }
    
    //==================================================================================================================
    void SplitPane::resetComponent(PanelId panelId)
    {
        juce::Viewport &pane = contentContainer.containers[static_cast<int>(panelId)];
        
        if (pane.getViewedComponent())
        {
            pane.setViewedComponent(nullptr, false);
            contentContainer.components[static_cast<int>(panelId)].reset();
    
            if (options.collapseMode != CollapseMode::AlwaysShow)
            {
                contentContainer.resized();
            }
            
            ComponentChanged(nullptr, panelId);
        }
    }
    
    void SplitPane::setComponent(juce::OptionalScopedPointer<juce::Component> component, PanelId panelId)
    {
        if (!contentContainer.components[0] && !contentContainer.components[1] && component.get())
        {
            if (options.orientation == Orientation::Vertical)
            {
                if (panelId == PanelId::Panel1)
                {
                    int x = std::clamp(component->getWidth(),
                                       options.panel1MinimumSize,
                                       contentContainer.getWidth() - options.panel2MinimumSize
                                                                   - style.separatorThickness);
                    
                    x = std::clamp(x, 0, contentContainer.getWidth());
                    contentContainer.separator.setTopLeftPosition(x, 0);
                }
                else
                {
                    int x = std::clamp(contentContainer.getWidth() - component->getWidth()
                                                                   - style.separatorThickness,
                                       options.panel1MinimumSize,
                                       contentContainer.getWidth() - options.panel2MinimumSize
                                                                   - style.separatorThickness);

                    x = std::clamp(x, 0, contentContainer.getWidth());
                    contentContainer.separator.setTopLeftPosition(x, 0);
                }
            }
            else
            {
                if (panelId == PanelId::Panel1)
                {
                    int y = std::clamp(component->getHeight(),
                                       options.panel1MinimumSize,
                                       contentContainer.getHeight() - options.panel2MinimumSize
                                                                    - style.separatorThickness);
    
                    y = std::clamp(y, 0, contentContainer.getHeight());
                    contentContainer.separator.setTopLeftPosition(0, y);
                }
                else
                {
                    int y = std::clamp(contentContainer.getHeight() - component->getHeight()
                                                                    - style.separatorThickness,
                                       options.panel1MinimumSize,
                                       contentContainer.getHeight() - options.panel2MinimumSize
                                                                    - style.separatorThickness);
    
                    y = std::clamp(y, 0, contentContainer.getHeight());
                    contentContainer.separator.setTopLeftPosition(0, y);
                }
            }
    
            contentContainer.updateLastPosition();
            contentContainer.repaint();
        }
    
        juce::Viewport &pane = contentContainer.containers[static_cast<int>(panelId)];
    
        if (pane.getViewedComponent())
        {
            pane.setViewedComponent(nullptr);
        }
        
        auto *comp = (contentContainer.components[static_cast<int>(panelId)] = std::move(component)).get();
        pane.setViewedComponent(comp, false);
        
        contentContainer.resized();
        ComponentChanged(comp, panelId);
    }
    
    void SplitPane::setComponent(std::nullptr_t, PanelId panelId) { resetComponent(panelId); }
    
    void SplitPane::setComponent(juce::Component &component, PanelId panelId)
    {
        setComponent({ &component, false }, panelId);
    }
    
    void SplitPane::swapPanes()
    {
        auto comp_1 = releaseComponent(PanelId::Panel1);
        auto comp_2 = releaseComponent(PanelId::Panel2);
        
        setComponent(std::move(comp_2), PanelId::Panel1);
        setComponent(std::move(comp_1), PanelId::Panel2);
    }
    
    //==================================================================================================================
    void SplitPane::setResizerPos(int pos)
    {
        if (options.orientation == Orientation::Vertical)
        {
            if (::getSizeState(contentContainer.getWidth(), options.panel1MinimumSize, options.panel2MinimumSize,
                               style.separatorThickness) != SplitContainerSizeState::Full)
            {
                return;
            }
        
            contentContainer.separator
                            .setTopLeftPosition(std::clamp(pos, options.panel1MinimumSize,
                                                           contentContainer.getWidth() - options.panel2MinimumSize
                                                                                       - style.separatorThickness), 0);
        }
        else
        {
            if (::getSizeState(contentContainer.getHeight(), options.panel1MinimumSize, options.panel2MinimumSize,
                               style.separatorThickness) != SplitContainerSizeState::Full)
            {
                return;
            }
        
            contentContainer.separator
                            .setTopLeftPosition(0, std::clamp(pos, options.panel1MinimumSize,
                                                              contentContainer.getHeight() - options.panel2MinimumSize
                                                                                           - style.separatorThickness));
        }
    
        contentContainer.updateLastPosition();
    }
    
    //==================================================================================================================
    void SplitPane::moveResizer(int amount) { setResizerPos(contentContainer.separator.getX() + amount); }
    void SplitPane::moveResizer(float rational)
    {
        const int value = (options.orientation == Orientation::Vertical ? contentContainer.getWidth()
                                                                        : contentContainer.getHeight());
        moveResizer(value * rational);
    }
    
    //==================================================================================================================
    void SplitPane::setContentPadding(Padding padding)
    {
        if (padding != std::exchange(style.contentPadding, padding) && contentContainer.initialised)
        {
            resized();
        }
    }

    void SplitPane::setBorderThickness(Thickness<int> thickness)
    {
        // Negative border values are illegal!
        // If you want the contents to go out of bounds, you can do so by supplying negative values
        // to the component's padding property
        jassert(thickness.left >= 0 && thickness.top >= 0 && thickness.right >= 0 && thickness.bottom >= 0);
        
        if (thickness != std::exchange(style.borderThickness, { std::max(thickness.left,   0),
                                                                std::max(thickness.top,    0),
                                                                std::max(thickness.right,  0),
                                                                std::max(thickness.bottom, 0) })
            && contentContainer.initialised)
        {
            resized();
            repaint();
        }
    }
    
    void SplitPane::setPanelBorderThickness(Thickness<int> thickness)
    {
        // Negative border values are illegal!
        // If you want the contents to go out of bounds, you can do so by supplying negative values
        // to the component's padding property
        jassert(thickness.left >= 0 && thickness.top >= 0 && thickness.right >= 0 && thickness.bottom >= 0);
    
        if (thickness != std::exchange(style.panelBorderThickness, { std::max(thickness.left,   0),
                                                                     std::max(thickness.top,    0),
                                                                     std::max(thickness.right,  0),
                                                                     std::max(thickness.bottom, 0) })
            && contentContainer.initialised)
        {
            contentContainer.resized();
            contentContainer.repaint();
        }
    }
    
    void SplitPane::setSeperatorThickness(int val)
    {
        // Separator thickness cannot be a negative value
        jassert(val >= 0);
        
        if (val != std::exchange(style.separatorThickness, std::max(val, 0)) && contentContainer.initialised)
        {
            contentContainer.resized();
            contentContainer.repaint();
        }
    }
    
    void SplitPane::setResizerCursor(juce::MouseCursor verticalCursor, juce::MouseCursor horizontalCursor)
    {
        style.cursorVertical   = verticalCursor;
        style.cursorHorizontal = horizontalCursor;
        
        if (options.orientation == Orientation::Vertical)
        {
            contentContainer.separator.setMouseCursor(verticalCursor);
        }
        else
        {
            contentContainer.separator.setMouseCursor(horizontalCursor);
        }
    }
    
    void SplitPane::setResizerCursor(juce::MouseCursor cursor, Orientation forOrientation)
    {
        if (forOrientation == Orientation::Vertical)
        {
            style.cursorVertical = cursor;
        }
        else
        {
            style.cursorHorizontal = cursor;
        }
    
        if (forOrientation == options.orientation)
        {
            contentContainer.separator.setMouseCursor(cursor);
        }
    }
    
    //==================================================================================================================
    void SplitPane::setResizeBehaviour(ResizeBehaviour resizeBehaviour) noexcept
    {
        if (options.resizeBehaviour != std::exchange(options.resizeBehaviour, resizeBehaviour)
            && (resizeBehaviour == ResizeBehaviour::Panel1 || resizeBehaviour == ResizeBehaviour::Panel2))
        {
            contentContainer.updateLastPosition();
        }
    }
    
    void SplitPane::setCollapseMode(CollapseMode collapseMode) noexcept
    {
        if (collapseMode != std::exchange(options.collapseMode, collapseMode) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    void SplitPane::setOrientation(Orientation parOrientation)
    {
        if (parOrientation != std::exchange(options.orientation, parOrientation))
        {
            if (!contentContainer.initialised)
            {
                if (options.orientation == Orientation::Vertical)
                {
                    contentContainer.separator.setMouseCursor(style.cursorVertical);
                }
                else
                {
                    contentContainer.separator.setMouseCursor(style.cursorHorizontal);
                }
                
                return;
            }
            
            const float      vert_p    = getHeight() / 100.0f;
            const float      horz_p    = getWidth()  / 100.0f;
            juce::Component &separator = contentContainer.separator;
    
            if (options.orientation == Orientation::Vertical)
            {
                const int value = std::round(horz_p * (separator.getY() / vert_p));
                
                if (contentContainer.getWidth()
                        >= (options.panel1MinimumSize + options.panel2MinimumSize + style.separatorThickness))
                {
                    separator.setBounds(std::clamp(value, options.panel1MinimumSize,
                                                   contentContainer.getWidth() - options.panel2MinimumSize
                                                                               - style.separatorThickness),
                                        0, style.separatorThickness, contentContainer.getHeight());
                }
                else
                {
                    separator.setBounds(value, 0, style.separatorThickness, contentContainer.getHeight());
                }
                
                contentContainer.containers[0].setScrollBarsShown(false, true);
                contentContainer.containers[1].setScrollBarsShown(false, true);
                
                contentContainer.separator.setMouseCursor(style.cursorVertical);
            }
            else
            {
                const int value = std::round(vert_p * (separator.getX() / horz_p));
                
                if (contentContainer.getHeight()
                        >= (options.panel1MinimumSize + options.panel2MinimumSize + style.separatorThickness))
                {
                    separator.setBounds(0, std::clamp(value, options.panel1MinimumSize, contentContainer.getHeight()
                                                                                        - options.panel2MinimumSize
                                                                                        - style.separatorThickness),
                                        contentContainer.getWidth(), style.separatorThickness);
                }
                else
                {
                    separator.setBounds(0, value, contentContainer.getWidth(), style.separatorThickness);
                }
    
                contentContainer.containers[0].setScrollBarsShown(true, false);
                contentContainer.containers[1].setScrollBarsShown(true, false);
    
                contentContainer.separator.setMouseCursor(style.cursorHorizontal);
            }
            
            contentContainer.updateLastPosition();
            contentContainer.repaint();
    
            if (options.collapseMode != CollapseMode::AlwaysShow)
            {
                contentContainer.resized();
            }
        }
    }
    
    void SplitPane::setPanel1Minimum(int newMinimum)
    {
        if (newMinimum != std::exchange(options.panel1MinimumSize, newMinimum) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    void SplitPane::setPanel2Minimum(int newMinimum)
    {
        if (newMinimum != std::exchange(options.panel2MinimumSize, newMinimum) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    void SplitPane::setRememberLastPosition(bool shouldRemember)
    {
        if (shouldRemember != std::exchange(options.rememberLastSeparatorPosition, shouldRemember) && shouldRemember)
        {
            contentContainer.updateLastPosition();
        }
    }
    
    //==================================================================================================================
    void SplitPane::setStyle(Style newStyle)
    {
        std::swap(style, newStyle);
        
        if (!contentContainer.initialised)
        {
            return;
        }
        
        if (style.cursorVertical != newStyle.cursorVertical || style.cursorHorizontal != newStyle.cursorHorizontal)
        {
            setResizerCursor(style.cursorVertical, style.cursorHorizontal);
        }
        
        if (style.contentPadding != newStyle.contentPadding || style.borderThickness != newStyle.borderThickness)
        {
            // Negative border values are illegal!
            // If you want the contents to go out of bounds, you can do so by supplying negative values
            // to the component's padding property
            jassert(   style.borderThickness.left  >= 0 && style.borderThickness.top    >= 0
                    && style.borderThickness.right >= 0 && style.borderThickness.bottom >= 0);
            style.borderThickness.left   = std::max(style.borderThickness.left, 0);
            style.borderThickness.top    = std::max(style.borderThickness.top, 0);
            style.borderThickness.right  = std::max(style.borderThickness.right, 0);
            style.borderThickness.bottom = std::max(style.borderThickness.bottom, 0);
            
            resized();
            repaint();
        }

        if (   style.separatorThickness   != newStyle.separatorThickness
            || style.panelBorderThickness != newStyle.panelBorderThickness)
        {
            // Separator thickness cannot be a negative value
            jassert(style.separatorThickness >= 0);
            style.separatorThickness = std::max(style.separatorThickness, 0);
            
            contentContainer.resized();
            contentContainer.repaint();
        }
    }
    
    void SplitPane::setOptions(Options newOptions)
    {
        std::swap(options, newOptions);
        setOrientation(newOptions.orientation);
        
        if (options.orientation == newOptions.orientation &&
              (options.collapseMode      != newOptions.collapseMode
            || options.panel1MinimumSize != newOptions.panel1MinimumSize
            || options.panel2MinimumSize != newOptions.panel2MinimumSize))
        {
            contentContainer.resized();
        }
        
        if (options.resizeBehaviour != newOptions.resizeBehaviour)
        {
            contentContainer.updateLastPosition();
        }
    }
    //==================================================================================================================
    // endregion SplitPane
    //******************************************************************************************************************
    //==================================================================================================================
    //==================================================================================================================
    // endregion SplitPane
    //******************************************************************************************************************
    // region MultiPagePane
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region InternalTabBar
    //==================================================================================================================
    class MultiPagePane::InternalTabBar : public juce::Component,
                                          private LookAndFeel_Jaut, private juce::ComponentListener
    {
    public:
        class TabStrip : public juce::Viewport
        {
        public:
            struct DragHelper : public juce::MouseListener
            {
                //======================================================================================================
                struct Constrainer : juce::ComponentBoundsConstrainer
                {
                    void applyBoundsToComponent(Component &tab, juce::Rectangle<int> bounds) override
                    {
                        juce::Component &parent = *tab.getParentComponent();
                        tab.setBounds(bounds.constrainedWithin(parent.getBounds()));
                    }
                };
                
                //======================================================================================================
                juce::ComponentDragger tabDragger;
                Constrainer            dragConstrainer;
                
                TabStrip &tabStrip;
                
                TabButton *draggedTab { nullptr };
                int        dragIndex  { 0 };
                
                //======================================================================================================
                explicit DragHelper(TabStrip &parTabStrip)
                    : tabStrip(parTabStrip)
                {}
                
                //======================================================================================================
                TabButton* getDraggedTab() const noexcept
                {
                    return draggedTab;
                }
                
                //======================================================================================================
                void mouseDown(const juce::MouseEvent &e) override
                {
                    if (tabStrip.tabs.empty() || (!tabStrip.canReorder && !tabStrip.canExchange))
                    {
                        return;
                    }
                    
                    if (auto *const tab = dynamic_cast<TabButton*>(e.eventComponent))
                    {
                        draggedTab = tab;
                        
                        if (tabStrip.canReorder)
                        {
                            tabDragger.startDraggingComponent(tab, e);
                            tab->toFront(false);
        
                            for (int i = 0; i < tabStrip.getNumTabs(); ++i)
                            {
                                if (tab == tabStrip.tabs.at(static_cast<SizeTypes::Vector>(i)))
                                {
                                    dragIndex = i;
                                    break;
                                }
                            }
                        }
                    }
                }
        
                void mouseDrag(const juce::MouseEvent &e) override
                {
                    if (draggedTab)
                    {
                        if (tabStrip.canReorder)
                        {
                            tabDragger.dragComponent(draggedTab, e, &dragConstrainer);
        
                            TabButton *const prev = dragIndex > 0
                                                    ? tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex) - 1)
                                                    : nullptr;
                            TabButton *const next = (dragIndex + 1) < tabStrip.getNumTabs()
                                                    ? tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex) + 1)
                                                    : nullptr;
        
                            const juce::Rectangle<int> bounds = draggedTab->getBounds();
        
        
                            if (prev && bounds.getX() < (prev->getX() + Const_TabSwapSensitivity))
                            {
                                const auto prev_index = static_cast<SizeTypes::Vector>(dragIndex) - 1;
                                std::swap(tabStrip.tabs.at(static_cast<SizeTypes::Vector>(prev_index)),
                                          tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex)));
            
                                const int new_right = prev->getX() + draggedTab->getWidth();
                                prev->setTopLeftPosition(new_right, 0);
            
                                --dragIndex;
                                return;
                            }
                            else if (next && (bounds.getRight() > next->getRight() - Const_TabSwapSensitivity))
                            {
                                const auto next_index = static_cast<SizeTypes::Vector>(dragIndex) + 1;
                                std::swap(tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex)),
                                          tabStrip.tabs.at(static_cast<SizeTypes::Vector>(next_index)));
            
                                const int new_right = next->getX() - draggedTab->getWidth();
                                next->setTopLeftPosition(new_right, 0);
            
                                ++dragIndex;
                                return;
                            }
                        }
                        
                        if (tabStrip.canExchange && dragExchange(e))
                        {
                            mouseUp(e);
                        }
                    }
                }
        
                void mouseUp(const juce::MouseEvent&) override
                {
                    if (draggedTab)
                    {
                        std::exchange(draggedTab, nullptr)
                            ->setTopLeftPosition(dragIndex == 0
                                                     ? 0
                                                     : tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex - 1))
                                                           ->getRight(), 0);
                    }
                }
                
                bool dragExchange(const juce::MouseEvent &e) const
                {
                    if (juce::Component *const par_comp = tabStrip.tabBar.getParentComponent()->getParentComponent())
                    {
                        if (Window *const wind = dynamic_cast<Window*>(par_comp))
                        {
                            Window::DragAndDropContainerProxy &dadcp = wind->getDragAndDropProxy();
                            bool result = false;
                            
                            std::visit([&](auto &dadc)
                            {
                                result = executeDrag(dadc, e);
                            }, dadcp);
                            
                            return result;
                        }
                    }
                    
                    jaut::DragAndDropContainerAdvanced *const dada_container =
                        jaut::DragAndDropContainerAdvanced::findParentDragContainerFor(&tabStrip.tabBar);
                    
                    if (dada_container)
                    {
                        return executeDrag(dada_container, e);
                    }
                    
                    juce::DragAndDropContainer *const dad_container =
                        juce::DragAndDropContainer::findParentDragContainerFor(&tabStrip.tabBar);
                    
                    return executeDrag(dad_container, e);
                }
        
                template<class Container>
                bool executeDrag(Container *container, const juce::MouseEvent &e) const
                {
                    if (container && !container->isDragAndDropActive())
                    {
                        const juce::Point<int> mouse_pos = tabStrip.canReorder ? tabStrip.content.getMouseXYRelative()
                                                                               : e.position.toInt();
                
                        if (mouse_pos.getX() <= -30 || mouse_pos.getY() <= -30
                            || mouse_pos.getX() > (tabStrip.content.getWidth()  + 30)
                            || mouse_pos.getY() > (tabStrip.content.getHeight() + 30))
                        {
                            const juce::Image snapshot = createTabSnapshot(draggedTab);
                    
                            juce::DynamicObject::Ptr   obj      = tabStrip.tabBar.dragDropJson.clone();
                            juce::DynamicObject *const tab_info = obj->getProperty("tab_info").getDynamicObject();
                    
                            tab_info->setProperty("page_id", draggedTab->getName());
                            tab_info->setProperty("name",    draggedTab->getButtonText());
                    
                            container->startDragging(obj.get(), tabStrip.tabBar.getParentComponent(), snapshot,
                                                     tabStrip.canExtract, nullptr, &e.source);
                            return true;
                        }
                    }
            
                    return false;
                }
            };
            
            //==========================================================================================================
            explicit TabStrip(InternalTabBar &parTabBar)
                : tabBar(parTabBar), dragger(*this)
            {
                setViewedComponent(&content, false);
            }
        
            //==========================================================================================================
            void resized() override
            {
                content.setSize(content.getWidth(), getHeight());
                juce::Viewport::resized();
            }
            
            //==========================================================================================================
            void addTab(TabButton *tab)
            {
                tab->addMouseListener(&dragger, false);
                content.addAndMakeVisible(tab);
                tabs.emplace_back(tab);
                
                tab->setTopLeftPosition(content.getWidth(), 0);
                content.setSize(tab->getRight(), getHeight());
            }
            
            void removeTab(TabButton *tabToRemove)
            {
                for (auto it = tabs.begin(); it != tabs.end(); ++it)
                {
                    TabButton *const tab = *it;
                    
                    if (tab == tabToRemove)
                    {
                        tab->removeMouseListener(&dragger);
                        content.removeChildComponent(tab);
                        tabs.erase(it);
                        
                        rearrangeTabs(static_cast<int>(std::distance(tabs.begin(), it)));
        
                        return;
                    }
                }
            }
        
            void addAll(const std::vector<TabButton*> &parTabs)
            {
                if (parTabs.empty())
                {
                    return;
                }
                
                int last_right = content.getWidth();
            
                for (auto &tab : parTabs)
                {
                    content.addAndMakeVisible(tab);
                    tab->addMouseListener(&dragger, false);
                    tab->setTopLeftPosition(last_right, 0);
                    last_right += tab->getWidth();
                }
            
                tabs.insert(tabs.end(), parTabs.begin(), parTabs.end());
                content.setSize(last_right, getHeight());
            }
            
            std::vector<TabButton*> removeAndReturnAll()
            {
                for (auto &tab : tabs)
                {
                    tab->removeMouseListener(&dragger);
                }
                
                auto vec = tabs;
                tabs.clear();
                content.removeAllChildren();
                return vec;
            }
            
            //==========================================================================================================
            void setNewTabHeight(int parTabHeight)
            {
                content.setSize(content.getWidth(), parTabHeight);
                
                for (TabButton *tab : tabs)
                {
                    tab->setSize(tab->getWidth(), parTabHeight);
                }
            }
            
            void setCanReorder(bool parCanReorder) noexcept
            {
                canReorder = parCanReorder;
            }
            
            void setCanExchange(bool parCanExchange) noexcept
            {
                canExchange = parCanExchange;
            }
            
            void setCanExtract(bool parCanExtract) noexcept
            {
                canExtract = parCanExtract;
            }
            
            //==========================================================================================================
            bool hasTabs() const noexcept
            {
                return !tabs.empty();
            }
            
            bool hasHiddenTabs() const noexcept
            {
                return !tabs.empty() && !getViewArea().contains(content.getBounds());
            }
        
            //==========================================================================================================
            int getNumTabs() const noexcept
            {
                return static_cast<int>(tabs.size());
            }
            
            const std::vector<TabButton*>& getTabs() const noexcept
            {
                return tabs;
            }
        
            //==========================================================================================================
            void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails &wheel) override
            {
                setViewPosition(getViewPositionX() + static_cast<int>(wheel.deltaY * -50.0f), 0);
            }
            
        private:
            static juce::Image createTabSnapshot(TabButton *tab)
            {
                juce::Image dragImage = tab->createComponentSnapshot(tab->getLocalBounds())
                                           .convertedToFormat(juce::Image::ARGB);
                dragImage.multiplyAllAlphas(0.5f);
            
                return dragImage;
            }
        
            //==========================================================================================================
            std::vector<TabButton*> tabs;
            
            InternalTabBar &tabBar;
            
            juce::Component content;
            DragHelper      dragger;
        
            bool canReorder  { false };
            bool canExchange { false };
            bool canExtract  { false };
            
            //==========================================================================================================
            void rearrangeTabs(int index)
            {
                if (!tabs.empty())
                {
                    int last_right = 0;
        
                    if (index == 0)
                    {
                        TabButton *const tab = tabs.at(0);
                        tab->setTopLeftPosition(0, 0);
                        last_right = tab->getWidth();
                        ++index;
                    }
                    else
                    {
                        last_right = tabs.at(static_cast<SizeTypes::Vector>(index) - 1)->getRight();
                    }
        
                    for (int i = index; i < getNumTabs(); ++i)
                    {
                        TabButton *tab = tabs.at(static_cast<SizeTypes::Vector>(i));
                        tab->setTopLeftPosition(last_right, 0);
                        last_right = tab->getRight();
                    }
        
                    content.setSize(last_right, getHeight());
                }
                else
                {
                    content.setSize(0, getHeight());
                }
            }
        };
        
        //==============================================================================================================
        using TabPointer = std::unique_ptr<TabButton>;
        using TabVector  = std::vector<TabPointer>;
        
        //==============================================================================================================
        std::function<void(const juce::String&)>       onTabClosed   { nullptr };
        std::function<void(const juce::String&)>       onTabChanged  { nullptr };
        std::function<PagePtr(const juce::String&)>    onTabDetached { nullptr };
        std::function<void(const juce::String&, bool)> onTabPinned   { nullptr };
        
        //==============================================================================================================
        explicit InternalTabBar()
            : tabStripNormal(*this),
              tabStripPinned(*this)
        {
            lookAndFeelChanged();
            
            tabStripNormal.setScrollBarsShown(false, false);
            addAndMakeVisible(tabStripNormal);
            
            tabStripPinned.setScrollBarsShown(false, false);
            addAndMakeVisible(tabStripPinned);
            
            buttonHiddenTabs.setLookAndFeel(this);
            buttonHiddenTabs.onClick = [this]()
            {
                juce::PopupMenu popup_menu;
                
                if (hasSeperateRow)
                {
                    const bool has_hidden_tabs = tabStripNormal.hasHiddenTabs();
                    
                    if (tabStripPinned.hasHiddenTabs())
                    {
                        for (const TabButton *tab : tabStripPinned.getTabs())
                        {
                            if (tab->getTabIndex() != activeTab
                                && !tabStripPinned.getViewArea().contains(tab->getBounds()))
                            {
                                popup_menu.addItem(tab->getButtonText(), [this, tab]()
                                {
                                    activateTab(tab->getTabIndex());
                                    moveToTab  (tab);
                                });
                            }
                        }
        
                        if (has_hidden_tabs)
                        {
                            popup_menu.addSeparator();
                        }
                    }
                    
                    if (has_hidden_tabs)
                    {
                        for (const TabButton *tab : tabStripNormal.getTabs())
                        {
                            if (tab->getTabIndex() != activeTab
                                && !tabStripNormal.getViewArea().contains(tab->getBounds()))
                            {
                                popup_menu.addItem(tab->getButtonText(), [this, tab]()
                                {
                                    activateTab(tab->getTabIndex());
                                    moveToTab  (tab);
                                });
                            }
                        }
                    }
                }
                else
                {
                    for (const auto &tab : tabs)
                    {
                        if (tab->getTabIndex() != activeTab && !tabStripNormal.getViewArea().contains(tab->getBounds()))
                        {
                            TabButton *const tab_button = tab.get();
                            popup_menu.addItem(tab->getButtonText(), [this, tab_button]()
                            {
                                activateTab(tab_button->getTabIndex());
                                moveToTab  (tab_button);
                            });
                        }
                    }
                }
        
                juce::PopupMenu::Options popup_options;
                popup_options.withParentComponent(this);
                popup_menu.showMenu(popup_options);
            };
            addChildComponent(buttonHiddenTabs);
            
            dragDropJson.setProperty("drag_id",  DragAndDrop_ID);
            dragDropJson.setProperty("tab_info", new juce::DynamicObject);
        }
        
        ~InternalTabBar() override
        {
            if (auto *const parent = dynamic_cast<MultiPagePane*>(getParentComponent()))
            {
                parent->removeComponentListener(this);
            }
        }
        
        //==============================================================================================================
        void resized() override
        {
            juce::Rectangle<int> bounds = getLocalBounds();
            
            if (getRequiredWidth() > getWidth())
            {
                const int tab_half              = tabHeight / 2;
                const juce::Rectangle<int> rect = bounds.removeFromRight(tabHeight);
                buttonHiddenTabs.setVisible(true);
                buttonHiddenTabs
                    .setBounds(marginHiddenTabs.trimRectangle(rect.withPosition(rect.getCentreX() - tab_half,
                                                                                rect.getCentreY() - tab_half)
                                                                  .withHeight(tabHeight)));
            }
            else
            {
                buttonHiddenTabs.setVisible(false);
            }
            
            tabStripPinned.setBounds(bounds                    .withHeight(tabHeight * hasPinnedTabs()));
            tabStripNormal.setBounds(tabStripPinned.getBounds().withHeight(tabHeight * hasNonPinnedTabs())
                                                               .withY     (tabStripPinned.getBottom()));
            
            if (!tabs.empty())
            {
                const TabButton *const tab = tabs.at(static_cast<SizeTypes::Vector>(activeTab)).get();
                const TabStrip &strip      = hasSeperateRow && tab->isPinned() ? tabStripPinned : tabStripNormal;
        
                if (!strip.getViewArea().contains(tab->getBounds()))
                {
                    moveToTab(tab);
                }
            }
        }
        
        //==============================================================================================================
        void addTab(const Style &parStyle, const TabFactory &parFactory, const juce::String &id,
                    const juce::String &name)
        {
            const int tab_index = static_cast<int>(tabs.size());
            TabPointer tab      = std::make_unique<TabButton>(parStyle, parFactory, id, name);
            tab->updateTabHeight(tabHeight);
            tab->setTabIndex(tab_index);
            tab->onActivating = [this](const TabButton &parTab)
            {
                tabs.at(static_cast<SizeTypes::Vector>(std::exchange(activeTab, parTab.getTabIndex())))
                    ->setActive(false);
        
                const TabStrip &strip = hasSeperateRow && parTab.isPinned() ? tabStripPinned : tabStripNormal;
        
                if (!strip.getViewArea().contains(parTab.getBounds()))
                {
                    moveToTab(&parTab);
                }
        
                onTabChanged(parTab.getName());
            };
            tab->onClosing = [this](const TabButton &parTab)
            {
                removeTab(parTab.getTabIndex());
            };
            tab->onPinning = [this](const TabButton &parTab, bool state)
            {
                pinTab(parTab.getTabIndex(), state);
            };
            
            tabStripNormal.addTab(tab.get());
            tabs.emplace_back(std::move(tab));
            
            activateTab(tab_index);
        }
        
        void removeTab(int index)
        {
            onTabClosed(removeInternally(index));
        }
        
        void activateTab(int index)
        {
            TabButton &tab = *tabs.at(static_cast<SizeTypes::Vector>(index));
            tabs.at(static_cast<SizeTypes::Vector>(std::exchange(activeTab, index)))->setActive(false);
            tab.setActive(true);
        
            const TabStrip &strip = hasSeperateRow && tab.isPinned() ? tabStripPinned : tabStripNormal;
        
            if (!strip.getViewArea().contains(tab.getBounds()))
            {
                moveToTab(&tab);
            }
            
            onTabChanged(tab.getName());
        }
        
        void pinTab(int index, bool state)
        {
            TabButton *tab = tabs.at(static_cast<SizeTypes::Vector>(index)).get();
            tab->setPinned(state);
            
            if (hasSeperateRow)
            {
                if (state)
                {
                    tabStripNormal.removeTab(tab);
                    tabStripPinned.addTab   (tab);
                }
                else
                {
                    tabStripPinned.removeTab(tab);
                    tabStripNormal.addTab   (tab);
                }
            }
            
            onTabPinned(tab->getName(), state);
        }
        
        PagePtr detachTab(int index)
        {
            return onTabDetached(removeInternally(index));
        }
        
        //==============================================================================================================
        void setNewTabHeight(int parTabHeight)
        {
            tabHeight = parTabHeight;
            tabStripPinned.setNewTabHeight(parTabHeight);
            tabStripNormal.setNewTabHeight(parTabHeight);
        }
        
        void setHiddenTabsMargin(Margin margin)
        {
            marginHiddenTabs = margin;
        }
        
        void setNewItemHeight(int itemHeight)
        {
            for (TabPointer &tab : tabs)
            {
                tab->updateTabSize(itemHeight);
                tab->setSize(tabHeight, tab->getWidth());
            }
        }
        
        void setPinBehaviour(bool separateTabs)
        {
            if ((hasSeperateRow = separateTabs))
            {
                for (TabPointer &tab : tabs)
                {
                    if (tab->isPinned())
                    {
                        tabStripNormal.removeTab(tab.get());
                        tabStripPinned.addTab   (tab.get());
                    }
                }
            }
            else
            {
                tabStripNormal.addAll(tabStripPinned.removeAndReturnAll());
            }
        }
        
        void setTabsReorderable(bool canReorder) noexcept
        {
            tabStripNormal.setCanReorder(canReorder);
            tabStripPinned.setCanReorder(canReorder);
        }
        
        void setTabsCanBeExchanged(bool canExchange) noexcept
        {
            tabStripNormal.setCanExchange(canExchange);
            tabStripPinned.setCanExchange(canExchange);
        }
        
        void setTabsCanBeExtracted(bool canExtract) noexcept
        {
            tabStripNormal.setCanExtract(canExtract);
            tabStripPinned.setCanExtract(canExtract);
        }
        
        //==============================================================================================================
        bool hasTabs() const noexcept
        {
            return !tabs.empty();
        }
        
        bool hasNonPinnedTabs() const noexcept
        {
            return tabStripNormal.hasTabs();
        }
        
        bool hasPinnedTabs() const noexcept
        {
            if (hasSeperateRow)
            {
                return tabStripPinned.hasTabs();
            }
            
            for (auto &tab : tabs)
            {
                if (tab->isPinned())
                {
                    return true;
                }
            }
            
            return false;
        }
        
        //==============================================================================================================
        bool hasHiddenPinnedTabs() const noexcept
        {
            return tabStripPinned.hasHiddenTabs();
        }
        
        bool hasHiddenNormalTabs() const noexcept
        {
            return tabStripNormal.hasHiddenTabs();
        }
        
        bool hasHiddenTabs() const noexcept
        {
            return hasHiddenPinnedTabs() || hasHiddenNormalTabs();
        }
        
        //==============================================================================================================
        int getRequiredWidth() const noexcept
        {
            return std::max(tabStripPinned.getViewedComponent()->getWidth(),
                            tabStripNormal.getViewedComponent()->getWidth());
        }
        
        int getTabHeight() const noexcept
        {
            return tabHeight;
        }
        
        //==============================================================================================================
        const TabVector& getAllTabs() const noexcept
        {
            return tabs;
        }
        
        int getActiveIndex() const noexcept
        {
            return activeTab;
        }
        
    private:
        TabVector tabs;
        
        juce::TextButton buttonHiddenTabs;
        
        juce::DynamicObject dragDropJson;
        
        TabStrip tabStripNormal;
        TabStrip tabStripPinned;
        
        Margin marginHiddenTabs;
        
        LookAndFeel_Jaut *lookAndFeel { nullptr };
        
        int  activeTab      { 0 };
        int  tabHeight      { 0 };
        bool hasSeperateRow { false };
        
        //==============================================================================================================
        void componentNameChanged(Component &component) override
        {
            dragDropJson.setProperty("id", component.getName());
        }
        
        //==============================================================================================================
        void parentHierarchyChanged() override
        {
            lookAndFeelChanged();
            
            if (auto *const parent = dynamic_cast<MultiPagePane*>(getParentComponent()))
            {
                parent->addComponentListener(this);
                componentNameChanged(*parent);
            }
        }
        
        void lookAndFeelChanged() final
        {
            if (lookAndFeel = dynamic_cast<LookAndFeel_Jaut*>(&getLookAndFeel()); !lookAndFeel)
            {
                lookAndFeel = &LookAndFeel_Jaut::getDefaultLaf();
                setLookAndFeel(lookAndFeel);
            }
        }
        
        //==============================================================================================================
        juce::String removeInternally(int index)
        {
            TabPointer tab = std::move(tabs.at(static_cast<SizeTypes::Vector>(index)));
            (hasSeperateRow && tab->isPinned() ? (tabStripPinned) : (tabStripNormal)).removeTab(tab.get());
        
            tabs.erase(tabs.begin() + index);
            updateTabIndices(index);
        
            if (activeTab == index)
            {
                if (activeTab != 0)
                {
                    --activeTab;
                }
            }
            else if (activeTab > index)
            {
                --activeTab;
            }
        
            if (!tabs.empty())
            {
                TabButton &ntab = *tabs.at(static_cast<SizeTypes::Vector>(activeTab));
                ntab.setActive(true);
            
                const TabStrip &strip = hasSeperateRow && ntab.isPinned() ? tabStripPinned : tabStripNormal;
            
                if (!strip.getViewArea().contains(ntab.getBounds()))
                {
                    moveToTab(&ntab);
                }
            
                onTabChanged(ntab.getName());
            }
            
            return tab->getName();
        }
        
        //==============================================================================================================
        void updateTabIndices(int index)
        {
            for (int i = index; i < static_cast<int>(tabs.size()); ++i)
            {
                tabs.at(static_cast<SizeTypes::Vector>(i))->setTabIndex(i);
            }
        }
        
        void moveToTab(const TabButton *tab)
        {
            const juce::Rectangle<int> bounds = tab->getBounds();
            TabStrip &tab_strip               = hasSeperateRow && tab->isPinned() ? tabStripPinned : tabStripNormal;
            const juce::Rectangle<int> view   = tab_strip.getViewArea();
            
            if (bounds.getX() > view.getX() && view.getWidth() >= bounds.getWidth())
            {
                tab_strip.setViewPosition(bounds.getRight() - tab_strip.getWidth(), 0);
            }
            else
            {
                tab_strip.setViewPosition(bounds.getX(), 0);
            }
        }
        
        //==============================================================================================================
        void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}
        void drawButtonText(juce::Graphics &g, juce::TextButton &button, bool isOver, bool isDown) override
        {
            lookAndFeel->drawMultiTabPaneMoreTabsButton(g, button, isOver, isDown);
        }
    };
    //==================================================================================================================
    // endregion InternalTabBar
    //******************************************************************************************************************
    // region TabButton
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region LookAndFeelProxy
    //==================================================================================================================
    class MultiPagePane::TabButton::LookAndFeelProxy : public LookAndFeel_Jaut
    {
    public:
        void drawToggleButton(juce::Graphics &g, juce::ToggleButton &txtButton, bool isOver, bool isDown) override
        {
            if (txtButton.getName() == "itemPinButton")
            {
                MultiPagePane::TabButton &tab = *static_cast<MultiPagePane::TabButton*>(txtButton.getParentComponent());
                tab.lookAndFeel->drawMultiTabPaneTabPinButton(g, tab, txtButton, isOver, isDown);
            }
        }
        
        void drawButtonText(juce::Graphics &g, juce::TextButton &txtButton, bool isOver, bool isDown) override
        {
            if (txtButton.getName() == "itemCloseButton")
            {
                MultiPagePane::TabButton &tab = *static_cast<MultiPagePane::TabButton*>(txtButton.getParentComponent());
                tab.lookAndFeel->drawMultiTabPaneTabCloseButton(g, tab, txtButton, isOver, isDown);
            }
        }
        
        void drawButtonBackground(juce::Graphics&, Button&, const juce::Colour&, bool, bool) override {}
    };
    //==================================================================================================================
    // endregion LookAndFeelProxy
    //******************************************************************************************************************
    // region TabButton
    //==================================================================================================================
    MultiPagePane::TabButton::LafProxyPtr MultiPagePane::TabButton::buttonProxyLaf
        = LafProxyPtr(new LookAndFeelProxy, [](LookAndFeelProxy *laf) { delete laf; });
    
    //==================================================================================================================
    MultiPagePane::TabButton::TabButton(const Style &parStyle, const TabFactory &parFactory, const juce::String &id,
                                        const juce::String &parText)
        : juce::Button(id),
          factory(parFactory)
    {
        JAUT_INIT_LAF()
        const std::vector<TabFactory::TabItemLayout> &layouts = factory.getTabLayout();
        
        for (const TabFactory::TabItemLayout &layout : layouts)
        {
            juce::Component *comp;
    
            switch (layout.itemType)
            {
                case TabFactory::TabItem::ItemImage:
                {
                    auto *img = new juce::ImageComponent("itemImage");
                    img->setImage(factory.getImageForPage(id));
                    img->setInterceptsMouseClicks(false, false);
            
                    comp = img;
                    break;
                }
                
                case TabFactory::TabItem::ItemText:
                {
                    auto *label = new juce::Label("itemText", parText);
                    label->setInterceptsMouseClicks(false, false);
                    label->setJustificationType(juce::Justification::centredLeft);
                    label->setBorderSize({});
    
                    comp = label;
                    break;
                }
    
                case TabFactory::TabItem::ItemCloseButton:
                {
                    auto *button    = new juce::TextButton("itemCloseButton");
                    button->onClick = [this]() { onClosing(*this); };
                    button->addMouseListener(this, false);
    
                    comp = button;
                    break;
                }
    
                case TabFactory::TabItem::ItemPinButton:
                {
                    auto *button          = new juce::ToggleButton("itemPinButton");
                    button->onStateChange = [this, button]()
                    {
                        const bool state = button->getToggleState();
                        
                        if (state != pinned)
                        {
                            onPinning(*this, state);
                        }
                        
                        setPinned(state);
                    };
                    button->addMouseListener(this, false);
    
                    comp = button;
                    break;
                }
            }
    
            comp->setLookAndFeel(buttonProxyLaf.get());
            addAndMakeVisible(comp);
            layoutComponents.emplace_back(comp);
        }
        
        setButtonText(parText);
        updateTabSize(parStyle.tabButtonItemHeight);
    }
    
    MultiPagePane::TabButton::~TabButton()
    {
        for (auto &child : getChildren())
        {
            child->setLookAndFeel(nullptr);
        }
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::paintButton(juce::Graphics &g, bool mouseOver, bool mouseDown)
    {
        lookAndFeel->drawMultiTabPaneTabBackground(g, *this, mouseOver, mouseDown);
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::mouseDown(const juce::MouseEvent &e)
    {
        if (e.mods.isLeftButtonDown() && e.eventComponent == this)
        {
            if (!isActive())
            {
                onActivating(*this);
            }
    
            setActive(true);
        }
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::setActive(bool shouldBeActive)
    {
        if (std::exchange(active, shouldBeActive) != shouldBeActive)
        {
            repaint();
        }
    }
    
    bool MultiPagePane::TabButton::isActive() const noexcept
    {
        return active;
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::setPinned(bool pin)
    {
        if (std::exchange(pinned, pin) != pin)
        {
            const auto &layouts = factory.getTabLayout();
        
            for (int i = 0; i < static_cast<int>(layouts.size()); ++i)
            {
                if (layouts.at(static_cast<SizeTypes::Vector>(i)).itemType == TabFactory::TabItem::ItemPinButton)
                {
                    static_cast<juce::ToggleButton*>(getChildComponent(i))
                        ->setToggleState(pin, juce::dontSendNotification);
                }
            }
            
            repaint();
        }
    }
    
    bool MultiPagePane::TabButton::isPinned() const noexcept
    {
        return pinned;
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::setTabIndex(int index)
    {
        tabIndex = index;
    }
    
    int MultiPagePane::TabButton::getTabIndex() const noexcept
    {
        return tabIndex;
    }
    
    //==================================================================================================================
    void MultiPagePane::TabButton::updateTabSize(int itemHeight)
    {
        const std::vector<TabFactory::TabItemLayout> &layout = factory.getTabLayout();
        int prev_right = 0;
        
        for (int i = 0; i < static_cast<int>(layout.size()); ++i)
        {
            const TabFactory::TabItemLayout &item = layout.at(static_cast<SizeTypes::Vector>(i));
            juce::Component &component            = *getChildComponent(i);
            int item_width;
            
            if (item.itemType == TabFactory::TabItem::ItemImage)
            {
                const juce::Image img = static_cast<juce::ImageComponent &>(component).getImage(); // NOLINT
                item_width            = img.isValid() ? img.getWidth() / img.getHeight() * itemHeight : 0;
            }
            else if (item.itemType == TabFactory::TabItem::ItemText)
            {
                item_width = static_cast<int>(std::ceil(lookAndFeel->getMultiTabPaneFont()
                                                                    .getStringWidthFloat(getButtonText())));
            }
            else
            {
                item_width = itemHeight;
            }
            
            const Margin &margin = item.itemMargin;
            component.setBounds(margin.left + prev_right, margin.top, item_width, itemHeight);
            prev_right = component.getRight() + margin.right;
        }
        
        tabWidth = prev_right;
    }
    
    void MultiPagePane::TabButton::updateTabHeight(int tabHeight)
    {
        setSize(tabWidth, tabHeight);
    }
    //==================================================================================================================
    // endregion TabButton
    //******************************************************************************************************************
    //==================================================================================================================
    //==================================================================================================================
    // endregion TabButton
    //******************************************************************************************************************
    // region DesktopTabPane
    //==================================================================================================================
    MultiPagePane::Window::Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> parFactory,
                                  Style parStyle, Options parOptions)
        : ResizableWindow(name, false)
    {
        JAUT_INIT_LAF();
        
        auto pane = std::make_unique<MultiPagePane>(std::move(parFactory), parStyle, parOptions);
        pane->LastTabClosing += makeHandler(this, &Window::forceToCloseWindow);
        setContentOwned(pane.release(), false);
        
        setSize(400, 250);
        setUsingNativeTitleBar(true);
        setOpaque(true);
    }
    
    MultiPagePane::Window::Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> parFactory,
                                  Style parStyle)
        : Window(name, std::move(parFactory), parStyle, {})
    {}
    
    MultiPagePane::Window::Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> parFactory,
                                  Options parOptions)
        : Window(name, std::move(parFactory), {}, parOptions)
    {}
    
    MultiPagePane::Window::Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> parFactory)
        : Window(name, std::move(parFactory), {}, {})
    {}
    
    //==================================================================================================================
    void MultiPagePane::Window::resized()
    {
        ResizableWindow::resized();
        getContentComponent()->setBounds(getLocalBounds());
    }
    
    //==================================================================================================================
    int MultiPagePane::Window::getDesktopWindowStyleFlags() const
    {
        return lookAndFeel->getWindowStyleFlags();
    }
    
    MultiPagePane &MultiPagePane::Window::getMultiPagePane() noexcept
    {
        return *static_cast<MultiPagePane*>(getContentComponent());
    }
    
    //==================================================================================================================
    void MultiPagePane::Window::userTriedToCloseWindow()
    {
        WindowClosing(this);
    }
    
    //==================================================================================================================
    void MultiPagePane::Window::forceToCloseWindow(const juce::String&)
    {
        removeFromDesktop();
        userTriedToCloseWindow();
    }
    //==================================================================================================================
    // endregion DesktopTabPane
    //******************************************************************************************************************
    // region MultiPagePane
    //==================================================================================================================
    MultiPagePane::MultiPagePane(FactoryPtr parFactory)
        : MultiPagePane(std::move(parFactory), {}, {})
    {}
    
    MultiPagePane::MultiPagePane(FactoryPtr parFactory, Style parStyle)
        : MultiPagePane(std::move(parFactory), parStyle, {})
    {}
    
    MultiPagePane::MultiPagePane(FactoryPtr parFactory, Options parOptions)
        : MultiPagePane(std::move(parFactory), {}, parOptions)
    {}
    
    MultiPagePane::MultiPagePane(FactoryPtr parFactory, Style parStyle, Options parOptions)
        : tabBar(std::make_unique<InternalTabBar>()), factory(std::move(parFactory)),
          style(parStyle), options(parOptions), pageView(ContentPane::Style { 0, 0 })
    {
        JAUT_INIT_LAF()
        
        // The TabFactory instance can't be null
        jassert(this->factory != nullptr);
        // A tab's size-properties can't go below 0
        jassert(parStyle.tabButtonHeight >= 0 && parStyle.tabButtonItemHeight >= 0);
        
        // Tab style
        tabBar->setNewTabHeight    (parStyle.tabButtonHeight);
        tabBar->setHiddenTabsMargin(parStyle.moreTabsButtonMargin);
        
        // Tab options
        tabBar->setPinBehaviour   (::hasPinFlag(parOptions, PinBehaviour::PutInExtraRow));
        tabBar->setTabsReorderable(parOptions.allowTabReordering);
        
        tabBar->setTabsCanBeExchanged(parOptions.allowDragExchange);
        tabBar->setTabsCanBeExtracted(parOptions.allowPageExtraction);
        
        // Tab callbacks
        tabBar->onTabClosed = [this](const juce::String &pageId)
        {
            if (pageView.getCurrentComponent() == pages.at(pageId).get())
            {
                pageView.resetComponent();
            }
            
            const bool was_last = (pages.size() == 1);
            
            pages.erase(pageId);
            resized(); // NOLINT
        
            TabClosed(pageId);
        
            if (was_last)
            {
                LastTabClosing.invoke(pageId);
                
                if (backgroundComponent)
                {
                    pageView.setComponent(*backgroundComponent);
                }
            }
        };
        
        tabBar->onTabChanged = [this](const juce::String &pageId)
        {
            pageView.setComponent(*pages.at(pageId));
            TabChanged(pageId);
        };
        
        tabBar->onTabPinned = [this](const juce::String &pageId, bool state)
        {
            if (::hasPinFlag(options, PinBehaviour::PutInExtraRow))
            {
                resized(); // NOLINT
            }
        
            TabPinStateChanged(pageId, state);
        };
        
        tabBar->onTabDetached = [this](const juce::String &pageId)
        {
            if (pageView.getCurrentComponent() == pages.at(pageId).get())
            {
                pageView.resetComponent();
            }
            
            PagePtr page        = std::move(pages.at(pageId));
            const bool was_last = (pages.size() == 1);
            
            pages.erase(pageId);
            resized(); // NOLINT
            
            TabClosed(pageId);
        
            if (was_last)
            {
                LastTabClosing(pageId);
        
                if (backgroundComponent)
                {
                    pageView.setComponent(*backgroundComponent);
                }
            }
            
            return page;
        };
        // Add components
        addAndMakeVisible(*tabBar);
        addAndMakeVisible(pageView);
    }
    
    MultiPagePane::~MultiPagePane() = default;
    
    //==================================================================================================================
    void MultiPagePane::paint(juce::Graphics &g)
    {
        lookAndFeel->drawMultiTabPaneBackground(g, *this);
    }
    
    void MultiPagePane::paintOverChildren(juce::Graphics &g)
    {
        lookAndFeel->drawMultiTabPaneBorder(g, *this);
        
        if (!paintDropBox)
        {
            return;
        }
        
        g.setColour(juce::Colours::white);
        g.setOpacity(0.2f);
        
        if (pages.empty())
        {
            g.fillAll();
        }
        else
        {
            g.fillRect(tabBar->getBounds());
        }
    }
    
    void MultiPagePane::resized()
    {
        initialised = true;
        
        const juce::Rectangle<int> prev_bounds = tabBar->getBounds();
        const TabBarLayout layout       = style.tabBarLayout;
        updateSpaceBounds(layout);
        
        if (layout == TabBarLayout::Top || layout == TabBarLayout::Bottom)
        {
            tabBar->setBounds(style.tabBarMargin.trimRectangle(tabSpaceBounds));
        }
        else
        {
            constexpr float rot_up = juce::float_Pi * 1.5f;
            constexpr float rot_dn = juce::float_Pi * 0.5f;
            
            const Margin &tab_margin = style.tabBarMargin;
            juce::Rectangle<int> tab_rect { tabSpaceBounds.getHeight() - tab_margin.top  - tab_margin.bottom,
                                            tabSpaceBounds.getWidth()  - tab_margin.left - tab_margin.right };
            
            if (layout == TabBarLayout::SideLeft)
            {
                tab_rect.setX(tabSpaceBounds.getX() + tab_margin.left);
                
                const int req_width = tabBar->getRequiredWidth();
                
                if (req_width > tab_rect.getWidth())
                {
                    tab_rect.setY(tabSpaceBounds.getY() + tab_margin.top + tab_rect.getWidth());
                }
                else
                {
                    tab_rect.setY(tabSpaceBounds.getY() + tab_margin.top + req_width);
                }
                
                tabBar->setTransform(juce::AffineTransform::rotation(rot_up, static_cast<float>(tab_rect.getX()),
                                                                             static_cast<float>(tab_rect.getY())));
            }
            else
            {
                tab_rect.setX(tabSpaceBounds.getRight() - tab_margin.right);
                tab_rect.setY(tabSpaceBounds.getY()     + tab_margin.top);
        
                tabBar->setTransform(juce::AffineTransform::rotation(rot_dn, static_cast<float>(tab_rect.getX()),
                                                                             static_cast<float>(tab_rect.getY())));
            }
        
            tabBar->setBounds(tab_rect);
        }
        
        pageView.setBounds(style.pageViewMargin.trimRectangle(pageSpaceBounds));
        
        if (prev_bounds == tabBar->getBounds())
        {
            tabBar->resized();
        }
    }
    
    //==================================================================================================================
    void MultiPagePane::addPage(juce::OptionalScopedPointer<juce::Component> component, const juce::String &id,
                                const juce::String &name)
    {
        jassert(factory != nullptr);
        const juce::String page_id = id.trim().toLowerCase();
        
        if (pages.find(page_id) == pages.end())
        {
            if (!component)
            {
                return;
            }
            
            if (auto *parent = component->getParentComponent())
            {
                parent->removeChildComponent(component.get());
            }
            
            component->setName(name.isEmpty() ? page_id : name);
            pages.emplace(page_id, std::move(component));
            tabBar->addTab(style, *factory, id, !name.isEmpty() ? name : id);
            
            resized();
            TabOpened(id);
        }
        else
        {
            openPage(page_id);
        }
    }
    
    void MultiPagePane::closePage(int index)
    {
        if (fit(index, 0, getNumPages()))
        {
            tabBar->removeTab(index);
        }
    }
    
    void MultiPagePane::closePage(const juce::String &pageId)
    {
        closePage(getIndexOf(pageId));
    }
    
    void MultiPagePane::openPage(int index)
    {
        if (index != tabBar->getActiveIndex() && fit(index, 0, getNumPages()))
        {
            tabBar->activateTab(index);
        }
    }
    
    void MultiPagePane::openPage(const juce::String &pageId)
    {
        openPage(getIndexOf(pageId));
    }
    
    void MultiPagePane::pinPage(int index, bool shouldBePinned)
    {
        if (fit(index, 0, getNumPages()) && getTab(index)->isPinned() != shouldBePinned)
        {
            tabBar->pinTab(index, shouldBePinned);
        }
    }
    
    void MultiPagePane::pinPage(const juce::String &pageId, bool shouldBePinned)
    {
        pinPage(getIndexOf(pageId), shouldBePinned);
    }
    
    MultiPagePane::PagePtr MultiPagePane::detachPage(int index)
    {
        if (fit(index, 0, getNumPages()))
        {
            return tabBar->detachTab(index);
        }
        
        return {};
    }
    
    MultiPagePane::PagePtr MultiPagePane::detachPage(const juce::String &pageId)
    {
        return detachPage(getIndexOf(pageId));
    }
    
    void MultiPagePane::closeAllPages(bool forcePinned)
    {
        if (!::hasPinFlag(options, PinBehaviour::ClosingAllNoEffect) || forcePinned)
        {
            for (int i = 0; i < getNumPages(); ++i)
            {
                closePage(i);
            }
        }
        else
        {
            for (int i = 0; i < getNumPages(); ++i)
            {
                const auto &tab = getTab(i);
                
                if (!tab->isPinned())
                {
                    closePage(i);
                }
            }
        }
    }
    
    //==================================================================================================================
    int MultiPagePane::getNumPages() const noexcept
    {
        return static_cast<int>(pages.size());
    }
    
    bool MultiPagePane::hasAnyPages() const noexcept
    {
        return !pages.empty();
    }
    
    bool MultiPagePane::hasPinnedPages() const noexcept
    {
        return tabBar->hasPinnedTabs();
    }
    
    //==================================================================================================================
    juce::Component* MultiPagePane::getBackgroundComponent() noexcept
    {
        return backgroundComponent.get();
    }
    
    void MultiPagePane::setBackgroundComponent(std::nullptr_t)
    {
        pageView.resetComponent();
        backgroundComponent.reset();
    }
    
    void MultiPagePane::setBackgroundComponent(juce::OptionalScopedPointer<juce::Component> component)
    {
        if (component == backgroundComponent.get())
        {
            return;
        }
        
        if (pages.empty())
        {
            pageView.setComponent({ component.get(), false });
        }
        
        backgroundComponent = std::move(component);
    }
    
    void MultiPagePane::setBackgroundComponent(juce::Component &component)
    {
        setBackgroundComponent({ &component, false });
    }
    
    //==================================================================================================================
    juce::String MultiPagePane::getIdAt(int index) const noexcept
    {
        if (const TabButton *const tab = getTab(index))
        {
            return tab->getName();
        }
        
        return {};
    }
    
    int MultiPagePane::getIndexOf(const juce::String &pageId) const noexcept
    {
        if (const TabButton *const tab = getTab(pageId))
        {
            return tab->getTabIndex();
        }
        
        return -1;
    }
    
    //==================================================================================================================
    const MultiPagePane::TabButton* MultiPagePane::getTab(int index) const noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        return tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index)).get();
    }
    
    juce::Component* MultiPagePane::getPage(int index) noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        const juce::String id = tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index))->getName();
        return pages.at(id).get();
    }
    
    const juce::Component* MultiPagePane::getPage(int index) const noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        const juce::String id = tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index))->getName();
        return pages.at(id).get();
    }
    
    const MultiPagePane::TabButton* MultiPagePane::getTab(const juce::String &pageId) const noexcept
    {
        for (auto &tab : tabBar->getAllTabs())
        {
            if (tab->getName().equalsIgnoreCase(pageId.trim()))
            {
                return tab.get();
            }
        }
        
        return nullptr;
    }
    
    juce::Component* MultiPagePane::getPage(const juce::String &pageId) noexcept
    {
        const auto it = pages.find(pageId.trim().toLowerCase());
        return it != pages.end() ? it->second.get() : nullptr;
    }
    
    const juce::Component* MultiPagePane::getPage(const juce::String &pageId) const noexcept
    {
        const auto it = pages.find(pageId.trim().toLowerCase());
        return it != pages.end() ? it->second.get() : nullptr;
    }
    
    //==================================================================================================================
    int MultiPagePane::getActiveIndex() const noexcept
    {
        return tabBar->getActiveIndex();
    }
    
    juce::String MultiPagePane::getActiveId() const noexcept
    {
        const TabButton *tab = getTab(getActiveIndex());
        return tab ? tab->getName() : juce::String();
    }
    
    const MultiPagePane::TabButton* MultiPagePane::getActiveTab() const noexcept
    {
        return getTab(getActiveIndex());
    }
    
    juce::Component* MultiPagePane::getActivePage() noexcept
    {
        return hasAnyPages() ? pageView.getCurrentComponent() : nullptr;
    }
    
    const juce::Component* MultiPagePane::getActivePage() const noexcept
    {
        return hasAnyPages() ? pageView.getCurrentComponent() : nullptr;
    }
    
    //==================================================================================================================
    void MultiPagePane::setContentPadding(Padding padding)
    {
        if (style.contentPadding != std::exchange(style.contentPadding, padding) && initialised)
        {
            resized();
        }
    }
    
    void MultiPagePane::setBorderThickness(Thickness<int> thickness)
    {
        if (style.borderThickness != std::exchange(style.borderThickness, thickness) && initialised)
        {
            resized();
        }
    }
    
    void MultiPagePane::setTabBarMargin(Margin margin)
    {
        if (style.tabBarMargin != std::exchange(style.tabBarMargin, margin) && initialised)
        {
            resized();
        }
    }
    
    void MultiPagePane::setPageViewMargin(Margin margin)
    {
        if (style.pageViewMargin != std::exchange(style.pageViewMargin, margin) && initialised)
        {
            resized();
        }
    }
    
    void MultiPagePane::setMoreTabsButtonMargin(Margin margin)
    {
        if (style.moreTabsButtonMargin != std::exchange(style.moreTabsButtonMargin, margin))
        {
            tabBar->setHiddenTabsMargin(style.moreTabsButtonMargin);
            
            if (initialised)
            {
                resized();
            }
        }
    }
    
    void MultiPagePane::setTabBarLayout(TabBarLayout layout)
    {
        if (style.tabBarLayout != std::exchange(style.tabBarLayout, layout))
        {
            if (style.tabBarLayout == TabBarLayout::Top || style.tabBarLayout == TabBarLayout::Bottom)
            {
                tabBar->setTransform(juce::AffineTransform());
            }
            
            if (initialised)
            {
                resized();
            }
        }
    }
    
    void MultiPagePane::setTabButtonItemHeight(int newHeight)
    {
        // A tab's size-properties can't go below 0
        jassert(newHeight >= 0);
        
        if (style.tabButtonItemHeight != std::exchange(style.tabButtonItemHeight, std::max(0, newHeight)))
        {
            tabBar->setNewItemHeight(newHeight);
            
            if (initialised)
            {
                resized();
            }
        }
    }
    
    void MultiPagePane::setTabButtonHeight(int newHeight)
    {
        // A tab's size-properties can't go below 0
        jassert(newHeight >= 0);
        
        if (style.tabButtonHeight != std::exchange(style.tabButtonHeight, std::max(0, newHeight)))
        {
            tabBar->setNewTabHeight(newHeight);
            
            if (initialised)
            {
                resized();
            }
        }
    }
    
    //==================================================================================================================
    void MultiPagePane::setTabPinBehaviour(PinBehaviour newBehaviour)
    {
        const auto old_behaviour  = std::exchange(options.pinnedTabBehaviour, newBehaviour);
        const bool pin_in_new_tab = ::hasPinFlag(options, PinBehaviour::PutInExtraRow);
    
        if (((old_behaviour & PinBehaviour::PutInExtraRow) == PinBehaviour::PutInExtraRow) != pin_in_new_tab)
        {
            tabBar->setPinBehaviour(pin_in_new_tab);
            
            if (initialised)
            {
                resized();
            }
        }
    }
    
    void MultiPagePane::setAllowTabReordering(bool allow)
    {
        if (options.allowTabReordering != std::exchange(options.allowTabReordering, allow))
        {
            tabBar->setTabsReorderable(allow);
        }
    }
    
    void MultiPagePane::setAllowDragExchange(bool allow)
    {
        if (options.allowDragExchange != std::exchange(options.allowDragExchange, allow))
        {
            tabBar->setTabsCanBeExchanged(allow);
        }
    }
    
    void MultiPagePane::setAllowPageExtraction(bool allow)
    {
        if (options.allowPageExtraction != std::exchange(options.allowPageExtraction, allow))
        {
            tabBar->setTabsCanBeExtracted(allow);
        }
    }
    
    //==================================================================================================================
    void MultiPagePane::setStyle(const Style &parStyle)
    {
        const Style old_style = std::exchange(style, parStyle);
        bool needs_resize     = false;
        
        if (   old_style.tabBarMargin    != parStyle.tabBarMargin
            || old_style.contentPadding  != parStyle.contentPadding)
        {
            needs_resize = true;
        }
        
        if (old_style.borderThickness != parStyle.borderThickness)
        {
            repaint();
            needs_resize = true;
        }
        
        if (old_style.tabButtonItemHeight != parStyle.tabButtonItemHeight)
        {
            // A tab's size-properties can't go below 0
            jassert(parStyle.tabButtonItemHeight >= 0);
            
            tabBar->setNewItemHeight(parStyle.tabButtonItemHeight);
            needs_resize = true;
        }
        
        if (old_style.tabButtonHeight != parStyle.tabButtonHeight)
        {
            // A tab's size-properties can't go below 0
            jassert(parStyle.tabButtonHeight >= 0);
            
            tabBar->setNewTabHeight(parStyle.tabButtonHeight);
            needs_resize = true;
        }
        
        if (old_style.tabBarLayout != parStyle.tabBarLayout)
        {
            if (parStyle.tabBarLayout == TabBarLayout::Top || parStyle.tabBarLayout == TabBarLayout::Bottom)
            {
                tabBar->setTransform(juce::AffineTransform());
            }
        
            needs_resize = true;
        }
        
        if (old_style.moreTabsButtonMargin != parStyle.moreTabsButtonMargin)
        {
            tabBar->setHiddenTabsMargin(parStyle.moreTabsButtonMargin);
            needs_resize = true;
        }
        
        if (needs_resize)
        {
            if (initialised)
            {
                resized();
            }
        }
        else if (old_style.pageViewMargin != parStyle.pageViewMargin)
        {
            pageView.setBounds(parStyle.pageViewMargin.trimRectangle(pageSpaceBounds));
        }
    }
    
    void MultiPagePane::setOptions(const Options &parOptions)
    {
        const Options old_options = std::exchange(options, parOptions);
        const bool pin_in_new_tab = ::hasPinFlag(parOptions, PinBehaviour::PutInExtraRow);
        
        if (::hasPinFlag(old_options, PinBehaviour::PutInExtraRow) != pin_in_new_tab)
        {
            tabBar->setPinBehaviour(pin_in_new_tab);
            
            if (initialised)
            {
                resized();
            }
        }
        
        tabBar->setTabsReorderable   (parOptions.allowTabReordering);
        tabBar->setTabsCanBeExchanged(parOptions.allowDragExchange);
        tabBar->setTabsCanBeExtracted(parOptions.allowPageExtraction);
    }
    
    //==================================================================================================================
    juce::Rectangle<int> MultiPagePane::getTabSpaceBounds() const noexcept
    {
        return tabSpaceBounds;
    }
    
    juce::Rectangle<int> MultiPagePane::getPageSpaceBounds() const noexcept
    {
        return pageSpaceBounds;
    }
    
    //==================================================================================================================
    void MultiPagePane::itemDragEnter(const SourceDetails&)
    {
        paintDropBox = true;
        
        if (pages.empty())
        {
            repaint();
        }
        else
        {
            repaint(tabBar->getBounds());
        }
    }
    
    void MultiPagePane::itemDragExit(const SourceDetails&)
    {
        paintDropBox = false;
        repaint();
    }
    
    bool MultiPagePane::isInterestedInDragSource(const SourceDetails &dragSourceDetails)
    {
        if (const juce::DynamicObject *const json_cif = dragSourceDetails.description.getDynamicObject())
        {
            if (json_cif->getProperty("drag_id").toString() != DragAndDrop_ID)
            {
                return false;
            }
            
            if (const juce::DynamicObject *const tab_info = json_cif->getProperty("tab_info").getDynamicObject())
            {
                const juce::String id = tab_info->getProperty("page_id");
        
                if (!id.isEmpty())
                {
                    for (const auto &tab : tabBar->getAllTabs())
                    {
                        const juce::String &tab_id = tab->getName();
        
                        if (tab_id == id)
                        {
                            return false;
                        }
                    }
    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void MultiPagePane::itemDropped(const SourceDetails &dragSourceDetails)
    {
        const juce::DynamicObject *const tab_info = dragSourceDetails.description.getProperty("tab_info", {})
                                                                     .getDynamicObject();
        const juce::String page_id   = tab_info->getProperty("page_id");
        const juce::String page_name = tab_info->getProperty("name");
        
        MultiPagePane &other = *static_cast<MultiPagePane*>(dragSourceDetails.sourceComponent.get());
        pages.emplace(page_id, other.detachPage(page_id));
        tabBar->addTab(style, *factory, page_id, page_name);
        
        paintDropBox = false;
        resized();
        repaint();
    }
    
    //==================================================================================================================
    void MultiPagePane::updateSpaceBounds(TabBarLayout layout)
    {
        const Thickness<int> &border      = style.borderThickness;
        const Padding        &top_padding = style.contentPadding;
        const Margin         &tab_margin  = style.tabBarMargin;
        const int             tab_height  = style.tabButtonHeight * ((tabBar->hasPinnedTabs()
                                                                          && tabBar->hasNonPinnedTabs()) + 1);
        
        if (layout == TabBarLayout::Top || layout == TabBarLayout::Bottom)
        {
            const int width  = getWidth()     - top_padding.right - top_padding.left - border.left - border.right;
            const int height = tab_margin.top + tab_margin.bottom + tab_height;
            
            tabSpaceBounds .setSize(width, height);
            pageSpaceBounds.setSize(width, getHeight() - height - top_padding.top - top_padding.bottom
                                                                - border.top      - border.bottom);
            
            const int left = top_padding.left + border.left;
            
            if (layout == TabBarLayout::Top)
            {
                tabSpaceBounds .setPosition(left, top_padding.top + border.top);
                pageSpaceBounds.setPosition(left, tabSpaceBounds.getBottom());
            }
            else
            {
                pageSpaceBounds.setPosition(left, top_padding.top + border.top);
                tabSpaceBounds .setPosition(left, pageSpaceBounds.getBottom());
            }
        }
        else
        {
            const int width  = tab_margin.left + tab_margin.right + tab_height;
            const int height = getHeight()     - top_padding.top  - top_padding.bottom - border.top - border.bottom;
    
            tabSpaceBounds .setSize(width, height);
            pageSpaceBounds.setSize(getWidth() - width - top_padding.left - top_padding.right
                                                       - border.left      - border.right, height);
    
            const int top = top_padding.top + border.top;
    
            if (layout == TabBarLayout::SideLeft)
            {
                tabSpaceBounds .setPosition(top_padding.left + border.left, top);
                pageSpaceBounds.setPosition(tabSpaceBounds.getRight(),      top);
            }
            else
            {
                pageSpaceBounds.setPosition(top_padding.left + border.left, top);
                tabSpaceBounds .setPosition(pageSpaceBounds.getRight(),     top);
            }
        }
    }
    //==================================================================================================================
    // endregion MultiPagePane
    //******************************************************************************************************************
    //==================================================================================================================
    //==================================================================================================================
    // endregion MultiPagePane
    //******************************************************************************************************************
    // region DockingPane
    //==================================================================================================================
    DockingPane::DockingPane(jaut::SplitPane::Style style, jaut::SplitPane::Options options)
        : SplitPane(style, options),
          OverlayTarget(*this, OverlayTarget::allBlocks)
    {
        setCollapseMode(SplitPane::CollapseMode::Hide);
    }
    
    //==================================================================================================================
    void DockingPane::dockComponent(juce::OptionalScopedPointer<juce::Component> component, Anchor anchor)
    {
        /*
        if (!component)
        {
            return;
        }
        
        const auto &panel1 = getComponent(SplitPane::PanelId::Panel1);
        
        if (!panel1)
        {
            setComponent(std::move(component), SplitPane::PanelId::Panel1);
        }
        else
        {
            if (panel1 == component.get())
            {
                return;
            }
            
            if (anchor == Anchor::Fill)
            {
                if (auto *target = dynamic_cast<OverlayTarget*>(panel1))
                {
                    auto root_obj = std::make_unique<juce::DynamicObject>();
                    juce::var var = ;
                    
                    const juce::DragAndDropTarget::SourceDetails details {
                        var, component.release(), { 0, 0 }
                    };
                    
                    target->itemDropped(details);
                }
            }
            else if (!getComponent(SplitPane::PanelId::Panel2))
            {
                auto dock_1 = std::make_unique<DockingPane>(getStyle(), getOptions());
                auto dock_2 = std::make_unique<DockingPane>(getStyle(), getOptions());
                dock_1->dockComponent(releaseComponent(SplitPane::PanelId::Panel1), Anchor::Left);
                dock_2->dockComponent(std::move(component),                         Anchor::Left);
    
                setComponent(juce::OptionalScopedPointer<juce::Component>(std::move(dock_1)), PanelId::Panel1);
                setComponent(juce::OptionalScopedPointer<juce::Component>(std::move(dock_2)), PanelId::Panel2);
    
                switch (anchor)
                {
                    case Anchor::Left:
                        setOrientation(SplitPane::Orientation::Vertical);
                        swapPanes();
                        break;
                    case Anchor::Right:
                        setOrientation(SplitPane::Orientation::Vertical);
                        break;
                    case Anchor::Top:
                        setOrientation(SplitPane::Orientation::Horizontal);
                        swapPanes();
                        break;
                    case Anchor::Bottom:
                        setOrientation(SplitPane::Orientation::Horizontal);
                        break;
                    default: break;
                }
            }
        }
         */
    }
    
    juce::OptionalScopedPointer<juce::Component> DockingPane::undockComponent(SplitPane::PanelId panelId)
    {
        return {}; //return splitPane.releaseComponent(panelId);
    }
    
    //==================================================================================================================
    // endregion DockingPane
    //******************************************************************************************************************
    // region CoordinatePane
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region ValueStrip
    //==================================================================================================================
    template class CoordinatePane::ValueStrip<false>;
    template class CoordinatePane::ValueStrip<true>;
    
    //==================================================================================================================
    template<bool Vertical>
    CoordinatePane::ValueStrip<Vertical>::ValueStrip()
        : points(::getPoints(range, 0, mult))
    {}
    
    //==================================================================================================================
    template<bool Vertical>
    bool CoordinatePane::ValueStrip<Vertical>::setRange(juce::Range<double> newRange)
    {
        if (range == newRange)
        {
            return false;
        }
        
        if (newRange.getStart() == newRange.getEnd())
        {
            // You can't show zero coordinates on a cartesian component, make sure there is at least a 1e-7 difference
            // between the two points
            jassertfalse;
            return false;
        }
        
        std::swap(range, newRange);
        centreIndex = -1;
        
        if constexpr (!Vertical)
        {
            points = ::getPoints(range, area.getWidth(), mult);
        }
        else
        {
            points = ::getPoints(range, area.getHeight(), mult);
        }
        
        if (!points.empty())
        {
            tail = std::abs(range.getStart() - points[0])                 / mult;
            head = std::abs(range.getEnd()   - points[points.size() - 1]) / mult;
        }
        else
        {
            tail         = 0.0;
            head         = 0.0;
            mult         = 1.0;
            tickDistance = 0;
            
            return true;
        }
        
        if constexpr (!Vertical)
        {
            tickDistance = area.getWidth() / ((points.size() - 1) + tail + head);
        }
        else
        {
            tickDistance = area.getHeight() / ((points.size() - 1) + tail + head);
            std::reverse(points.begin(), points.end());
        }
        
        juce::String debug = "Points [";
        
        for (auto point : points)
        {
            debug << point << ", ";
        }
        
        for (int i = 0; i < points.size(); ++i)
        {
            if (std::abs(points[i]) < ::zeroPivot)
            {
                centreIndex = i;
            }
        }
        
        return true;
    }
    
    //==================================================================================================================
    template<bool Vertical>
    bool CoordinatePane::ValueStrip<Vertical>::setBounds(juce::Rectangle<int> newArea)
    {
        if (area == newArea)
        {
            return false;
        }
        
        std::swap(area, newArea);
        centreIndex = -1;
        
        if constexpr (!Vertical)
        {
            points = ::getPoints(range, area.getWidth(), mult);
        }
        else
        {
            points = ::getPoints(range, area.getHeight(), mult);
        }
        
        if (!points.empty())
        {
            tail = std::abs(range.getStart() - points[0])                 / mult;
            head = std::abs(range.getEnd()   - points[points.size() - 1]) / mult;
        }
        else
        {
            tail         = 0.0;
            head         = 0.0;
            mult         = 1.0;
            tickDistance = 0;
            
            return true;
        }
        
        if constexpr (!Vertical)
        {
            tickDistance = area.getWidth() / ((points.size() - 1) + tail + head);
        }
        else
        {
            tickDistance = area.getHeight() / ((points.size() - 1) + tail + head);
            std::reverse(points.begin(), points.end());
        }
        
        for (int i = 0; i < points.size(); ++i)
        {
            if (std::abs(points[i]) < ::zeroPivot)
            {
                centreIndex = i;
                break;
            }
        }
        
        return true;
    }
    
    //==================================================================================================================
    template<bool Vertical>
    void CoordinatePane::ValueStrip<Vertical>::draw(juce::Graphics &g, int linePos)
    {
        if (points.empty())
        {
            return;
        }
        
        static const juce::Colour baseColour           = juce::Colours::white;
        static const juce::Colour colourText           = baseColour;
        static const juce::Colour colourTextOutOfRange = baseColour.withAlpha(0.5f);
        static const juce::Colour colourTick           = baseColour.withAlpha(0.2f);
        static const juce::Colour colourTickZero       = baseColour;
        static const juce::Colour colourTickQuarter    = baseColour.withAlpha(0.05f);
        
        const juce::Font font_desc = g.getCurrentFont();
        
        const double tick_start = getTickStart();
        const double tick_quart = tickDistance / 5.0;
        
        for (int i = 0; i < points.size(); ++i)
        {
            const double tick_value  = points[i];
            const double tick_offset = tickDistance * i;
            const double tick_pos    = tick_start + tick_offset;
            
            const juce::String tick_text(tick_value);
            const float text_width = font_desc.getStringWidthFloat(tick_text);
            
            // Draw lines and numbers
            if (std::abs(tick_value) >= ::zeroPivot)
            {
                const int text_size = text_width;
                
                
                
                // ==============
                // Ticks
                // ==============
                g.setColour(colourTick);
                
                if constexpr (!Vertical)
                {
                    g.drawRect(tick_pos, area.getY(), 1, area.getHeight());
                }
                else
                {
                    g.drawRect(area.getX(), tick_pos, area.getWidth(), 1);
                }
                
                
                
                // ==============
                // Values
                // ==============
                int text_x;
                int text_y;
                int align_flags;
                
                if constexpr (!Vertical)
                {
                    const int top_font_pos = (font_desc.getHeight() + ::textPadding);
                    text_x = tick_pos - (text_size / 2);
                    text_y = std::clamp(linePos + ::textPadding, area.getY() + ::textPadding,
                                        area.getBottom() - top_font_pos);
                    
                    if (linePos >= area.getY())
                    {
                        g.setColour((linePos < area.getBottom()) ? colourText : colourTextOutOfRange);
                        align_flags = juce::Justification::centredBottom;
                    }
                    else
                    {
                        g.setColour(colourTextOutOfRange);
                        align_flags = juce::Justification::centredTop;
                    }
                }
                else
                {
                    const int min_x = area.getX() + ::textPadding;
                    
                    text_x = std::clamp(linePos         -  text_size - ::textPadding, min_x,
                                        area.getRight() - (text_size + ::textPadding));
                    text_y = tick_pos - std::round(font_desc.getHeight() / 2.0);
                    
                    if (linePos >= area.getX())
                    {
                        g.setColour((linePos < area.getRight()) ? colourText : colourTextOutOfRange);
                        align_flags = juce::Justification::centredRight;
                    }
                    else
                    {
                        g.setColour(colourTextOutOfRange);
                        align_flags = juce::Justification::centredLeft;
                    }
                }
                
                g.drawText(tick_text, text_x, text_y, text_size, font_desc.getHeight(), align_flags);
            }
            
            // ==============
            // Intermediate ticks
            // ==============
            g.setColour(colourTickQuarter);
            
            if constexpr (!Vertical)
            {
                for (int j = 0; j < 5; ++j)
                {
                    const int qtick_pos = tick_pos + (tick_quart * j);
                    
                    if (qtick_pos >= area.getRight())
                    {
                        break;
                    }
                    
                    g.drawRect(qtick_pos, area.getY(), 1, area.getHeight());
                }
            }
            else
            {
                for (int j = 0; j < 5; ++j)
                {
                    const int qtick_pos = tick_pos + (tick_quart * j);
                    
                    if (qtick_pos >= area.getBottom())
                    {
                        break;
                    }
                    
                    g.drawRect(area.getX(), qtick_pos, area.getWidth(), 1);
                }
            }
        }
        
        g.setColour(colourTickQuarter);
        
        // Draw tail quarter ticks and zero lines
        if constexpr (!Vertical)
        {
            for (int j = 0; j < 5; ++j)
            {
                const int qtick_pos = tick_start - (tick_quart * j);
                
                if (qtick_pos < area.getX())
                {
                    break;
                }
                
                g.drawRect(qtick_pos, area.getY(), 1, area.getHeight());
            }
            
            const int zero_pos = getZeroLinePos();
            
            if (zero_pos >= area.getX() && zero_pos < area.getRight())
            {
                g.setColour(colourTickZero);
                g.drawRect(area.withWidth(1).withX(zero_pos));
            }
        }
        else
        {
            for (int j = 0; j < 5; ++j)
            {
                const int qtick_pos = tick_start - (tick_quart * j);
                
                if (qtick_pos < area.getY())
                {
                    break;
                }
                
                g.drawRect(area.getX(), qtick_pos, area.getWidth(), 1);
            }
            
            const int zero_pos = getZeroLinePos();
            
            if (zero_pos >= area.getY() && zero_pos < area.getBottom())
            {
                g.setColour(colourTickZero);
                g.drawRect(area.withHeight(1).withY(zero_pos));
            }
        }
    }
    
    //==================================================================================================================
    template<bool Vertical>
    int CoordinatePane::ValueStrip<Vertical>::getTickStart() const noexcept
    {
        if constexpr (!Vertical)
        {
            return area.getX() + static_cast<int>(tickDistance * tail);
        }
        else
        {
            return area.getY() + static_cast<int>(tickDistance * head);
        }
    }
    
    template<bool Vertical>
    int CoordinatePane::ValueStrip<Vertical>::getZeroLinePos() const
    {
        if (points.empty())
        {
            return -1;
        }
        
        const double tick_start = getTickStart();
        
        if (centreIndex >= 0)
        {
            return tick_start + (centreIndex * tickDistance);
        }
        else if (points[0] > 0)
        {
            if constexpr (!Vertical)
            {
                const double first_tick      = points[0];
                const int    indices_to_zero = first_tick / mult;
                return tick_start - (tickDistance * indices_to_zero);
            }
            else
            {
                const double first_tick      = points[points.size() - 1];
                const int    indices_to_zero = first_tick / mult;
                return tick_start + ((points.size() - 1) * tickDistance) + (tickDistance * indices_to_zero);
            }
        }
        else
        {
            if constexpr (!Vertical)
            {
                const double last_tick       = points[points.size() - 1];
                const int    indices_to_zero = -last_tick / mult;
                return tick_start + ((points.size() - 1) * tickDistance) + (tickDistance * indices_to_zero);
            }
            else
            {
                const double last_tick       = points[0];
                const int    indices_to_zero = -last_tick / mult;
                return tick_start - (tickDistance * indices_to_zero);
            }
        }
    }
    //==================================================================================================================
    // endregion ValueStrip
    //******************************************************************************************************************
    // region CoordinatePane
    //==================================================================================================================
    CoordinatePane::CoordinatePane()
        : labelX("", "X"),
          labelY("", "Y")
    {
        juce::Font font(12.0f);
        
        labelX.setColour(juce::Label::textColourId, juce::Colours::white);
        labelX.setFont(font);
        addAndMakeVisible(labelX);
        
        labelY.setColour(juce::Label::textColourId, juce::Colours::white);
        labelY.setFont(font);
        addAndMakeVisible(labelY);
    }
    
    //==================================================================================================================
    void CoordinatePane::paint(juce::Graphics &g)
    {
        g.setFont(font_text);
        
        stripX.draw(g, stripY.getZeroLinePos());
        stripY.draw(g, stripX.getZeroLinePos());
        
        const juce::Rectangle<int> graph_area = stripX.area;
        g.reduceClipRegion(graph_area);
        
        const int line_pos_x = stripX.getZeroLinePos();
        const int line_pos_y = stripY.getZeroLinePos();
        
        if (!graph_area.contains(line_pos_x, line_pos_y))
        {
            return;
        }
        
        const int text_width = font_text.getStringWidth("0");
        
        g.setColour(juce::Colours::white);
        g.drawText("0", line_pos_x - (text_width + ::textPadding), line_pos_y + ::textPadding,
                   text_width, font_text.getHeight(), juce::Justification::topRight);
    }
    
    void CoordinatePane::resized()
    {
        const juce::Rectangle<int> content_area = getLocalBounds().reduced(10);
        const juce::Rectangle<int> graph_area   = content_area.withTrimmedLeft(30).withTrimmedBottom(30);
        
        const bool changed_x = stripX.setBounds(graph_area);
        const bool changed_y = stripY.setBounds(graph_area);
        
        if (changed_x || changed_y)
        {
            updateLabelPosition();
            repaint();
        }
    }
    
    //==================================================================================================================
    void CoordinatePane::setXRange(juce::Range<double> range)
    {
        if (stripX.setRange(range))
        {
            updateLabelPosition();
            repaint();
        }
    }
    
    void CoordinatePane::setYRange(juce::Range<double> range)
    {
        if (stripY.setRange(range))
        {
            updateLabelPosition();
            repaint();
        }
    }
    
    //==================================================================================================================
    void CoordinatePane::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel)
    {
        const juce::Rectangle<int> graph_area = stripX.area;
        const auto m_pos = event.getPosition();
        
        if (!graph_area.contains(m_pos))
        {
            return;
        }
        
        const int wheel_delta = wheel.deltaY * 10;
        
        const double mid_x  = graph_area.getCentreX();
        const double mid_y  = graph_area.getCentreY();
        const double x_size = graph_area.getWidth()  / 2.0;
        const double y_size = graph_area.getHeight() / 2.0;
        
        const double left_amount   = 1.0 + ((1.0 / x_size) * (mid_x - m_pos.x));
        const double top_amount    = 1.0 + ((1.0 / y_size) * (mid_y - m_pos.y));
        const double right_amount  = 2.0 - left_amount;
        const double bottom_amount = 2.0 - top_amount;
        
        double amount_x;
        double amount_y;
        
        if (graph_area.getWidth() == graph_area.getHeight())
        {
            amount_x = stripX.mult / 5.0 * -wheel_delta;
            amount_y = stripY.mult / 5.0 * -wheel_delta;
        }
        else if (graph_area.getWidth() > graph_area.getHeight())
        {
            amount_x =  stripX.mult / 5.0 * -wheel_delta;
            amount_y = (stripY.mult / 5.0 * -wheel_delta) / graph_area.getWidth() * graph_area.getHeight();
        }
        else
        {
            amount_x = (stripX.mult / 5.0 * -wheel_delta) / graph_area.getHeight() * graph_area.getWidth();
            amount_y =  stripY.mult / 5.0 * -wheel_delta;
        }
        
        const bool changed_x = stripX.setRange(stripX.range.withStart(stripX.range.getStart() - amount_x * right_amount)
                                                     .withEnd  (stripX.range.getEnd()   + amount_x * left_amount));
        const bool changed_y = stripY.setRange(stripY.range.withStart(stripY.range.getStart() - amount_y * top_amount)
                                                     .withEnd  (stripY.range.getEnd()   + amount_y * bottom_amount));
        
        if (changed_x || changed_y)
        {
            updateLabelPosition();
            repaint();
        }
    }
    
    void CoordinatePane::mouseDown(const juce::MouseEvent &event)
    {
        prevPos    = event.getPosition();
        prevCursor = getMouseCursor();
        setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }
    
    void CoordinatePane::mouseUp(const juce::MouseEvent &event)
    {
        setMouseCursor(prevCursor);
    }
    
    void CoordinatePane::mouseDrag(const juce::MouseEvent &event)
    {
        const juce::Point<int> m_pos = event.getPosition();
        
        if (prevPos != m_pos)
        {
            const int diff_x = prevPos.getX() - m_pos.getX();
            const int diff_y = prevPos.getY() - m_pos.getY();
            
            const double amount_x = (stripX.mult / static_cast<int>(stripX.tickDistance)) * diff_x;
            const double amount_y = (stripY.mult / static_cast<int>(stripY.tickDistance)) * diff_y;
            
            stripX.setRange(stripX.range.withStart(stripX.range.getStart() + amount_x)
                                  .withEnd  (stripX.range.getEnd()   + amount_x));
            stripY.setRange(stripY.range.withStart(stripY.range.getStart() - amount_y)
                                  .withEnd  (stripY.range.getEnd()   - amount_y));
            updateLabelPosition();
            
            prevPos = m_pos;
            repaint();
        }
    }
    
    //==================================================================================================================
    double CoordinatePane::getXValueAt(int x)
    {
        /*
        const juce::Rectangle<int> graph_area = stripX.area;
        
        if (x >= graph_area.getX() && x < graph_area.getRight() && !stripX.points.empty())
        {
            const double tick_size  = stripX.getTickSize(graph_area.getWidth());
            const int    tick_start = stripX.getTickStart(graph_area.getX(), tick_size);
            
            if (x < tick_start)
            {
                const double max_perc_tail = tick_start - graph_area.getX();
                const double perc_of_tail  = max_perc_tail - tick_start - x;
                return stripX.tail / max_perc_tail * perc_of_tail;
            }
            
            int tick_nearest = (x - tick_start) / tick_size;
            const int next   = tick_start + tick_size * (tick_nearest + 1);
            
            if (next <= x && next != tick_nearest)
            {
                ++tick_nearest;
            }
            
            const int    tick_pos   = tick_nearest * tick_size;
            const double tick_value = stripX.points[tick_nearest];
            
            return tick_value + stripX.mult / tick_size * (x - tick_start - tick_pos);
        }*/
        
        return 0.0;
    }
    
    double CoordinatePane::getYValueAt(int y)
    {
        /*
        const juce::Rectangle<int> graph_area = stripX.area;
        
        if (y >= graph_area.getY() && y < graph_area.getBottom() && !stripY.points.empty())
        {
            const double tick_size  = stripX.getTickSize(graph_area.getHeight());
            const int    tick_start = stripX.getTickStart(graph_area.getBottom(), tick_size);
            
            if (y > tick_start)
            {
                const double max_perc_tail = tick_start - graph_area.getX();
                const double perc_of_tail  = max_perc_tail - tick_start - x;
                return stripX.tail / max_perc_tail * perc_of_tail;
            }
            
            int tick_nearest = (x - tick_start) / tick_size;
            const int next   = tick_start + tick_size * (tick_nearest + 1);
            
            if (next <= x && next != tick_nearest)
            {
                ++tick_nearest;
            }
            
            const int    tick_pos   = tick_nearest * tick_size;
            const double tick_value = stripX.points[tick_nearest];
            
            return tick_value + stripX.mult / tick_size * (x - tick_start - tick_pos);
        }
        */
        
        return 0.0;
    }
    
    //==================================================================================================================
    void CoordinatePane::updateLabelPosition()
    {
        const int t_max_w = ::getMaxWidth(font_text, stripY.points);
        
        const juce::Rectangle<int> graph_area = stripX.area;
        const int x_width = labelX.getFont().getStringWidth(labelX.getText()) + 10;
        const int y_width = labelY.getFont().getStringWidth(labelY.getText()) + 10;
        
        const int internal_padding = font_text.getHeight() + ::textPadding;
        
        labelX.setBounds(graph_area.getRight() - x_width,
                         std::clamp<int>(stripY.getZeroLinePos() +  internal_padding,
                                         graph_area.getY()       +  internal_padding,
                                         graph_area.getBottom()  - (internal_padding + font_text.getHeight() + 1)),
                         x_width, labelX.getFont().getHeight());
        
        const int text_margin = (t_max_w + internal_padding + ::textPadding);
        labelY.setBounds(std::clamp<int>(stripX.getZeroLinePos() - text_margin,
                                         graph_area.getX()       + ::textPadding + t_max_w,
                                         graph_area.getRight()   - text_margin),
                         graph_area.getY() + y_width, y_width, labelY.getFont().getHeight());
        labelY.setTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 1.5, labelY.getX(),
                                                            labelY.getY()));
    }
    //==================================================================================================================
    // endregion CoordinatePane
    //******************************************************************************************************************
    //==================================================================================================================
    //==================================================================================================================
    // endregion CoordinatePane
    //******************************************************************************************************************
    // region LookAndFeel_Jaut
    //==================================================================================================================
    LookAndFeel_Jaut::LookAndFeel_Jaut()
    {
        using UICol = ColourScheme::UIColour;
        
        const juce::uint32 colours[] {
            CharFormat::ColourFormat0Id, juce::Colours::black                .getARGB(),
            CharFormat::ColourFormat1Id, juce::Colours::darkblue             .getARGB(),
            CharFormat::ColourFormat2Id, juce::Colours::darkgreen            .getARGB(),
            CharFormat::ColourFormat3Id, juce::Colour ::fromString("#00AAAA").getARGB(),
            CharFormat::ColourFormat4Id, juce::Colours::darkred              .getARGB(),
            CharFormat::ColourFormat5Id, juce::Colour ::fromString("#AA00AA").getARGB(),
            CharFormat::ColourFormat6Id, juce::Colours::gold                 .getARGB(),
            CharFormat::ColourFormat7Id, juce::Colours::grey                 .getARGB(),
            CharFormat::ColourFormat8Id, juce::Colours::darkgrey             .getARGB(),
            CharFormat::ColourFormat9Id, juce::Colours::blue                 .getARGB(),
            CharFormat::ColourFormatAId, juce::Colours::green                .getARGB(),
            CharFormat::ColourFormatBId, juce::Colours::aqua                 .getARGB(),
            CharFormat::ColourFormatCId, juce::Colours::red                  .getARGB(),
            CharFormat::ColourFormatDId, juce::Colour ::fromString("#FF55FF").getARGB(),
            CharFormat::ColourFormatEId, juce::Colours::yellow               .getARGB(),
            CharFormat::ColourFormatFId, juce::Colours::white                .getARGB(),
    
            ContentPane::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground),
            ContentPane::ColourBorderId,     ::getArgbFrom(this, UICol::outline),
    
            SplitPane::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground),
            SplitPane::ColourSeparatorId,  ::getArgbFrom(this, UICol::windowBackground),
            SplitPane::ColourBorderId,     ::getArgbFrom(this, UICol::outline),
    
            MultiPagePane::ColourBackgroundId,              ::getArgbFrom(this, UICol::windowBackground),
            MultiPagePane::ColourBorderId,                  ::getArgbFrom(this, UICol::outline),
            MultiPagePane::ColourTabId,                     ::getArgbFrom(this, UICol::windowBackground),
            MultiPagePane::ColourTabActiveId,               ::getArgbFrom(this, UICol::widgetBackground),
            MultiPagePane::ColourTabHighlightId,            ::getArgbFrom(this, UICol::highlightedFill),
            MultiPagePane::ColourTabTextId,                 ::getArgbFrom(this, UICol::defaultText),
            MultiPagePane::ColourTabCloseButtonId,          ::getArgbFrom(this, UICol::defaultText),
            MultiPagePane::ColourTabCloseButtonHighlightId, ::getArgbFrom(this, UICol::highlightedText)
        };
    
        for (int i = 0; i < juce::numElementsInArray(colours); i += 2)
        {
            setColour(static_cast<int>(colours[i]), juce::Colour(colours[i + 1]));
        }
    }
    
    //==================================================================================================================
    void LookAndFeel_Jaut::drawContentPaneBackground(juce::Graphics &g, const ContentPane &contentPane)
    {
        g.setColour(contentPane.findColour(ContentPane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawContentPaneBorder(juce::Graphics &g, const ContentPane &contentPane)
    {
        g.setColour(contentPane.findColour(SplitPane::ColourBorderId));
        g.fillPath(::getBorderForComponent(contentPane.getLocalBounds(), contentPane.getStyle().borderThickness));
    }
    
    //==================================================================================================================
    void LookAndFeel_Jaut::drawSplitContainerBackground(juce::Graphics &g, const SplitPane &splitPane)
    {
        g.setColour(splitPane.findColour(SplitPane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawSplitContainerBorder(juce::Graphics &g, const juce::Component &component,
                                                    juce::Rectangle<int> bounds, Thickness<int> borderThickness)
    {
        g.setColour(component.findColour(SplitPane::ColourBorderId, true));
        g.fillPath(::getBorderForComponent(bounds, borderThickness));
    }
    
    void LookAndFeel_Jaut::drawSplitContainerSeparator(juce::Graphics &g, const juce::Component &separator)
    {
        g.setColour(separator.findColour(SplitPane::ColourSeparatorId, true));
        g.fillRect(separator.getBounds());
    }
    
    //==================================================================================================================
    juce::Font LookAndFeel_Jaut::getMultiTabPaneFont()
    {
        return juce::Font().withHeight(13.0f);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane &multiPagePane)
    {
        g.setColour(multiPagePane.findColour(MultiPagePane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBorder(juce::Graphics &g, const MultiPagePane &multiTabPane)
    {
        g.setColour(multiTabPane.findColour(MultiPagePane::ColourBorderId));
        g.fillPath(::getBorderForComponent(multiTabPane.getLocalBounds(), multiTabPane.getStyle().borderThickness));
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                                          bool isMouseOver, bool isMouseDown)
    {
        constexpr float w      = 8.0f;
        constexpr float h      = 5.0f;
        constexpr float half_w = w / 2.0f;
    
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
    
        g.setColour(isMouseOver ? button.findColour(MultiPagePane::ColourTabHighlightId, true)
                                : isMouseDown ? button.findColour(MultiPagePane::ColourTabActiveId, true)
                                              : juce::Colours::transparentBlack);
        g.fillRoundedRectangle(bounds.toFloat(), 2.0f);
    
        juce::Path path_down;
        path_down.addLineSegment({ 0.0f,   0.0f, half_w, h    }, 1.5f);
        path_down.addLineSegment({ half_w, h,    w,      0.0f }, 1.5f);
    
        g.setColour(button.findColour(MultiPagePane::ColourTabTextId, true));
        g.addTransform(juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - half_w,
                                                          bounds.getHeight() / 2.0f - h / 2.0f));
        g.fillPath(path_down);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabBackground(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                         bool isMouseOver, bool)
    {
        constexpr int bottom_line = 2;
    
        const bool is_active      = tabButton.isActive();
        const juce::Colour colour = is_active ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true)
                                              : (isMouseOver
                                                     ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true)
                                                     : tabButton.findColour(MultiPagePane::ColourTabId,          true));
        g.setColour(is_active && isMouseOver ? colour.darker() : colour);
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabText(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                   const juce::Label &label, bool, bool)
    {
        g.setColour(tabButton.findColour(MultiPagePane::ColourTabTextId, true));
        g.setFont(getMultiTabPaneFont());
        g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centredLeft);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabCloseButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                          const juce::TextButton &button, bool isMouseOver, bool)
    {
        constexpr float s =     7.0f;
        constexpr float c = s * 1.8f;
        
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Colour main_colour      = tabButton.findColour(MultiPagePane::ColourTabCloseButtonId, true);
        
        if (isMouseOver)
        {
            g.setColour(main_colour);
            g.fillEllipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f, c, c);
            
            const juce::Colour colour = tabButton.isActive()
                ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true)
                : (isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true)
                               : tabButton.findColour(MultiPagePane::ColourTabId,          true));
            
            g.setColour(colour);
        }
        else
        {
            g.setColour(main_colour);
        }
    
        const float p_comp = isMouseOver / 2.0f;
        const float s_less = s           - p_comp;
        
        juce::Path path_x;
        path_x.addLineSegment({ p_comp, p_comp, s_less, s_less }, 1.0f + p_comp);
        path_x.addLineSegment({ s_less, p_comp, p_comp, s_less }, 1.0f + p_comp);
    
        g.fillPath(path_x, juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - s / 2.0f,
                                                              bounds.getHeight() / 2.0f - s / 2.0f));
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabPinButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                        const juce::ToggleButton &button, bool isMouseOver, bool)
    {
        constexpr float s =     7.0f;
        constexpr float c = s * 1.8f;
    
        const juce::Colour &colour_text     = tabButton.findColour(MultiPagePane::ColourTabTextId, true);
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Rectangle<float> ellipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f,
                                             c, c);
        
        if (tabButton.isActive())
        {
            g.setColour(isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabActiveId, true) : colour_text);
        }
        else
        {
            g.setColour(isMouseOver ? tabButton.findColour(MultiPagePane::ColourTabHighlightId, true) : colour_text);
        }
        
        if (tabButton.isPinned())
        {
            g.fillEllipse(ellipse.reduced(2.0f));
        }
        else
        {
            g.drawEllipse(ellipse.reduced(2.0f), 1.5f);
        }
    }
    
    //==================================================================================================================
    int LookAndFeel_Jaut::getWindowStyleFlags() const
    {
        using Flags = juce::ComponentPeer::StyleFlags;
        return Flags::windowHasTitleBar | Flags::windowHasCloseButton;
    }
    
    //==================================================================================================================
    JAUT_IMPL_LAF(ContentPane)
    JAUT_IMPL_LAF(SplitPane)
    JAUT_IMPL_LAF(MultiPagePane)
    JAUT_IMPL_LAF(MultiPagePane::TabButton)
    JAUT_IMPL_LAF(MultiPagePane::Window)
    //==================================================================================================================
    // endregion LookAndFeel_Jaut
    //******************************************************************************************************************
}
