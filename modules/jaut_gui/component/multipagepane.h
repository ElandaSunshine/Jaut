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
    @file   multitabpane.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  An extremely customisable component that lets you add pages that will be controlled by a tab bar.
 *  Every page you add will get its own tab component, which stands in correlation to said page.
 *
 *  You can customize almost any aspect about it, tab-bar layout, page and tab size, component padding and margin,
 *  there is no limit.
 *
 *  To begin with, you need to create an implementation of MultiPagePane::TabFactory which is responsible for
 *  mapping ids to images, page components and tab-item-layout.
 *  Everything specified in your layout, will define how your component will behave.
 *
 *  You can also dynamically change style options about the component by using setStyle.
 */
class JAUT_API MultiPagePane : public juce::Component, private juce::DragAndDropContainer
{
public:
    /** The drag-and-drop identifier when using juce::DragAndDropContainer and juce::DragAndDropTarget */
    static constexpr const char *DragAndDrop_ID = "6d062147-33ef-4ea4-b9ec-04756959fe08";

    //==================================================================================================================
    enum JAUT_API ColourIds
    {
        ColourBackgroundId              = nextColourId<19>,
        ColourTabId                     = nextColourId<20>,
        ColourTabActiveId               = nextColourId<21>,
        ColourTabHighlightId            = nextColourId<22>,
        ColourTabTextId                 = nextColourId<23>,
        ColourTabCloseButtonId          = nextColourId<24>,
        ColourTabCloseButtonHighlightId = nextColourId<25>
    };
    
    /** Defines several different style options which define what this component will or should look like. */
    struct JAUT_API Style
    {
        /** Defines the position of the tab-bar. */
        enum class JAUT_API TabBarLayout
        {
            /** The tab-bar will appear on the top of the component. */
            Top,
            
            /** The tab-bar will appear on the bottom of the component. */
            Bottom,
            
            /** The tab-bar will appear on the left-side of the component, rotated 90 degrees counter-clockwise. */
            SideLeft,
    
            /** The tab-bar will appear on the right-side of the component, rotated 90 degrees clockwise. */
            SideRight
        };
        
        //==============================================================================================================
        /** The padding between all child-components and this one. */
        Padding componentPadding;
        
        /** The margin between this container and the tab-bar. */
        Margin tabBarMargin;
        
        /** The margin between this container and the page-view. */
        Margin pageViewMargin;
        
        /** The margin between the tab-bar and the more tabs button. */
        Margin moreTabsButtonMargin { 4 };
        
        /** The layout of the tab bar. */
        TabBarLayout tabBarLayout { TabBarLayout::Top };
        
        /**
         *  The height of the items inside tabs.
         *
         *  For buttons this will also determine their widths, images will have their width resized according to their
         *  aspect ratio and text will be stretched to be at least fully contained.
         *
         *  If this is below zero, the size will be determined by tabBarHeight and the respective item
         *  margin properties.
         *
         *  If this and tabBarHeight both are below zero, the LookAndFeel-set font-size will determine the items
         *  heights.
         */
        int tabBarItemSize { -1 };
        
        /**
         *  The tab height that will be used.
         *  This is absolute which means this will, regardless tabBarItemSize, force the height onto the tab-bar or
         *  in other words: If this is below tabBarItemSize but above negative, the tab will be sized regardless
         *  the item height.
         *
         *  If below zero, this will automatically determine the tab height by the tab's layout-item margin properties
         *  and tabBarItemSize.
         *
         *  This does not impact the height of the tab's items.
         */
        int tabBarHeight { 24 };
    };
    
    /**
     *  The TabFactory specifies the tab and page behaviour.
     *  This is important to be used as it will determine which id is what page-component and what tabs should look
     *  like.
     *
     *  This will also map images to tabs if images are being actively used by this TabFactory object.
     *
     *  If you don't want to use a custom version of this, you can use DefaultTabFactory and pass a lambda with
     *  components mapped with ids.
     *
     *  Note that TabFactories are static, you cannot change them afterwards, so you may create your logic inside
     *  the derived class.
     */
    struct JAUT_API TabFactory
    {
        /** The type of item to add to all tabs. */
        enum class JAUT_API TabItem
        {
            /** A juce::Label containing the tab's name */
            ItemText,
            
            /** A juce::ImageComponent containing the id-mapped image */
            ItemImage,
            
            /** A juce::TextButton handling the closing of the corresponding tab */
            ItemCloseButton,
            
            /** A juce::ToggleButton indicating and managing the pin state of the corresponding tab */
            ItemPinButton
        };
        
        /**
         *  Specifies the layout of an item the tab bar will consist of.
         *  It basically tells the MultiPagePane which type of item to add with what distance to other items.
         */
        struct JAUT_API TabItemLayout
        {
            /**
             *  The type of item to add.
             *  @see jaut::MultiPagePane::TabFactory::TabItem
             */
            TabItem itemType;
            
            /** The distance between the this item and another item or the tab itself. */
            Margin itemMargin;
            
            //==========================================================================================================
            /**
             *  Creates a new TabItemLayout object with the specified type and a default margin of 0 on all sides.
             *  @param type The type of the item
             */
            explicit constexpr TabItemLayout(TabItem type) noexcept
                : itemType(type)
            {}
    
            /**
             *  Creates a new TabItemLayout object with the specified type and a given margin.
             *
             *  @param type       The type of the item
             *  @param itemMargin The margin between this item and other items or the tab bounds
             */
            constexpr TabItemLayout(TabItem type, Margin itemMargin) noexcept
                : itemType(type), itemMargin(itemMargin)
            {}
        };
        
        //==============================================================================================================
        virtual ~TabFactory() = default;
        
        //==============================================================================================================
        /**
         *  This specifies the tab's layout in the order specified.
         *  You can have multiple instances of the same object if needed for whatever reason.
         *
         *  @return A collection of tab items
         */
        virtual const std::vector<TabItemLayout>& getTabLayout() const = 0;
        
        /**
         *  Creates a specific page-component based on the given id.
         *
         *  @param id The id of the component to create
         *  @return The managed component pointer
         */
        virtual std::unique_ptr<juce::Component> createPageForId(const juce::String &id) = 0;
    
        /**
         *  Creates a specific image based on the given id.
         *
         *  @param id The id of the image to create
         *  @return The image
         */
        virtual juce::Image getImageForPage(const juce::String &id) { return {}; }
    };
    
    struct JAUT_API TabFactoryTemplates
    {
        class JAUT_API Default : public TabFactory
        {
        public:
            using PageAllocCallback = std::function<std::unique_ptr<juce::Component>(const juce::String &id)>;
            
            //==========================================================================================================
            static std::unique_ptr<TabFactory> create(PageAllocCallback pageAllocator)
            {
                return std::make_unique<Default>(std::move(pageAllocator));
            }
            
            //==========================================================================================================
            explicit Default(PageAllocCallback pageAllocator)
                : pageCallback(std::move(pageAllocator))
            {
                jassert(pageCallback != nullptr);
                layouts.emplace_back(TabFactory::TabItem::ItemText,        Margin(10, 0, 7, 0));
                layouts.emplace_back(TabFactory::TabItem::ItemCloseButton, Margin(0,  0, 4, 0));
            }
            
            //==========================================================================================================
            const std::vector<TabItemLayout>& getTabLayout() const override
            {
                return layouts;
            }
            
            std::unique_ptr<juce::Component> createPageForId(const juce::String &id) override
            {
                return pageCallback(id);
            }

        private:
            std::vector<TabItemLayout> layouts;
            PageAllocCallback pageCallback;
        };
    };
    
    class InternalTabBar;
    
    /**
     *  The button that will be displayed in the tab-bar of this component.
     *  A TabButton tracks its own state and tells you whether it is active or pinned and what its index is.
     *
     *  The TabButton interface is only accessible in const state when dealing with MultiPagePane and you cannot
     *  create instances of it.
     *  However, the reason for it being public is to know about its state.
     */
    class JAUT_API TabButton : public juce::Button, juce::LookAndFeel_V4
    {
    public:
        std::function<void(int)> onTabClicked;
        std::function<void(int)> onClosing;
        std::function<void(int, bool)> onPinning;
        
        //==============================================================================================================
        ~TabButton() override;
        
        //==============================================================================================================
        void paintButton(juce::Graphics&, bool, bool) override;
        void resized() override;
        
        //==============================================================================================================
        void mouseDown(const juce::MouseEvent&) override;
        
        //==============================================================================================================
        void setActive(bool active);
        
        /**
         *  Get whether this is the active tab or no.
         *  @return True if it's active
         */
        bool isActive() const noexcept;
        
        //==============================================================================================================
        void setPinned(bool pin);
        
        /**
         *  Get whether this tab is pinned or not.
         *  @return True if it's pinned
         */
        bool isPinned() const noexcept;
        
        //==============================================================================================================
        void setTabIndex(int index);
        
        /**
         *  Get the index of this TabButton inside its MultiPagePane.
         *  @return The index of the tab
         */
        int getTabIndex() const noexcept;
    
        //==============================================================================================================
        void triggerTabClick();
        
        //==============================================================================================================
        void updateStyle(const Style &style);
        
    private:
        friend class InternalTabBar;
        
        //==============================================================================================================
        std::vector<std::unique_ptr<juce::Component>> layoutComponents;
        TabFactory &factory;
        
        int tabHeight;
        int itemHeight;
        
        int  tabIndex { 0 };
        bool active { false };
        bool pinned { false };
        
        //==============================================================================================================
        TabButton(const Style&, TabFactory&, const juce::String&, const juce::String&);
        
        //==============================================================================================================
        void drawButtonBackground(juce::Graphics&, Button&, const juce::Colour&, bool, bool) override;
        void drawButtonText(juce:: Graphics&, juce::TextButton&, bool, bool) override {}
        
        JAUT_CREATE_LAF()
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        /**
         *  Gets the font being used for drawing the tab text.
         *  @return The font to be used
         */
        virtual juce::Font getMultiTabPaneFont() = 0;
        
        /**
         *  Draws the background of the multi-page component.
         *
         *  @param g            The graphics context
         *  @param multiTabPane The multi-page component
         */
        virtual void drawMultiTabPaneBackground(juce::Graphics &g, const MultiPagePane &multiTabPane) = 0;
        
        /**
         *  Draws the button that shows the hidden tabs.
         *
         *  @param g           The graphics context
         *  @param button      The more-tabs button
         *  @param isMouseOver Whether the mouse is over the button
         *  @param isMouseDown Whether the button is being pressed
         */
        virtual void drawMultiTabPaneMoreTabsButton(juce::Graphics &g, const juce::Button &button,
                                                    bool isMouseOver, bool isMouseDown) = 0;
        
        /**
         *  Draws the tab background.
         *
         *  @param g           The graphics context
         *  @param tabButton   The tab component
         *  @param isMouseOver Whether the mouse is over the tab
         *  @param isMouseDown Whether the tab is being pressed
         */
        virtual void drawMultiTabPaneTabBackground(juce::Graphics &g, const TabButton &tabButton,
                                                   bool isMouseOver, bool isMouseDown) = 0;
        
        /**
         *  Draws the tab text.
         *
         *  @param g           The graphics context
         *  @param tabButton   The tab component
         *  @param label       The label
         *  @param isMouseOver Whether the mouse is over the tab
         *  @param isMouseDown Whether the tab is being pressed
         */
        virtual void drawMultiTabPaneTabText(juce::Graphics &g, const TabButton &tabButton,
                                             const juce::Label &label,
                                             bool isMouseOver, bool isMouseDown) = 0;
        
        /**
         *  Draws the close button of the tab.
         *
         *  @param g           The graphics context
         *  @param tabButton   The tab component
         *  @param button      The close button
         *  @param isMouseOver Whether the mouse is over the close button
         *  @param isMouseDown Whether the close button is being pressed
         */
        virtual void drawMultiTabPaneTabCloseButton(juce::Graphics &g, const TabButton &tabButton,
                                                    const juce::TextButton &button,
                                                    bool isMouseOver, bool isMouseDown) = 0;
        
        /**
         *  Draws the pin button of the tab.
         *
         *  @param g           The graphics context
         *  @param tabButton   The tab component
         *  @param button      The pin button
         *  @param isMouseOver Whether the mouse is over the pin button
         *  @param isMouseDown Whether the pin button is being pressed
         */
        virtual void drawMultiTabPaneTabPinButton(juce::Graphics &g, const TabButton &tabButton,
                                                  const juce::ToggleButton &button,
                                                  bool isMouseOver, bool isMouseDown) = 0;
    };
    
    //==================================================================================================================
    /** The pointer type of the TabFactory passed into the MultiPagePane constructor. */
    using FactoryPtr = std::unique_ptr<TabFactory>;
    
    //==================================================================================================================
    using LastTabClosingHandler     = EventHandler<const juce::String&>;
    using TabClosedHandler          = EventHandler<const juce::String&>;
    using TabOpenedHandler          = EventHandler<const juce::String&>;
    using TabChangedHandler         = EventHandler<const juce::String&>;
    using TabPinStateChangedHandler = EventHandler<const juce::String&, bool>;
    using TabDetachedHandler        = EventHandler<const juce::String&>;
    
    /** Dispatched whenever the last tab was closed. */
    Event<LastTabClosingHandler> LastTabClosing;
    
    /** Dispatched whenever one of the open tabs was closed. */
    Event<TabClosedHandler> TabClosed;
    
    /** Dispatched whenever a new tab was opened. */
    Event<TabOpenedHandler> TabOpened;
    
    /** Dispatched whenever the active tab was changed. */
    Event<TabChangedHandler> TabChanged;
    
    /** Dispatched whenever a tab's pin state changed. */
    Event<TabPinStateChangedHandler> TabPinStateChanged;
    
    /** Dispatched whenever a tab was dispatched from this MultiPagePane instance. */
    Event<TabDetachedHandler> TabDetached;
    
    //==================================================================================================================
    /**
     *  Creates a new MultiPagePane instance.
     *  @param factory The TabFactory unique_ptr
     */
    explicit MultiPagePane(FactoryPtr factory);
    
    /**
     *  Creates a new MultiPagePane instance.
     *
     *  @param factory The TabFactory unique_ptr
     *  @param style   The initial style
     */
    MultiPagePane(FactoryPtr factory, Style style);
    ~MultiPagePane() override;
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //==================================================================================================================
    /**
     *  Opens a new page and adds it to this component with the specified id.
     *  If the tab with the specified id already exists, nothing happens.
     *
     *  @param pageId The id of the page component to create
     *  @param name   The initial name displayed on the tab bar or leave empty if you want the id to be displayed
     */
    void addPage(const juce::String &pageId, const juce::String &name = juce::String());
    
    /**
     *  Closes a page and removes it from this component at the specified tab-index.
     *  If the tab doesn't exist, nothing happens.
     *
     *  @param index The index of the tab to be closed
     */
    void closePage(int index);
    
    /**
     *  Changes the active page to the page with the specified tab-index.
     *  If the tab is already opened, nothing happens.
     *
     *  @param index The index of the tab to be opened
     */
    void openPage(int index);
    
    /**
     *  Changes the pin-state of the tab at the specified index.
     *  If the pin-state is the same as specified, nothing happens.
     *
     *  @param index          The index of the tab to be pinned.
     *  @param shouldBePinned Whether the page should be pinned
     */
    void pinPage(int index, bool shouldBePinned);
    
    //==================================================================================================================
    /**
     *  Gets the amount of pages in this component.
     *  @return The amount of pages
     */
    int getNumPages() const noexcept;
    
    /**
     *  Gets whether this component has any pages or not.
     *  @return True if this component has any pages
     */
    bool hasAnyPages() const noexcept;
    
    //==================================================================================================================
    /**
     *  Gets the tab button at the specified index or nullptr if the index was out-of-bounds.
     *
     *  @param index The index of the tab
     *  @return The tab if the index is in range, nullptr otherwise
     */
    const TabButton* getTab(int index) const noexcept;
    
    /**
     *  Gets the page component at the specified index or nullptr if the index was out-of-bounds.
     *
     *  @param index The index of the page component
     *  @return The page component if the index is in range, nullptr otherwise
     */
    juce::Component* getPage(int index) noexcept;
    
    /**
     *  Gets the page component at the specified index or nullptr if the index was out-of-bounds.
     *
     *  @param index The index of the page component
     *  @return The page component if the index is in range, nullptr otherwise
     */
    const juce::Component* getPage(int index) const noexcept;
    
    /**
     *  Gets the tab button associated to this page-id.
     *
     *  @param pageId The page-id of the page associated to this tab
     *  @return The   tab if the id was found, nullptr otherwise
     */
    const TabButton* getTab(const juce::String &pageId) const noexcept;
    
    /**
     *  Gets the page component with this page-id.
     *
     *  @param pageId The page-id of the page
     *  @return The page component if the id was found, nullptr otherwise
     */
    juce::Component* getPage(const juce::String &pageId) noexcept;
    
    /**
     *  Gets the page component with this page-id.
     *
     *  @param pageId The page-id of the page
     *  @return The page component if the id was found, nullptr otherwise
     */
    const juce::Component* getPage(const juce::String &pageId) const noexcept;
    
    //==================================================================================================================
    /**
     *  Gets the index of the current active tab.
     *  @return The active tab-index
     */
    int getActiveIndex() const noexcept;
    
    /**
     *  Gets the id of the page associated to the current active tab.
     *  @return The active page id
     */
    juce::String getActiveId() const noexcept;
    
    /**
     *  Gets the active tab.
     *  @return The active tab
     */
    const TabButton* getActiveTab() const noexcept;
    
    /**
     *  Gets the active page component.
     *  @return The active page component
     */
    juce::Component* getActivePage() noexcept;
    
    /**
     *  Gets the active page component.
     *  @return The active page component
     */
    const juce::Component* getActivePage() const noexcept;
    
    //==================================================================================================================
    /**
     *  Sets the new style and updates the UI of this component.
     *  @param style The new style to use
     */
    void setStyle(const Style &style);
    
    /**
     *  Gets the current style of this component.
     *  @return The current style
     */
    const Style& getStyle() const noexcept;
    
    //==================================================================================================================
    /**
     *  Gets the rectangle the tab-bar resides in.
     *  This is not neccessarilly the bounds of the tab-bar, but rather the whole space of the tab-bar without its
     *  margin properties.
     *
     *  @return The tab-space rect
     */
    juce::Rectangle<int> getTabSpaceBounds() const noexcept;
    
    /**
     *  Gets the rectangle the page-view resides in.
     *  This is not neccessarilly the bounds of the page-view, but rather the whole space of the page-view without its
     *  margin properties.
     *
     *  @return The page-view rect
     */
    juce::Rectangle<int> getPageSpaceBounds() const noexcept;
    
private:
    std::unordered_map<juce::String, std::unique_ptr<juce::Component>> pages;
    
    ContentPane contentPane;
    std::unique_ptr<InternalTabBar> tabBar;
    FactoryPtr factory;
    
    Style style;
    
    juce::Rectangle<int> tabSpaceBounds;
    juce::Rectangle<int> pageSpaceBounds;
    
    //==================================================================================================================
    void updateSpaceBounds(Style::TabBarLayout);
    void updateTabBarBounds();
    
    JAUT_CREATE_LAF()
};
}
