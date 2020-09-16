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
#include <jaut_gui/mouse/draganddropcontaineradvanced.h>
#include "multipagepane.h"

namespace
{
/** The amount of difference on the x-axis between two tabs before swapping them. */
inline constexpr int Const_TabSwapSensitivity = 25;

//======================================================================================================================
bool hasPinFlag(const jaut::MultiPagePane::Options &options, jaut::MultiPagePane::Options::PinBehaviour flag)
{
    return (options.pinnedTabBehaviour & flag) == flag;
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
ContentPane::ContentPane(juce::Component &parContent) noexcept
    : content(parContent)
{
    JAUT_INIT_LAF()
}

ContentPane::ContentPane(juce::Component *parContent, bool owned)
    : content(parContent, owned)
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

const juce::Component* ContentPane::getCurrentComponent() const noexcept
{
    return content.get();
}
//======================================================================================================================
// endregion ContentPane
//**********************************************************************************************************************
// region SplitContainer
//======================================================================================================================
SplitContainer::SplitContainer(SplitContainer::Orientation parOrientation) noexcept
    : constrainer(parOrientation), orientation(parOrientation)
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
            seperator.setBounds(static_cast<int>(std::ceil(static_cast<float>(getWidth()) / 2.0f - h_thickness)), 0,
                                thickness, getHeight());
        }
        else
        {
            seperator.setBounds(0, static_cast<int>(std::ceil(static_cast<float>(getHeight()) / 2.0f - h_thickness)),
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
    ContentPane &pane = order == ComponentOrder::LeftOrTop ? firstComponent : secondComponent;

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
        const int diff             = static_cast<int>(std::ceil(static_cast<float>(val - prev) / 2.0f));
        const juce::Rectangle rect = seperator.getBounds();

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
class MultiPagePane::InternalTabBar final : public juce::Component, private juce::LookAndFeel_V4,
                                                                    private juce::ComponentListener
{
public:
    class TabStrip : public juce::Viewport
    {
    public:
        struct DragHelper : public juce::MouseListener
        {
            //==========================================================================================================
            struct Constrainer : juce::ComponentBoundsConstrainer
            {
                void applyBoundsToComponent(Component &tab, juce::Rectangle<int> bounds) override
                {
                    juce::Component &parent = *tab.getParentComponent();
                    tab.setBounds(bounds.constrainedWithin(parent.getBounds()));
                }
            };
            
            //==========================================================================================================
            juce::ComponentDragger tabDragger;
            Constrainer            dragConstrainer;
            
            TabStrip &tabStrip;
            
            TabButton *draggedTab { nullptr };
            int        dragIndex  { 0 };
            
            //==========================================================================================================
            explicit DragHelper(TabStrip &parTabStrip)
                : tabStrip(parTabStrip)
            {}
            
            //==========================================================================================================
            TabButton* getDraggedTab() const noexcept
            {
                return draggedTab;
            }
            
            //==========================================================================================================
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
                    std::exchange(draggedTab, nullptr)->setTopLeftPosition(dragIndex == 0 ? 0
                                      : tabStrip.tabs.at(static_cast<SizeTypes::Vector>(dragIndex - 1))->getRight(), 0);
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
        
        //==============================================================================================================
        explicit TabStrip(InternalTabBar &parTabBar)
            : tabBar(parTabBar), dragger(*this)
        {
            setViewedComponent(&content, false);
        }
    
        //==============================================================================================================
        void resized() override
        {
            content.setSize(content.getWidth(), getHeight());
            juce::Viewport::resized();
        }
        
        //==============================================================================================================
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
        
        //==============================================================================================================
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
        
        //==============================================================================================================
        bool hasTabs() const noexcept
        {
            return !tabs.empty();
        }
        
        bool hasHiddenTabs() const noexcept
        {
            return !tabs.empty() && !getViewArea().contains(content.getBounds());
        }
    
        //==============================================================================================================
        int getNumTabs() const noexcept
        {
            return static_cast<int>(tabs.size());
        }
        
        const std::vector<TabButton*>& getTabs() const noexcept
        {
            return tabs;
        }
    
        //==============================================================================================================
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
    
        //==============================================================================================================
        std::vector<TabButton*> tabs;
        
        InternalTabBar &tabBar;
        
        juce::Component content;
        DragHelper      dragger;
    
        bool canReorder  { false };
        bool canExchange { false };
        bool canExtract  { false };
        
        //==============================================================================================================
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
    
    //==================================================================================================================
    using TabPointer = std::unique_ptr<TabButton>;
    using TabVector  = std::vector<TabPointer>;
    
    //==================================================================================================================
    std::function<void(const juce::String&)>       onTabClosed   { nullptr };
    std::function<void(const juce::String&)>       onTabChanged  { nullptr };
    std::function<PagePtr(const juce::String&)>    onTabDetached { nullptr };
    std::function<void(const juce::String&, bool)> onTabPinned   { nullptr };
    
    //==================================================================================================================
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
                        if (tab->getTabIndex() != activeTab && !tabStripPinned.getViewArea().contains(tab->getBounds()))
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
                        if (tab->getTabIndex() != activeTab && !tabStripNormal.getViewArea().contains(tab->getBounds()))
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
    
    //==================================================================================================================
    void resized() override
    {
        juce::Rectangle<int> bounds = getLocalBounds();
        
        if (getRequiredWidth() > getWidth())
        {
            const int tab_half              = tabHeight / 2;
            const juce::Rectangle<int> rect = bounds.removeFromRight(tabHeight);
            buttonHiddenTabs.setVisible(true);
            buttonHiddenTabs.setBounds(marginHiddenTabs.trimRectangle(rect.withPosition(rect.getCentreX() - tab_half,
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
    
    //==================================================================================================================
    void addTab(const Style &parStyle, const TabFactory &parFactory, const juce::String &id, const juce::String &name)
    {
        const int tab_index = static_cast<int>(tabs.size());
        TabPointer tab      = std::make_unique<TabButton>(parStyle, parFactory, id, name);
        tab->updateTabHeight(tabHeight);
        tab->setTabIndex(tab_index);
        tab->onActivating = [this](const TabButton &parTab)
        {
            tabs.at(static_cast<SizeTypes::Vector>(std::exchange(activeTab, parTab.getTabIndex())))->setActive(false);
    
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
    
    //==================================================================================================================
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
    
    //==================================================================================================================
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
    
    //==================================================================================================================
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
    
    //==================================================================================================================
    int getRequiredWidth() const noexcept
    {
        return std::max(tabStripPinned.getViewedComponent()->getWidth(),
                        tabStripNormal.getViewedComponent()->getWidth());
    }
    
    int getTabHeight() const noexcept
    {
        return tabHeight;
    }
    
    //==================================================================================================================
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
    
    //==================================================================================================================
    void componentNameChanged(Component &component) override
    {
        dragDropJson.setProperty("id", component.getName());
    }
    
    //==================================================================================================================
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
    
    //==================================================================================================================
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
    
    //==================================================================================================================
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
    
    //==================================================================================================================
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}
    void drawButtonText(juce::Graphics &g, juce::TextButton &button, bool isOver, bool isDown) override
    {
        lookAndFeel->drawMultiTabPaneMoreTabsButton(g, button, isOver, isDown);
    }
};
//======================================================================================================================
// endregion InternalTabBar
//**********************************************************************************************************************
// region TabButton
//======================================================================================================================
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

//======================================================================================================================
void MultiPagePane::TabButton::paintButton(juce::Graphics &g, bool mouseOver, bool mouseDown)
{
    lookAndFeel->drawMultiTabPaneTabBackground(g, *this, mouseOver, mouseDown);
}

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
void MultiPagePane::TabButton::setPinned(bool pin)
{
    if (std::exchange(pinned, pin) != pin)
    {
        const auto &layouts = factory.getTabLayout();
    
        for (int i = 0; i < static_cast<int>(layouts.size()); ++i)
        {
            if (layouts.at(static_cast<SizeTypes::Vector>(i)).itemType == TabFactory::TabItem::ItemPinButton)
            {
                // NOLINTNEXTLINE
                static_cast<juce::ToggleButton*>(getChildComponent(i))->setToggleState(pin, juce::dontSendNotification);
            }
        }
        
        repaint();
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

//======================================================================================================================
void MultiPagePane::TabButton::drawButtonText(juce::Graphics &g, juce::TextButton &button, bool isOver, bool isDown)
{
    if (button.getName() == "itemCloseButton")
    {
        lookAndFeel->drawMultiTabPaneTabCloseButton(g, *this, button, isOver, isDown);
    }
}

void MultiPagePane::TabButton::drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool isOver, bool isDown)
{
    if (button.getName() == "itemPinButton")
    {
        lookAndFeel->drawMultiTabPaneTabPinButton(g, *this, button, isOver, isDown);
    }
}
//======================================================================================================================
// endregion TabButton
//**********************************************************************************************************************
// region DesktopTabPane
//======================================================================================================================
MultiPagePane::Window::Window(const juce::String &name, std::unique_ptr<TabFactory> parFactory, Style parStyle,
                              Options parOptions)
    : ResizableWindow(name, false)
{
    JAUT_INIT_LAF();
    
    auto pane = std::make_unique<MultiPagePane>(std::move(parFactory), parStyle, parOptions);
    pane->LastTabClosing += make_handler(&Window::forceToCloseWindow, this);
    setContentOwned(pane.release(), false);
    
    setSize(400, 250);
    setUsingNativeTitleBar(true);
    setOpaque(true);
    setVisible(true);
}

MultiPagePane::Window::Window(const juce::String &name, std::unique_ptr<TabFactory> parFactory, Style parStyle)
    : Window(name, std::move(parFactory), parStyle, {})
{}

MultiPagePane::Window::Window(const juce::String &name, std::unique_ptr<TabFactory> parFactory, Options parOptions)
    : Window(name, std::move(parFactory), {}, parOptions)
{}

MultiPagePane::Window::Window(const juce::String &name, std::unique_ptr<TabFactory> parFactory)
    : Window(name, std::move(parFactory), {}, {})
{}

//======================================================================================================================
void MultiPagePane::Window::resized()
{
    ResizableWindow::resized();
    getContentComponent()->setBounds(getLocalBounds());
}

//======================================================================================================================
int MultiPagePane::Window::getDesktopWindowStyleFlags() const
{
    return lookAndFeel->getWindowStyleFlags();
}

MultiPagePane &MultiPagePane::Window::getMultiPagePane() noexcept
{
    return *static_cast<MultiPagePane*>(getContentComponent());
}

//======================================================================================================================
void MultiPagePane::Window::userTriedToCloseWindow()
{
    WindowClosing(this);
}

//======================================================================================================================
void MultiPagePane::Window::forceToCloseWindow(const juce::String&)
{
    removeFromDesktop();
    userTriedToCloseWindow();
}
//======================================================================================================================
// endregion DesktopTabPane
//**********************************************************************************************************************
// region MultiPagePane
//======================================================================================================================
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
      style(parStyle), options(parOptions)
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
    tabBar->setPinBehaviour   (::hasPinFlag(parOptions, Options::PinBehaviour::PutInExtraRow));
    tabBar->setTabsReorderable(parOptions.allowTabReordering);
    
    tabBar->setTabsCanBeExchanged(parOptions.allowDragExchange);
    tabBar->setTabsCanBeExtracted(parOptions.allowPageExtraction);
    
    // Tab callbacks
    tabBar->onTabClosed = [this](const juce::String &pageId)
    {
        if (pageView.getCurrentComponent()->getName() == pageId)
        {
            pageView.resetComponent();
        }
        
        const bool was_last = (pages.size() == 1);
        
        pages.erase(pageId);
        resized();
    
        TabClosed(pageId);
    
        if (was_last)
        {
            LastTabClosing(pageId);
            
            if (backgroundComponent)
            {
                pageView.resetComponent(*backgroundComponent);
            }
        }
    };
    tabBar->onTabChanged = [this](const juce::String &pageId)
    {
        pageView.resetComponent(*pages.at(pageId));
        TabChanged(pageId);
    };
    tabBar->onTabPinned = [this](const juce::String &pageId, bool state)
    {
        if (::hasPinFlag(options, Options::PinBehaviour::PutInExtraRow))
        {
            resized();
        }
    
        TabPinStateChanged(pageId, state);
    };
    tabBar->onTabDetached = [this](const juce::String &pageId)
    {
        if (pageView.getCurrentComponent()->getName() == pageId)
        {
            pageView.resetComponent();
        }
        
        PagePtr page        = std::move(pages.at(pageId));
        const bool was_last = (pages.size() == 1);
        
        pages.erase(pageId);
        resized();
        
        TabClosed(pageId);
    
        if (was_last)
        {
            LastTabClosing(pageId);
    
            if (backgroundComponent)
            {
                pageView.resetComponent(*backgroundComponent);
            }
        }
        
        return page;
    };
    // Add components
    addAndMakeVisible(*tabBar);
    addAndMakeVisible(pageView);
}

MultiPagePane::~MultiPagePane() = default;

//======================================================================================================================
void MultiPagePane::paint(juce::Graphics &g)
{
    lookAndFeel->drawMultiTabPaneBackground(g, *this);
}

void MultiPagePane::paintOverChildren(juce::Graphics &g)
{
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
    const juce::Rectangle<int> prev_bounds = tabBar->getBounds();
    const Style::TabBarLayout layout       = style.tabBarLayout;
    updateSpaceBounds(layout);
    
    if (layout == Style::TabBarLayout::Top || layout == Style::TabBarLayout::Bottom)
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
        
        if (layout == Style::TabBarLayout::SideLeft)
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

//======================================================================================================================
void MultiPagePane::addPage(const juce::String &pageId, const juce::String &name)
{
    jassert(factory != nullptr);
    const juce::String id = pageId.trim().toLowerCase();

    if (pages.find(id) == pages.end())
    {
        if (auto new_page = factory->createPageForId(id))
        {
            new_page->setName(id);
            pages.emplace(id, std::move(new_page));
            tabBar->addTab(style, *factory, id, !name.isEmpty() ? name : id);
            
            resized();
    
            TabOpened(id);
        }
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
    if (!::hasPinFlag(options, Options::PinBehaviour::ClosingAllNoEffect) || forcePinned)
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

//======================================================================================================================
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

//======================================================================================================================
juce::Component* MultiPagePane::getBackgroundComponent() noexcept
{
    return backgroundComponent.get();
}

void MultiPagePane::setBackgroundComponent(juce::Component *component, bool owned)
{
    if (component == backgroundComponent.get())
    {
        return;
    }
    
    if (pages.empty())
    {
        pageView.resetComponent(component, false);
    }
    
    backgroundComponent.set(component, owned);
}

void MultiPagePane::setBackgroundComponent(juce::Component &component)
{
    setBackgroundComponent(&component, false);
}

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
void MultiPagePane::setStyle(const Style &parStyle)
{
    const Style old_style = std::exchange(style, parStyle);
    bool needs_resize     = false;
    
    if (old_style.tabBarMargin     != parStyle.tabBarMargin
     || old_style.componentPadding != parStyle.componentPadding)
    {
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
        if (parStyle.tabBarLayout == Style::TabBarLayout::Top || parStyle.tabBarLayout == Style::TabBarLayout::Bottom)
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
        resized();
    }
    else if (old_style.pageViewMargin != parStyle.pageViewMargin)
    {
        pageView.setBounds(parStyle.pageViewMargin.trimRectangle(pageSpaceBounds));
    }
}

const MultiPagePane::Style& MultiPagePane::getStyle() const noexcept
{
    return style;
}

void MultiPagePane::setOptions(const Options &parOptions)
{
    const Options old_options = std::exchange(options, parOptions);
    const bool pin_in_new_tab = ::hasPinFlag(parOptions,  Options::PinBehaviour::PutInExtraRow);
    
    if (::hasPinFlag(old_options, Options::PinBehaviour::PutInExtraRow) != pin_in_new_tab)
    {
        tabBar->setPinBehaviour(pin_in_new_tab);
        resized();
    }
    
    tabBar->setTabsReorderable(parOptions.allowTabReordering);
    tabBar->setTabsCanBeExchanged(parOptions.allowDragExchange);
    tabBar->setTabsCanBeExtracted(parOptions.allowPageExtraction);
}

const MultiPagePane::Options& MultiPagePane::getOptions() const noexcept
{
    return options;
}

//======================================================================================================================
juce::Rectangle<int> MultiPagePane::getTabSpaceBounds() const noexcept
{
    return tabSpaceBounds;
}

juce::Rectangle<int> MultiPagePane::getPageSpaceBounds() const noexcept
{
    return pageSpaceBounds;
}

//======================================================================================================================
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

//======================================================================================================================
void MultiPagePane::updateSpaceBounds(Style::TabBarLayout layout)
{
    const Padding &top_padding = style.componentPadding;
    const Margin  &tab_margin  = style.tabBarMargin;
    const int tab_height       = style.tabButtonHeight * ((tabBar->hasPinnedTabs() && tabBar->hasNonPinnedTabs()) + 1);

    if (layout == Style::TabBarLayout::Top || layout == Style::TabBarLayout::Bottom)
    {
        const int width  = getWidth()     - top_padding.right - top_padding.left;
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
        const int height = getHeight()     - top_padding.top  - top_padding.bottom;
        
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

    const juce::Colour &colour_text     = findColour(MultiPagePane::ColourTabTextId);
    const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();

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
    path_x.addLineSegment({ 0.0f, 0.0f, s,    s }, 1.5f);
    path_x.addLineSegment({ s,    0.0f, 0.0f, s }, 1.5f);

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
                                         c,                                   c);
    g.setColour(isMouseOver ? colour_text : findColour(MultiPagePane::ColourTabCloseButtonId));
    g.fillEllipse(ellipse);
    
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

//======================================================================================================================
int LookAndFeel_Jaut::getWindowStyleFlags() const
{
    using Flags = juce::ComponentPeer::StyleFlags;
    return Flags::windowHasTitleBar | Flags::windowHasCloseButton;
}

//======================================================================================================================
JAUT_IMPL_LAF(ContentPane)
JAUT_IMPL_LAF(SplitContainer)
JAUT_IMPL_LAF(MultiPagePane)
JAUT_IMPL_LAF(MultiPagePane::TabButton)
JAUT_IMPL_LAF(MultiPagePane::Window)
//======================================================================================================================
// endregion LookAndFeel_Jaut
//**********************************************************************************************************************
}
