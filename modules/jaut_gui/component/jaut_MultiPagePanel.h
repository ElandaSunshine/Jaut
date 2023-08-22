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
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   jaut_MultiPagePanel.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_gui/component/jaut_ExtraComponent.h>
#include <jaut_gui/lookandfeel/jaut_CidNamespace.h>
#include <jaut_gui/structure/jaut_SizeStructs.h>

#include <jaut_core/define/jaut_Define.h>



namespace jaut
{
    /**
     *  An extremely customisable component that lets you add pages that will be controlled by a tab bar.<br>
     *  Every page you add will get its own tab component, which stands in correlation to said page.
     *  <br><br>
     *  You can customize almost any aspect about it, tab-bar layout, page and tab size, component padding and margin,
     *  there is no limit.
     *  <br><br>
     *  To begin with, you need to create an implementation of MultiPagePane::TabFactory which is responsible for
     *  mapping ids to images, page components and tab-item-layout.<br>
     *  Everything specified in your layout, will define how your component will behave.
     *  <br><br>
     *  You can also dynamically change style options about the component by using setStyle.
     */
    class JAUT_API MultiPagePanel : public ExtraComponent, public juce::DragAndDropTarget
    {
    public:
        enum ColourIds
        {
            ColourBackgroundId            = CidnsJaut::getId<23>,
            ColourBorderId                = CidnsJaut::getId<24>,
            ColourTabBackgroundId         = CidnsJaut::getId<25>,
            ColourTabBackgroundActiveId   = CidnsJaut::getId<26>,
            ColourTabBackgroundHoverId    = CidnsJaut::getId<27>
        };
        
        /** Defines the position of the tab-bar. */
        enum class TabBarLayout
        {
            /** The tab-bar will appear on the top of the component. */
            Top,
            
            /** The tab-bar will appear on the bottom of the component. */
            Bottom,
            
            /** The tab-bar will appear on the left-side of the component. */
            SideLeft,
            
            /** The tab-bar will appear on the right-side of the component. */
            SideRight
        };
        
        struct Style
        {
            /** The margin between this container and the tab-bar. */
            Margin<int> tabBarMargin = 0;
            
            /** The margin between this container and the page-view. */
            Margin<int> pageViewMargin = 0;
            
            /** The layout of the tab bar. */
            TabBarLayout tabBarLayout = TabBarLayout::Top;
        };
        
        /** The style options for constructing this panel. */
        struct InitStyle : public Style, public ContainerStyle {};
        
        class AbstractTabFactory;
        
        struct Options
        {
            /**
             *  The tab factory that's supposed to manage and create the tab buttons.<br>
             *  If this is nullptr, the default factory will be used.
             */
            std::unique_ptr<AbstractTabFactory> tabFactory = nullptr;
            
            /** Defines whether you can reorder tabs by dragging them around. */
            bool allowTabReordering = true;
            
            /** Defines whether tabs can be dragged out of the tab-bar to other components. */
            bool allowDragExchange = false;
            
            /**
             *  Defines whether tabs can be extracted from the main window to become their own modal window.
             *  This only works if allowDragExchange is true.
             */
            bool allowPageExtraction = false;
            
            /**
             *  Whether the mouse button needs to be relieved first to activate a tab.<br>
             *  If this is false, just pressing the mouse button will already change the tab.
             */
            bool activateTabOnFullPress = false;
        };
        
        class TabButton;
        
        /**
         *  The TabFactory specifies the tab and page behaviour.<br>
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
        class AbstractTabFactory
        {
        public:
            virtual ~AbstractTabFactory() = default;
            
            //==========================================================================================================
            /**
             *  Gets the height or width of the tab bar, depending on the TabBarLayout, which determines
             *  how big the tab bar should be to contain the tab components.
             *  <br><br>
             *  If this returns 0 or below, it will automatically determine the needed height based on the outermost
             *  tab. (position and size of the tab button closest to the ending edge of the tab area)<br>
             *  However, do not that this does not account for whether the tab button is
             *  currently visible on screen or not.
             *  <br><br>
             *  If the tab layout is top or bottom, this will determine the height, otherwise the width of the area.
             *  
             *  @return The size of the tab button area (for both width and height, in case the tab layout changes)
             */
            JAUT_NODISCARD
            virtual Size<int> getTabAreaSize() const = 0;
            
            //==========================================================================================================
            /**
             *  Initialises a new tab button.
             *  <br><br>
             *  Do note that there is no certain way to identify child components, so best would be to use indices.<br>
             *  A good way would be to keep an enum which numbers map to the indices of the child components.
             *  
             *  @param tabButton The new TabButton object
             */
            virtual void tabPrepare(TabButton &tabButton) = 0;
            
            /**
             *  This is where you define the bounds of the tab button.<br>
             *  It will be called whenever a tab is created, or any tab has changed in bounds or was closed.
             *  <br><br>
             *  This is also where you set the layout for the child components of the tab button, but note
             *  that you will not always need to change the child components, so make sure to check whether that's
             *  really necessary.
             *  
             *  @param tabButton The TabButton object to resize
             */
            virtual void tabResized(int index, TabButton &tabButton) = 0;
            
            //==========================================================================================================
            /**
             *  This determines whether a tab needs to be updated based on another tab having had a change in bounds
             *  or was closed.
             *  
             *  @param changed The TabButton that changed bounds or was closed
             *  @param other   All other tabs (one by one), that should be updated if this returns true
             *  
             *  @return True to update "other"
             */
            JAUT_NODISCARD
            virtual bool tabNeedsUpdate(const TabButton &changed, const TabButton &other) const noexcept = 0;
            
            //==========================================================================================================
            /**
             *  Optionally can be overridden to execute an action whenever a tab was closed.
             *  
             *  @param tabId The id of the tab that was closed
             */
            virtual void onTabClosed(const juce::String &tabId) {}
        };
        
        /** The button that will be displayed in the tab-bar of this component. */
        class TabButton : public juce::Component,
                          private juce::ComponentListener
        {
        public:
            using ActivityChangedHandler = EventHandler<TabButton&>;
            using BeforeClosingHandler   = EventHandler<TabButton&, bool&>;
            using NameChangedHandler     = EventHandler<TabButton&>;
            
            /**
             *  Dispatched whenever this tab was activated or deactivated. (Selected and open)
             *  
             *  @param par1 The TabButton which activity state has changed
             */
            Event<ActivityChangedHandler> eventActivityChanged;
            
            /**
             *  Dispatched whenever the tab is about to be closed.<br>
             *  A handler can decide to stop the closing operation with the cancel argument,
             *  do note though that this can be set by any handler so make sure you know where you change it.
             *  
             *  @param par1 The TabButton that's about to be closed
             *  @param par2 Whether to cancel the closing operation
             */
            Event<BeforeClosingHandler> eventBeforeClosing;
            
            /**
             *  Dispatched whenever the name of this tab changed.
             *  
             *  @param par1 The TabButton which name changed
             */
            Event<NameChangedHandler> eventNameChanged;
            
            //==========================================================================================================
            /**
             *  Creates a new TabButton object.<br>
             *  You wouldn't ever need to create one yourself as these are handled internally, but go ahead if you need
             *  to.
             *  
             *  @param name         The name of this TabButton, which will be shown in the label (if set)
             *  @param panel        The MultiPagePanel object this TabButton belongs to
             *  @param closeHandler The handler to handle a close event when the close() was called
             */
            TabButton(const juce::String &name, MultiPagePanel &panel, std::function<void()> closeHandler);
            
            //==========================================================================================================
            void paint(juce::Graphics &g) override;
            
            //==========================================================================================================
            /**
             *  Gets whether this tab is the active tab.
             *  
             *  @return True if this tab is currently active
             */
            JAUT_NODISCARD
            bool isActive() const noexcept;
            
            //==========================================================================================================
            /** Tries to activate this tab if it not already is. */
            void activate(bool shouldBeActive);
            
            /** Closes the tab. */
            void close();
            
            //==========================================================================================================
            void mouseDown (const juce::MouseEvent &e) override;
            void mouseUp   (const juce::MouseEvent &e) override;
            void mouseEnter(const juce::MouseEvent &e) override;
            void mouseExit (const juce::MouseEvent &e) override;
            
        private:
            MultiPagePanel &panel;
            
            std::function<void()> closeHandler;
            
            bool active      { false };
            bool mouseIsOver { false };
            bool mouseIsDown { false };
            
            //==========================================================================================================
            void componentNameChanged(Component &component) override;
            
            //==========================================================================================================
            JAUT_INTERN_CREATE_LAF()
            
            //==========================================================================================================
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabButton)
        };
        
        struct LookAndFeelMethods
        {
            /**
             *  Draws the background of the multi-page panel.
             *
             *  @param g     The graphics context
             *  @param panel The multi-page component
             */
            virtual void drawMultiPagePanelBackground(juce::Graphics &g, const MultiPagePanel &panel) = 0;
    
            /**
             *  Draws the border of the multi-page panel.
             *
             *  @param g     The graphics context
             *  @param panel The multi-page component
             */
            virtual void drawMultiPagePanelBorder(juce::Graphics &g, const MultiPagePanel &panel) = 0;
            
            /**
             *  Draws the tab-button background.
             *
             *  @param g           The graphics context
             *  @param tabButton   The tab component
             *  @param isMouseOver Whether the mouse is over the tab
             *  @param isMouseDown Whether the tab is being pressed
             */
            virtual void drawMultiPagePanelTabBackground(juce::Graphics  &g,
                                                         const TabButton &tabButton,
                                                         bool            isMouseOver,
                                                         bool            isMouseDown) = 0;
        };
        
        //==============================================================================================================
        using TabClosedHandler        = EventHandler<const juce::String&, bool>;
        using ActiveTabChangedHandler = EventHandler<const juce::String&, const juce::String&>;
        
        /** 
         *  Dispatched whenever one of the open tabs was closed.
         *  
         *  @param par1 The id of the tab that was closed
         *  @param par2 Whether the tab was the last tab that was closed
         */
        Event<TabClosedHandler> eventTabClosed;
        
        /** 
         *  Dispatched whenever the active tab was changed.
         *  
         *  @param par1 The tab that is now active
         *  @param par2 The tab that was active before this tab, or an empty string if there was no tab before
         */
        Event<ActiveTabChangedHandler> eventActiveTabChanged;
    
        //==============================================================================================================
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param name    The name of the component
         *  @param style   The initial style
         *  @param options The initial options
         */
        MultiPagePanel(const juce::String &name, InitStyle style, Options options);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param style   The initial style
         *  @param options The initial options
         */
        MultiPagePanel(InitStyle style, Options options);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param name  The name of the component
         *  @param style The initial style
         */
        MultiPagePanel(const juce::String &name, InitStyle style);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param name    The name of the component
         *  @param options The initial options
         */
        MultiPagePanel(const juce::String &name, Options options);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param style The initial style
         */
        explicit MultiPagePanel(InitStyle style);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param options The initial options
         */
        explicit MultiPagePanel(Options style);
        
        /**
         *  Constructs a new MultiPagePanel object.
         *  
         *  @param name The name of the component
         */
        explicit MultiPagePanel(const juce::String &name);
        
        /** Creates a new MultiPagePanel default instance. */
        MultiPagePanel() = default;
        
        ~MultiPagePanel() override;
        
        //==============================================================================================================
        void paintContents(juce::Graphics &g) override;
        void resizeContents() override;
        
        void paintOverChildren(juce::Graphics&) override;
    
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
        class Pimpl;
        
        //==============================================================================================================
        Style   style;
        Options options;
        
        std::unique_ptr<Pimpl> pimpl;
        
        //==============================================================================================================
        void updateSpaceBounds(TabBarLayout);
        
        //==============================================================================================================
        JAUT_INTERN_CREATE_LAF()
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiPagePanel)
    };
}
