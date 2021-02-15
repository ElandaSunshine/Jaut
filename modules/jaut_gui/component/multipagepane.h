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
    @file   multipagepane.h
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
    class JAUT_API MultiPagePane : public juce::Component, public juce::DragAndDropTarget
    {
    private:
        class InternalTabBar;
    
    public:
        /**
         *  The drag-and-drop identifier which identifies this component when juce::DragAndDropContainer
         *  and juce::DragAndDropTarget are being used.
         */
        static constexpr const char *DragAndDrop_ID = "6d062147-33ef-4ea4-b9ec-04756959fe08";
        
        //==============================================================================================================
        enum JAUT_API ColourIds
        {
            ColourBackgroundId              = nextColourId<26>,
            ColourBorderId                  = nextColourId<27>,
            ColourTabId                     = nextColourId<28>,
            ColourTabActiveId               = nextColourId<29>,
            ColourTabHighlightId            = nextColourId<30>,
            ColourTabTextId                 = nextColourId<31>,
            ColourTabCloseButtonId          = nextColourId<32>,
            ColourTabCloseButtonHighlightId = nextColourId<33>
        };
    
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
        
        struct JAUT_API Style
        {
            /** The padding between all child-components and this one. */
            Padding contentPadding;
            
            /** The thickness of the component border. */
            Thickness<int> borderThickness;
            
            /** The margin between this container and the tab-bar. */
            Margin tabBarMargin;
            
            /** The margin between this container and the page-view. */
            Margin pageViewMargin;
            
            /** The margin between the tab-bar and the more tabs button. */
            Margin moreTabsButtonMargin { 4 };
            
            /** The layout of the tab bar. */
            TabBarLayout tabBarLayout { TabBarLayout::Top };
            
            /** The height of the items on any tab-button. */
            int tabButtonItemHeight { 20 };
            
            /** The height of any tab-button. */
            int tabButtonHeight { 20 };
        };
    
        enum class JAUT_API PinBehaviour
        {
            /** Prevents pinned tabs from being closed when all others get closed at once. */
            ClosingAllNoEffect = 1,
        
            /** Adds an extra tab-row above the normal one and puts the pinned tabs there. */
            PutInExtraRow = 2
        };
    
        friend PinBehaviour operator|(PinBehaviour left, PinBehaviour right) noexcept
        {
            return static_cast<PinBehaviour>(static_cast<unsigned>(left) | static_cast<unsigned>(right));
        }
    
        friend PinBehaviour operator&(PinBehaviour left, PinBehaviour right) noexcept
        {
            return static_cast<PinBehaviour>(static_cast<unsigned>(left) & static_cast<unsigned>(right));
        }
    
        friend PinBehaviour& operator|=(PinBehaviour &left, PinBehaviour right) noexcept
        {
            return (left = left | right);
        }
        
        /** Defines the behavious of the components of this class. */
        struct JAUT_API Options
        {
            /** Defines the behaviour pinned tabs should have. */
            PinBehaviour pinnedTabBehaviour { PinBehaviour::ClosingAllNoEffect };
            
            /** Defines whether you can reorder tabs by dragging them around. */
            bool allowTabReordering { false };
            
            /** Defines whether tabs can be dragged out of the tab-bar to other components. */
            bool allowDragExchange { false };
            
            /**
             *  Defines whether tabs can be extracted from the main window to become their own modal window.
             *  This only works if allowDragExchange is true.
             */
            bool allowPageExtraction { false };
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
         *  your TabFactory implementation.
         *
         *  @see jaut::MultiPagePane::TabFactoryTemplates
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
             *  It basically tells the MultiPagePane which type of item to add with what bounds.
             */
            struct JAUT_API TabItemLayout
            {
                /**
                 *  The type of item to add.
                 *  @see jaut::MultiPagePane::TabFactory::TabItem
                 */
                TabItem itemType;
                
                /** The margin of this item. */
                Margin itemMargin;
    
                //======================================================================================================
                /**
                 *  Creates a new TabItemLayout object with the specified type and margin.
                 *
                 *  @param type   The type of the item
                 *  @param margin The margin of the item
                 */
                constexpr TabItemLayout(TabItem type, Margin margin) noexcept
                    : itemType(type), itemMargin(margin)
                {}
            };
    
            //==========================================================================================================
            virtual ~TabFactory() = default;
    
            //==========================================================================================================
            /**
             *  This specifies the tab's layout in the order specified.
             *  You can have multiple instances of the same object if needed for whatever reason.
             *
             *  @return A collection of tab items
             */
            virtual const std::vector<TabItemLayout>& getTabLayout() const = 0;
        
            /**
             *  Creates a specific image based on the given id.
             *
             *  @param id The id of the image to create
             *  @return The image
             */
            virtual juce::Image getImageForPage(const juce::String &id) const { juce::ignoreUnused(id); return {}; }
            
            /**
             *  Clones this tab-factory object.
             *  @return The cloned tab-factory
             */
            virtual std::unique_ptr<TabFactory> clone() = 0;
        };
        
        /**
         *  This struct contains a list of tabfactory templates so that you don't necessarily
         *  have to create your own.
         */
        struct JAUT_API TabFactorTemplates
        {
            /** A default template that just adds a tab text and a close button. */
            class JAUT_API Default : public TabFactory
            {
            public:
                static std::unique_ptr<TabFactory> create() { return std::make_unique<Default>(); }
                
                //======================================================================================================
                const std::vector<TabItemLayout>& getTabLayout() const override { return layout;   }
                std::unique_ptr<TabFactory>       clone()              override { return create(); }

            private:
                std::vector<TabItemLayout> layout {
                    { TabItem::ItemCloseButton, { 5, 0, 2, 0 } },
                    { TabItem::ItemText,        { 2, 0, 9, 0 } }
                };
            };
        };
        
        /**
         *  The button that will be displayed in the tab-bar of this component.
         *  A TabButton tracks its own state and tells you whether it is active or pinned and what its index is.
         *
         *  The TabButton interface is only accessible in const state when dealing with MultiPagePane and you cannot
         *  create instances of it.
         *  However, the reason for it being public is to know about its state.
         */
        class JAUT_API TabButton : public juce::Button
        {
        public:
            std::function<void(const TabButton&)>       onActivating;
            std::function<void(const TabButton&)>       onClosing;
            std::function<void(const TabButton&, bool)> onPinning;
    
            //==========================================================================================================
            TabButton(const Style&, const TabFactory&, const juce::String&, const juce::String&);
            ~TabButton() override;
    
            //==========================================================================================================
            void paintButton(juce::Graphics&, bool, bool) override;
    
            //==========================================================================================================
            void mouseDown(const juce::MouseEvent&) override;
    
            //==========================================================================================================
            void setActive(bool active);
            
            /**
             *  Get whether this is the active tab or no.
             *  @return True if it's active
             */
            bool isActive() const noexcept;
    
            //==========================================================================================================
            void setPinned(bool pin);
            
            /**
             *  Get whether this tab is pinned or not.
             *  @return True if it's pinned
             */
            bool isPinned() const noexcept;
    
            //==========================================================================================================
            void setTabIndex(int index);
            
            /**
             *  Get the index of this TabButton inside its MultiPagePane.
             *  @return The index of the tab
             */
            int getTabIndex() const noexcept;
    
            //==========================================================================================================
            void updateTabSize(int itemHeight);
            void updateTabHeight(int tabHeight);
            
        private:
            class LookAndFeelProxy;
    
            //==========================================================================================================
            using LafProxyPtr = std::unique_ptr<LookAndFeelProxy, std::function<void(LookAndFeelProxy*)>>;
            
            //==========================================================================================================
            static LafProxyPtr buttonProxyLaf;
    
            //==========================================================================================================
            std::vector<std::unique_ptr<juce::Component>> layoutComponents;
            const TabFactory &factory;
            
            int tabWidth { 0 };
            int tabIndex { 0 };
            bool active  { false };
            bool pinned  { false };
            
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
             *  Draws the border of the MultiPagePane.
             *
             *  @param g            The graphics context
             *  @param multiTabPane The multi-page component
             */
            virtual void drawMultiTabPaneBorder(juce::Graphics &g, const MultiPagePane &multiTabPane) = 0;
            
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
        
        /**
         *  This is just an optional class so you don't have to roll your own window class when adding tabs to
         *  the desktop.
         *  You can always have your custom window class if needed.
         */
        class JAUT_API Window : public juce::ResizableWindow
        {
        public:
            struct JAUT_API LookAndFeelMethods
            {
                /**
                 *  Gets the style flags for this window.
                 *  By default this will be a native-window with only a close-button.
                 *
                 *  @return The juce::ComponentPeer:StyleFlags to use
                 */
                virtual int getWindowStyleFlags() const = 0;
            };
    
            //==========================================================================================================
            using DragAndDropContainerProxy = std::variant<juce::DragAndDropContainer*, DragAndDropContainerAdvanced*>;
    
            //==========================================================================================================
            using WindowClosingHandler = EventHandler<Window*>;
            
            /** Dispatched whenever the window is about to be closed. */
            Event<WindowClosingHandler> WindowClosing;
    
            //==========================================================================================================
            Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> factory, Style style,
                   Options options);
            Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> factory, Style style);
            Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> factory, Options options);
            Window(const juce::String &name, juce::OptionalScopedPointer<TabFactory> factory);
    
            //==========================================================================================================
            template<class DragAndDropContainerType>
            void setDragAndDropProxy(DragAndDropContainerType *dragAndDropContainerProxy)
            {
                static_assert(std::is_base_of_v<juce::DragAndDropContainer,   DragAndDropContainerType>
                           || std::is_base_of_v<DragAndDropContainerAdvanced, DragAndDropContainerType>,
                              "DragAndDropContainerType must either be a juce::DragAndDropContainer"
                              "or jaut::DragAndDropContainerAdvanced instance");
                
                dragAndDropContainer = DragAndDropContainerProxy(dragAndDropContainerProxy);
            }
        
            DragAndDropContainerProxy& getDragAndDropProxy() noexcept
            {
                return dragAndDropContainer;
            }
    
            //==========================================================================================================
            void resized() override;
    
            //==========================================================================================================
            int getDesktopWindowStyleFlags() const override;
            MultiPagePane& getMultiPagePane() noexcept;
    
            //==========================================================================================================
            void userTriedToCloseWindow() override;
            
        private:
            DragAndDropContainerProxy dragAndDropContainer;
    
            //==========================================================================================================
            void forceToCloseWindow(const juce::String&);
    
            //==========================================================================================================
            JAUT_CREATE_LAF()
        };
    
        //==============================================================================================================
        /** The pointer type of the TabFactory passed into the MultiPagePane constructor. */
        using FactoryPtr = juce::OptionalScopedPointer<TabFactory>;
        
        /** The pointer type of any tab page contained in MultiPagePane components. */
        using PagePtr = juce::OptionalScopedPointer<juce::Component>;
    
        //==============================================================================================================
        using LastTabClosingHandler     = EventHandler<const juce::String&>;
        using TabClosedHandler          = EventHandler<const juce::String&>;
        using TabOpenedHandler          = EventHandler<const juce::String&>;
        using TabChangedHandler         = EventHandler<const juce::String&>;
        using TabPinStateChangedHandler = EventHandler<const juce::String&, bool>;
        
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
    
        //==============================================================================================================
        /**
         *  Creates a new MultiPagePane instance with default options and default style.
         *  @param factory The TabFactory unique_ptr
         */
        explicit MultiPagePane(FactoryPtr factory);
        
        /**
         *  Creates a new MultiPagePane instance with default options.
         *
         *  @param factory The TabFactory unique_ptr
         *  @param style   The initial style
         */
        MultiPagePane(FactoryPtr factory, Style style);
        
        /**
         *  Creates a new MultiPagePane instance with default style.
         *
         *  @param factory The TabFactory unique_ptr
         *  @param options The initial options
         */
        MultiPagePane(FactoryPtr factory, Options options);
        
        /**
         *  Creates a new MultiPagePane instance.
         *
         *  @param factory The TabFactory unique_ptr
         *  @param style   The initial style
         *  @param options The initial options
         */
        MultiPagePane(FactoryPtr factory, Style style, Options options);
        
        ~MultiPagePane() override;
    
        //==============================================================================================================
        void paint(juce::Graphics&) override;
        void paintOverChildren(juce::Graphics&) override;
        void resized() override;
    
        //==============================================================================================================
        /**
         *  Opens a new page and adds it to this component with the specified id.
         *  If the tab with the specified id already exists, nothing happens.
         *
         *  @param component The component to add
         *  @param id        The id of the page to avoid ambiguity if tabs share the same name
         *  @param name      The initial name displayed on the tab bar or leave empty if you want the id to be displayed
         */
        void addPage(juce::OptionalScopedPointer<juce::Component> component, const juce::String &id,
                     const juce::String &name = juce::String());
        
        /**
         *  Closes a page and removes it from this component at the specified tab-index.
         *  If the tab doesn't exist, nothing happens.
         *
         *  @param index The index of the page to be closed
         */
        void closePage(int index);
        
        /**
         *  Closes a page with this id and removes it from this component.
         *  If the page doesn't exist, nothing happens.
         *
         *  @param pageId The id of the page to be closed
         */
        void closePage(const juce::String &pageId);
        
        /**
         *  Changes the active page to the page with the specified tab-index.
         *  If the tab is already opened, nothing happens.
         *
         *  @param index The index of the tab to be opened
         */
        void openPage(int index);
        
        /**
         *  Changes the active page to the page with this id.
         *  If the page is already opened, nothing happens.
         *
         *  @param pageId The id of the page to be opened
         */
        void openPage(const juce::String &pageId);
        
        /**
         *  Changes the pin-state of the tab at the specified index.
         *  If the pin-state is the same as specified, nothing happens.
         *
         *  @param index          The index of the tab to be pinned.
         *  @param shouldBePinned Whether the page should be pinned
         */
        void pinPage(int index, bool shouldBePinned);
        
        /**
         *  Changes the pin-state of the page with this id.
         *  If the pin-state is the same as specified, nothing happens.
         *
         *  @param pageId         The id of the page to be pinned.
         *  @param shouldBePinned Whether the page should be pinned
         */
        void pinPage(const juce::String &id, bool shouldBePinned);
        
        /**
         *  Removes the tab and page from this component and returns a juce::OptionScopedPointer containing
         *  the page component.
         *
         *  @param index The index of the page to be detached
         *  @return The page-component or nullptr if there was none with this index
         */
        PagePtr detachPage(int index);
        
        /**
         *  Removes the tab and page from this component and returns a juce::OptionScopedPointer containing
         *  the page component.
         *
         *  @param pageId The id of the page to be detached
         *  @return The page-component or nullptr if there was none with this id
         */
        PagePtr detachPage(const juce::String &pageId);
        
        /**
         *  Closes all open pages.
         *
         *  If TabFactory::getPinBehaviour contains the flag PinBehaviour::ClosingAllNoEffect and forcePinned is false,
         *  pinned pages won't be touched.
         *  If this flag is not set, pinned pages will be closed regardless.
         *
         *  @param forcePinned Force pinned pages to be closed
         */
        void closeAllPages(bool forcePinned);
    
        //==============================================================================================================
        /**
         *  Gets the amount of pages in this component.
         *  @return The amount of pages
         */
        int getNumPages() const noexcept;
        
        /**
         *  Gets whether this component has any pages.
         *  @return True if this component has any pages
         */
        bool hasAnyPages() const noexcept;
        
        /**
         *  Gets whether this component has any pinned pages.
         *  @return True if there is at least one pinned page
         */
        bool hasPinnedPages() const noexcept;
    
        //==============================================================================================================
        /**
         *  Gets the component that is shown when this element contains no pages.
         *  @return The component or nullptr if none was set
         */
        juce::Component* getBackgroundComponent() noexcept;
    
        /** Removes the component that is shown when this object has no pages. */
        void setBackgroundComponent(std::nullptr_t);
        
        /**
         *  Sets the component that is shown when this object has no pages.
         *
         *  @param component The component to set
         *  @param owned     If the component should be owned by this class
         */
        void setBackgroundComponent(juce::OptionalScopedPointer<juce::Component> component);
        
        /**
         *  Sets the component that is shown when this object has no pages.
         *  The component will not be owned by this object.
         *
         *  @param component The component to set
         */
        void setBackgroundComponent(juce::Component &component);
    
        //==============================================================================================================
        /**
         *  Gets the id of the page at the specified index.
         *
         *  @param index The index of the page
         *  @return The id of the page at the specified index or empty if none was found
         */
        juce::String getIdAt(int index) const noexcept;
        
        /**
         *  Gets the index of the specified page.
         *
         *  @param pageId The id of the page to get the index for
         *  @return The index of the page or -1 if none was found
         */
        int getIndexOf(const juce::String &pageId) const noexcept;
    
        //==============================================================================================================
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
    
        //==============================================================================================================
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
    
        //==============================================================================================================
        /**
         *  Sets the padding between border and child components.
         *  @param padding The new padding
         */
        void setContentPadding(Padding padding);
        
        /**
         *  Sets the thickness of the component border.
         *  @param thickness The new thickness
         */
        void setBorderThickness(Thickness<int> thickness);
        
        /**
         *  Sets the margin around the tab bar.
         *  @param margin The new margin
         */
        void setTabBarMargin(Margin margin);
        
        /**
         *  Sets the margin around the page view component.
         *  @param margin The new margin
         */
        void setPageViewMargin(Margin margin);
        
        /**
         *  Sets the margin around the more tabs button.
         *  @param margin The new margin
         */
        void setMoreTabsButtonMargin(Margin margin);
        
        /**
         *  Sets the layout of the tab bar.
         *  @param layout The new layout
         *  @see TabBarLayout
         */
        void setTabBarLayout(TabBarLayout layout);
        
        /**
         *  Sets the height of the items on the tab buttons.
         *  @param newHeight The new height
         */
        void setTabButtonItemHeight(int newHeight);
        
        /**
         *  Sets the height of the tab button.
         *  @param newHeight The new height
         */
        void setTabButtonHeight(int newHeight);
    
        //==============================================================================================================
        /**
         *  Sets the behaviour of pinned tabs.
         *  @param behaviour The new behaviour
         */
        void setTabPinBehaviour(PinBehaviour behaviour);
        
        /**
         *  Sets whether tab reordering is allowed or not.
         *  @param allow Whether tab reordering should be allowed
         */
        void setAllowTabReordering(bool allow);
        
        /**
         *  Sets whether tabs can be dragged out to be dragged to other components.
         *  @param allow Whether dragging tabs out should be allowed
         */
        void setAllowDragExchange(bool allow);
        
        /**
         *  Sets whether tabs can become their own modal window when dragged out.
         *  @param allow Whether dragging tabs to become windows is allowed
         */
        void setAllowPageExtraction(bool allow);
        
        //==============================================================================================================
        /**
         *  Sets the new style and updates the UI of this component.
         *  @param style The new style to use
         */
        void setStyle(const Style &style);
        
        /**
         *  Gets the current style of this component.
         *  @return The current style
         */
        const Style& getStyle() const noexcept { return style; }
        
        /**
         *  Sets the new options and updates this component.
         *  @param options The new options to use
         */
        void setOptions(const Options &options);
        
        /**
         *  Gets the current options of this component.
         *  @return The current options
         */
        const Options& getOptions() const noexcept { return options; }
    
        //==============================================================================================================
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
         *  This is not necessarily the bounds of the page-view, but rather the whole space of the page-view without
         *  its margin properties.
         *
         *  @return The page-view rect
         */
        juce::Rectangle<int> getPageSpaceBounds() const noexcept;
    
        //==============================================================================================================
        void itemDragEnter(const SourceDetails&) override;
        void itemDragExit(const SourceDetails&) override;
        bool isInterestedInDragSource(const SourceDetails&) override;
        void itemDropped(const SourceDetails&) override;
    
    private:
        std::unordered_map<juce::String, PagePtr> pages;
        
        ContentPane pageView;
        std::unique_ptr<InternalTabBar> tabBar;
        
        juce::OptionalScopedPointer<juce::Component> backgroundComponent;
        
        FactoryPtr factory;
        Style      style;
        Options    options;
        
        juce::Rectangle<int> tabSpaceBounds;
        juce::Rectangle<int> pageSpaceBounds;
        
        bool paintDropBox { false };
        bool initialised  { false };
    
        //==============================================================================================================
        void updateSpaceBounds(TabBarLayout);
        
        JAUT_CREATE_LAF()
    };
}
