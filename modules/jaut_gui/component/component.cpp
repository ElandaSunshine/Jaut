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

template<class Fn>
int internalWidthLoop(const jaut::MultiPagePane::TabFactory &factory, jaut::LookAndFeel_Jaut *laf,
                      const jaut::MultiPagePane::TabButton &tab, Fn &&heightFunc)
{
    const auto &layouts = factory.getTabLayout();
    int width           = 0;
    
    for (jaut::SizeTypes::Vector i = 0; i < layouts.size(); ++i)
    {
        const auto &layout         = layouts.at(i);
        const jaut::Margin &margin = layout.itemMargin;
        const int height           = std::forward<Fn>(heightFunc)(margin);

        int item_width = height;

        if (layout.itemType == jaut::MultiPagePane::TabFactory::TabItem::ItemText)
        {
            item_width = std::ceil(laf->getMultiTabPaneFont().getStringWidthFloat(tab.getButtonText()));
        }
        else if (layout.itemType == jaut::MultiPagePane::TabFactory::TabItem::ItemImage)
        {
            const juce::Image img = static_cast<juce::ImageComponent&>(*tab.getChildComponent(i)).getImage();
            item_width = img.isNull() ? 0 : (img.getWidth() / img.getHeight()) * height;
        }

        width += margin.left + item_width + margin.right;
    }

    return width;
}

template<class Fn>
void internalComponentLoop(const jaut::MultiPagePane::TabFactory &factory, jaut::LookAndFeel_Jaut *laf,
                           const jaut::MultiPagePane::TabButton &tab, Fn &&heightFunc)
{
    const auto &layouts = factory.getTabLayout();
    int last_right      = 0;

    for (jaut::SizeTypes::Vector i = 0; i < layouts.size(); ++i)
    {
        const auto &layout         = layouts.at(i);
        const jaut::Margin &margin = layout.itemMargin;
        const int height           = std::forward<Fn>(heightFunc)(margin);
        int item_width             = height;
        juce::Component &cp        = *tab.getChildComponent(i);

        if (layout.itemType == jaut::MultiPagePane::TabFactory::TabItem::ItemText)
        {
            item_width = std::ceil(laf->getMultiTabPaneFont().getStringWidthFloat(tab.getButtonText()));
        }
        else if (layout.itemType == jaut::MultiPagePane::TabFactory::TabItem::ItemImage)
        {
            const juce::Image img = static_cast<juce::ImageComponent&>(cp).getImage();
            item_width = img.isNull() ? 0 : (img.getWidth() / img.getHeight()) * height;
        }

        cp.setBounds(last_right + margin.left, margin.top, item_width, height);
        last_right += margin.left + item_width + margin.right;
    }
}

int getTabWidth(const jaut::MultiPagePane::TabFactory &factory, const jaut::MultiPagePane::Style &style,
                const jaut::MultiPagePane::TabButton  &tab,           jaut::LookAndFeel_Jaut     *laf)
{
    if (const int item_size = style.tabBarItemSize; item_size >= 0)
    {
        return internalWidthLoop(factory, laf, tab, [item_size](const jaut::Margin&) { return item_size; });
    }
    else
    {
        if (const int tab_height = style.tabBarHeight; tab_height >= 0)
        {
            return internalWidthLoop(factory, laf, tab, [tab_height](const jaut::Margin &margin)
                                                        {
                                                            return tab_height - margin.top - margin.bottom;
                                                        });
        }
        else
        {
            const int font_size = std::ceil(laf->getMultiTabPaneFont().getHeight());
            return internalWidthLoop(factory, laf, tab, [font_size](const jaut::Margin&) { return font_size; });
        }
    }
}

int getTabHeight(const jaut::MultiPagePane::TabFactory &factory, const jaut::MultiPagePane::Style &style,
                 jaut::LookAndFeel_Jaut *laf)
{
    if (const int tab_height = style.tabBarHeight; tab_height >= 0)
    {
        return tab_height;
    }
    else
    {
        if (style.tabBarItemSize >= 0)
        {
            int max = 0;

            for (const auto &layout : factory.getTabLayout())
            {
                const jaut::Margin margin = layout.itemMargin;
                max = std::max(max, style.tabBarItemSize + margin.top + margin.bottom);
            }

            return max;
        }
        else
        {
            const int item_height = std::ceil(laf->getMultiTabPaneFont().getHeight());
            int max = 0;

            for (const auto &layout : factory.getTabLayout())
            {
                const jaut::Margin margin = layout.itemMargin;
                max = std::max(max, item_height + margin.top + margin.bottom);
            }

            return max;
        }
    }
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
{
    JAUT_INIT_LAF()
}

ContentPane::ContentPane(juce::Component *content, bool owned)
    : content(content, owned)
{
    JAUT_INIT_LAF()
}

//======================================================================================================================
void ContentPane::paint(juce::Graphics &g)
{
    if (!content)
    {
        lookAndFeel->drawContentPaneBackground(g, getLocalBounds());
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

bool ContentPane::resetComponent(std::nullptr_t)
{
    return resetComponent();
}

bool ContentPane::resetComponent(juce::Component *cnt, bool owned)
{
    if (content.get() == cnt)
    {
        return false;
    }

    content.set(cnt, owned);
    removeAllChildren();

    if (cnt)
    {
        addAndMakeVisible(cnt);
    }

    ComponentChanged.invoke(content.get());
    resized();

    return true;
}

bool ContentPane::resetComponent(juce::Component &cnt)
{
    return resetComponent(&cnt, false);
}
//======================================================================================================================
juce::Component* ContentPane::getCurrentComponent() noexcept
{
    return content.get();
}
//======================================================================================================================
// endregion ContentPane
//**********************************************************************************************************************
// region SplitContainer
//======================================================================================================================
SplitContainer::SplitContainer(SplitContainer::Orientation orientation) noexcept
    : constrainer(orientation), orientation(orientation)
{
    JAUT_INIT_LAF()

    addAndMakeVisible(firstComponent);
    addAndMakeVisible(secondComponent);
    addAndMakeVisible(seperator);
    seperator.addMouseListener(this, false);
}

//======================================================================================================================
void SplitContainer::paint(juce::Graphics &g)
{
    lookAndFeel->drawSplitContainerBackground(g, getLocalBounds());
    lookAndFeel->drawSplitContainerSeperator (g, seperator.getBounds(), orientation);
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

        if (orientation == Orientation::Horizontal)
        {
            seperator.setBounds(std::ceil(static_cast<float>(getWidth()) / 2.0f - h_thickness), 0,
                               thickness, getHeight());
        }
        else
        {
            seperator.setBounds(0, std::ceil(static_cast<float>(getHeight()) / 2.0f - h_thickness),
                               getWidth(), thickness);
        }

        initialised = true;
    }
    else
    {
        if (orientation == Orientation::Horizontal)
        {
            const int new_x = static_cast<int>(std::ceil(static_cast<float>(getWidth() - prevSize.getWidth()) / 2.0f));
            constrainer.setBoundsForComponent(&seperator, { seperator.getX() + new_x, 0, thickness, getHeight() },
                                              false, false, false, false);
        }
        else
        {
            const int new_y = static_cast<int>(std::ceil(static_cast<float>(getHeight() - prevSize.getHeight())
                                                         / 2.0f));
            constrainer.setBoundsForComponent(&seperator, { 0, seperator.getY() + new_y, getWidth(), thickness },
                                              false, false, false, false);
        }
    }

    prevSize = getLocalBounds();
}

//======================================================================================================================
void SplitContainer::setComponent(juce::Component *component, ComponentOrder order, bool owned)
{
    auto &pane = order == ComponentOrder::LeftOrTop ? firstComponent : secondComponent;

    if (pane.resetComponent(component, owned))
    {
        ComponentChanged(component, order);
    }
}

void SplitContainer::setComponent(juce::Component &component, ComponentOrder order)
{
    setComponent(&component, order, false);
}

//======================================================================================================================
void SplitContainer::mouseDown(const juce::MouseEvent &event)
{
    dragger.startDraggingComponent(&seperator, event);
}

void SplitContainer::mouseDrag(const juce::MouseEvent &event)
{
    dragger.dragComponent(&seperator, event, &constrainer);
}

//======================================================================================================================
void SplitContainer::childBoundsChanged(juce::Component *child)
{
    if (child == &seperator)
    {
        if (orientation == Orientation::Horizontal)
        {
            firstComponent .setBounds(0, 0, seperator.getX(), getHeight());
            secondComponent.setBounds(seperator.getRight(), 0, getWidth() - seperator.getRight(), getHeight());
        }
        else
        {
            firstComponent .setBounds(0, 0, getWidth(), seperator.getY());
            secondComponent.setBounds(0, seperator.getBottom(), getWidth(), getHeight() - seperator.getBottom());
        }
    }
}

//======================================================================================================================
void SplitContainer::setSeperatorThickness(int val)
{
    if (const int prev = std::exchange(thickness, val); prev != val)
    {
        const int  diff = static_cast<int>(std::ceil(static_cast<float>(val - prev) / 2.0f));
        const auto rect = seperator.getBounds();

        if (orientation == Orientation::Horizontal)
        {
            const int new_x = seperator.getX() - diff;
            seperator.setBounds(rect.withWidth(val).withX(new_x));
        }
        else
        {
            const int new_y = seperator.getY() - diff;
            seperator.setBounds(rect.withHeight(val).withY(new_y));
        }
    }
}
//======================================================================================================================
// endregion SplitContainer
//**********************************************************************************************************************
// region MultiPagePane
//======================================================================================================================
//======================================================================================================================
//**********************************************************************************************************************
// region InternalTabBar
//======================================================================================================================
class MultiPagePane::InternalTabBar final : public juce::Component, juce::LookAndFeel_V4
{
public:
    struct TabId
    {
        TabButton button;
        juce::String id;

        //==========================================================================================================
        TabId(const Style &style, TabFactory &factory, juce::String id, const juce::String &name)
            : button(style, factory, id, !name.isEmpty() ? name : id), id(std::move(id))
        {}
    };

    class TabStrip : public juce::Viewport
    {
    public:
        class TabDragger : public juce::MouseListener
        {
        public:
            class TabConstrainer : public juce::ComponentBoundsConstrainer
            {
                void applyBoundsToComponent(Component &component, juce::Rectangle<int> bounds) override
                {
                    const int max_x = component.getParentWidth() - component.getWidth();
                    component.setTopLeftPosition(std::clamp(bounds.getX(), 0, max_x), 0);
                }
            };

            //==========================================================================================================
            TabStrip &strip;

            //==========================================================================================================
            explicit TabDragger(TabStrip &strip) noexcept
                : strip(strip)
            {}

            //==========================================================================================================
            void mouseDown(const juce::MouseEvent &e) override
            {
                if (!e.mods.isLeftButtonDown())
                {
                    return;
                }

                if (auto *comp = dynamic_cast<TabButton*>(e.eventComponent))
                {
                    int selected = -1;

                    for (SizeTypes::Vector i = 0; i < strip.tabs.size(); ++i)
                    {
                        if (&strip.tabs.at(i)->button == comp)
                        {
                            selected = i;
                            break;
                        }
                    }

                    if (selected >= 0)
                    {
                        comp->toFront(false);
                        dragger.startDraggingComponent((button = comp), e);
                        selectedIndex = selected;
                    }
                }
            }

            void mouseDrag(const juce::MouseEvent &e) override
            {
                if (button)
                {
                    const TabButton *const prev = selectedIndex > 0
                                             ? &strip.tabs.at(static_cast<SizeTypes::Vector>(selectedIndex) - 1)->button
                                             : nullptr;
                    const TabButton *const next = (selectedIndex + 1) < static_cast<int>(strip.tabs.size())
                                             ? &strip.tabs.at(static_cast<SizeTypes::Vector>(selectedIndex) + 1)->button
                                             : nullptr;

                    dragger.dragComponent(button, e, &constrainer);

                    const auto bounds = button->getBounds();

                    if (prev && bounds.getX() < (prev->getX() + Const_TabSwapSensitivity))
                    {
                        const int prev_index = selectedIndex - 1;
                        strip.swapTabs(prev_index, selectedIndex);
                        rearrangeTabs();
                        --selectedIndex;
                    }
                    else if (next && (bounds.getRight() > next->getRight() - Const_TabSwapSensitivity))
                    {
                        const int next_index = selectedIndex + 1;
                        strip.swapTabs(selectedIndex, next_index);
                        rearrangeTabs();
                        ++selectedIndex;
                    }
                }
            }

            void mouseUp(const juce::MouseEvent &event) override
            {
                if (button)
                {
                    const int x = selectedIndex == 0 ? 0
                                  : strip.tabs.at(static_cast<SizeTypes::Vector>(selectedIndex) - 1)->button.getRight();
                    button->setTopLeftPosition(x, 0);
                    button = nullptr;
                }
            }

            void rearrangeTabs()
            {
                for (SizeTypes::Vector i = 0; i < strip.tabs.size(); ++i)
                {
                    auto &comp = strip.tabs.at(i)->button;

                    if (&comp != button)
                    {
                        if (i == 0)
                        {
                            comp.setTopLeftPosition(0, 0);
                        }
                        else
                        {
                            auto &prev = strip.tabs.at(i - 1)->button;

                            if (&prev != button)
                            {
                                comp.setTopLeftPosition(prev.getRight(), 0);
                            }
                            else
                            {
                                const int pre_w = prev.getWidth();
                                const int x = i == 1 ? pre_w : strip.tabs.at(i - 2)->button.getRight() + pre_w;
                                comp.setTopLeftPosition(x, 0);
                            }
                        }
                    }
                }
            }

        private:
            TabConstrainer constrainer;
            juce::ComponentDragger dragger;
            int selectedIndex { 0 };
            TabButton *button { nullptr };
        };

        //==============================================================================================================
        TabDragger dragger;
        std::vector<std::unique_ptr<TabId>> tabs;
        juce::Component container;

        //==============================================================================================================
        explicit TabStrip()
            : dragger(*this)
        {
            setViewedComponent(&container, false);
            setScrollBarsShown(false, false);
        }

        //==============================================================================================================
        void addTab(std::unique_ptr<TabId> tabId)
        {
            tabId->button.addMouseListener(&dragger, false);
            container.addAndMakeVisible(tabId->button);
            tabs.emplace_back(std::move(tabId));
            rearrangeTabList(static_cast<int>(tabs.size()) - 1);
        }

        void removeTab(int index)
        {
            container.removeChildComponent(&tabs.at(static_cast<SizeTypes::Vector>(index))->button);
            tabs.erase(tabs.begin() + index);
            rearrangeTabList(index);
        }

        //==============================================================================================================
        int numTabs() const noexcept
        {
            return tabs.size();
        }

        void swapTabs(int tab1, int tab2)
        {
            auto &p_comp = tabs.at(static_cast<SizeTypes::Vector>(tab1));
            p_comp->button.setTabIndex(tab2);

            auto &n_comp = tabs.at(static_cast<SizeTypes::Vector>(tab2));
            n_comp->button.setTabIndex(tab1);

            std::swap(p_comp, n_comp);

            int &current_tab = reinterpret_cast<InternalTabBar*>(getParentComponent())->currentOpenTab;
            current_tab      = (tab1 == current_tab ? tab2 : tab1);
        }

        //==============================================================================================================
        void rearrangeTabList(int index)
        {
            auto start_index = static_cast<SizeTypes::Vector>(index);

            if (index == 0 && !tabs.empty())
            {
                ++start_index;

                auto &tab_button = tabs.at(0)->button;
                tab_button.setTopLeftPosition(0, 0);
                tab_button.setTabIndex(0);
            }

            for (auto i = start_index; i < tabs.size(); ++i)
            {
                auto &tab_button = tabs.at(i)->button;
                tab_button.setTopLeftPosition(tabs.at(i - 1)->button.getRight(), 0);
                tab_button.setTabIndex(i);
            }

            if (!tabs.empty())
            {
                container.setSize(tabs.at(tabs.size() - 1)->button.getRight(), getHeight());
            }
            else
            {
                container.setSize(0, getHeight());
            }
        }

        //==========================================================================================================
        void updateStyle(const Style &newStyle)
        {
            for (auto &tab : tabs)
            {
                tab->button.updateStyle(newStyle);
            }
        }

        //==============================================================================================================
        void mouseWheelMove(const juce::MouseEvent&e, const juce::MouseWheelDetails &wheel) override
        {
            const auto v_area = getViewArea();

            if (!v_area.contains(container.getLocalBounds()))
            {
                const int x = getViewPositionX() + static_cast<int>(wheel.deltaY * -50.0f);
                setViewPosition(std::clamp(x, 0, container.getWidth() - getWidth()), 0);
            }
        }
    };

    //==================================================================================================================
    std::function<void(const juce::String&)> onTabAdded;
    std::function<void(const juce::String&)> onTabActivated;
    std::function<void(const juce::String&)> onTabRemoved;
    std::function<void(const juce::String&, bool)> onTabPinned;

    //==================================================================================================================
    explicit InternalTabBar(const Style &style, TabFactory &factory)
        : style(style), factory(factory)
    {
        JAUT_INIT_LAF()

        addAndMakeVisible(tabList);

        menuButton.setLookAndFeel(this);
        menuButton.onClick = [this]()
        {
            juce::PopupMenu popup_menu;

            for (const auto &tab : tabList.tabs)
            {
                if (!tabList.getViewArea().contains(tab->button.getBounds()))
                {
                    popup_menu.addItem(tab->button.getButtonText(), [this, &tab]()
                    {
                        const int tab_index = tab->button.getTabIndex();

                        if (currentOpenTab != tab_index)
                        {
                            tab->button.triggerTabClick();
                        }
                        else
                        {
                            moveToTab(tab_index);
                        }
                    });
                }
            }

            juce::PopupMenu::Options options;
            options.withParentComponent(this);
            popup_menu.showMenu(options);
        };

        addChildComponent(menuButton);
    }

    //==================================================================================================================
    void resized() override
    {
        tabList.setSize(getWidth(), getHeight());
        updateHiddenTabsMenu();

        if (menuButton.isVisible())
        {
            tabList.setSize(getWidth() - getHeight(), getHeight());
            menuButton.setBounds(style.moreTabsButtonMargin
                                      .trimRectangle(getLocalBounds().removeFromRight(getHeight())));
        }

        if (!tabList.tabs.empty())
        {
            const auto button_bounds = tabList.tabs.at(static_cast<SizeTypes::Vector>(currentOpenTab))
                                                  ->button.getBounds();
            const auto view_area     = tabList.getViewArea();

            if (!view_area.contains(button_bounds))
            {
                if (button_bounds.getX() > view_area.getX() && view_area.getWidth() >= button_bounds.getWidth())
                {
                    tabList.setViewPosition(button_bounds.getRight() - tabList.getWidth(), 0);
                }
                else
                {
                    tabList.setViewPosition(button_bounds.getX(), 0);
                }
            }
        }
    }

    //==================================================================================================================
    bool updateStyle(const Style &newStyle)
    {
        bool updated = false;

        if (style.tabBarHeight != newStyle.tabBarHeight || style.tabBarItemSize != newStyle.tabBarItemSize)
        {
            tabList.updateStyle(newStyle);

            if (!tabList.tabs.empty())
            {
                tabList.getViewedComponent()->setSize(tabList.tabs.at(tabList.tabs.size() - 1)->button.getRight(),
                                                      getHeight());
            }
            else
            {
                tabList.getViewedComponent()->setSize(0, getHeight());
            }

            updated = true;
        }

        if (menuButton.isVisible() && style.moreTabsButtonMargin != newStyle.moreTabsButtonMargin)
        {
            menuButton.setBounds(newStyle.moreTabsButtonMargin
                                         .trimRectangle(getBounds().removeFromRight(getHeight())));
            updated = true;
        }
        
        style = newStyle;
        return updated;
    }

    //==================================================================================================================
    void addTab(const juce::String &id, const juce::String &name = juce::String())
    {
        auto tab = std::make_unique<TabId>(style, factory, id, name);
        tab->button.onTabClicked = [this](int index) { activateTab(index); };
        tab->button.onClosing    = [this](int index) { removeTab(index); };
        tab->button.setSize(::getTabWidth(factory, style, tab->button, lookAndFeel), getHeight());
        tabList.addTab(std::move(tab));
        onTabAdded(id);
    }

    void removeTab(int index)
    {
        const juce::String id = getIdAt(index);
        tabList.removeTab(index);

        if (index == currentOpenTab)
        {
            if (index > 0)
            {
                --currentOpenTab;
            }

            if (tabList.numTabs() > 0)
            {
                tabList.tabs.at(static_cast<SizeTypes::Vector>(currentOpenTab))->button.setActive(true);
                moveToTab(currentOpenTab);
            }
        }
        else if (index < currentOpenTab)
        {
            --currentOpenTab;
        }

        onTabRemoved(id);
    }

    void activateTab(int index)
    {
        tabList.tabs.at(static_cast<SizeTypes::Vector>(std::exchange(currentOpenTab, index)))->button.setActive(false);
        tabList.tabs.at(static_cast<SizeTypes::Vector>(currentOpenTab))                      ->button.setActive(true);
        moveToTab(index);
        onTabActivated(getIdAt(index));
    }

    void pinTab(int index, bool state)
    {
        tabList.tabs.at(static_cast<SizeTypes::Vector>(index))->button.setPinned(state);
        onTabPinned(getIdAt(index), state);
    }

    //==================================================================================================================
    const juce::String& getIdAt(int index) const noexcept
    {
        return tabList.tabs.at(static_cast<SizeTypes::Vector>(index))->id;
    }

    //==================================================================================================================
    int getActiveTabIndex() const noexcept
    {
        return currentOpenTab;
    }

    const juce::String& getActiveId() const noexcept
    {
        return tabList.tabs.at(static_cast<SizeTypes::Vector>(currentOpenTab))->id;
    }

    const TabButton *getTabBy(const juce::String &id)
    {
        for (auto &tab : tabList.tabs)
        {
            if (id == tab->id)
            {
                return &tab->button;
            }
        }

        return nullptr;
    }

    const TabButton &getTabAt(int index) const noexcept
    {
        return tabList.tabs.at(static_cast<SizeTypes::Vector>(index))->button;
    }

    //==================================================================================================================
    void moveToTab(int index)
    {
        const auto &tab = tabList.tabs.at(static_cast<SizeTypes::Vector>(index));
        const auto view_area = tabList.getViewArea();
        
        if (const auto bounds = tab->button.getBounds(); !view_area.contains(bounds))
        {
            if (bounds.getX() > tabList.getViewArea().getX() && view_area.getWidth() >= bounds.getWidth())
            {
                tabList.setViewPosition(bounds.getRight() - tabList.getWidth(), 0);
            }
            else
            {
                tabList.setViewPosition(bounds.getX(), 0);
            }
        }
    }

    //==================================================================================================================
    int getTotalRequiredWidth() const noexcept
    {
        return tabList.getViewedComponent()->getWidth() + (menuButton.isVisible() ? menuButton.getWidth() : 0);
    }

private:
    TabStrip tabList;
    Style style;

    juce::TextButton menuButton;
    TabFactory &factory;
    LookAndFeel_Jaut *lookAndFeel { nullptr };

    int currentOpenTab { 0 };

    //==================================================================================================================
    void parentHierarchyChanged() override
    {
        lookAndFeelChanged();
    }

    void lookAndFeelChanged() override
    {
        if (lookAndFeel = dynamic_cast<LookAndFeel_Jaut*>(&getLookAndFeel()); !lookAndFeel)
        {
            lookAndFeel = &LookAndFeel_Jaut::getDefaultLaf();
        }
    }

    //==================================================================================================================
    void updateHiddenTabsMenu()
    {
        menuButton.setVisible(!tabList.getViewArea().contains(tabList.getViewedComponent()->getLocalBounds()));
    }

    //==================================================================================================================
    void drawButtonBackground(juce::Graphics &g, juce::Button&, const juce::Colour&, bool isOver, bool isDown) override
    {
        lookAndFeel->drawMultiTabPaneMoreTabsButton(g, menuButton, isOver, isDown);
    }
};
//======================================================================================================================
// endregion InternalTabBar
//**********************************************************************************************************************
// region TabButton
//======================================================================================================================
MultiPagePane::TabButton::TabButton(const Style &style, TabFactory &factory, const juce::String &id,
                                    const juce::String &name)
    : juce::Button(name),
      factory(factory), tabHeight(style.tabBarHeight), itemHeight(style.tabBarItemSize)
{
    JAUT_INIT_LAF()
    const auto &layouts = factory.getTabLayout();

    for (const auto & layout : layouts)
    {
        juce::Component *comp;

        switch (layout.itemType)
        {
            case TabFactory::TabItem::ItemText:
            {
                auto *label = new juce::Label("itemText", name);
                label->setInterceptsMouseClicks(false, false);
                label->setJustificationType(juce::Justification::centredLeft);
                label->setBorderSize({});

                comp = label;
                break;
            }

            case TabFactory::TabItem::ItemImage:
            {
                auto *img = new juce::ImageComponent("itemImage");
                img->setImage(factory.getImageForPage(id));
                img->setInterceptsMouseClicks(false, false);

                comp = img;
                break;
            }

            case TabFactory::TabItem::ItemCloseButton:
            {
                auto *button    = new juce::TextButton("itemCloseButton");
                button->onClick = [this]() { onClosing(tabIndex); };
                button->addMouseListener(this, false);

                comp = button;
                break;
            }

            case TabFactory::TabItem::ItemPinButton:
            {
                auto *button          = new juce::ToggleButton("itemPinButton");
                button->onStateChange = [this, button]() { onPinning(tabIndex, (pinned = button->getToggleState())); };
                button->addMouseListener(this, false);

                comp = button;
                break;
            }
        }

        comp->setLookAndFeel(this);
        addAndMakeVisible(comp);
        layoutComponents.emplace_back(comp);
    }
}

MultiPagePane::TabButton::~TabButton() = default;

//======================================================================================================================
void MultiPagePane::TabButton::paintButton(juce::Graphics &g, bool mouseOver, bool mouseDown)
{
    lookAndFeel->drawMultiTabPaneTabBackground(g, *this, mouseOver, mouseDown);
}

void MultiPagePane::TabButton::resized()
{
    if (const int item_size = itemHeight; item_size >= 0)
    {
        internalComponentLoop(factory, lookAndFeel, *this, [item_size](const auto&) { return item_size; });
    }
    else
    {
        if (const int tab_height = tabHeight; tab_height >= 0)
        {
            internalComponentLoop(factory, lookAndFeel, *this, [tab_height](const auto &margin)
            {
                return tab_height - margin.top - margin.bottom;
            });
        }
        else
        {
            const int font_size = std::ceil(lookAndFeel->getMultiTabPaneFont().getHeight());
            internalComponentLoop(factory, lookAndFeel, *this, [font_size](const auto&) { return font_size; });
        }
    }
}

//======================================================================================================================
void MultiPagePane::TabButton::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isLeftButtonDown()
     && e.eventComponent->getName() != "itemCloseButton" && e.eventComponent->getName() != "itemPinButton")
    {
        triggerTabClick();
    }
}

//======================================================================================================================
void MultiPagePane::TabButton::setActive(bool shouldBeActive)
{
    active = shouldBeActive;
    repaint();
}

bool MultiPagePane::TabButton::isActive() const noexcept
{
    return active;
}

//======================================================================================================================
void MultiPagePane::TabButton::setPinned(bool pin)
{
    if (pin == pinned)
    {
        return;
    }

    const auto &layouts = factory.getTabLayout();

    for (SizeTypes::Vector i = 0; i < layouts.size(); ++i)
    {
        if (layouts.at(i).itemType == TabFactory::TabItem::ItemPinButton)
        {
            static_cast<juce::ToggleButton*>(getChildComponent(i))->setToggleState(pin, juce::sendNotification);
        }
    }
}

bool MultiPagePane::TabButton::isPinned() const noexcept
{
    return pinned;
}

//======================================================================================================================
void MultiPagePane::TabButton::setTabIndex(int index)
{
    tabIndex = index;
}

int MultiPagePane::TabButton::getTabIndex() const noexcept
{
    return tabIndex;
}

//======================================================================================================================
void MultiPagePane::TabButton::triggerTabClick()
{
    onTabClicked(tabIndex);
}

//======================================================================================================================
void MultiPagePane::TabButton::updateStyle(const Style &newStyle)
{
    tabHeight  = newStyle.tabBarHeight;
    itemHeight = newStyle.tabBarItemSize;
    resized();
}

//======================================================================================================================
void MultiPagePane::TabButton::drawButtonBackground(juce::Graphics &g, Button &button, const juce::Colour&,
                                                    bool isOver, bool isDown)
{
    if (button.getName() == "itemCloseButton")
    {
        lookAndFeel->drawMultiTabPaneTabCloseButton(g, *this, static_cast<juce::TextButton&>(button), isOver, isDown);
    }
    else if (button.getName() == "itemPinButton")
    {
        lookAndFeel->drawMultiTabPaneTabPinButton(g, *this, static_cast<juce::ToggleButton&>(button), isOver, isDown);
    }
}
//======================================================================================================================
JAUT_IMPL_LAF(MultiPagePane::TabButton)
//======================================================================================================================
// endregion TabButton
//**********************************************************************************************************************
// region MultiPagePane
//======================================================================================================================
MultiPagePane::MultiPagePane(FactoryPtr factory)
    : MultiPagePane(std::move(factory), {})
{}

MultiPagePane::MultiPagePane(FactoryPtr factory, Style style)
    : tabBar(std::make_unique<InternalTabBar>(style, *factory)),
      factory(std::move(factory)), style(style)
{
    JAUT_INIT_LAF()

    // The TabFactory instance can't be null
    jassert(this->factory != nullptr);

    tabBar->onTabActivated = [this](const juce::String &id)
    {
        contentPane.resetComponent(*pages.at(id).get());
        TabChanged(id);
    };
    tabBar->onTabRemoved = [this](const juce::String &id)
    {
        if (pages.at(id).get() == contentPane.getCurrentComponent())
        {
            contentPane.resetComponent();
        }

        if (pages.size() == 1)
        {
            LastTabClosing(id);
        }

        pages.erase(id);
        TabClosed (id);
        updateTabBarBounds();

        if (!pages.empty())
        {
            const juce::String open_id = tabBar->getActiveId();
            contentPane.resetComponent(*pages.at(open_id));
            TabChanged(open_id);
        }
    };
    tabBar->onTabAdded = [this](const juce::String &id)
    {
        TabOpened (id);
        updateTabBarBounds();
        openPage(static_cast<int>(pages.size()) - 1);
    };
    tabBar->onTabPinned = [this](const juce::String &id, bool state)
    {
        TabPinStateChanged(id, state);
    };
    tabBar->setSize(0, ::getTabHeight(*this->factory, style, lookAndFeel));
    addAndMakeVisible(*tabBar);
    addAndMakeVisible(contentPane);
}

MultiPagePane::~MultiPagePane() = default;

//======================================================================================================================
void MultiPagePane::paint(juce::Graphics &g)
{
    lookAndFeel->drawMultiTabPaneBackground(g, *this);
}

void MultiPagePane::resized()
{
    const auto prev_rect = tabBar->getBounds();
    
    const Style::TabBarLayout layout = style.tabBarLayout;
    updateSpaceBounds(layout);
    
    if (layout == Style::TabBarLayout::Top || layout == Style::TabBarLayout::Bottom)
    {
        tabBar->setBounds(style.tabBarMargin.trimRectangle(tabSpaceBounds));
    }
    else
    {
        const Margin &tab_margin = style.tabBarMargin;
        juce::Rectangle<int> tab_rect { tabSpaceBounds.getX()      + tab_margin.left, 0,
                                        tabSpaceBounds.getHeight() - tab_margin.top  - tab_margin.bottom,
                                        tabSpaceBounds.getWidth()  - tab_margin.left - tab_margin.right };
        
        if (layout == Style::TabBarLayout::SideLeft)
        {
            if (const int req_width = tabBar->getTotalRequiredWidth(); req_width < tab_rect.getWidth())
            {
                tab_rect.setWidth(req_width);
            }
            
            tab_rect.setY(tabSpaceBounds.getY() + tab_rect.getWidth() + tab_margin.top);
            tabBar->setTransform(juce::AffineTransform::rotation(juce::float_Pi * 1.5f,
                                                                 static_cast<float>(tab_rect.getX()),
                                                                 static_cast<float>(tab_rect.getY())));
        }
        else
        {
            tab_rect.setX(tabSpaceBounds.getRight() - tab_margin.right);
            tab_rect.setY(tabSpaceBounds.getY()     + tab_margin.top);
    
            tabBar->setTransform(juce::AffineTransform::rotation(juce::float_Pi * 0.5f,
                                                                 static_cast<float>(tab_rect.getX()),
                                                                 static_cast<float>(tab_rect.getY())));
        }
    
        tabBar->setBounds(tab_rect);
    }

    contentPane.setBounds(style.pageViewMargin.trimRectangle(pageSpaceBounds));
    
    if (prev_rect == tabBar->getBounds())
    {
        tabBar->resized();
    }
}

//======================================================================================================================
void MultiPagePane::addPage(const juce::String &pageId, const juce::String &name)
{
    jassert(factory != nullptr);
    const juce::String id = pageId.trim().toLowerCase();

    if (pages.find(id) == pages.end())
    {
        if (auto new_page = factory->createPageForId(id))
        {
            pages.emplace(id, std::move(new_page));
            tabBar->addTab(id, name);
        }
    }
}

void MultiPagePane::closePage(int index)
{
    if (fit<int>(index, 0, pages.size()))
    {
        tabBar->removeTab(index);
    }
}

void MultiPagePane::openPage(int index)
{
    if (fit<int>(index, 0, pages.size()) && index != tabBar->getActiveTabIndex())
    {
        tabBar->activateTab(index);
    }
}

void MultiPagePane::pinPage(int index, bool shouldBePinned)
{
    auto &tab = tabBar->getTabAt(index);

    if (fit<int>(index, 0, pages.size()) && getTab(index)->isPinned() != shouldBePinned)
    {
        tabBar->pinTab(index, shouldBePinned);
    }
}

//======================================================================================================================
int MultiPagePane::getNumPages() const noexcept
{
    return pages.size();
}

bool MultiPagePane::hasAnyPages() const noexcept
{
    return !pages.empty();
}

//======================================================================================================================
const MultiPagePane::TabButton* MultiPagePane::getTab(int index) const noexcept
{
    return hasAnyPages() ? &tabBar->getTabAt(index) : nullptr;
}

juce::Component* MultiPagePane::getPage(int index) noexcept
{
    return hasAnyPages() ? pages.at(tabBar->getIdAt(index)).get() : nullptr;
}

const juce::Component* MultiPagePane::getPage(int index) const noexcept
{
    return hasAnyPages() ? pages.at(tabBar->getIdAt(index)).get() : nullptr;
}

const MultiPagePane::TabButton* MultiPagePane::getTab(const juce::String &pageId) const noexcept
{
    return tabBar->getTabBy(pageId.trim().toLowerCase());
}

juce::Component* MultiPagePane::getPage(const juce::String &pageId) noexcept
{
    auto it = pages.find(pageId.trim().toLowerCase());
    return it != pages.end() ? it->second.get() : nullptr;
}

const juce::Component* MultiPagePane::getPage(const juce::String &pageId) const noexcept
{
    auto it = pages.find(pageId.trim().toLowerCase());
    return it != pages.end() ? it->second.get() : nullptr;
}

//======================================================================================================================
int MultiPagePane::getActiveIndex() const noexcept
{
    return hasAnyPages() ? tabBar->getActiveTabIndex() : -1;
}

juce::String MultiPagePane::getActiveId() const noexcept
{
    return hasAnyPages() ? tabBar->getActiveId() : "";
}

const MultiPagePane::TabButton* MultiPagePane::getActiveTab() const noexcept
{
    return hasAnyPages() ? &tabBar->getTabAt(getActiveIndex()) : nullptr;
}

juce::Component* MultiPagePane::getActivePage() noexcept
{
    return hasAnyPages() ? pages.at(getActiveId()).get() : nullptr;
}

const juce::Component* MultiPagePane::getActivePage() const noexcept
{
    return hasAnyPages() ? pages.at(getActiveId()).get() : nullptr;
}

//======================================================================================================================
void MultiPagePane::setStyle(const Style &newStyle)
{
    if (tabBar->updateStyle(newStyle) || style.tabBarMargin != newStyle.tabBarMargin
        || style.tabBarLayout != newStyle.tabBarLayout || style.componentPadding != newStyle.componentPadding)
    {
        resized();
    }

    if (style.pageViewMargin != newStyle.pageViewMargin)
    {
        const auto layout      = style.tabBarLayout;
        const auto tab_margin  = style.tabBarMargin;
        juce::Rectangle<int> page_space;

        if (layout == Style::TabBarLayout::Top || layout == Style::TabBarLayout::Bottom)
        {
            const int height = tab_margin.top + tab_margin.bottom + ::getTabHeight(*factory, style, lookAndFeel);
            const int y      = layout == Style::TabBarLayout::Top ? height : 0;
            page_space.setBounds(0, y, getWidth(), getHeight() - height);
        }
        else
        {
            const int width = tab_margin.left + tab_margin.right + ::getTabHeight(*factory, style, lookAndFeel);
            const int x     = layout == Style::TabBarLayout::SideLeft ? width : 0;
            page_space.setBounds(x, 0, getWidth() - width, getHeight());
        }

        contentPane.setBounds(style.pageViewMargin.trimRectangle(page_space));
    }

    style = newStyle;
}

const MultiPagePane::Style& MultiPagePane::getStyle() const noexcept
{
    return style;
}

juce::Rectangle<int> MultiPagePane::getTabSpaceBounds() const noexcept
{
    return tabSpaceBounds;
}

juce::Rectangle<int> MultiPagePane::getPageSpaceBounds() const noexcept
{
    return pageSpaceBounds;
}

//======================================================================================================================
void MultiPagePane::updateSpaceBounds(Style::TabBarLayout layout)
{
    const Padding &top_padding = style.componentPadding;
    const Margin  &tab_margin  = style.tabBarMargin;
    const int      tab_height  = ::getTabHeight(*factory, style, lookAndFeel);

    if (layout == Style::TabBarLayout::Top || layout == Style::TabBarLayout::Bottom)
    {
        const int width  = getWidth() - top_padding.right - top_padding.left;
        const int height = tab_margin.top + tab_margin.bottom + tab_height;

        tabSpaceBounds .setSize(width, height);
        pageSpaceBounds.setSize(width, getHeight() - height - top_padding.top - top_padding.bottom);

        if (layout == Style::TabBarLayout::Top)
        {
            tabSpaceBounds .setPosition(top_padding.left, top_padding.top);
            pageSpaceBounds.setPosition(top_padding.left, tabSpaceBounds.getBottom());
        }
        else
        {
            pageSpaceBounds.setPosition(top_padding.left, top_padding.top);
            tabSpaceBounds .setPosition(top_padding.left, pageSpaceBounds.getBottom());
        }
    }
    else
    {
        const int width  = tab_margin.left + tab_margin.right + tab_height;
        const int height = getHeight() - top_padding.top - top_padding.bottom;

        tabSpaceBounds .setSize(width,                                                     height);
        pageSpaceBounds.setSize(getWidth() - width - top_padding.left - top_padding.right, height);

        if (layout == Style::TabBarLayout::SideLeft)
        {
            tabSpaceBounds .setPosition(top_padding.left,          top_padding.top);
            pageSpaceBounds.setPosition(tabSpaceBounds.getRight(), top_padding.top);
        }
        else
        {
            pageSpaceBounds.setPosition(top_padding.left,           top_padding.top);
            tabSpaceBounds .setPosition(pageSpaceBounds.getRight(), top_padding.top);
        }
    }
}

void MultiPagePane::updateTabBarBounds()
{
    const auto prev_rect = tabBar->getBounds();

    const Style::TabBarLayout layout = style.tabBarLayout;
    const Margin &tab_margin         = style.tabBarMargin;
    const int tab_height             = ::getTabHeight(*factory, style, lookAndFeel);

    if (layout == Style::TabBarLayout::SideLeft)
    {
        const int total_width = std::min(tabBar->getTotalRequiredWidth(),
                                         getHeight() - tab_margin.top - tab_margin.bottom);
        const int rel_x = tab_margin.left;
        const int rel_y = tab_margin.top + total_width;
        tabBar->setTransform(juce::AffineTransform::rotation(juce::float_Pi * 1.5f, static_cast<float>(rel_x),
                                                             static_cast<float>(rel_y)));
        tabBar->setBounds(rel_x, rel_y, total_width, tab_height);
    }

    if (prev_rect == tabBar->getBounds())
    {
        tabBar->resized();
    }
}

//======================================================================================================================
// endregion MultiPagePane
//**********************************************************************************************************************
//======================================================================================================================
//======================================================================================================================
// endregion MultiPagePane
//**********************************************************************************************************************
// region LookAndFeel_Jaut
//======================================================================================================================
LookAndFeel_Jaut::LookAndFeel_Jaut()
{
    const auto &currentColourScheme = getCurrentColourScheme();

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

        ContentPane::ColourBackgroundId, DefaultColours::Colour_Background.getARGB(),

        SplitContainer::ColourBackgroundId, DefaultColours::Colour_Background.getARGB(),
        SplitContainer::ColourSeperatorId, DefaultColours::Colour_Border    .getARGB(),

        MultiPagePane::ColourBackgroundId, DefaultColours::Colour_Background.getARGB(),
        MultiPagePane::ColourTabId, DefaultColours::Colour_Button.getARGB(),
        MultiPagePane::ColourTabActiveId, DefaultColours::Colour_ButtonDown.getARGB(),
        MultiPagePane::ColourTabHighlightId, DefaultColours::Colour_ButtonOver.getARGB(),
        MultiPagePane::ColourTabTextId, DefaultColours::Colour_ButtonText.getARGB(),
        MultiPagePane::ColourTabCloseButtonId, juce::Colours ::transparentBlack.getARGB(),
        MultiPagePane::ColourTabCloseButtonHighlightId, DefaultColours::Colour_Button.getARGB()
    };

    for (int i = 0; i < juce::numElementsInArray(colours); i += 2)
    {
        setColour(static_cast<int>(colours[i]), juce::Colour(colours[i + 1]));
    }
}

//======================================================================================================================
void LookAndFeel_Jaut::drawContentPaneBackground(juce::Graphics &g, juce::Rectangle<int>)
{
    g.setColour(findColour(ContentPane::ColourBackgroundId));
    g.fillAll();
}

//======================================================================================================================
void LookAndFeel_Jaut::drawSplitContainerBackground(juce::Graphics &g, juce::Rectangle<int>)
{
    g.setColour(findColour(SplitContainer::ColourBackgroundId));
    g.fillAll();
}

void LookAndFeel_Jaut::drawSplitContainerSeperator(juce::Graphics &g, juce::Rectangle<int> bounds,
                                                   SplitContainer::Orientation)
{
    g.setColour(findColour(SplitContainer::ColourSeperatorId));
    g.fillRect(bounds);
}

//======================================================================================================================
juce::Font LookAndFeel_Jaut::getMultiTabPaneFont()
{
    return juce::Font().withHeight(13.0f);
}

void LookAndFeel_Jaut::drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane&)
{
    g.setColour(findColour(MultiPagePane::ColourBackgroundId));
    g.fillAll();
}

void LookAndFeel_Jaut::drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                                      bool isMouseOver, bool isMouseDown)
{
    constexpr float w      = 8.0f;
    constexpr float h      = 5.0f;
    constexpr float half_w = w / 2.0f;

    const juce::Rectangle<int> bounds = button.getLocalBounds();

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
                                                     bool isMouseOver, bool isMouseDown)
{
    constexpr int bottom_line = 2;

    const bool is_active      = tabButton.isActive();
    const juce::Colour colour = is_active ? findColour(MultiPagePane::ColourTabActiveId)
                                          : (isMouseOver ? findColour(MultiPagePane::ColourTabHighlightId)
                                                         : findColour(MultiPagePane::ColourTabId));
    g.setColour(is_active && isMouseOver ? colour.darker() : colour);
    g.fillAll();

    if (is_active)
    {
        const juce::Rectangle<int> bounds = tabButton.getLocalBounds();
        g.setColour(findColour(MultiPagePane::ColourTabTextId));
        g.fillRect(0, bounds.getHeight() - bottom_line, bounds.getWidth(), bottom_line);
    }
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

    const juce::Colour &colour_text   = findColour(MultiPagePane::ColourTabTextId);
    const juce::Rectangle<int> bounds = button.getLocalBounds();

    g.setColour(isMouseOver ? colour_text : findColour(MultiPagePane::ColourTabCloseButtonId));
    g.fillEllipse({ bounds.getWidth()  / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f, c, c });

    if (tabButton.isActive())
    {
        g.setColour(isMouseOver ? findColour(MultiPagePane::ColourTabActiveId) : colour_text);
    }
    else
    {
        g.setColour(isMouseOver ? findColour(MultiPagePane::ColourTabHighlightId) : colour_text);
    }

    juce::Path path_x;
    path_x.addLineSegment({ 0.0f, 0.0f, s,    s }, 1.0f);
    path_x.addLineSegment({ s,    0.0f, 0.0f, s }, 1.0f);

    g.fillPath(path_x, juce::AffineTransform::translation(bounds.getWidth()  / 2.0f - s / 2.0f,
                                                          bounds.getHeight() / 2.0f - s / 2.0f));
}

void LookAndFeel_Jaut::drawMultiTabPaneTabPinButton(juce::Graphics &g, const MultiPagePane::TabButton &tabButton,
                                                    const juce::ToggleButton &button, bool isMouseOver,
                                                    bool isMouseDown)
{
    constexpr float s =     7.0f;
    constexpr float c = s * 1.8f;

    const juce::Colour &colour_text    = findColour(MultiPagePane::ColourTabTextId);
    const juce::Rectangle<int> bounds  = button.getLocalBounds();
    const juce::Rectangle<float> ellipse(bounds.getWidth() / 2.0f - c / 2.0f, bounds.getHeight() / 2.0f - c / 2.0f,
                                         c,                                   c);
    g.setColour(isMouseOver ? colour_text : findColour(MultiPagePane::ColourTabCloseButtonId));
    g.fillEllipse(ellipse);

    g.setColour(findColour(MultiPagePane::ColourTabHighlightId));

    if (tabButton.isPinned())
    {
        g.fillEllipse(ellipse.reduced(2.0f));
    }
    else
    {
        g.drawEllipse(ellipse.reduced(2.0f), 2.0f);
    }
}

//======================================================================================================================
JAUT_IMPL_LAF(ContentPane)
JAUT_IMPL_LAF(SplitContainer)
JAUT_IMPL_LAF(MultiPagePane)
//======================================================================================================================
// endregion LookAndFeel_Jaut
//**********************************************************************************************************************
}
