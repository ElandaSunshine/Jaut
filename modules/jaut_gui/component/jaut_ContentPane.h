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
    @file   jaut_ContentPane.h
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
     *  A simple component that contains another component and stretches it to a parent.<br>
     *  This is great when you don't want to add your component directly to a parent but rather through
     *  a intermediate component.
     */
    class JAUT_API ContentPane : public ExtraComponent
    {
    public:
        enum ColourId
        {
            ColourBackgroundId = CidnsJaut::getId<18>,
            ColourBorderId     = CidnsJaut::getId<19>
        };
        
        struct LookAndFeelMethods
        {
            /**
             *  Draws the background of the ContentPane.
             *  This will only be called if the ContentPane object contains no content-component.
             *
             *  @param g           The graphics context
             *  @param contentPane The content pane
             */
            virtual void drawContentPaneBackground(juce::Graphics &g, const ContentPane &pane) = 0;
            
            /**
             *  Draws the border of the ContentPane.
             *
             *  @param g           The graphics context
             *  @param contentPane The content pane
             */
            virtual void drawContentPaneBorder(juce::Graphics &g, const ContentPane &pane) = 0;
        };
        
        //==============================================================================================================
        using ComponentChangedHandler = EventHandler<juce::Component*>;
        
        /** Dispatched whenever the contained component was changed. */
        Event<ComponentChangedHandler> eventContentChanged;
        
        //==============================================================================================================
        /**
         *  Constructs a new ContentPane object.
         *  
         *  @param name  The name of the component
         *  @param style The initial style
         */
        ContentPane(const juce::String &name, ContainerStyle style);
        
        /**
         *  Constructs a new ContentPane object.
         *  
         *  @param style The initial style
         */
        explicit ContentPane(ContainerStyle style);
        
        /**
         *  Constructs a new ContentPane object.
         *  
         *  @param name The name of the component
         */
        explicit ContentPane(const juce::String &name);
        
        /** Constructs a new ContentPane object. */
        ContentPane() noexcept = default;
        
        //==============================================================================================================
        void resizeContents() override;
    
        //==============================================================================================================
        /**
         *  Sets the contained component and sets it to a new one.
         *  
         *  @param component The new owned component for this pane
         */
        void setComponent(std::unique_ptr<juce::Component> component);
        
        /**
         *  Sets the contained component and sets it to a new one or empties it if nullptr.
         *
         *  @param component The new non-owned component for this pane
         *  
         *  @return True if the component was successfully changed or false if the current component was the same
         */
        bool setComponent(juce::Component *component);
        
        /**
         *  Sets the contained component and sets it to a new one or empties it if nullptr.
         *
         *  @param component The new non-owned component for this pane
         *  
         *  @return True if the component was successfully changed or false if the current component was the same
         */
        bool setComponent(juce::Component &component);
    
        //==============================================================================================================
        /**
         *  Gets the current contained component.<br>
         *  
         *  @return The current contained component or nullptr if none was set
         */
        JAUT_NODISCARD
        juce::Component* getCurrentComponent() noexcept;
        
        /**
         *  Gets the current contained component.
         *  
         *  @return The current contained component or nullptr if none was set
         */
        JAUT_NODISCARD
        const juce::Component* getCurrentComponent() const noexcept;
        
        /**
         *  Gets and releases the current contained component.
         *  <br><br>
         *  This essentially means to return either the component, or nullptr if no component was set; all components
         *  that were contained by this pane will be removed.
         *  
         *  @return The component pointer
         */
        juce::OptionalScopedPointer<juce::Component> releaseComponent();
        
        //==============================================================================================================
        /**
         *  Gets whether the currently contained component is owned by this ContentPane or not.
         *  
         *  @return True if the component is owned, false if not
         */
        JAUT_NODISCARD
        bool isOwned() const noexcept;
        
    private:
        using ContentPtr = juce::OptionalScopedPointer<juce::Component>;
        
        //==============================================================================================================
        ContentPtr content;
        
        //==============================================================================================================
        void drawBackground(juce::Graphics&) override;
        void drawBorder    (juce::Graphics&) override;
        
        //==============================================================================================================
        bool updateComponent(ContentPtr contentPtr);
        
        //==============================================================================================================
        JAUT_INTERN_CREATE_LAF()
    };
}
