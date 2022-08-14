/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    @file   jaut_Config.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include "jaut_core/define/jaut_Define.h"
#include <jaut_provider/jaut_provider_define.h>
#include <jaut_core/signal/event/jaut_Event.h>
#include <jaut_core/util/jaut_CommonUtils.h>

#include <juce_events/juce_events.h>



namespace jaut
{
    //==================================================================================================================
    struct IConfigParser;
    
    //==================================================================================================================
    /**
     *  The Config class provides a toolset for reading/writing from/to config files on the local file system.
     *  The config file contains properties holding data to the properties defined in the config file.
     *
     *  Property IDs are case-insensitive and whitespace-less internally,
     *  so make sure you are using correct IDs for your properties.
     *
     *  This class is not thread-safe.
     *
     *  @see jaut::Config::Property
     */
    class JAUT_API Config
    {
    public:
        /**
         *  The Property class contains data for a config object.
         *  Every property can contain child properties, by that means, you can nest properties.
         *
         *  Properties need to have valid identifiers, this means,
         *  they can only contain alphanumeric characters and underscores.
         *
         *  Also note that, Property objects can only either be value- or map-properties.
         *  This means, as soon as a property possesses one child-Property object, it will be handled as such.
         *  You can still store values in map-properties, but they will remain untouched by config loading and saving.
         *
         *  This class is not thread-safe.
         *
         *  @see jaut::Config
         */
        class Property;
        
        /** This struct provides basic options the config file uses and needs to operate as supposed to. */
        struct Options final
        {
            //==========================================================================================================
            /** Defines the cause of a probable auto-save. */
            enum class PolicyTarget
            {
                PropertyAdded,
                ValueChanged
            };
            
            //==========================================================================================================
            /** The policy for when the config should be getting auto-saved. */
            using AutoSavePolicy = std::function<bool(const Property &modifiedProperty, PolicyTarget target)>;
    
            //==========================================================================================================
            /** The default auto-save policy, triggered on every change that has been recorded but not on additions. */
            static constexpr auto policyOnValueChange = [](const Property&, PolicyTarget target)
            {
                return (target == PolicyTarget::ValueChanged);
            };
    
            /** An alternative auto-save policy, triggered on every change that has been recorded, even on additions. */
            static constexpr auto policyOnAnyChange = [](const Property&, PolicyTarget) { return true; };
            
            //==========================================================================================================
            /**
             *  The custom policy that will determine when the config should be auto-saved.
             *  If this is nullptr, auto-save will be disabled.
             */
            AutoSavePolicy autoSavePolicy = nullptr;
            
            /** A brief summary or notice which should be, dependent on the parser, prepended in the config file. */
            juce::String configNotice = "";
            
            /**
             *  The default category the config file defines, allowed are only alphanumeric characters and underscores.
             *  This id, as all the others too, must be non-empty.
             */
            juce::String defaultCategory = "general";
            
            /**
             *  The name of the config file. (including extension)
             *
             *  If this name is empty, the config class will name the file "config" and will try to guess
             *  the extension from the parser.
             *  If the extension could not be guessed, it will just use "conf" as extension.
             */
            juce::String fileName = "";
            
            /**
             *  Whether the config should act-up on incompatible value modifications.
             *
             *  If a property's value has been set to a type it is not valid for, then properties will throw a
             *  jaut::ConfigIncompatibleTypeException.
             */
            bool strictTyping = true;
            
            /** Whether the config file should be automatically created if it doesn't exist. */
            bool createFileIfNotExists = true;
            
            //==========================================================================================================
            Options() {} // NOLINT
        };
        
        //==============================================================================================================
        /** This is just an identifier for category-id parameters used to denote default categories. */
        static constexpr const char *defaultCategory = "";
        
        //==============================================================================================================
        /** The category map type. */
        using CategoryMap = std::map<juce::String, Property>;
        
        /** The config parser pointer type. */
        using ConfigParserPtr = std::unique_ptr<IConfigParser>;
        
        //==============================================================================================================
        using ValueChangedHandler       = jaut::EventHandler<const Config&, const Property&, const juce::var&>;
        using PropertyAddedHandler      = jaut::EventHandler<const Config&, const Property&, const Property&>;
        using SavedAutomaticallyHandler = jaut::EventHandler<const Config&>;
    
        /**
         *  Dispatched whenever the value of this property has changed.
         *
         *  @param par1 The config object which raised the event
         *  @param par2 The property which has changed
         *  @param par3 The old value before the update
         */
        Event<ValueChangedHandler> ValueChanged;
    
        /**
         *  Dispatched whenever a property was added.
         *
         *  @param par1 The config object which raised the event
         *  @param par2 The property to which a property was added too
         *  @param par3 The property which was added
         */
        Event<PropertyAddedHandler> PropertyAdded;
        
        /**
         *  Dispatched whenever the config was saved automatically.
         *  @param par The config object
         */
        Event<SavedAutomaticallyHandler> SavedAutomatically;
        
        //==============================================================================================================
        /**
         *  Creates a config instance with a json parsing backend.
         *
         *  @param directory The directory in which the config will be saved
         *  @param options   Options of the config class
         *  @return The new config instance
         */
        JAUT_NODISCARD
        static Config backendJson(juce::File directory, Options options = {});
        
        /**
         *  Creates a config instance with a xml parsing backend.
         *
         *  @param directory The directory in which the config will be saved
         *  @param options   Options of the config class
         *  @return The new config instance
         */
        JAUT_NODISCARD
        static Config backendXml(juce::File directory, Options options = {});
        
        #if JAUT_OPTLIB_YAML_CPP
        /**
         *  Creates a config instance with a yaml parsing backend.
         *
         *  @param directory The directory in which the config will be saved
         *  @param options   Options of the config class
         *  @return The new config instance
         */
        JAUT_NODISCARD
        static Config backendYaml(juce::File directory, Options options = {});
        #endif
        
        //==============================================================================================================
        /**
         *  Constructs a new Config object.
         *  Note, if the parser is null, any IO call will result in UB as the config class assumes it is valid.
         *
         *  @param directory The directory in which the config will be saved
         *  @param parser    The parser object to parse and process config files
         *  @param options   Options of the config class
         *
         *  @throws jaut::ConfigIOException If the file path was invalid
         *  @throws jaut::ConfigPropertyIdException If the default category name was invalid
         *
         *  @see Config::ParserType
         */
        Config(juce::File directory, NonNull<ConfigParserPtr> configParser, Options options = {});
        
        //==============================================================================================================
        /**
         *  Tries to load data from the config file stored on your local file system.
         *  @returns True if the config-file could be read, otherwise false (if file didn't exist)
         *
         *  @throws jaut::ConfigParseException If an exception occurred during parsing of the config file
         */
        bool load();
        
        /**
         *  Tries to write data to the config file stored on your local file system.
         *  @returns True if the config was successfully saved, false if the file didn't exist and automatic file
         *           creation was disabled
         *
         *  @throws jaut::ConfigIOException If an exception occurred during creating the config file
         */
        bool save() const; // NOLINT
        
        //==============================================================================================================
        /**
         *  Tries to find a child property that satisfies the search expression.
         *  If you want a shortcut to search in the default category, you can start the expression with a dot.
         *
         *  @param expression The search expression
         *  @return The child property if it was found, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If any of the ids in the expression were invalid
         */
        JAUT_NODISCARD
        Property* findProperty(const juce::String &expression);
        
        /**
         *  Tries to find a child property that satisfies the search expression.
         *
         *  @param expression The search expression
         *  @return The child property if it was found, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If any of the ids in the expression were invalid
         */
        JAUT_NODISCARD
        const Property* findProperty(const juce::String &expression) const;
        
        //==============================================================================================================
        /**
         *  Gets the config file name.
         *
         *  @param withoutExtension If this is false, the returned name will keep the file extension if it has one
         *  @returns The name of the config file
         */
        JAUT_NODISCARD
        juce::String getConfigName(bool withoutExtension = false) const;
        
        /**
         *  Gets a property from the category map.
         *
         *  @param id         The id of the property
         *  @param categoryId The name of the category the property is in, or empty for the default category
         *  @returns The property if it exists, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If either property or category id were invalid
         */
        JAUT_NODISCARD
        Property* getProperty(const juce::String &id, const juce::String &categoryId = defaultCategory);
        
        /**
         *  Gets a property from the category map.
         *
         *  @param id         The name of the property
         *  @param categoryId The name of the category the property is in, or empty for the default category
         *  @returns The property if it exists, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If either property or category id were invalid
         */
        JAUT_NODISCARD
        const Property* getProperty(const juce::String &id, const juce::String &categoryId = defaultCategory) const;
        
        /**
         *  Gets the comment of a category.
         *
         *  @param categoryId The category of the comment, or empty for the default category
         *  @returns The comment if it exists, otherwise an empty string
         *
         *  @throws jaut::ConfigPropertyIdException If category was an invalid id
         */
        JAUT_NODISCARD
        juce::String getCategoryComment(const juce::String &categoryId) const;
        
        /**
         *  Gets the options for this config object.
         *  @return The options object
         */
        JAUT_NODISCARD
        const Options& getOptions() const noexcept;
        
        /**
         *  Gets the config file.
         *  @return The config filepath
         */
        JAUT_NODISCARD
        const juce::File& getConfigFile() const noexcept;
    
        //==============================================================================================================
        /**
         *  Sets the comment for a certain category.
         *
         *  @param categoryId The category for the comment, or empty if the default category should be used
         *  @param comment  The comment for the category
         *  @return True if the category was found, otherwise false
         *
         *  @throws jaut::ConfigPropertyIdException If category was an invalid id
         */
        bool setCategoryComment(const juce::String &categoryId, const juce::String &comment);
        
        //==============================================================================================================
        /**
         *  Creates a new property and adds it to the config.
         *  If the property already exists it will return the existing one.
         *
         *  @param id           The name of the new property
         *  @param categoryId   The category the property should belong to, or empty for the default category
         *  @param defaultValue The default value the property should be initialized with
         *  @param notify       Whether to send the change to all listeners
         *  @returns The (new) child property by name,
         *           additionally a boolean evaluating to true when the property had been newly created
         *
         *  @throws jaut::ConfigPropertyIdException If either property or category id were invalid
         */
        std::pair<bool, Property&> createProperty(const juce::String     &id,
                                                  const juce::var        &defaultValue = {},
                                                  const juce::String     &categoryId   = defaultCategory,
                                                  juce::NotificationType notify        = juce::sendNotification);
        
        /**
         *  Resets a whole category's properties (and children) to their defaults.
         *  @param category The category to reset, or empty for the default category
         *  @param notify   Whether to send the change to all listeners
         */
        void resetCategory(const juce::String &categoryId, juce::NotificationType notify = juce::sendNotification);
        
        /**
         *  Resets all category's properties (and children) to their defaults.
         *  @param notify Whether to send the change to all listeners
         */
        void resetAll(juce::NotificationType notify = juce::sendNotification);
        
    private:
        class PropertyBuilder;
        
        //==============================================================================================================
        template<class T>
        JAUT_NODISCARD static auto getCategory_impl(T &config, const juce::String &categoryId)
            -> decltype(config.getCategory(""));
        
        template<class T>
        JAUT_NODISCARD static auto getProperty_impl(T &config, const juce::String &id, const juce::String &category)
            -> decltype(config.getProperty("",""));
        
        template<class T>
        JAUT_NODISCARD static auto findProperty_impl(T &config, const juce::String &expression)
            -> decltype(config.findProperty(""));
        
        //==============================================================================================================
        Options         options;
        CategoryMap     properties;
        juce::File      fullPath;
        ConfigParserPtr parser;
        
        //==============================================================================================================
        JAUT_NODISCARD Property*       getCategory(const juce::String &categoryId);
        JAUT_NODISCARD const Property* getCategory(const juce::String &categoryId) const;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
    };
    
    class Config::PropertyBuilder final
    {
    private:
        friend class Config;
        
        //==============================================================================================================
        static Property& constructInMap(const juce::String& id, const Config &config, Config::CategoryMap &map);
    };
    
    class Config::Property final
    {
    private:
        struct InternalDelegate_t {};
        static constexpr InternalDelegate_t internalDelegate_v = InternalDelegate_t{};
        
    public:
        /** The type of property this Property object represents. */
        enum class PropertyType
        {
            Value,
            Map
        };
        
        //==============================================================================================================
        /** The map-type containing this' property's children. */
        using ChildMap = std::map<juce::String, Property>;
        
        /** The iterator type of the map containing this' property's children. */
        using Iterator = ChildMap::iterator;
        
        /** The const iterator type of the map containing this' property's children. */
        using ConstIterator = ChildMap::const_iterator;
        
        //==============================================================================================================
        using ValueChangedHandler  = EventHandler<const Property&, const juce::var&>;
        using PropertyAddedHandler = EventHandler<const Property&, const Property&>;
        
        /**
         *  Dispatched whenever the value of this property has changed.
         *
         *  @param par1 The property which raised the event
         *  @param par2 The old value prior the update
         */
        Event<ValueChangedHandler> ValueChanged;
        
        /**
         *  Dispatched whenever a child property was added.
         *
         *  @param par1 The property which raised the event
         *  @param par2 The property which was added
         */
        Event<PropertyAddedHandler> PropertyAdded;
        
        //==============================================================================================================
        #if !DOXYGEN
        // This constructor is not meant for user construction, rather, it is used to create properties internally.
        Property(InternalDelegate_t, juce::String id, juce::var defaultValue, const Config &config,
                 const Property *parent);
        #endif
        
        //==============================================================================================================
        /**
         *  Gets a child property by its id if it exists, if not it will default construct a property with that id
         *  and add it to the map. (for strict typing, it will be a string property)
         *
         *  @param id The id of the child property (alphanumeric and/or underscores)
         *  @returns The child (new) property by that name
         *
         *  @throws jaut::ConfigPropertyIdException If the id was invalid
         */
        Property& operator[](const juce::String &id);
        
        //==============================================================================================================
        /**
         *  Creates and adds a new child property if it doesn't exist already.
         *  If a property by that name already exists, gets the existing one.
         *
         *  @param id           The id of the child property (alphanumeric and/or underscores)
         *  @param defaultValue The default value the child property should have
         *  @param notify       Whether to send the change to all listeners
         *  @returns The (new) child property by name,
         *           additionally a boolean evaluating to true when the property had been newly created
         *
         *  @throws jaut::ConfigPropertyIdException If the id was invalid
         */
        std::pair<bool, Property&> createProperty(const juce::String     &id,
                                                  const juce::var        &defaultValue = {},
                                                  juce::NotificationType notify        = juce::sendNotification);
    
        /**
         *  Tries to find a child property in this property or any of the child properties recursively that
         *  satisfies the search expression.
         *
         *  @param expression The search expression
         *  @return The child property that was found, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If any of the ids in the expression were invalid
         */
        JAUT_NODISCARD
        Property* findProperty(const juce::String &expression);
        
        /**
         *  Tries to find a child property in this property or any of the child properties recursively that
         *  satisfies the search expression.
         *
         *  @param expression The search expression
         *  @return The child property that was found, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If any of the ids in the expression were invalid
         */
        JAUT_NODISCARD
        const Property* findProperty(const juce::String &expression) const;
        
        //==============================================================================================================
        /**
         *  Gets the amount of child properties the property map contains.
         *  @returns The size of the property map
         */
        JAUT_NODISCARD
        int size() const noexcept;
        
        /**
         *  Checks whether this property contains a particular child property or not.
         *
         *  @param id The id of the child property (alphanumeric and/or underscores)
         *  @returns True if it contains a property with this name
         *
         *  @throws jaut::ConfigPropertyIdException If the id was invalid
         */
        JAUT_NODISCARD
        bool hasProperty(const juce::String &id) const;
        
        /**
         *  Checks whether this property contains any child properties.
         *  @return True if there are child properties, false otherwise
         */
        JAUT_NODISCARD
        bool hasChildren() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets a child property from the property map.
         *
         *  @param id The id of the child property (alphanumeric and/or underscores)
         *  @returns The child property if it exists, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If the id was invalid
         */
        JAUT_NODISCARD
        Property* getProperty(const juce::String &id);
        
        /**
         *  Gets a child property from the property map.
         *
         *  @param id The id of the child property (alphanumeric and/or underscores)
         *  @returns The child property if it exists, otherwise nullptr
         *
         *  @throws jaut::ConfigPropertyIdException If the id was invalid
         */
        JAUT_NODISCARD
        const Property* getProperty(const juce::String &id) const;
        
        /**
         *  Gets the id of this property.
         *  @returns The id of this property
         */
        JAUT_NODISCARD
        const juce::String& getId() const noexcept;
        
        /**
         *  Gets the category name this property is in.
         *  The category is the top-most property, hence gets the id of the top-most property.
         *
         *  @return The name of the category
         */
        JAUT_NODISCARD
        const juce::String& getCategory() const noexcept;
        
        /**
         *  Gets the full name of the property.
         *
         *  This represents a period seperated string of the full property hierarchy up until (including)
         *  the property's category.
         *
         *  @return The absolute property name
         */
        JAUT_NODISCARD
        juce::String getAbsoluteName() const;
        
        /**
         *  Gets the comment of this property.
         *  @returns The comment of the property
         */
        JAUT_NODISCARD
        const juce::String& getComment() const noexcept;
        
        /**
         *  Gets the current value of this property.
         *  @returns The value of the property
         */
        JAUT_NODISCARD
        const juce::var& getValue() const noexcept;
        
        /**
         *  Gets the initial default value this property was created with.
         *  @returns The default value
         */
        JAUT_NODISCARD
        const juce::var& getDefaultValue() const noexcept;
        
        //==============================================================================================================
        /**
         *  Sets the comment of this property.
         *
         *  Comments, if the parser implements them, are a useful feature to describe
         *  what a property in the config file does.
         *
         *  @param comment The comment to set
         */
        void setComment(juce::String comment) noexcept;
        
        /**
         *  Sets a new value for this property if the value is not the same.
         *
         *  @param value  The new value of the property
         *  @param notify Whether to send the change to all listeners if the value has changed
         *  @return True if the new value was different and has changed, otherwise false
         *
         *  @throws jaut::ConfigIncompatibleTypeException If strict typing is enabled and the type of this property
         *                                                and the new value mismatched
         */
        bool setValue(const juce::var &value, juce::NotificationType notify = juce::sendNotification);
        
        //==============================================================================================================
        /**
         *  Resets the properties current value, and alternatively its children, to default.
         *
         *  @param recurse If this is true, all child properties will be reset too
         *  @param notify  Whether to send the change to all listeners if the value has changed
         */
        void reset(bool recurse, juce::NotificationType notify = juce::sendNotification);
    
        //==============================================================================================================
        /**
         *  Gets the PropertyType of this property object.
         *  @return jaut::Config::Property::PropertyType::Map for objects that contain child-Property objects,
         *          jaut::Config::Property::PropertyType::Value otherwise
         */
        JAUT_NODISCARD
        PropertyType getPropertyType() const noexcept;
        
        /**
         *  Gets whether this Property object is a map property.
         *  @return True if this Property objects is a map property
         */
        JAUT_NODISCARD
        bool isMapProperty() const noexcept;
    
        /**
         *  Gets whether this Property object is a value property.
         *  @return True if this Property objects is a value property
         */
        JAUT_NODISCARD
        bool isValueProperty() const noexcept;
        
        /**
         *  Gets whether this Property object is a category property.
         *  @return True if this Property objects is a category property
         */
        JAUT_NODISCARD
        bool isCategory() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets this property's value as string.
         *  @returns The string representation of the property's value
         */
        JAUT_NODISCARD
        juce::String toString() const;
        
        //==============================================================================================================
        JAUT_NODISCARD
        Iterator begin() noexcept;
        
        JAUT_NODISCARD
        Iterator end() noexcept;
        
        JAUT_NODISCARD
        ConstIterator begin() const noexcept;
        
        JAUT_NODISCARD
        ConstIterator end() const noexcept;
        
        JAUT_NODISCARD
        ConstIterator cbegin() const noexcept;
        
        JAUT_NODISCARD
        ConstIterator cend() const noexcept;
    
    private:
        friend Config::PropertyBuilder;
        
        //==============================================================================================================
        template<class T>
        static T* getProperty_impl(T &property, const juce::String &id);
        
        template<class T>
        static T* findProperty_impl(T &property, const juce::StringArray &hierarchy, int index);
        
        //==============================================================================================================
        ChildMap properties;
        
        juce::var    defaultValue;
        juce::var    value;
        juce::String id;
        juce::String comment;
        
        const Config   &config;
        const Property *parent;
        
        //==============================================================================================================
        void postPropertyAdded(const Property &property);
        void postValueChanged(const juce::var &oldValue);
        
        //==============================================================================================================
        bool modifyValue(const juce::var &newValue, juce::NotificationType notify);
        
        //==============================================================================================================
        void recurseReset(juce::NotificationType notify);
        void recurseResolveHierarchy(juce::StringArray &names) const;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Property)
    };
}
