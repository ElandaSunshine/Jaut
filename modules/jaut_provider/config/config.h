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
    @file   config.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{

/**
 *  The Config class provides a toolset for reading/writing from/to config files on the local file system.
 *  The config file contains properties holding data to the properties defined in the config file.
 *
 *  Property IDs are case-insensitive and whitespaceless internally, so make sure you are using correct IDs for your
 *  properties.
 *
 *  @see jaut::Config::Property
 */
class JAUT_API Config final
{
private:
    class PropertyCategory;

public:
    class Property;
    
    //==================================================================================================================
    /** The event handler, handling value change events */
    using ValueChangedHandler = EventHandler<juce::String, juce::var, juce::var>;
    
    /** The event handler, handling property addition events */
    using PropertyAddedHandler = EventHandler<Config::Property>;
    
    /** The map type containing the root properties. */
    using PropertyMap = std::map<juce::String, Config::Property>;
    
    /** The map type containing the categories. */
    using CategoryMap = std::unordered_map<juce::String, Config::PropertyCategory>;
    
    /** The return type of const member functions returning a Property object */
    using ConstProperty = ConstProxy<jaut::Config::Property>;
    
    /** The type of config parser. */
    using ConfigParserType = IConfigParser<Config::Property, Config::ConstProperty>;
    
    //==================================================================================================================
    struct JAUT_API ErrorCodes final
    {
        /** The child property already existed. */
        static constexpr int PropertyExists = 0x0001;
    
        /** This property or the child property was invalid. */
        static constexpr int PropertyInvalid = 0x0002;
    
        /** The property is already owned by a config. */
        static constexpr int PropertyOwned = 0x0003;
    
        /** The config file is already getting modified by another program. */
        static constexpr int FileIsOpen = 0x0004;
    
        /** The config file to read-from/write-to was not found. */
        static constexpr int FileNotFound = 0x0005;
        
        /** The config's parser was invalid and couldn't be used. */
        static constexpr int InvalidParser = 0x0006;
    };
    
    /**
     *  The Property class contains data for a config object.
     *  Every property can contain child properties, by that means, you can nest properties.
     *  @see Config
     */
    class JAUT_API Property
    {
    public:
        //==============================================================================================================
        /** The map-type containing this property's child properties */
        using ChildMap = std::map<juce::String, Property>;

        /** The iterator type of the ChildMap type */
        using Iterator = ChildMap::iterator;

        /** The const iterator type of the ChildMap type */
        using ConstIterator = ChildMap::const_iterator;
    
        //==============================================================================================================
        /** Posts an event when the value of this property was changed. */
        Event<ValueChangedHandler> ValueChanged;
        
        /** Posts an event when a new child property was added to this property. */
        Event<PropertyAddedHandler> PropertyAdded;
    
        //==============================================================================================================
        /**
         *  Constructs an invalid property.
         */
        Property() = default;
    
        /**
         *  Constructs a new property.
         *  @param name         The name of the property
         *  @param defaultValue The default value of the property
         */
        Property(const juce::String &name, const juce::var &defaultValue);
        Property(const Property &other) noexcept;
        Property(Property &&other) noexcept;
        ~Property();
    
        //==============================================================================================================
        Property &operator=(const Property &right) noexcept;
        Property &operator=(Property &&right) noexcept;
        
        //==============================================================================================================
        /**
         *  Gets a child property by its name if it exists, if not it will default construct a property and add it
         *  to the map. (invalid property)
         *
         *  If you don't want a default child property to be added if it doesn't exist, check out getProperty()!
         *  You can resolve child properties with the period character if enabled.
         *
         *  <b>For example:</b>
         *  Your property "test" contains a child property with the name "testchild", you can acquire this by
         *  specifying "test.testchild".
         *
         *  @param name The name of the child property to fetch
         *  @returns The child property with that name
         */
        Property operator[](const juce::String &name);

        //==============================================================================================================
        /**
         *  Creates and adds a new child property if it doesn't exist already, if a property already exists
         *  it will return the existing one.
         *  You can not resolve properties with this function call.
         *
         *  @param name         The name of the new child property
         *  @param defaultValue (Optional) The default value the child property should have
         *  @param notification Whether to send the change to all listeners
         *  @returns The newly created/Existing child property
         */
        Property createProperty(const juce::String &name, const juce::var &defaultValue = juce::var(),
                                juce::NotificationType notification = juce::sendNotification);
        
        /**
         *  Adds a child property to this property.
         *  
         *  The error-codes this function may produce are:
         *  <ul>
         *      <li>Config::ErrorCodes::PropertyExists</li>
         *      <li>Config::ErrorCodes::PropertyOwned</li>
         *  </ul>
         * 
         *  @param property The property to add
         *  @param notification Whether to send the change to all listeners
         *  @return The result of this operation
         */
        OperationResult addProperty(Property property, juce::NotificationType notification = juce::sendNotification);
        
        /**
         *  Gets a child property from the property map or an invalid if no such was found.
         *  You can resolve child properties with the period character.
         *
         *  <b>For example:</b>
         *  Your property "test" contains a child property with the name "testchild", you can acquire this by
         *  specifying "test.testchild".
         *
         *  @param name The name of the child property
         *  @returns The child property if it exists, else an invalid property will be returned
         */
        Property getProperty(const juce::String &name);
        
        /**
         *  Gets a child property from the property map or an invalid if no such was found.
         *
         *  You can resolve child properties with the period character if enabled.
         *
         *  <b>For example:</b>
         *  Your property "test" contains a child property with the name "testchild", you can acquire this by
         *  specifying "test.testchild".
         *
         *  @param name The name of the child property
         *  @returns The child property if it exists, else an invalid property will be returned
         */
        ConstProperty getProperty(const juce::String &name) const;
        
        //==============================================================================================================
        /**
         *  Gets the amount of child properties the property map contains.
         *  @returns The size of the property map
         */
        int size() const noexcept;

        /**
         *  Checks whether this property has any valid child properties or not.
         *  @returns True if any valid child properties are contained
         */
        bool hasValid() const noexcept;

        /**
         *  Checks whether this property is a valid property or not.
         *  @returns True if this property is valid
         */
        bool isValid() const noexcept;

        /**
         *  Checks whether this property contains a particular child property or not.
         *
         *  @param name The name of the child property
         *  @returns True if it contains a property with this name
         */
        bool hasProperty(const juce::String &name) const;

        //==============================================================================================================
        /**
         *  Gets the name of this property.
         *  @returns The name of the property
         */
        juce::String getName() const noexcept;
        
        /**
         *  Gets the category name this property's root sits in.
         *  @return The name of the category
         */
        juce::String getTopCategory() const noexcept;
        
        /**
         *  Gets the full name of the property.
         *  This represents a period seperated string of the full property hierarchy up until the category.
         *
         *  @return The absolute property name
         */
        juce::String getAbsoluteName() const;
        
        /**
         *  Gets the comment of this property.
         *  @returns The comment of the property
         */
        juce::String getComment() const noexcept;

        /**
         *  Sets the comment of this property.
         *
         *  Comments, if the parser implements them, are a useful feature to describe
         *  what a property in the config file does.
         *
         *  @param commentText The comment to set
         */
        void setComment(const juce::String &commentText) noexcept;

        /**
         *  Sets a new value for this property.
         *  @param value The new value of the property
         */
        void setValue(const juce::var &value, juce::NotificationType notification = juce::sendNotification) noexcept;

        /**
         *  Gets the current value of this property.
         *  @returns The value of the property
         */
        const juce::var &getValue() const noexcept;

        /**
         *  Resets the properties current value, and alternatively its child's, to default.
         *  @param recurse If this is true, all child properties will be reset too
         */
        void reset(bool recurse = false, juce::NotificationType notification = juce::sendNotification) noexcept;

        //==============================================================================================================
        /**
         *  Gets this property's value as string.
         *  @returns The string representation of the property's value
         */
        juce::String toString() const noexcept;

        //==============================================================================================================
        Iterator begin() noexcept;
        Iterator end()   noexcept;
        ConstIterator begin()  const noexcept;
        ConstIterator end()    const noexcept;
        ConstIterator cbegin() const noexcept;
        ConstIterator cend()   const noexcept;
    
        //==============================================================================================================
        friend void swap(Property &left, Property &right) noexcept
        {
            std::swap(left.data, right.data);
        }

    private:
        friend class Config;
    
        //==============================================================================================================
        struct PropertyData;
        std::shared_ptr<PropertyData> data;
    
        //==============================================================================================================
        void setConfig(Config *config);
        
        //==============================================================================================================
        Property recurseChilds(juce::StringArray &names, bool add,juce::NotificationType);
        ConstProperty recurseChilds(juce::StringArray &names) const;
    };
    
    /** This struct provides basic options the config file uses and needs to operate as supposed to. */
    struct JAUT_API Options final
    {
        /**
         *  A brief summary or notice which should be, dependent on the parser,
         *  prepended in the config file.
         */
        juce::String configNotice;

        /** The default category the config file defines. */
        juce::String defaultCategory { "general" };

        /** The name of the config file. (including extension if desired) */
        juce::String fileName { "settings.conf" };

        /** Whether the config should be auto-saved whenever a change has happened. */
        bool autoSave { false };

        /**
         *  Determines whether the config file should be locked whenever it reads or writes to the config file.
         *  This way, when another instance of this app tries to read or write to the file, it has to wait until the
         *  owning process/thread releases the lock.
         */
        bool processSynced { false };
    };
    
    //==============================================================================================================
    /** Event when the value of any child property was changed. */
    Event<ValueChangedHandler> ValueChanged;
    
    /** Event when a new property was added to the config. */
    Event<PropertyAddedHandler> PropertyAdded;
    
    //==============================================================================================================
    /**
     *  Constructs a new Config object.
     *
     *  Note that the parser must be non-null, else the config won't be able to read and write config files.
     *
     *  @param root         The absolute folder path the config file should be stored in
     *  @param options      Options that the config object should use to perform its task as desired
     *  @param configParser The type of parser which should be used to parse the config
     *
     *  @see Config::ParserType
     */
    Config(const juce::String &root, Options options, std::unique_ptr<ConfigParserType> configParser);
    Config(Config &&other) noexcept;
    ~Config();

    //==================================================================================================================
    Config& operator=(Config &&other) noexcept;

    //==================================================================================================================
    /**
     *  Tries to load data from the config file stored on your local file system.
     *  
     *  The error-codes this function may produce are:
     *  <ul>
     *      <li>Config::ErrorCodes::InvalidParser</li>
     *      <li>Config::ErrorCodes::FileIsOpen</li>
     *      <li>Config::ErrorCodes::FileNotFound</li>
     *  </ul>
     * 
     *  @returns The result of this operation
     */
    OperationResult load();

    /**
     *  Tries to write data to the config file stored on your local file system.
     * 
     *  The error-codes this function may produce are:
     *  <ul>
     *      <li>Config::ErrorCodes::InvalidParser</li>
     *      <li>Config::ErrorCodes::FileIsOpen</li>
     *      <li>Config::ErrorCodes::FileNotFound</li>
     *  </ul>
     * 
     *  @returns The result of this operation
     */
    OperationResult save() const;

    //==================================================================================================================
    /**
     *  Gets the config file name.
     *
     *  @param withoutExtension If this is true, the returned name will keep the file extension if it has one
     *  @returns The name of the config file
     */
    juce::String getConfigName(bool withoutExtension = false) const;
    
    /**
     *  Gets a property from the config.
     *
     *  @param name     The name of the property
     *  @param category The name of the category the property is in
     *  @returns The property if it exists or an invalid property
     */
    Property getProperty(const juce::String &name, const juce::String &category = juce::String());
    
    /**
     *  Gets a property from the config.
     *
     *  @param name     The name of the property
     *  @param category The name of the category the property is in
     *  @returns The property if it exists or an invalid property
     */
    ConstProperty getProperty(const juce::String &name, const juce::String &category = juce::String()) const;

    /**
     *  Gathers all properties into one root property and creates properties for the various categories.
     *  @returns The root Property object containing all categories and subsequent Property objects
     */
    Property getAllProperties();

    /**
     *  Gathers all properties into one root property and creates properties for the various categories.
     *  @returns The root Property object containing all categories and subsequent Property objects
     */
    ConstProperty getAllProperties() const;

    /**
     *  Gets the comment of a category.
     *
     *  @param category The category of the comment
     *  @returns The comment if it exists or an empty string
     */
    juce::String getCategoryComment(const juce::String &category) const;

    /**
     *  Sets the comment for a certain category.
     *
     *  @param category The category for the comment
     *  @param comment  The comment for the category
     */
    void setCategoryComment(const juce::String &category, const juce::String &comment);

    /**
     *  Gets the current parser of the config.
     *  @returns The config's parser
     */
    const ConfigParserType* getParser() const noexcept;

    /**
     *  You can disable auto save for a moment when you don't want the config to auto save.
     *  On the next call to auto save this will be reset to its default save, so you will need to call this again
     *  to preven auto-saving one more time.
     *
     *  @param bypass True if auto save should be disabled, false if not
     */
    void bypassAutosave(bool bypass) noexcept;

    /**
     *  Gets the options for this config object.
     *  @return The options object
     */
    const Options& getOptions() const noexcept;

    //==================================================================================================================
    /**
     *  Adds a property to the config if it doesn't exist already.
     *  
     *  The error-codes this function may produce are:
     *  <ul>
     *      <li>Config::ErrorCodes::PropertyExists</li>
     *      <li>Config::ErrorCodes::PropertyOwned</li>
     *  </ul>
     * 
     *  @param prop     The property to add to the config
     *  @param category The category the property should belong to
     *  @return The result of the operation
     */
    OperationResult addProperty(Property prop, const juce::String &category = juce::String()) noexcept;

    /**
     *  Creates a new property and adds it to the config.
     *  If the property already exists it will return it.
     *
     *  @param name         The name of the new property
     *  @param defaultValue The default value the property should be initialized with
     *  @param category     The category the property should belong to
     */
    Property createProperty(const juce::String &name, const juce::var &defaultValue = juce::var(),
                            const juce::String &category = juce::String()) noexcept;

    /**
     *  Resets a whole category's property values to their defaults.
     *  @param category The category to reset
     */
    void resetCategory(const juce::String &category = juce::String());
    
    /**
     *  Resets all category's property values to their defaults.
     */
    void resetAll();
    
    //==================================================================================================================
    friend void swap(Config &left, Config &right)
    {
        std::swap(left.autoSaveActive, right.autoSaveActive);
        std::swap(left.fullPath, right.fullPath);
        std::swap(left.options, right.options);
        std::swap(left.parser, right.parser);
        std::swap(left.categories, right.categories);
        std::swap(left.ipMutex, right.ipMutex);
    }

private:
    friend Property;
    
    //==================================================================================================================
    struct PropertyCategory final
    {
        PropertyMap properties;
        juce::String comment;
    };
    
    //==================================================================================================================
    using ProcessLockPtr      = std::unique_ptr<juce::InterProcessLock>;
    using ProcessLockGuard    = juce::InterProcessLock::ScopedLockType;
    using ProcessLockGuardPtr = std::unique_ptr<ProcessLockGuard>;
    
    //==================================================================================================================
    Options options;
    CategoryMap categories;
    juce::String fullPath;
    std::unique_ptr<ConfigParserType> parser;
    ProcessLockPtr ipMutex;
    bool autoSaveActive { false };

    //==================================================================================================================
    bool shouldAutosave() noexcept;
    ProcessLockGuard* createIpcLock() const;
    
    //==================================================================================================================
    Property getAllPropertiesPseudoConst() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};
}
