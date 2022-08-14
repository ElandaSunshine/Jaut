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
    @file   styleablecomponent.h
    @date   02, February 2021
    
    ===============================================================
 */

#pragma once

namespace jaut
{
    struct StyleableComponentBase
    {
        struct Style
        {
            /**
             *  Determines the padding between the component's border-inside and content.
             *  Negative values are allowed.
             */
            Padding contentPadding;
            
            /**
             *  Determines the border thickness of the component.
             *  Negative values will be treated as 0.
             */
            Thickness<int> borderThickness;
        };
    };
    
    template<class TSelf = StyleableComponentBase>
    class StyleableComponent : public juce::Component
    {
    public:
        enum JAUT_API ColourIds
        {
            ColourBackground,
            ColourBorder
        };
        
        struct JAUT_API LookAndFeelMethods
        {
            virtual void drawStyleablComponentBackground(juce::Component &component) = 0;
            virtual void drawStyleablComponentBorder(juce::Component &component) = 0;
        };
        
        //==============================================================================================================
        using Style = typename TSelf::Style;
    
        //==============================================================================================================
        using StyleChangedHandler = EventHandler<const Style&, const Style&>;
        
        /** Dispatched when the style of this component changed. */
        Event<StyleChangedHandler> StyleChanged;
    
        //==============================================================================================================
        StyleableComponent()
        {
            addAndMakeVisible(content);
            lookAndFeelChanged();
        }
        
        //==============================================================================================================
        virtual void paint(juce::Graphics &g) override
        {
        
        }
    
        virtual void resized() override
        {
            content.setBounds(style.borderThickness
                                   .trimRectangle(style.contentPadding.trimRectangle(getLocalBounds())));
            onResized();
        }
    
        //==============================================================================================================
        /**
         *  Gets the coordinate of the content area on the x-axis.
         *  @return The x coordinate of the content area
         */
        int getInnerX() const noexcept { return content.getX(); }
    
        /**
         *  Gets the coordinate of the content area on the y-axis.
         *  @return The y coordinate of the content area
         */
        int getInnerY() const noexcept { return content.getY(); }
        
        /**
         *  Gets the width of the content area.
         *  @return The width of the content area
         */
        int getInnerWidth() const noexcept { return content.getWidth(); }
    
        /**
         *  Gets the height of the content area.
         *  @return The height of the content area
         */
        int getInnerHeight() const noexcept { return content.getHeight(); }
        
        /**
         *  Gets the coordinate of the righthand-side of the content area.
         *  @return The righthand-side coordinate of the content area
         */
        int getInnerRight() const noexcept { return content.getRight(); }
    
        /**
         *  Gets the coordinate of the bottom of the content area.
         *  @return The bottom coordinate of the content area
         */
        int getInnerBottom() const noexcept { return content.getBottom(); }
        
        /**
         *  Gets the position of the content area.
         *  @return The position of the content area
         */
        juce::Point<int> getInnerPosition() const noexcept { return content.getPosition(); }
        
        /**
         *  Gets the bounds of the content area.
         *  @return The bounds of the content
         */
        juce::Rectangle<int> getInnerBounds() const noexcept { return content.getBounds(); }
    
        /**
         *  Gets the local bounds of the content area.
         *  This is similar to getContentBounds with the difference that x and y originate at 0.
         *
         *  @return The bounds of the content
         */
        juce::Rectangle<int> getInnerLocalBounds() const noexcept { return content.getLocalBounds(); }
    
        //==============================================================================================================
        /**
         *  Gets the screen coordinate of the content area on the x-axis.
         *  @return The x coordinate of the content area on the screen
         */
        int getInnerScreenX() const { return content.getScreenX(); }
    
        /**
         *  Gets the screen coordinate of the content area on the y-axis.
         *  @return The y coordinate of the content area on the screen
         */
        int getInnerScreenY() const { return content.getScreenY(); }
    
        /**
         *  Gets the position of the content area on the screen.
         *  @return The screen position of the content area
         */
        juce::Point<int> getInnerScreenPosition() const { return content.getScreenPosition(); }
    
        /**
         *  Gets the bounds of the content area on the screen.
         *  @return The screen bounds of the content
         */
        juce::Rectangle<int> getInnerScreenBounds() const { return content.getScreenBounds(); }
    
        //==============================================================================================================
        /** Returns the number of child components that this component contains.

            @see getChildren, getChildComponent, getIndexOfChildComponent
        */
        int getNumChildComponents() const noexcept { return content.getNumChildComponents(); }
    
        /** Returns one of this component's child components, by it index.

            The component with index 0 is at the back of the z-order, the one at the
            front will have index (getNumChildComponents() - 1).
    
            If the index is out-of-range, this will return a null pointer.
    
            @see getChildren, getNumChildComponents, getIndexOfChildComponent
        */
        Component* getChildComponent(int index) const noexcept { return content.getChildComponent(index); }
    
        /** Returns the index of this component in the list of child components.

            A value of 0 means it is first in the list (i.e. behind all other components). Higher
            values are further towards the front.
    
            Returns -1 if the component passed-in is not a child of this component.
    
            @see getChildren, getNumChildComponents, getChildComponent, addChildComponent, toFront, toBack, toBehind
        */
        int getIndexOfChildComponent(const Component *child) const noexcept
        {
            return content.getIndexOfChildComponent(child);
        }
    
        /** Provides access to the underlying array of child components.
            The most likely reason you may want to use this is for iteration in a range-based for loop.
        */
        const juce::Array<juce::Component*>& getChildren() const noexcept { return content.getChildren(); }
    
        /** Looks for a child component with the specified ID.
            @see setComponentID, getComponentID
        */
        Component* findChildWithID(juce::StringRef componentID) const noexcept
        {
            return content.findChildWithID(componentID);
        }
    
        /** Adds a child component to this one.

            Adding a child component does not mean that the component will own or delete the child - it's
            your responsibility to delete the component. Note that it's safe to delete a component
            without first removing it from its parent - doing so will automatically remove it and
            send out the appropriate notifications before the deletion completes.
    
            If the child is already a child of this component, then no action will be taken, and its
            z-order will be left unchanged.
    
            @param child    the new component to add. If the component passed-in is already
                            the child of another component, it'll first be removed from it current parent.
            @param zOrder   The index in the child-list at which this component should be inserted.
                            A value of -1 will insert it in front of the others, 0 is the back.
            @see removeChildComponent, addAndMakeVisible, addChildAndSetID, getChild,
                 ComponentListener::componentChildrenChanged
        */
        void addChildComponent(Component *child, int zOrder = -1) { content.addChildComponent(child, zOrder); }
    
        /** Adds a child component to this one.
    
            Adding a child component does not mean that the component will own or delete the child - it's
            your responsibility to delete the component. Note that it's safe to delete a component
            without first removing it from its parent - doing so will automatically remove it and
            send out the appropriate notifications before the deletion completes.
    
            If the child is already a child of this component, then no action will be taken, and its
            z-order will be left unchanged.
    
            @param child    the new component to add. If the component passed-in is already
                            the child of another component, it'll first be removed from it current parent.
            @param zOrder   The index in the child-list at which this component should be inserted.
                            A value of -1 will insert it in front of the others, 0 is the back.
            @see removeChildComponent, addAndMakeVisible, addChildAndSetID, getChild,
                 ComponentListener::componentChildrenChanged
        */
        void addChildComponent(Component &child, int zOrder = -1) { content.addChildComponent(child, zOrder); }
    
        /** Adds a child component to this one, and also makes the child visible if it isn't already.
    
            This is the same as calling setVisible (true) on the child and then addChildComponent().
            See addChildComponent() for more details.
    
            @param child    the new component to add. If the component passed-in is already
                            the child of another component, it'll first be removed from it current parent.
            @param zOrder   The index in the child-list at which this component should be inserted.
                            A value of -1 will insert it in front of the others, 0 is the back.
        */
        void addAndMakeVisible (Component *child, int zOrder = -1) { content.addAndMakeVisible(child, zOrder); }
    
        /** Adds a child component to this one, and also makes the child visible if it isn't already.
    
            This is the same as calling setVisible (true) on the child and then addChildComponent().
            See addChildComponent() for more details.
    
            @param child    the new component to add. If the component passed-in is already
                            the child of another component, it'll first be removed from it current parent.
            @param zOrder   The index in the child-list at which this component should be inserted.
                            A value of -1 will insert it in front of the others, 0 is the back.
        */
        void addAndMakeVisible(Component& child, int zOrder = -1) { content.addAndMakeVisible(child, zOrder); }
    
        /** Adds a child component to this one, makes it visible, and sets its component ID.
            @see addAndMakeVisible, addChildComponent
        */
        void addChildAndSetID(Component *child, const juce::String &componentID)
        {
            content.addChildAndSetID(child, componentID);
        }
    
        /** Removes one of this component's child-components.
    
            If the child passed-in isn't actually a child of this component (either because
            it's invalid or is the child of a different parent), then no action is taken.
    
            Note that removing a child will not delete it! But it's ok to delete a component
            without first removing it - doing so will automatically remove it and send out the
            appropriate notifications before the deletion completes.
    
            @see addChildComponent, ComponentListener::componentChildrenChanged
        */
        void removeChildComponent(Component *childToRemove) { content.removeChildComponent(childToRemove); }
    
        /** Removes one of this component's child-components by index.
    
            This will return a pointer to the component that was removed, or null if
            the index was out-of-range.
    
            Note that removing a child will not delete it! But it's ok to delete a component
            without first removing it - doing so will automatically remove it and send out the
            appropriate notifications before the deletion completes.
    
            @see addChildComponent, ComponentListener::componentChildrenChanged
        */
        Component* removeChildComponent(int childIndexToRemove) { content.removeChildComponent(childIndexToRemove); }
    
        /** Removes all this component's children.
            Note that this won't delete them! To do that, use deleteAllChildren() instead.
        */
        void removeAllChildren() { content.removeAllChildren(); }
    
        /** Removes and deletes all of this component's children.
            My advice is to avoid this method! It's an old function that is only kept here for
            backwards-compatibility with legacy code, and should be viewed with extreme
            suspicion by anyone attempting to write modern C++. In almost all cases, it's much
            smarter to manage the lifetimes of your child components via modern RAII techniques
            such as simply making them member variables, or using std::unique_ptr, OwnedArray,
            etc to manage their lifetimes appropriately.
            @see removeAllChildren
        */
        void deleteAllChildren() { content.deleteAllChildren(); }
    
        //==============================================================================================================
        /** Finds the appropriate look-and-feel to use for this component.
            
            If the component hasn't had a look-and-feel explicitly set, this will
            return the parent's look-and-feel, or just the default one if there's no
            parent.
    
            @see setLookAndFeel, lookAndFeelChanged
        */
        LookAndFeel_Jaut& getLookAndFeel() const noexcept
        {
            return static_cast<LookAndFeel_Jaut&>(juce::Component::getLookAndFeel());
        }
    
        /** Sets the look and feel to use for this component.
    
            This will also change the look and feel for any child components that haven't
            had their look set explicitly.
    
            The object passed in will not be deleted by the component, so it's the caller's
            responsibility to manage it. It may be used at any time until this component
            has been deleted.
    
            Calling this method will also invoke the sendLookAndFeelChange() method.
    
            @see getLookAndFeel, lookAndFeelChanged
        */
        void setLookAndFeel(LookAndFeel_Jaut *newLookAndFeel) { juce::Component::setLookAndFeel(newLookAndFeel); }
    
        //==============================================================================================================
        
        
        //==============================================================================================================
        void setStyle(Style newStyle, juce::NotificationType notification = juce::NotificationType::sendNotification)
        {
            const Style old_style = std::exchange(style, std::move(newStyle));
            
            bool update = false;
            
            if (old_style.contentPadding != style.contentPadding
                || old_style.borderThickness != style.borderThickness)
            {
                resized();
                update = true;
            }
            
            if (notification != juce::dontSendNotification && (onStyleChanged(old_style, style) || update))
            {
                StyleChanged.invoke(old_style, style);
            }
        }

    protected:
        Style style;
        
        //==============================================================================================================
        /**
         *  Override this function to change your component whenever the style was changed.
         *
         *  @param oldStyle The old style
         *  @param newStyle The new style
         *  @return True if changes were made, false if not
         */
        virtual bool onStyleChanged(const Style &oldStyle, const Style &newStyle) { return false; }
        
        /**
         *  Override this function to paint on the component.
         *  @param g The graphics context
         */
        virtual void onPaint(juce::Graphics &g) {}
    
        /** Override this function to adjust your child components to their parent whenever it was resized. */
        virtual void onResized() {}
        
        virtual void onParentHierarchyChanged() {}
        
        virtual void onLookAndFeelChanged() {}
        
        virtual void onChildrenChanged() {}
        
    private:
        juce::Component content;
        LookAndFeel_Jaut *lookAndFeel;
    
        //==============================================================================================================
        void parentHierarchyChanged() override
        {
            lookAndFeelChanged();
            onParentHierarchyChanged();
        }
    
        void lookAndFeelChanged() override
        {
            if (lookAndFeel = dynamic_cast<LookAndFeel_Jaut*>(&getLookAndFeel()); !lookAndFeel)
            {
                lookAndFeel = &getDefaultLaf();
                setLookAndFeel(lookAndFeel);
            }
            
            onLookAndFeelChanged();
        }
        
        void childrenChanged() override { onChildrenChanged(); }
    
        //==============================================================================================================
        void setLookAndFeel(juce::LookAndFeel *newLookAndFeel) { juce::Component::setLookAndFeel(newLookAndFeel); }
    };
}
