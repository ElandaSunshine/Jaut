/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   config.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{

class IConfigParser;

/**
    The Config class provides a toolset for reading and writing from/to config files on the local file system.

    The config file contains jaut::Config::Property objects holding data to the properties defined in the config file,

    @see jaut::Config::Property
 */
class JAUT_API Config final
{
public:
    /**
        The Property class contains data for/from a Config object.

        Every property can contain sub properties, by that means you can nest properties.

        You don't need to pass these by reference or pointer as the underlying implementation implements a shared
        pointer to keep track of the data object.

        Note that property names are case-insensitive!

        @see Config
     */
    class JAUT_API Property final
    {
    public:
        //==============================================================================================================
        struct JAUT_API Listener
        {
            virtual void onValueChanged(const String &name, var oldValue, var newValue) = 0;
            virtual void onPropertyAdded(Property property) {}
        };

        //==============================================================================================================
        using t_ChildMap  = std::map<juce::String, Property>;
        using t_Iterator  = t_ChildMap::iterator;
        using t_CIterator = t_ChildMap::iterator;

        //==============================================================================================================
        /** Constructs an invalid property object. */
        Property() noexcept = default;

        /**
            Constructs a new property object.

            @param name         The name of the property (cannot be empty)
            @param defaultValue The default value the property should have
         */
        Property(const String &name, const var &defaultValue);
        ~Property() noexcept;

        //==============================================================================================================
        /**
            Gets a sub-property by its name if it exists, if not it will default
            construct a property and add it to the map. (invalid property)

            If you don't want a default sub-property to be added if it doesn't exist,
            check out getProperty()!

            @param name The name of the sub-property to fetch
            @returns    The sub-property with that name
         */
        Property operator[](const String &name) noexcept;

        //==============================================================================================================
        /**
            Creates and adds a new sub-property if it doesn't exist already.

            @param name         The name of the new sub-property
            @param defaultValue (Optional) The default value the sub-property should have
            @returns            The newly created/existing sub-property
         */
        Property createProperty(const String &name, const var &defaultValue = var()) noexcept;

        /**
            Adds a sub-property instance if it doesn't already contain one with the
            same name.

            @param prop The sub-property to add to the map
         */
        void addProperty(Property prop) noexcept;

        /**
            Gets a sub-property from the property map.

            @param name The name of the sub-property
            @returns The sub-property if it exists, else an invalid property will be
                     returned
         */
        Property getProperty(const String &name) const noexcept;

        //==============================================================================================================
        /**
            Gets the amount of sub-properties the property map contains.

            @returns The size of the property map
         */
        const int size() const noexcept;

        /**
            Checks whether this property has any valid sub-properties or not.

            @returns True if any valid sub-properties are contained
         */
        const bool hasValid() const noexcept;

        /**
            Checks whether this property is a valid property or not.

            @returns True if this property is valid
         */
        const bool isValid() const;

        /**
            Checks whether this property contains a particular sub-property or not.

            @param name The of the sub-property
            @returns True if it contains a property with that name
         */
        const bool hasProperty(const String &name) const noexcept;

        //==============================================================================================================
        /**
            Gets the name of this property.

            @returns The name of the property
         */
        const String getName() const noexcept;

        /**
            Gets the comment of this property.

            @returns The comment of the property
         */
        const String getComment() const noexcept;

        /**
            Sets the comment of this property.

            Comments, if the parser implements them, are a useful feature to describe
            what a property in the config file does.

            @param comment The comment to set
         */
        void setComment(const String &comment) noexcept;

        /**
            Sets a new value for this property.

            @param value The new value of the property
         */
        void setValue(const var &value) noexcept;

        /**
            Gets the current value of this property.

            @returns The value of the property
         */
        const var &getValue() const noexcept;

        /**
            Resets the properties current value, and alternatively its child's, to default.

            @param recursive If this is true, all children properties will be reset too
         */
        void reset(bool recursive = false) noexcept;

        //==============================================================================================================
        /**
            Gets this' property value as string.

            @returns The string representation of the property's value
         */
        const String toString() const noexcept;

        //==============================================================================================================
        t_Iterator  begin()        noexcept;
        t_Iterator  end()          noexcept;
        t_CIterator begin()  const noexcept;
        t_CIterator end()    const noexcept;
        t_CIterator cbegin() const noexcept;
        t_CIterator cend()   const noexcept;

        //==============================================================================================================
        void addListener(Listener *listener) noexcept;
        void removeListener(Listener *listener) noexcept;

        //==============================================================================================================
        friend void swap(Property &left, Property &right) noexcept
        {
            left.data.swap(right.data);
        }

    private:
        struct SharedObject;
        friend class Config;
        
        std::shared_ptr<SharedObject> data;

        //==============================================================================================================
        void addConfig(Config *config) noexcept;
        void postAddedListener(Property prop) noexcept;
        void postValueChangedListener(const String &name, var oldValue, var newValue) noexcept;
    };

    struct JAUT_API Listener
    {
        virtual void onValueChanged(const String &name, var oldValue, var newValue) = 0;
        virtual void onPropertyAdded(Property property) {}
    };

    /**
        This struct provides basic options the config file uses and needs to perform its task the way it should.
    */
    struct JAUT_API Options final
    {
        /**
            Constructs a default options object.
         */
        Options() noexcept;

        /** Whether the config should be auto-saved whenever a change has happened. */
        bool autoSave;

        /**
            Determines whether the config file should be locked whenever it reads or
            writes to the config file.

            This way, when another instance of this app tries to read or write to the
            file, it has to wait until the owning process/thread freed the lock.
         */
        bool processSynced;

        /**
            A brief summary or notice which should be, dependend on the parser,
            prepended in the config file.
         */
        String configNotice;

        /** The default category the config file defines. */
        String defaultCategory;

        /** The name of the config file. (including extension if desired) */
        String fileName;
    };

    using t_PropertyMap  = std::map<juce::String, Property>;
    using t_CategoryMap  = std::unordered_map<juce::String, t_PropertyMap>;
    using t_CommentMap   = std::unordered_map<juce::String, juce::String>;
    using t_ProcessLock  = juce::InterProcessLock::ScopedLockType;
    using p_ProcessMutex = std::unique_ptr<juce::InterProcessLock>;
    using p_ProcessLock  = std::unique_ptr<t_ProcessLock>;
    using p_ConfigParser = std::unique_ptr<IConfigParser, void(*)(IConfigParser*)>;

    //==================================================================================================================
    /**
        Constructs a new Config object.

        If parserType in the passed options object is set to custom, you can pass a custom parser implementation to the
        constructor so you can use your own parser.

        @param root         The absolute folder path the config file should be stored in
        @param options      Options the config object should use to perform its task as desired
        @param customParser (Optional) The custom parser if parserType was set to custom

        @see Config::ParserType
     */
    Config(const juce::String &root, const Options &options, std::unique_ptr<jaut::IConfigParser> configParser);

    /**
     * Moves the Config object to another object.
     * Be aware that this doesn't copy the contained listeners!
     *
     * @param other The other config object
     */
    Config(Config &&other) noexcept;
    ~Config();

    //==================================================================================================================
    Config &operator=(Config &&other) noexcept;

    //==================================================================================================================
    /**
        Tries to load data from the config file stored on your local file system.

        @returns True if the config file was successfully loaded
     */
    const bool load();

    /**
        Tries to write data to the config file stored on your local file system.

        @returns True if the config file was successfully written
     */
    const bool save() const;

    //==================================================================================================================
    /**
        Gets the config file name.

        @param withoutExtension If this is true, the returned name will keep the file extension if it has one
        @returns The name of the config file
     */
    const String getConfigName(bool withoutExtension = false) const noexcept;

    /**
        Gets a property from the config.

        @param name     The name of the property
        @param category The name of the category the property is in
        @returns The property if it exists or an invalid property
     */
    Property getProperty(const String &name, const String &category = String()) const noexcept;

    /**
        Gathers all properties into one root property and creates properties for the various categories.

        @returns The root Property object containing all categories and subsequent Property objects
     */
    Property getAllProperties();

    /**
        Gathers all properties into one root property and creates properties for the various categories.

        @returns The root Property object containing all categories and subsequent Property objects
     */
    const Property getAllProperties() const;

    /**
        Gets the comment of a category.

        @param category The category of the comment
        @returns The comment if it exists or an empty string
     */
    String getCategoryComment(const String &category) const noexcept;

    /**
        Sets the comment for a certain category.

        @param category The category for the comment
        @param comment  The comment for the category
     */
    void setCategoryComment(const String &category, const String &comment) noexcept;

    /**
        Gets the current parser of the config.
        @returns The config's parser
     */
    const IConfigParser *getParser() const noexcept;

    /**
        You can disable auto save for a moment when you don't want the config to auto save.
        @param bypass True if auto save should be disabled, false if not
     */
    void bypassAutosave(bool bypass) noexcept;

    /**
        Gets the options for this config object.
        @return The options object
     */
    const Options &getOptions() const noexcept;

    //==================================================================================================================
    /**
        Adds a property to the config if it doesn't exist already.

        @param prop     The property to add to the config
        @param category The category the property should belong to
     */
    void addProperty(Property prop, const String &category = String()) noexcept;

    /**
        Creates a new property and adds it to the config.
        If the property already exists it will return it.

        @param name         The name of the new property
        @param defaultValue The default value the property should be initialized with
        @param category     The category the property should belong to
     */
    Property createProperty(const String &name, const var &defaultValue = var(),
                            const String &category = String()) noexcept;

    /**
        Resets a whole category's property values.
        Be aware that this can't be undone!
     */
    void resetCategory(const String &category = String()) noexcept;

    //==================================================================================================================
    void addListener(Listener *listener) noexcept;
    void removeListener(Listener *listener) noexcept;

    //==================================================================================================================
    friend void swap(Config &left, Config &right)
    {
        std::swap(left.lock, right.lock);
        std::swap(left.autoSaveActive, right.autoSaveActive);
        std::swap(left.options, right.options);
        left.parser.swap(right.parser);
        left.ipMutex.swap(right.ipMutex);
        left.categories.swap(right.categories);
        left.comments.swap(right.comments);
        left.fullPath.swapWith(right.fullPath);
    }

private:
    friend Property;

    bool autoSaveActive;
    t_CategoryMap categories;
    t_CommentMap comments;
    String fullPath;
    p_ProcessMutex ipMutex;
    juce::ListenerList<Listener> listeners;
    InterProcessLock *lock;
    p_ConfigParser parser;
    Options options;

    //==================================================================================================================
    const bool shouldAutosave() const noexcept;
    t_ProcessLock *createIpcLock() const;
    void postAddedListener(Property prop) noexcept;
    void postValueChangedListener(const String &name, var oldValue, var newValue) noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};
}
