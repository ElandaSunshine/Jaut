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
    @file   jaut_MultiPagePanel.cpp
    @date   21, July 2020

    ===============================================================
 */

#include <jaut_gui/component/jaut_MultiPagePanel.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region MultiPagePanel::Pimpl
//======================================================================================================================
namespace jaut
{
    class MultiPagePanel::Pimpl
    {
        
    };
}
//======================================================================================================================
// endregion MultiPagePanel::Pimpl
//**********************************************************************************************************************
// region MultiPagePanel::TabButton
//======================================================================================================================
namespace jaut
{
    MultiPagePanel::TabButton::TabButton(const juce::String    &parName,
                                         MultiPagePanel        &parPanel,
                                         std::function<void()> parCloseHandler)
        : juce::Component(parName),
          panel(parPanel),
          closeHandler(std::move(parCloseHandler))
    {
        JAUT_INTERN_INIT_LAF();
    }
    
    //==================================================================================================================
    void MultiPagePanel::TabButton::paint(juce::Graphics &parG)
    {
        lookAndFeel->drawMultiPagePanelTabBackground(parG, *this, mouseIsOver, mouseIsDown);
    }
    
    //==================================================================================================================
    bool MultiPagePanel::TabButton::isActive() const noexcept
    {
        return active;
    }
    
    //==================================================================================================================
    void MultiPagePanel::TabButton::activate(bool parShouldBeActive)
    {
        if (active != parShouldBeActive)
        {
            active = parShouldBeActive;
            eventActivityChanged.invoke(*this);
        }
    }
    
    void MultiPagePanel::TabButton::close()
    {
        bool cancel = false;
        eventBeforeClosing.invoke(*this, cancel);
        
        if (!cancel)
        {
            closeHandler();
        }
    }
    
    //==================================================================================================================
    void MultiPagePanel::TabButton::mouseDown(const juce::MouseEvent &parE)
    {
        if (parE.mods.isLeftButtonDown())
        {
            if (!panel.options.activateTabOnFullPress)
            {
                activate(true);
            }
            
            if (!std::exchange(mouseIsDown, true))
            {
                repaint();
            }
        }
    }
    
    void MultiPagePanel::TabButton::mouseUp(const juce::MouseEvent &parE)
    {
        if (std::exchange(mouseIsDown, false))
        {
            if (panel.options.activateTabOnFullPress)
            {
                activate(true);
            }
            
            repaint();
        }
    }
    
    void MultiPagePanel::TabButton::mouseEnter(const juce::MouseEvent&)
    {
        if (!std::exchange(mouseIsOver, true))
        {
            repaint();
        }
    }
    
    void MultiPagePanel::TabButton::mouseExit(const juce::MouseEvent&)
    {
        if (std::exchange(mouseIsOver, false))
        {
            repaint();
        }
    }
    
    //==================================================================================================================
    void MultiPagePanel::TabButton::componentNameChanged(juce::Component &parComponent)
    {
        eventNameChanged.invoke(*this);
        repaint();
    }
}
//======================================================================================================================
// endregion MultiPagePanel::TabButton
//**********************************************************************************************************************
// region MultiPagePanel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    MultiPagePanel::MultiPagePanel(const juce::String &parName, InitStyle parStyle, Options parOptions)
        : ExtraComponent(parName, static_cast<ContainerStyle>(parStyle)), // NOLINT
          style  (static_cast<Style>(parStyle)), // NOLINT
          options(std::move(parOptions)),
          pimpl  (std::make_unique<Pimpl>())
    {
        JAUT_INTERN_INIT_LAF();
        
        
    }
    
    MultiPagePanel::MultiPagePanel(InitStyle parStyle, Options parOptions)
        : MultiPagePanel("", parStyle, std::move(parOptions))
    {}
    
    MultiPagePanel::MultiPagePanel(const juce::String &parName, InitStyle parStyle)
        : MultiPagePanel(parName, parStyle, {})
    {}
    
    MultiPagePanel::MultiPagePanel(const juce::String &parName, Options parOptions)
        : MultiPagePanel(parName, {}, std::move(parOptions))
    {}
    
    MultiPagePanel::MultiPagePanel(InitStyle parStyle)
        : MultiPagePanel("", parStyle, {})
    {}
    
    MultiPagePanel::MultiPagePanel(Options parOptions)
        : MultiPagePanel("", {}, std::move(parOptions))
    {}
    
    MultiPagePanel::MultiPagePanel(const juce::String &parName)
        : MultiPagePanel(parName, {}, {})
    {}
    
    MultiPagePanel::~MultiPagePanel() = default;
    
    //==================================================================================================================
    void MultiPagePanel::paintContents(juce::Graphics &g)
    {
        lookAndFeel->drawMultiTabPaneBackground(g, *this);
    }
    
    void MultiPagePanel::paintOverChildren(juce::Graphics &g)
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
    
    void MultiPagePanel::resized()
    {
        initialised = true;
        
        const juce::Rectangle<int> prev_bounds = tabBar->getBounds();
        const TabBarLayout layout = style.tabBarLayout;
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
            juce::Rectangle<int> tab_rect { tabSpaceBounds.getHeight() - tab_margin.top - tab_margin.bottom,
                                            tabSpaceBounds.getWidth() - tab_margin.left - tab_margin.right
            };
            
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
                tab_rect.setY(tabSpaceBounds.getY() + tab_margin.top);
                
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
        
        return { };
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
    juce::Component *MultiPagePane::getBackgroundComponent() noexcept
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
        
        return { };
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
    const MultiPagePane::TabButton *MultiPagePane::getTab(int index) const noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        return tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index)).get();
    }
    
    juce::Component *MultiPagePane::getPage(int index) noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        const juce::String id = tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index))->getName();
        return pages.at(id).get();
    }
    
    const juce::Component *MultiPagePane::getPage(int index) const noexcept
    {
        if (!fit(index, 0, getNumPages()))
        {
            return nullptr;
        }
        
        const juce::String id = tabBar->getAllTabs().at(static_cast<SizeTypes::Vector>(index))->getName();
        return pages.at(id).get();
    }
    
    const MultiPagePane::TabButton *MultiPagePane::getTab(const juce::String &pageId) const noexcept
    {
        for (auto &tab: tabBar->getAllTabs())
        {
            if (tab->getName().equalsIgnoreCase(pageId.trim()))
            {
                return tab.get();
            }
        }
        
        return nullptr;
    }
    
    juce::Component *MultiPagePane::getPage(const juce::String &pageId) noexcept
    {
        const auto it = pages.find(pageId.trim().toLowerCase());
        return it != pages.end() ? it->second.get() : nullptr;
    }
    
    const juce::Component *MultiPagePane::getPage(const juce::String &pageId) const noexcept
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
    
    const MultiPagePane::TabButton *MultiPagePane::getActiveTab() const noexcept
    {
        return getTab(getActiveIndex());
    }
    
    juce::Component *MultiPagePane::getActivePage() noexcept
    {
        return hasAnyPages() ? pageView.getCurrentComponent() : nullptr;
    }
    
    const juce::Component *MultiPagePane::getActivePage() const noexcept
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
        const auto old_behaviour = std::exchange(options.pinnedTabBehaviour, newBehaviour);
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
        bool needs_resize = false;
        
        if (old_style.tabBarMargin != parStyle.tabBarMargin || old_style.contentPadding != parStyle.contentPadding)
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
        
        tabBar->setTabsReorderable(parOptions.allowTabReordering);
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
    void MultiPagePane::itemDragEnter(const SourceDetails &)
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
    
    void MultiPagePane::itemDragExit(const SourceDetails &)
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
                    for (const auto &tab: tabBar->getAllTabs())
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
        const juce::DynamicObject
            *const tab_info = dragSourceDetails.description.getProperty("tab_info", { }).getDynamicObject();
        const juce::String page_id = tab_info->getProperty("page_id");
        const juce::String page_name = tab_info->getProperty("name");
        
        MultiPagePane &other = *static_cast<MultiPagePane *>(dragSourceDetails.sourceComponent.get());
        pages.emplace(page_id, other.detachPage(page_id));
        tabBar->addTab(style, *factory, page_id, page_name);
        
        paintDropBox = false;
        resized();
        repaint();
    }
    
    //==================================================================================================================
    void MultiPagePane::updateSpaceBounds(TabBarLayout layout)
    {
        const Thickness<int> &border = style.borderThickness;
        const Padding &top_padding = style.contentPadding;
        const Margin &tab_margin = style.tabBarMargin;
        const int tab_height = style.tabButtonHeight * ((tabBar->hasPinnedTabs() && tabBar->hasNonPinnedTabs()) + 1);
        
        if (layout == TabBarLayout::Top || layout == TabBarLayout::Bottom)
        {
            const int width = getWidth() - top_padding.right - top_padding.left - border.left - border.right;
            const int height = tab_margin.top + tab_margin.bottom + tab_height;
            
            tabSpaceBounds.setSize(width, height);
            pageSpaceBounds.setSize(width, getHeight() - height - top_padding.top - top_padding.bottom - border.top -
                                           border.bottom);
            
            const int left = top_padding.left + border.left;
            
            if (layout == TabBarLayout::Top)
            {
                tabSpaceBounds.setPosition(left, top_padding.top + border.top);
                pageSpaceBounds.setPosition(left, tabSpaceBounds.getBottom());
            }
            else
            {
                pageSpaceBounds.setPosition(left, top_padding.top + border.top);
                tabSpaceBounds.setPosition(left, pageSpaceBounds.getBottom());
            }
        }
        else
        {
            const int width = tab_margin.left + tab_margin.right + tab_height;
            const int height = getHeight() - top_padding.top - top_padding.bottom - border.top - border.bottom;
            
            tabSpaceBounds.setSize(width, height);
            pageSpaceBounds
                .setSize(getWidth() - width - top_padding.left - top_padding.right - border.left - border.right,
                         height);
            
            const int top = top_padding.top + border.top;
            
            if (layout == TabBarLayout::SideLeft)
            {
                tabSpaceBounds.setPosition(top_padding.left + border.left, top);
                pageSpaceBounds.setPosition(tabSpaceBounds.getRight(), top);
            }
            else
            {
                pageSpaceBounds.setPosition(top_padding.left + border.left, top);
                tabSpaceBounds.setPosition(pageSpaceBounds.getRight(), top);
            }
        }
    }
}
//======================================================================================================================
// endregion MultiPagePanel
//**********************************************************************************************************************
// region LookAndFeel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    void LookAndFeel::drawMultiPagePanelBackground(juce::Graphics &parG, const MultiPagePanel &parPanel)
    {
        drawExtraComponentBackground(parG, parPanel, parPanel.findColour(MultiPagePanel::ColourBackgroundId, true));
    }
    
    void LookAndFeel::drawMultiPagePanelBorder(juce::Graphics &parG, const MultiPagePanel &parPanel)
    {
        drawExtraComponentBorder(parG, parPanel, parPanel.findColour(MultiPagePanel::ColourBorderId, true));
    }
    
    void LookAndFeel::drawMultiPagePanelTabBackground(juce::Graphics                  &parG,
                                                      const MultiPagePanel::TabButton &parButton,
                                                      bool                            parMouseOver,
                                                      bool                            parMouseDown)
    {
        if (!parButton.isActive())
        {
            parG.setColour(((parMouseOver || parMouseDown)
                ? parButton.findColour(MultiPagePanel::ColourTabBackgroundHoverId)
                : parButton.findColour(MultiPagePanel::ColourTabBackgroundId)));
        }
        else
        {
            parG.setColour(parButton.findColour(MultiPagePanel::ColourTabBackgroundActiveId));
        }
        
        parG.fillAll();
    }
    
    //==================================================================================================================
    JAUT_INTERN_IMPL_LAF(MultiPagePanel)
    JAUT_INTERN_IMPL_LAF(MultiPagePanel::TabButton)
}
//======================================================================================================================
// endregion LookAndFeel
//**********************************************************************************************************************
