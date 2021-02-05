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
    
    constexpr jaut::SplitContainer::ResizeBehaviour getResizeBehaviour(jaut::SplitContainer::ResizeBehaviour behaviour,
                                                                       int prevXY, int xy, int prevWidthHeight,
                                                                       int widthHeight) noexcept
    {
        if (behaviour != jaut::SplitContainer::ResizeBehaviour::Directional)
        {
            return behaviour;
        }
        
        if (prevXY != xy && (prevXY + prevWidthHeight) != (xy + widthHeight))
        {
            return jaut::SplitContainer::ResizeBehaviour::Centred;
        }
        else if (prevXY == xy)
        {
            return jaut::SplitContainer::ResizeBehaviour::Panel2;
        }
        
        return jaut::SplitContainer::ResizeBehaviour::Panel1;
    }
    
    void resizeSplitContainer(jaut::SplitContainer::ResizeBehaviour resizeBehaviour, juce::Component &comp,
                              int prevWidthHeight, int prevXY, int xy, int widthHeight, int heightWidth,
                              int separatorXY, int sepWidthHeight, int sepHeightWidth, int sepRightBottom,
                              int min1, int min2, bool vertical)
    {
        if (sepHeightWidth != heightWidth)
        {
            if (!vertical)
            {
                comp.setSize(sepWidthHeight, heightWidth);
            }
            else
            {
                comp.setSize(heightWidth, sepWidthHeight);
            }
        }
        
        const SplitContainerSizeState prev_state = getSizeState(prevWidthHeight, min1, min2, sepWidthHeight);
        const SplitContainerSizeState this_state = getSizeState(widthHeight,     min1, min2, sepWidthHeight);
        
        if (this_state == SplitContainerSizeState::Full)
        {
            const int  top_max   = widthHeight - min2 - sepWidthHeight;
            const auto behaviour = getResizeBehaviour(resizeBehaviour, prevXY, xy, prevWidthHeight, widthHeight);
    
            if (behaviour == jaut::SplitContainer::ResizeBehaviour::Centred)
            {
                if (prev_state == SplitContainerSizeState::Full || prev_state == SplitContainerSizeState::Semi)
                {
                    const int value = std::round((separatorXY / (prevWidthHeight / 100.0)) * (widthHeight / 100.0));
                    setPositionOf(comp, std::clamp(value, min1, top_max), vertical);
                }
                else
                {
                    setPositionOf(comp, (widthHeight / 2.0) - (sepWidthHeight / 2.0), vertical);
                }
            }
            else if (behaviour == jaut::SplitContainer::ResizeBehaviour::Panel2)
            {
                if (prev_state == SplitContainerSizeState::Semi || prev_state == SplitContainerSizeState::Small)
                {
                    setPositionOf(comp, min1, vertical);
                }
                else
                {
                    if ((widthHeight - sepRightBottom) >= min2)
                    {
                        comp.getParentComponent()->childBoundsChanged(&comp);
                    }
                    else
                    {
                        setPositionOf(comp, top_max, vertical);
                    }
                }
            }
            else
            {
                if (prev_state == SplitContainerSizeState::Semi || prev_state == SplitContainerSizeState::Small)
                {
                    setPositionOf(comp, top_max, vertical);
                }
                else
                {
                    if (separatorXY >= min1)
                    {
                        setPositionOf(comp, separatorXY + (widthHeight - prevWidthHeight), vertical);
                    }
                    else
                    {
                        comp.getParentComponent()->childBoundsChanged(&comp);
                    }
                }
            }
        }
        else if (this_state == SplitContainerSizeState::Semi)
        {
            if (min1 > min2)
            {
                setPositionOf(comp, widthHeight - min2 - sepWidthHeight, vertical);
            }
            else
            {
                comp.getParentComponent()->childBoundsChanged(&comp);
            }
        }
        else
        {
            setPositionOf(comp, std::round((widthHeight / 2.0) - (sepWidthHeight / 2.0)), vertical);
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
}
//======================================================================================================================
// endregion Namespace
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
        lookAndFeel->drawContentPaneBackground(g, getLocalBounds());
    }
    
    void ContentPane::paintOverChildren(juce::Graphics &g)
    {
        lookAndFeel->drawContentPaneBorder(g, getLocalBounds(), style.borderThickness);
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
    // region SplitContainer
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region ContentContainer
    //==================================================================================================================
    SplitContainer::ContentContainer::ContentContainer(SplitContainer &container)
        : parent(container), constrainer(container)
    {
        if (parent.options.orientation == Orientation::Vertical)
        {
            containers[0].setScrollBarsShown(false, true);
            containers[1].setScrollBarsShown(false, true);
        }
        else
        {
            containers[0].setScrollBarsShown(true, false);
            containers[1].setScrollBarsShown(true, false);
        }
    
        addAndMakeVisible(containers[0]);
        addAndMakeVisible(containers[1]);
        addAndMakeVisible(separator);
    
        separator.addMouseListener(this, false);
    }
    
    //==================================================================================================================
    void SplitContainer::ContentContainer::paint(juce::Graphics &g)
    {
        LookAndFeel_Jaut &laf = *parent.lookAndFeel;
        laf.drawSplitContainerSeparator(g, separator.getBounds(), parent.options.orientation);
        
        if (parent.options.orientation == Orientation::Vertical)
        {
            laf.drawSplitContainerBorder(g, getLocalBounds().withWidth(separator.getX()),
                                         parent.style.panelBorderThickness);
            laf.drawSplitContainerBorder(g, getLocalBounds().withWidth(getWidth() - separator.getRight())
                                                            .withX(separator.getRight()),
                                         parent.style.panelBorderThickness);
        }
        else
        {
            laf.drawSplitContainerBorder(g, getLocalBounds().withHeight(separator.getY()),
                                         parent.style.panelBorderThickness);
            laf.drawSplitContainerBorder(g, getLocalBounds().withHeight(getHeight() - separator.getBottom())
                                                            .withY(separator.getBottom()),
                                         parent.style.panelBorderThickness);
        }
    }
    
    void SplitContainer::ContentContainer::resized()
    {
        if (getWidth() <= 0 || getHeight() <= 0)
        {
            return;
        }
    
        const Options &loptions = parent.options;
        const Style   &lstyle   = parent.style;
        
        if (!initialised)
        {
            if (loptions.orientation == Orientation::Vertical)
            {
                const int value = std::round(getWidth() / 2.0f - (lstyle.separatorThickness / 2.0f));
            
                if (getWidth() >= (loptions.panel1MinimumSize + loptions.panel2MinimumSize + lstyle.separatorThickness))
                {
                    const int max = getWidth() - loptions.panel2MinimumSize - lstyle.separatorThickness;
                    separator.setBounds(std::clamp(value, loptions.panel1MinimumSize, max), 0,
                                        lstyle.separatorThickness, getHeight());
                }
                else
                {
                    separator.setBounds(value, 0, lstyle.separatorThickness, getHeight());
                }
            }
            else
            {
                const int value = std::round(getHeight() / 2.0f - (lstyle.separatorThickness / 2.0f));
            
                if (getHeight() >= (loptions.panel1MinimumSize + loptions.panel2MinimumSize
                                                               + lstyle.separatorThickness))
                {
                    const int max = getHeight() - loptions.panel2MinimumSize - lstyle.separatorThickness;
                    separator.setBounds(0, std::clamp(value, loptions.panel1MinimumSize, max), getWidth(),
                                        lstyle.separatorThickness);
                }
                else
                {
                    separator.setBounds(0, value, getWidth(), lstyle.separatorThickness);
                }
            }
        
            initialised = true;
        }
        else
        {
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
            
            if (loptions.orientation == Orientation::Vertical)
            {
                ::resizeSplitContainer(loptions.resizeBehaviour, separator, prevSize.getWidth(), prevSize.getX(),
                                       getX(), getWidth(), getHeight(), separator.getX(), separator.getWidth(),
                                       separator.getHeight(), separator.getRight(), loptions.panel1MinimumSize,
                                       loptions.panel2MinimumSize, false);
            }
            else
            {
                ::resizeSplitContainer(loptions.resizeBehaviour, separator, prevSize.getHeight(), prevSize.getY(),
                                       getY(), getHeight(), getWidth(), separator.getY(), separator.getHeight(),
                                       separator.getWidth(), separator.getBottom(), loptions.panel1MinimumSize,
                                       loptions.panel2MinimumSize, true);
            }
        }
    
        prevSize = getLocalBounds().withX(parent.getX()).withY(parent.getY());
    }
    
    //==================================================================================================================
    void SplitContainer::ContentContainer::mouseDown(const juce::MouseEvent &event)
    {
        dragger.startDraggingComponent(&separator, event);
    }
    
    void SplitContainer::ContentContainer::mouseDrag(const juce::MouseEvent &event)
    {
        dragger.dragComponent(&separator, event, &constrainer);
    }
    
    //==================================================================================================================
    void SplitContainer::ContentContainer::childBoundsChanged(Component *child)
    {
        if (child == &separator)
        {
            const Options &loptions = parent.options;
            const Style   &lstyle   = parent.style;
            
            if (loptions.orientation == Orientation::Vertical)
            {
                const int new_min_1 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.left
                                                                 - lstyle.panelBorderThickness.right;
                const int new_min_2 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.left
                                                                 - lstyle.panelBorderThickness.right;
                
                containers[0].setBounds(lstyle.panelBorderThickness.trimRectangle({ separator.getX(), getHeight() }));
                containers[1].setBounds(lstyle.panelBorderThickness.trimRectangle({ separator.getRight(), 0,
                                                                                    getWidth() - separator.getRight(),
                                                                                    getHeight() }));
            
                if (auto *comp = components[0].get())
                {
                    comp->setBounds(0, 0, std::max(containers[0].getWidth(), new_min_1), getHeight());
                }
            
                if (auto *comp = components[1].get())
                {
                    comp->setBounds(0, 0, std::max(containers[1].getWidth(), new_min_2), getHeight());
                }
            }
            else
            {
                const int new_min_1 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.top
                                                                 - lstyle.panelBorderThickness.bottom;
                const int new_min_2 = loptions.panel1MinimumSize - lstyle.panelBorderThickness.top
                                                                 - lstyle.panelBorderThickness.bottom;
                
                containers[0].setBounds(lstyle.panelBorderThickness.trimRectangle({ getWidth(), separator.getY() }));
                containers[1].setBounds(lstyle.panelBorderThickness.trimRectangle({ 0, separator.getBottom(),
                                                                                    getWidth(),
                                                                                    getHeight()
                                                                                        - separator.getBottom() }));
            
                if (auto *comp = components[0].get())
                {
                    comp->setBounds(0, 0, getWidth(), std::max(containers[0].getHeight(), new_min_1));
                }
            
                if (auto *comp = components[1].get())
                {
                    comp->setBounds(0, 0, getWidth(), std::max(containers[1].getHeight(), new_min_2));
                }
            }
        }
    }
    //==================================================================================================================
    // endregion ContentContainer
    //******************************************************************************************************************
    // region SplitContainer
    //==================================================================================================================
    SplitContainer::SplitContainer(Style parStyle, Options parOptions)
        : style(std::move(parStyle)), options(std::move(parOptions)),
          contentContainer(*this)
    {
        JAUT_INIT_LAF()
        addAndMakeVisible(contentContainer);
    }
    
    SplitContainer::SplitContainer(Style   parStyle)   : SplitContainer(std::move(parStyle), Options{}) {}
    SplitContainer::SplitContainer(Options parOptions) : SplitContainer(Style{}, std::move(parOptions)) {}
    SplitContainer::SplitContainer()                   : SplitContainer(Style{}, Options{})             {}
    
    //==================================================================================================================
    void SplitContainer::paint(juce::Graphics &g)
    {
        lookAndFeel->drawSplitContainerBackground(g, getLocalBounds());
    }
    
    void SplitContainer::paintOverChildren(juce::Graphics &g)
    {
        lookAndFeel->drawSplitContainerBorder(g, getLocalBounds(), style.borderThickness);
    }
    
    void SplitContainer::resized()
    {
        contentContainer.setBounds(style.borderThickness.trimRectangle(style.contentPadding
                                                                            .trimRectangle(getLocalBounds())));
    }
    
    //==================================================================================================================
    void SplitContainer::resetComponent(PanelId panelId)
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
    
    void SplitContainer::setComponent(juce::OptionalScopedPointer<juce::Component> component, PanelId panelId)
    {
        juce::Viewport &pane = contentContainer.containers[static_cast<int>(panelId)];
        
        if (pane.getViewedComponent() != component.get())
        {
            auto *comp = (contentContainer.components[static_cast<int>(panelId)] = std::move(component)).get();
            pane.setViewedComponent(comp, false);
    
            if (options.collapseMode != CollapseMode::AlwaysShow)
            {
                contentContainer.resized();
            }
            
            ComponentChanged(comp, panelId);
        }
    }
    
    void SplitContainer::setComponent(std::nullptr_t, PanelId panelId) { resetComponent(panelId); }
    
    void SplitContainer::setComponent(juce::Component &component, PanelId panelId)
    {
        setComponent({ &component, false }, panelId);
    }
    
    //==================================================================================================================
    void SplitContainer::setContentPadding(Padding padding)
    {
        if (padding != std::exchange(style.contentPadding, padding) && contentContainer.initialised)
        {
            resized();
        }
    }

    void SplitContainer::setBorderThickness(Thickness<int> thickness)
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
    
    void SplitContainer::setPanelBorderThickness(Thickness<int> thickness)
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
    
    void SplitContainer::setSeperatorThickness(int val)
    {
        if (val != std::exchange(style.separatorThickness, val) && contentContainer.initialised)
        {
            contentContainer.resized();
            contentContainer.repaint();
        }
    }
    
    //==================================================================================================================
    void SplitContainer::setOrientation(Orientation parOrientation)
    {
        if (parOrientation != std::exchange(options.orientation, parOrientation) && contentContainer.initialised)
        {
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
            }
            
            contentContainer.repaint();
    
            if (options.collapseMode != CollapseMode::AlwaysShow)
            {
                contentContainer.resized();
            }
        }
    }
    
    void SplitContainer::setCollapseMode(CollapseMode collapseMode) noexcept
    {
        if (collapseMode != std::exchange(options.collapseMode, collapseMode) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    void SplitContainer::setPanel1Minimum(int newMinimum)
    {
        if (newMinimum != std::exchange(options.panel1MinimumSize, newMinimum) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    void SplitContainer::setPanel2Minimum(int newMinimum)
    {
        if (newMinimum != std::exchange(options.panel2MinimumSize, newMinimum) && contentContainer.initialised)
        {
            contentContainer.resized();
        }
    }
    
    //==================================================================================================================
    void SplitContainer::setStyle(Style newStyle)
    {
        std::swap(style, newStyle);
        
        if (!contentContainer.initialised)
        {
            return;
        }
        
        if (style.contentPadding != newStyle.contentPadding || style.borderThickness != newStyle.borderThickness)
        {
            resized();
            repaint();
        }

        if (   style.separatorThickness   != newStyle.separatorThickness
            || style.panelBorderThickness != newStyle.panelBorderThickness)
        {
            contentContainer.resized();
            contentContainer.repaint();
        }
    }
    
    void SplitContainer::setOptions(Options newOptions)
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
    }
    //==================================================================================================================
    // endregion SplitContainer
    //******************************************************************************************************************
    //==================================================================================================================
    //==================================================================================================================
    // endregion SplitContainer
    //******************************************************************************************************************
    // region MultiPagePane
    //==================================================================================================================
    //==================================================================================================================
    //******************************************************************************************************************
    // region InternalTabBar
    //==================================================================================================================
    class MultiPagePane::InternalTabBar final : public juce::Component, private juce::LookAndFeel_V4,
                                                                        private juce::ComponentListener
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
        
        void lookAndFeelChanged() override
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
    
            comp->setLookAndFeel(this);
            addAndMakeVisible(comp);
            layoutComponents.emplace_back(comp);
        }
        
        setButtonText(parText);
        updateTabSize(parStyle.tabButtonItemHeight);
    }
    
    MultiPagePane::TabButton::~TabButton() = default;
    
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
    void MultiPagePane::TabButton::drawButtonText(juce::Graphics &g, juce::TextButton &button, bool isOver, bool isDown)
    {
        if (button.getName() == "itemCloseButton")
        {
            lookAndFeel->drawMultiTabPaneTabCloseButton(g, *this, button, isOver, isDown);
        }
    }
    
    void MultiPagePane::TabButton::drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool over, bool down)
    {
        if (button.getName() == "itemPinButton")
        {
            lookAndFeel->drawMultiTabPaneTabPinButton(g, *this, button, over, down);
        }
    }
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
        pane->LastTabClosing += make_handler(&Window::forceToCloseWindow, this);
        setContentOwned(pane.release(), false);
        
        setSize(400, 250);
        setUsingNativeTitleBar(true);
        setOpaque(true);
        
        // setVisible(true);
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
        lookAndFeel->drawMultiTabPaneBorder(g, getLocalBounds(), style.borderThickness);
        
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
    DockingPane::DockingPane(jaut::SplitContainer::Style style, jaut::SplitContainer::Options options)
        : container(std::move(style), std::move(options))
    {
        container.setCollapseMode(SplitContainer::CollapseMode::Hide);
    }
    
    //==================================================================================================================
    void DockingPane::dockComponent(juce::OptionalScopedPointer<juce::Component> component, Anchor anchor)
    {
        // Stop operation if the component is invalid or if the pane is already full
        /*if (!component || multiContainer)
        {
            return;
        }
        
        if (!soloContainer)
        {
            if (!createSplitContainer)
            {
                // The creation callback must not be a nullptr
                jassertfalse;
                return;
            }
            
            soloContainer = createContentPane();
            soloContainer->setComponent(std::move(component));
            addAndMakeVisible(soloContainer.get());
        }
        else
        {
            const bool is_horizontal = (anchor == Anchor::Left || anchor == Anchor::Right);
            multiContainer = createSplitContainer();
            
            if (!is_horizontal)
            {
                multiContainer->setOrientation(SplitContainer::Orientation::Horizontal);
            }
            
            auto old_comp = soloContainer->releaseComponent();
            
            removeChildComponent(soloContainer.get());
            soloContainer.reset();
            
            if (is_horizontal)
            {
                multiContainer->setComponent(std::move(old_comp),
                                             anchor == Anchor::Left ? SplitContainer::PanelId::Panel2
                                                                    : SplitContainer::PanelId::Panel1);
                multiContainer->setComponent(std::move(component),
                                             anchor == Anchor::Left ? SplitContainer::PanelId::Panel1
                                                                    : SplitContainer::PanelId::Panel2);
            }
            else
            {
                multiContainer->setComponent(std::move(old_comp),
                                             anchor == Anchor::Top ? SplitContainer::PanelId::Panel2
                                                                   : SplitContainer::PanelId::Panel1);
                multiContainer->setComponent(std::move(component),
                                             anchor == Anchor::Top ? SplitContainer::PanelId::Panel1
                                                                   : SplitContainer::PanelId::Panel2);
            }
            
            addAndMakeVisible(multiContainer.get());
        }
        
        resized();*/
    }
    
    juce::OptionalScopedPointer<juce::Component> DockingPane::undockComponent(DockingPane::Anchor anchor)
    {
        /*if (soloContainer)
        {
            auto comp = soloContainer->releaseComponent();
            soloContainer.reset();
            return comp;
        }
        else if (multiContainer)
        {
            if (((anchor == Anchor::Left || anchor == Anchor::Right) && multiContainer->getOrientation()
                    != SplitContainer::Orientation::Vertical)
                || ((anchor == Anchor::Top || anchor == Anchor::Bottom) && multiContainer->getOrientation()
                       != SplitContainer::Orientation::Horizontal))
            {
                return {};
            }
            
            auto comp_1 = multiContainer->releaseComponent(SplitContainer::ComponentOrder::LeftOrTop);
            auto comp_2 = multiContainer->releaseComponent(SplitContainer::ComponentOrder::RightOrBottom);
            
            multiContainer.reset();
            
            if (anchor == Anchor::Left || anchor == Anchor::Top)
            {
                dockComponent(std::move(comp_2), {});
                return comp_1;
            }
            else
            {
                dockComponent(std::move(comp_1), {});
                return comp_2;
            }
        }
        
        return {};*/
    }
    
    //==================================================================================================================
    void DockingPane::resized()
    {
        container.setBounds(getLocalBounds());
    }
    //==================================================================================================================
    // endregion DockingPane
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
            
            SplitContainer::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground),
            SplitContainer::ColourSeparatorId,  ::getArgbFrom(this, UICol::windowBackground),
            SplitContainer::ColourBorderId,     ::getArgbFrom(this, UICol::outline),
                
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
    void LookAndFeel_Jaut::drawContentPaneBackground(juce::Graphics &g, juce::Rectangle<int>)
    {
        g.setColour(findColour(ContentPane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawContentPaneBorder(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                 Thickness<int> thickness)
    {
        g.setColour(findColour(SplitContainer::ColourBorderId));
        g.fillPath(::getBorderForComponent(bounds, thickness));
    }
    
    //==================================================================================================================
    void LookAndFeel_Jaut::drawSplitContainerBackground(juce::Graphics &g, juce::Rectangle<int>)
    {
        g.setColour(findColour(SplitContainer::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawSplitContainerBorder(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                    Thickness<int> thickness)
    {
        g.setColour(findColour(SplitContainer::ColourBorderId));
        g.fillPath(::getBorderForComponent(bounds, thickness));
    }
    
    void LookAndFeel_Jaut::drawSplitContainerSeparator(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                       SplitContainer::Orientation)
    {
        g.setColour(findColour(SplitContainer::ColourSeparatorId));
        g.fillRect(bounds);
    }
    
    //==================================================================================================================
    juce::Font LookAndFeel_Jaut::getMultiTabPaneFont()
    {
        return juce::Font().withHeight(13.0f);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane&)
    {
        g.setColour(findColour(MultiPagePane::ColourBackgroundId));
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneBorder(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                  Thickness<int> thickness)
    {
        g.setColour(findColour(MultiPagePane::ColourBorderId));
        g.fillPath(::getBorderForComponent(bounds, thickness));
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                                          bool isMouseOver, bool isMouseDown)
    {
        constexpr float w      = 8.0f;
        constexpr float h      = 5.0f;
        constexpr float half_w = w / 2.0f;
    
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
    
        g.setColour(isMouseOver ? findColour(MultiPagePane::ColourTabHighlightId)
                                : isMouseDown ? findColour(MultiPagePane::ColourTabActiveId)
                                              : juce::Colours::transparentBlack);
        g.fillRoundedRectangle(bounds.toFloat(), 2.0f);
    
        juce::Path path_down;
        path_down.addLineSegment({ 0.0f,   0.0f, half_w, h    }, 1.5f);
        path_down.addLineSegment({ half_w, h,    w,      0.0f }, 1.5f);
    
        g.setColour(findColour(MultiPagePane::ColourTabTextId));
        g.addTransform(juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - half_w,
                                                          bounds.getHeight() / 2.0f - h / 2.0f));
        g.fillPath(path_down);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabBackground(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                         bool isMouseOver, bool)
    {
        constexpr int bottom_line = 2;
    
        const bool is_active      = tabButton.isActive();
        const juce::Colour colour = is_active ? findColour(MultiPagePane::ColourTabActiveId)
                                              : (isMouseOver ? findColour(MultiPagePane::ColourTabHighlightId)
                                                             : findColour(MultiPagePane::ColourTabId));
        g.setColour(is_active && isMouseOver ? colour.darker() : colour);
        g.fillAll();
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabText(juce::Graphics &g, const MultiPagePane::TabButton&,
                                                   const juce::Label &label, bool, bool)
    {
        g.setColour(findColour(MultiPagePane::ColourTabTextId));
        g.setFont(getMultiTabPaneFont());
        g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centredLeft);
    }
    
    void LookAndFeel_Jaut::drawMultiTabPaneTabCloseButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                          const juce::TextButton &button, bool isMouseOver, bool)
    {
        constexpr float s =     7.0f;
        constexpr float c = s * 1.8f;
        
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Colour main_colour      = findColour(MultiPagePane::ColourTabCloseButtonId);
        
        if (isMouseOver)
        {
            g.setColour(main_colour);
            g.fillEllipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f, c, c);
            
            const juce::Colour colour = tabButton.isActive() ? findColour(MultiPagePane::ColourTabActiveId)
                                                             : (isMouseOver
                                                                    ? findColour(MultiPagePane::ColourTabHighlightId)
                                                                    : findColour(MultiPagePane::ColourTabId));
            
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
    
        const juce::Colour &colour_text     = findColour(MultiPagePane::ColourTabTextId);
        const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
        const juce::Rectangle<float> ellipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f,
                                             c, c);
        
        if (tabButton.isActive())
        {
            g.setColour(isMouseOver ? findColour(MultiPagePane::ColourTabActiveId) : colour_text);
        }
        else
        {
            g.setColour(isMouseOver ? findColour(MultiPagePane::ColourTabHighlightId) : colour_text);
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
    JAUT_IMPL_LAF(SplitContainer)
    JAUT_IMPL_LAF(MultiPagePane)
    JAUT_IMPL_LAF(MultiPagePane::TabButton)
    JAUT_IMPL_LAF(MultiPagePane::Window)
    //==================================================================================================================
    // endregion LookAndFeel_Jaut
    //******************************************************************************************************************
}
