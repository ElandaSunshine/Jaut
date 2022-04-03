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
    @file   config.cpp
    @date   04, May 2019
    
    ===============================================================
 */

namespace jaut
{
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
juce::var emptyValue;

inline juce::String fixPathString(juce::String currentPath)
{
    if (currentPath.isEmpty())
    {
        return "";
    }

    if (currentPath.containsChar('\\'))
    {
        currentPath = currentPath.replaceCharacters("\\", "/");

        if (currentPath.contains("//"))
        {
            juce::CharPointer_UTF8 char_ptr = currentPath.getCharPointer();
            juce::String new_uri;

            for (;;)
            {
                const juce::juce_wchar curr_char = char_ptr.getAndAdvance();

                if (curr_char == 0)
                {
                    break;
                }

                if (curr_char == '/' && *char_ptr == '/')
                {
                    continue;
                }

                new_uri += curr_char;
            }

            currentPath = new_uri;
        }
    }

    return currentPath;
}

inline juce::String trimAndValidate(const juce::String &name, bool periodCheck)
{
    const juce::String trimmed_name = name.removeCharacters(" ");

    // Always make sure the property's name is set!
    jassert(!trimmed_name.isEmpty());

    // Properties can't have the name "value" as this may break internal routines.
    jassert(!trimmed_name.equalsIgnoreCase("value"));

    if(periodCheck)
    {
        // Properties can't have dots in their names, as these are used to resolve child properties.
        jassert(!trimmed_name.contains("."));
    }

    return name;
}

inline juce::String getCategory(const jaut::Config *config, const juce::String &category)
{
    return category.isEmpty() ? config->getOptions().defaultCategory : category.removeCharacters(" ").toLowerCase();
}
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Property
//======================================================================================================================
struct Config::Property::PropertyData final
{
    // Main
    juce::String name;
    juce::var    defaultValue;
    juce::var    value;
    
    // References
    Config   *config;
    Property *parent;
    
    // Optional
    juce::String comment;
    ChildMap properties;
    
    //==================================================================================================================
    PropertyData(juce::String name, juce::var defaultValue)
        : name(std::move(name)),
          defaultValue(defaultValue),
          value(std::move(defaultValue)),
          config(nullptr), parent(nullptr)
    {}
};

//======================================================================================================================
Config::Property::Property(const juce::String &name, const juce::var &defaultValue)
    : data(std::make_unique<PropertyData>(jaut::trimAndValidate(name, true), defaultValue))
{}

Config::Property::Property(const Property &other) noexcept
    : data(other.data)
{}

Config::Property::Property(Property &&other) noexcept
    : Property()
{
    swap(*this, other);
}

Config::Property::~Property() = default;

//======================================================================================================================
Config::Property &Config::Property::operator=(const Property &right) noexcept
{
    Property temp(right);
    swap(*this, temp);
    return *this;
}

Config::Property &Config::Property::operator=(Property &&right) noexcept
{
    Property temp(std::move(right));
    swap(*this, temp);
    return *this;
}

//======================================================================================================================
Config::Property Config::Property::operator[](const juce::String &name)
{
    if (isValid())
    {
        const juce::String trimmed_name = jaut::trimAndValidate(name, false);
        
        if (trimmed_name.contains("."))
        {
            if (trimmed_name.endsWithChar('.') || trimmed_name.startsWithChar('.') || trimmed_name.contains(".."))
            {
                return {};
            }
            
            juce::StringArray property_names;
            property_names.addTokens(name, ".", "\"");
            
            if (property_names.isEmpty())
            {
                return {};
            }
            
            for (auto &prop_name : property_names)
            {
                if (prop_name.length() < 3 || prop_name.equalsIgnoreCase("value"))
                {
                    return {};
                }
            }
            
            return recurseChilds(property_names, true, juce::sendNotification);
        }

        const juce::String property_id = trimmed_name.toLowerCase();
        const auto property_it         = data->properties.find(property_id);
        Property property              = property_it->second;
        
        if (property_it == data->properties.end())
        {
            property = Property(name, {});
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(trimmed_name.toLowerCase(), property);
    
            PropertyAdded(property);
                
            if (data->config)
            {
                data->config->PropertyAdded(property);
            }
        }
        
        return property;
    }

    return {};
}

//======================================================================================================================
Config::Property Config::Property::createProperty(const juce::String &name, const juce::var &defaultValue,
                                                  juce::NotificationType notification)
{
    if (isValid())
    {
        const juce::String trimmed_name = jaut::trimAndValidate(name, true);
        const juce::String id           = trimmed_name.toLowerCase();
        auto it                         = data->properties.find(id);
        Property property               = it->second;
        
        if(it == data->properties.end())
        {
            property = Property(name, defaultValue);
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(id, property);
    
            if (notification != juce::dontSendNotification)
            {
                PropertyAdded(property);
    
                if (data->config)
                {
                    data->config->PropertyAdded(property);
                }
            }
        }
    
        return property;
    }
    
    return {};
}

OperationResult Config::Property::addProperty(Property property, juce::NotificationType notification)
{
    if (isValid() && property.isValid())
    {
        if(property.data->config)
        {
            return ErrorCodes::PropertyOwned;
        }
        
        const juce::String id = property.getName().toLowerCase();
        
        if (data->properties.find(id) == data->properties.end())
        {
            property.data->parent = this;
            property.setConfig(data->config);
            data->properties.emplace(id, property);
            
            if (notification != juce::dontSendNotification)
            {
                PropertyAdded(property);
    
                if (data->config)
                {
                    data->config->PropertyAdded(property);
                }
            }
            
            return true;
        }
        
        return ErrorCodes::PropertyExists;
    }
    
    return ErrorCodes::PropertyInvalid;
}

Config::Property Config::Property::getProperty(const juce::String &name)
{
    if (isValid())
    {
        const juce::String id = jaut::trimAndValidate(name, false).toLowerCase();
        
        if(id.contains("."))
        {
            if(id.endsWithChar('.') || id.startsWithChar('.') || id.contains(".."))
            {
                return {};
            }
            
            juce::StringArray property_names;
            property_names.addTokens(name, ".", "\"");
            
            if(property_names.isEmpty())
            {
                return {};
            }
            
            for (auto &prop_name : property_names)
            {
                if(prop_name.length() < 3 || prop_name.equalsIgnoreCase("value"))
                {
                    return {};
                }
            }
            
            return recurseChilds(property_names, false, juce::dontSendNotification);
        }
        
        const auto it = data->properties.find(id);
        
        if(it != data->properties.end())
        {
            return it->second;
        }
    }
    
    return {};
}

Config::ConstProperty Config::Property::getProperty(const juce::String &name) const
{
    if (isValid())
    {
        const juce::String id = jaut::trimAndValidate(name, false).toLowerCase();
        
        if(id.contains("."))
        {
            if(id.endsWithChar('.') || id.startsWithChar('.') || id.contains(".."))
            {
                return ConstProperty();
            }
            
            juce::StringArray property_names;
            property_names.addTokens(name, ".", "\"");
            
            if(property_names.isEmpty())
            {
                return ConstProperty();
            }
            
            for (auto &prop_name : property_names)
            {
                if(prop_name.length() < 3 || prop_name.equalsIgnoreCase("value"))
                {
                    return ConstProperty();
                }
            }
            
            return recurseChilds(property_names);
        }
        
        const auto it = data->properties.find(id);
        
        if(it != data->properties.end())
        {
            return it->second;
        }
    }
    
    return {};
}

//======================================================================================================================
int Config::Property::size() const noexcept
{
    return isValid() ? static_cast<int>(data->properties.size()) : 0;
}

bool Config::Property::hasValid() const noexcept
{
    if (isValid())
    {
        for (auto &[_, value] : data->properties)
        {
            if (value.isValid())
            {
                return true;
            }
        }
    }

    return false;
}

bool Config::Property::isValid() const noexcept
{
    return data != nullptr;
}

bool Config::Property::hasProperty(const juce::String &name) const
{
    const juce::String id = jaut::trimAndValidate(name, true).toLowerCase();
    return data->properties.find(id) != data->properties.end();
}

//======================================================================================================================
juce::String Config::Property::getName() const noexcept
{
    return isValid() ? data->name : juce::String();
}

juce::String Config::Property::getTopCategory() const noexcept
{
    if (isValid())
    {
        if(data->config)
        {
            for (auto &[category_name, category] : data->config->categories)
            {
                for (auto &[_, property] : category.properties)
                {
                    if (&property == this)
                    {
                        return category_name;
                    }
                }
            }
        }
    }
    
    return juce::String();
}

juce::String Config::Property::getAbsoluteName() const
{
    if (isValid())
    {
        const Property *this_parent = this;
        juce::StringArray full_path(data->name);
        
        while ((this_parent = this_parent->data->parent))
        {
            full_path.insert(0, this_parent->data->name);
        }
        
        if (data->config)
        {
            full_path.insert(0, getTopCategory());
        }
        
        return full_path.joinIntoString(".");
    }
    
    return juce::String();
}

juce::String Config::Property::getComment() const noexcept
{
    return isValid() ? data->comment : juce::String();
}

void Config::Property::setComment(const juce::String &commentText) noexcept
{
    if (isValid())
    {
        data->comment = commentText;
    }
}

void Config::Property::setValue(const juce::var &newValue, juce::NotificationType notification) noexcept
{
    if (isValid() && data->value != newValue)
    {
        juce::var &value = data->value;
        
        /**
         *  Always make sure that the value you are passing has the same object type than the stored value.
         *  This design decision makes type conversion a lot easier to maintain and thus, helps in parsing with multiple
         *  configuration languages.
         */
        jassert(value.hasSameTypeAs(newValue) || value.isVoid() || newValue.isVoid());
        
        const juce::var old_value = std::exchange(value, newValue);
        
        if (notification != juce::dontSendNotification)
        {
            ValueChanged(data->name, old_value, value);
    
            if (data->config)
            {
                data->config->ValueChanged(getAbsoluteName(), old_value, value);
            }
        }
    }
}

const juce::var &Config::Property::getValue() const noexcept
{
    return isValid() ? data->value : jaut::emptyValue;
}

void Config::Property::reset(bool recursive, juce::NotificationType notification) noexcept
{
    if(isValid())
    {
        setValue(data->defaultValue, notification);
    
        if(recursive)
        {
            for(auto &[_, property] : *this)
            {
                property.reset(true, notification);
            }
        }
    }
}

//======================================================================================================================
juce::String Config::Property::toString() const noexcept
{
    return getValue().toString();
}

//======================================================================================================================
Config::Property::Iterator Config::Property::begin() noexcept
{
    return isValid() ? data->properties.begin() : Iterator();
}

Config::Property::Iterator Config::Property::end() noexcept
{
    return isValid() ? data->properties.end() : Iterator();
}

Config::Property::ConstIterator Config::Property::begin() const noexcept
{
    return isValid() ? data->properties.begin() : ConstIterator();
}

Config::Property::ConstIterator Config::Property::end() const noexcept
{
    return isValid() ? data->properties.end() : ConstIterator();
}

Config::Property::ConstIterator Config::Property::cbegin() const noexcept
{
    return isValid() ? data->properties.begin() : ConstIterator();
}

Config::Property::ConstIterator Config::Property::cend() const noexcept
{
    return isValid() ? data->properties.end() : ConstIterator();
}

//======================================================================================================================
void Config::Property::setConfig(Config *config)
{
    if(isValid())
    {
        data->config = config;
        
        for (auto &[_, property] : data->properties)
        {
            property.setConfig(config);
        }
    }
}

//======================================================================================================================
Config::Property Config::Property::recurseChilds(juce::StringArray &names, bool add,
                                                 juce::NotificationType notification)
{
    if (!names.isEmpty())
    {
        const juce::String name = names.strings.getReference(0);
        const juce::String id   = name.toLowerCase();
        auto it = data->properties.find(id);
        names.removeRange(0, 1);
        
        if (it != data->properties.end())
        {
            return it->second.recurseChilds(names, add, notification);
        }
        else if (add)
        {
            Property property(name, {});
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(id, property);
    
            if (notification != juce::dontSendNotification)
            {
                PropertyAdded(property);
    
                if (data->config)
                {
                    data->config->PropertyAdded(property);
                }
            }
            
            return property.recurseChilds(names, add, notification);
        }
        
        return {};
    }
    
    return *this;
}

Config::ConstProperty Config::Property::recurseChilds(juce::StringArray &names) const
{
    if (!names.isEmpty())
    {
        const juce::String name = names.strings.getReference(0);
        const juce::String id   = name.toLowerCase();
        const auto it     = data->properties.find(id);
        names.removeRange(0, 1);
    
        if (it != data->properties.end())
        {
            return it->second.recurseChilds(names);
        }
        
        return {};
    }
    
    return *this;
}
//======================================================================================================================
// endregion Property
//**********************************************************************************************************************
// region Config
//======================================================================================================================
Config::Config(const juce::String &root, Options options, std::unique_ptr<ConfigParserType> configParser)
    : options(options),
      fullPath(jaut::fixPathString(root + "/" + options.fileName)),
      parser(std::move(configParser))
{
    const juce::File file = fullPath;
    
    /*
     * If this error occurs then you know that
     * you need to make sure the parent of the
     * file exists.
     * (Where should you save the config else?)
     *
     * Note:
     * Also, make sure that the parent directory of
     * the path never changes, as this will invalidate
     * your config if not handled properly.
     */
    jassert(file.getParentDirectory().exists());
    
    /*
     * Don't save this file as a directory.
     * This means that you probably didn't add any
     * valid file extension to the config's filepath.
     */
    jassert(!file.isDirectory());
    
    /*
     * The config can't work without a valid parser, make sure to supply one!
     */
    jassert(parser != nullptr);
    
    options.defaultCategory = options.defaultCategory.removeCharacters(" ").toLowerCase();
    ipMutex = options.processSynced ? std::make_unique<juce::InterProcessLock>(fullPath) : nullptr;
    (void) categories[options.defaultCategory.trim().toLowerCase()];
}

Config::Config(Config &&other) noexcept
{
    swap(*this, other);
    
    for(auto &[_, category] : categories)
    {
        for(auto &[_, property] : category.properties)
        {
            property.setConfig(this);
        }
    }
}

Config::~Config()
{
    for(auto &[_, category] : categories)
    {
        for(auto &[_, property] : category.properties)
        {
            property.setConfig(nullptr);
        }
    }
}

//======================================================================================================================
Config &Config::operator=(Config &&other) noexcept
{
    Config temp(std::move(other));
    swap(*this, temp);
    
    for(auto &[_, category] : categories)
    {
        for(auto &[_, property] : category.properties)
        {
            property.setConfig(this);
        }
    }

    return *this;
}

//======================================================================================================================
OperationResult Config::load()
{
    if (!parser)
    {
        return ErrorCodes::InvalidParser;
    }
    
    const juce::File config = fullPath;

    if (config.exists())
    {
        // Try to lock for no other threads and/or processes to modify the file at the same time!
        ProcessLockGuardPtr lock(createIpcLock());

        if (lock && !lock->isLocked())
        {
            return ErrorCodes::FileIsOpen;
        }

        return parser->parseConfig(config, getAllProperties());
    }

    return ErrorCodes::FileNotFound;
}

OperationResult Config::save() const
{
    if (!parser)
    {
        return ErrorCodes::InvalidParser;
    }
    
    const juce::File config = fullPath;

    if (config.getParentDirectory().exists())
    {
        // Try to lock for no other threads and/or processes to modify the file at the same time!
        ProcessLockGuardPtr lock(createIpcLock());

        if (lock && !lock->isLocked())
        {
            return ErrorCodes::FileIsOpen;
        }

        // Saving the config file
        Property root = getAllPropertiesPseudoConst();
        root.setComment(options.configNotice);

        return parser->writeConfig(config, root);
    }
    
    return ErrorCodes::FileNotFound;
}

//======================================================================================================================
juce::String Config::getConfigName(bool withoutExtension) const
{
    const juce::File config = fullPath;
    
    if (config.getParentDirectory().exists())
    {
        return withoutExtension ? config.getFileNameWithoutExtension() : config.getFileName();
    }
    
    return {};
}

Config::Property Config::getProperty(const juce::String &name, const juce::String &category)
{
    const juce::String category_name = jaut::getCategory(this, category);
    
    if (categories.find(category_name) != categories.end())
    {
        PropertyMap &property_map = categories.at(category_name).properties;
        
        if (name.contains(".") && !name.startsWith("."))
        {
            const auto it = property_map.find(name.upToFirstOccurrenceOf(".", false, false).toLowerCase());
        
            if (it != property_map.end())
            {
                return it->second.getProperty(name.fromFirstOccurrenceOf(".", false, false));
            }
        }
        
        const juce::String property_id = name.trim().toLowerCase();
        const auto it = property_map.find(property_id);
        
        if (it != property_map.end())
        {
            return it->second;
        }
    }
    
    return {};
}

Config::ConstProperty Config::getProperty(const juce::String &name, const juce::String &category) const
{
    const juce::String category_name = jaut::getCategory(this, category);
    
    if (categories.find(category_name) != categories.end())
    {
        const PropertyMap &property_map = categories.at(category_name).properties;
    
        if (name.contains(".") && !name.startsWith("."))
        {
            const auto it = property_map.find(name.upToFirstOccurrenceOf(".", false, false).toLowerCase());
            
            if (it != property_map.end())
            {
                return it->second.getProperty(name.fromFirstOccurrenceOf(".", false, false));
            }
        }
    
        const juce::String property_id = name.trim().toLowerCase();
        const auto it = property_map.find(property_id);
        
        if (it != property_map.end())
        {
            return it->second;
        }
    }
    
    return {};
}

Config::Property Config::getAllProperties()
{
    Property root("config", juce::var());

    for (auto &[category_name, category] : categories)
    {
        Property category_property(category_name, juce::var());
        category_property.setComment(category.comment);
        category_property.data->properties = category.properties;
        root.addProperty(category_property);
    }

    return root;
}

Config::ConstProperty Config::getAllProperties() const
{
    Property root("config", juce::var());
    
    for (const auto &[category_name, category] : categories)
    {
        Property category_property(category_name, juce::var());
        category_property.setComment(category.comment);
        category_property.data->properties = category.properties;
        root.addProperty(category_property);
    }
    
    return root;
}

juce::String Config::getCategoryComment(const juce::String &category) const
{
    const juce::String category_name = jaut::getCategory(this, category);
    const auto it              = categories.find(category_name);
    
    if (it != categories.end())
    {
        return it->second.comment;
    }

    return {};
}

void Config::setCategoryComment(const juce::String &category, const juce::String &comment)
{
    const juce::String category_name = jaut::getCategory(this, category);
    
    if (category_name != options.defaultCategory)
    {
        const auto it = categories.find(category_name);
        
        if (it != categories.end())
        {
            it->second.comment = comment;
        }
    }
}

const Config::ConfigParserType* Config::getParser() const noexcept
{
    return parser.get();
}

void Config::bypassAutosave(bool bypass) noexcept
{
    autoSaveActive = bypass;
}

const Config::Options &Config::getOptions() const noexcept
{
    return options;
}

juce::File Config::getConfigFile() const noexcept
{
    return fullPath;
}

//======================================================================================================================
Config::Property Config::createProperty(const juce::String &name, const juce::var &defaultValue,
                                        const juce::String &category) noexcept
{
    Property property(name, defaultValue);
    const juce::String category_name = jaut::getCategory(this, category);
    const juce::String property_id   = name.removeCharacters(" ").toLowerCase();
    const bool was_added             = categories[category_name].properties.emplace(property_id, property).second;

    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }
        
        property.setConfig(this);
        PropertyAdded(property);
        
        return property;
    }

    return categories.at(category_name).properties.at(property_id);
}

OperationResult Config::addProperty(Property property, const juce::String &category) noexcept
{
    if(property.isValid() && property.data->config)
    {
        return ErrorCodes::PropertyOwned;
    }
    
    const juce::String category_name = jaut::getCategory(this, category);
    const juce::String property_id   = property.getName().removeCharacters(" ").toLowerCase();
    const bool was_added             = categories[category_name].properties.emplace(property_id, property).second;
    
    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }
        
        property.setConfig(this);
        PropertyAdded(property);
        
        return true;
    }
    
    return ErrorCodes::PropertyExists;
}

void Config::resetCategory(const juce::String &category)
{
    const juce::String category_name = jaut::getCategory(this, category);
    const auto it = categories.find(category_name);
    
    if(it != categories.end())
    {
        for(auto &[_, property] : it->second.properties)
        {
            property.reset(true);
        }
    }
}

void Config::resetAll()
{
    for(auto &[_c, category] : categories)
    {
        for(auto &[_p, property] : category.properties)
        {
            property.reset(true);
        }
    }
}

//======================================================================================================================
bool Config::shouldAutosave() noexcept
{
    if (!autoSaveActive)
    {
        autoSaveActive = true;
        return false;
    }
    
    return options.autoSave;
}

Config::ProcessLockGuard* Config::createIpcLock() const
{
    return options.processSynced ? new ProcessLockGuard(*ipMutex) : nullptr;
}

Config::Property Config::getAllPropertiesPseudoConst() const
{
    Property root("config", juce::var());
    
    for (auto &[category_name, category] : categories)
    {
        Property category_property(category_name, juce::var());
        category_property.setComment(category.comment);
        category_property.data->properties = category.properties;
        root.addProperty(category_property);
    }
    
    return root;
}
//======================================================================================================================
// endregion Config
//**********************************************************************************************************************
}
