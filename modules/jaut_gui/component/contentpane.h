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
    @file   contentpane.h
    @date   21, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  A simple component that contains another component and stretches it to a parent.
 *  This is great when you don't want to add your component directly to a parent but rather through
 *  a intermediate component.
 *
 *  You can either pass a component to the constructor, or if you don't want to set it at construction state,
 *  you can use resetComponent().
 */
class JAUT_API ContentPane : public juce::Component
{
public:
    enum JAUT_API ColourIds
    {
        /** The background colour of the pane, this will only be visible if it's empty. */
        ColourBackgroundId = nextColourId<16>
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        /**
         * Draws the background of the ContentPane.
         * This will only be called if the ContentPane object contains no content-component.
         *
         * @param g      The graphics context
         * @param bounds The bounds of the ContentPane (local bounds)
         */
        virtual void drawContentPaneBackground(juce::Graphics &g, juce::Rectangle<int> bounds) = 0;
    };
    
    //==================================================================================================================
    using ComponentChangedHandler = EventHandler<juce::Component*>;
    
    /** Dispatched whenever the contained component was changed. */
    Event<ComponentChangedHandler> ComponentChanged;
    
    //==================================================================================================================
    /**
     *  Constructs a new ContentPane object with the given component contained but now owned.
     *  @param content The component to contain (not owned)
     */
    explicit ContentPane(juce::Component &content) noexcept;
    
    /**
     *  Constructs a new ContentPane object with the given component contained or no component.
     *
     *  @param content The component to be contained or nullptr if no component should be contained
     *  @param owned   True if the component should be deleted by the ContentPane or false if not
     */
    explicit ContentPane(juce::Component *content = nullptr, bool owned = true);
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //==================================================================================================================
    /**
     * Resets the contained component and empties the pane.
     * @return True if the component was successfully removed or false if it was null already
     */
    bool resetComponent();
    
    /**
     *  Resets the contained component and empties the pane.
     *  @return True if the component was successfully removed or false if it was null already
     */
    bool resetComponent(std::nullptr_t);
    
    /**
     *  Resets the contained component and sets it to a new one or empties it if nullptr.
     *
     *  @param content The component to be contained or nullptr if no component should be contained
     *  @param owned   True if the component should be deleted by the ContentPane or false if not
     *  @return True if the component was successfully changed or false if the current component was the same
     */
    bool resetComponent(juce::Component *content, bool owned);
    
    /**
     *  Resets the contained component and sets it to a new one.
     *
     *  @param content The component to be contained
     *  @return True if the component was successfully changed or false if the current component was the same
     */
    bool resetComponent(juce::Component &content);
    
    //==================================================================================================================
    /**
     *  Gets the current contained component.
     *  @return The current contained component or nullptr if none was set
     */
    juce::Component* getCurrentComponent() noexcept;
    
private:
    juce::OptionalScopedPointer<juce::Component> content;
    JAUT_CREATE_LAF()
};
}
