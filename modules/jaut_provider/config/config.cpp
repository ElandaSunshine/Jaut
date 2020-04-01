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
    @file   config.cpp
    @date   04, May 2019
    
    ===============================================================
 */

// region Namespace
namespace
{
var emptyValue;

inline String fixPathString(String currentPath)
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
            CharPointer_UTF8 char_ptr = currentPath.getCharPointer();
            juce_wchar curr_char; // NOLINT
            String new_uri;

            for (;;)
            {
                curr_char = char_ptr.getAndAdvance();

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

inline String trimAndValidate(const String &name, bool periodCheck)
{
    const String trimmed_name = name.removeCharacters(" ");

    /**
     *  Always make sure the property's name is set!
     */
    jassert(!trimmed_name.isEmpty());

    /**
     *  Properties can't have the name "value" as this may break internal routines.
     */
    jassert(!trimmed_name.equalsIgnoreCase("value"));

    if(periodCheck)
    {
        /**
         *  Properties can't have dots in their names, as these are used to resolve child properties.
         */
        jassert(!trimmed_name.contains("."));
    }

    return name;
}

inline String getCategory(const jaut::Config *config, const String &category)
{
    return category.isEmpty() ? config->getOptions().defaultCategory : category.removeCharacters(" ").toLowerCase();
}
}
// endregion Namespace

namespace jaut
{
// region Property
/* ==================================================================================
 * ================================== Property ======================================
 * ================================================================================== */
struct Config::Property::PropertyData final
{
    // Main
    String name;
    var    defaultValue;
    var    value;
    
    // References
    Config   *config;
    Property *parent;
    
    // Optional
    String   comment;
    ChildMap properties;
    
    //==================================================================================================================
    PropertyData(String name, var defaultValue)
        : name(std::move(name)),
          defaultValue(defaultValue),
          value(std::move(defaultValue)),
          config(nullptr), parent(nullptr)
    {}
};

//======================================================================================================================
Config::Property::Property(const String &name, const var &defaultValue)
    : data(std::make_unique<PropertyData>(::trimAndValidate(name, true), defaultValue))
{}

Config::Property::Property(const Property &other) noexcept
    : data(other.data)
{}

Config::Property::Property(Property &&other) noexcept
    : data(std::move(other.data))
{
    other.data = nullptr;
}

Config::Property::~Property()
{
    setConfig(nullptr);
}

//======================================================================================================================
Config::Property &Config::Property::operator=(const Property &right) noexcept
{
    Property temp(right);
    swap(*this, temp);
    return *this;
}

Config::Property &Config::Property::operator=(Property &&right) noexcept
{
    swap(*this, right);
    right.data = nullptr;
    return *this;
}

//======================================================================================================================
Config::Property Config::Property::operator[](const String &name)
{
    if (isValid())
    {
        const String trimmed_name = ::trimAndValidate(name, false);
        
        if (trimmed_name.contains("."))
        {
            if (trimmed_name.endsWithChar('.') || trimmed_name.startsWithChar('.') || trimmed_name.contains(".."))
            {
                return {};
            }
            
            StringArray property_names;
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
            
            return recurseChilds(property_names, sendNotification);
        }

        const String property_id = trimmed_name.toLowerCase();
        const auto property_it   = data->properties.find(property_id);
        Property property        = property_it->second;
        
        if (property_it == data->properties.end())
        {
            property = Property(name, {});
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(trimmed_name.toLowerCase(), property);
    
            {
                PropertyAddedHandler handler = PropertyAdded;
                handler(property);
                
                if (data->config)
                {
                    PropertyAddedHandler cfg_handler = data->config->PropertyAdded;
                    cfg_handler(property);
                }
            }
        }
        
        return property;
    }

    return {};
}

//======================================================================================================================
Config::Property Config::Property::createProperty(const String &name, const var &defaultValue,
                                                  NotificationType notification)
{
    if (isValid())
    {
        const String trimmed_name = ::trimAndValidate(name, true);
        const String id           = trimmed_name.toLowerCase();
        const auto it             = data->properties.find(id);
        Property property         = it->second;
        
        if(it == data->properties.end())
        {
            property = Property(name, {});
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(id, property);
    
            if (notification != dontSendNotification)
            {
                PropertyAddedHandler handler = PropertyAdded;
                handler(property);
        
                if (data->config)
                {
                    PropertyAddedHandler cfg_handler = data->config->PropertyAdded;
                    cfg_handler(property);
                }
            }
        }
        
        return property;
    }
    
    return {};
}

OperationResult Config::Property::addProperty(Property property, NotificationType notification)
{
    if (isValid() && property.isValid())
    {
        if(property.data->config)
        {
            return ErrorCodes::PropertyOwned;
        }
        
        const String id = property.getName().toLowerCase();
        
        if (data->properties.find(id) == data->properties.end())
        {
            property.data->parent = this;
            property.setConfig(data->config);
            data->properties.emplace(id, property);
            
            if (notification != dontSendNotification)
            {
                PropertyAddedHandler handler = PropertyAdded;
                handler(property);
                
                if (data->config)
                {
                    PropertyAddedHandler cfg_handler = data->config->PropertyAdded;
                    cfg_handler(property);
                }
            }
            
            return true;
        }
        
        return ErrorCodes::PropertyExists;
    }
    
    return ErrorCodes::PropertyInvalid;
}

Config::Property Config::Property::getProperty(const String &name)
{
    if (isValid())
    {
        const String id = ::trimAndValidate(name, false).toLowerCase();
        
        if(id.contains("."))
        {
            if(id.endsWithChar('.') || id.startsWithChar('.') || id.contains(".."))
            {
                return {};
            }
            
            StringArray property_names;
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

const Config::Property Config::Property::getProperty(const String &name) const
{
    if (isValid())
    {
        const String id = ::trimAndValidate(name, false).toLowerCase();
        
        if(id.contains("."))
        {
            if(id.endsWithChar('.') || id.startsWithChar('.') || id.contains(".."))
            {
                return {};
            }
            
            StringArray property_names;
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

bool Config::Property::hasProperty(const String &name) const
{
    const String id = ::trimAndValidate(name, true).toLowerCase();
    return data->properties.find(id) != data->properties.end();
}

//======================================================================================================================
String Config::Property::getName() const noexcept
{
    return isValid() ? data->name : String();
}

String Config::Property::getTopCategory() const noexcept
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
    
    return String();
}

String Config::Property::getAbsoluteName() const
{
    if (isValid())
    {
        const Property *this_parent = this;
        StringArray full_path(data->name);
        
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
    
    return String();
}

String Config::Property::getComment() const noexcept
{
    return isValid() ? data->comment : String();
}

void Config::Property::setComment(const String &commentText) noexcept
{
    if (isValid())
    {
        data->comment = commentText;
    }
}

void Config::Property::setValue(const var &newValue, NotificationType notification) noexcept
{
    if (isValid() && data->value != newValue)
    {
        var &value = data->value;
        
        /**
         *  Always make sure that the value you are passing has the same object type than the stored value.
         *  This design decision makes type conversion a lot easier to maintain and thus, helps in parsing with multiple
         *  configuration languages.
         */
        jassert(value.hasSameTypeAs(newValue) || value.isVoid() || newValue.isVoid());
        
        const var old_value = std::exchange(value, newValue);
        
        if (notification != dontSendNotification)
        {
            ValueChangedHandler handler = ValueChanged;
            handler(data->name, old_value, value);
            
            if (data->config)
            {
                ValueChangedHandler cfg_handler = data->config->ValueChanged;
                cfg_handler(getAbsoluteName(), old_value, value);
            }
        }
    }
}

const var &Config::Property::getValue() const noexcept
{
    return isValid() ? data->value : ::emptyValue;
}

void Config::Property::reset(bool recursive, NotificationType notification) noexcept
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
String Config::Property::toString() const noexcept
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
Config::Property Config::Property::recurseChilds(StringArray &names, NotificationType notification)
{
    if (!names.isEmpty())
    {
        const String name = names.strings.getReference(0);
        const String id   = name.toLowerCase();
        auto it = data->properties.find(id);
        names.removeRange(0, 1);
        
        if (it != data->properties.end())
        {
            return it->second.recurseChilds(names, notification);
        }
        else
        {
            Property property(name, {});
            property.data->parent = this;
            property.data->config = data->config;
            data->properties.emplace(id, property);
    
            if (notification != dontSendNotification)
            {
                PropertyAddedHandler handler = PropertyAdded;
                handler(property);
                
                if (data->config)
                {
                    PropertyAddedHandler cfg_handler = data->config->PropertyAdded;
                    cfg_handler(property);
                }
            }
            
            return property.recurseChilds(names, notification);
        }
    }
    
    return *this;
}

const Config::Property Config::Property::recurseChilds(StringArray &names) const
{
    if (!names.isEmpty())
    {
        const String name = names.strings.getReference(0);
        const String id   = name.toLowerCase();
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
// endregion Property
// region Config
/* ==================================================================================
 * =================================== Config =======================================
 * ================================================================================== */

Config::Config(const String &root, Options options, std::unique_ptr<ConfigParserType> configParser)
    : autoSaveActive(true), fullPath(std::move(::fixPathString(root + "/" + options.fileName))),
      options(std::move(options)), parser(std::move(configParser))
{
    File file(fullPath);
    
    /*
       If this error occurs then you know that
       you need to make sure the parent of the
       file exists.
       (Where should you save the config else?)

       Note:
       Also, make sure that the parent directory of
       the path never changes, as this will invalidate
       your config if not handled properly.
    */
    jassert(file.getParentDirectory().exists());
    
    /*
       Don't save this file as a directory.
       This means that you probably didn't add any
       valid file extension to the config's filepath.
    */
    jassert(!file.isDirectory());
    
    /*
     * The config can't work without a valid parser, make sure to supply one!
     */
    jassert(parser != nullptr);
    
    options.defaultCategory = options.defaultCategory.removeCharacters(" ").toLowerCase();
    ipMutex = options.processSynced ? std::make_unique<InterProcessLock>(fullPath) : nullptr;
    (void) categories[options.defaultCategory.trim().toLowerCase()];
}

Config::Config(Config &&other) noexcept
    : autoSaveActive(other.autoSaveActive),
      categories(std::move(other.categories)),
      fullPath(std::move(other.fullPath)),
      ipMutex(std::move(other.ipMutex)),
      parser(std::move(other.parser)),
      options(std::move(other.options))
{
    for(auto &[_, category] : categories)
    {
        for(auto &[_, property] : category.properties)
        {
            property.setConfig(this);
        }
    }
    
    other.parser  = nullptr;
    other.ipMutex = nullptr;
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
    swap(*this, other);
    other.parser  = nullptr;
    other.ipMutex = nullptr;
    
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
    
    const File config(fullPath);

    if (config.exists())
    {
        // Try to lock for no other threads and/or processes to modify the
        // file at the same time!
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
    
    const File config (fullPath);

    if (config.getParentDirectory().exists())
    {
        // Try to lock for no other threads and/or processes to modify the
        // file at the same time!
        ProcessLockGuardPtr lock(createIpcLock());

        if (lock && !lock->isLocked())
        {
            return ErrorCodes::FileIsOpen;
        }

        // Saving the config file
        Property root = getAllProperties();

        if (!options.configNotice.isEmpty())
        {
            root.setComment(options.configNotice);
        }

        return parser->writeConfig(config, root);
    }
    
    return ErrorCodes::FileNotFound;
}

//======================================================================================================================
String Config::getConfigName(bool withoutExtension) const
{
    const File config(fullPath);
    
    if (config.getParentDirectory().exists())
    {
        return withoutExtension ? config.getFileNameWithoutExtension() : config.getFileName();
    }

    return {};
}

Config::Property Config::getProperty(const String &name, const String &category)
{
    const String category_name = ::getCategory(this, category);
    
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
        
        const String property_id = name.trim().toLowerCase();
        const auto it            = property_map.find(property_id);
        
        if (it != property_map.end())
        {
            return it->second;
        }
    }
    
    return {};
}

const Config::Property Config::getProperty(const String &name, const String &category) const
{
    const String category_name = ::getCategory(this, category);
    
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
    
        const String property_id = name.trim().toLowerCase();
        const auto it            = property_map.find(property_id);
        
        if (it != property_map.end())
        {
            return it->second;
        }
    }
    
    return {};
}

Config::Property Config::getAllProperties()
{
    Property root("config", var());

    for (auto &[category_name, category] : categories)
    {
        Property category_property(category_name, var());
        category_property.setComment(category.comment);
        category_property.data->properties = category.properties;
        root.addProperty(category_property);
    }

    return root;
}

const Config::Property Config::getAllProperties() const
{
    Property root("config", var());
    
    for (const auto &[category_name, category] : categories)
    {
        Property category_property(category_name, var());
        category_property.setComment(category.comment);
        category_property.data->properties = category.properties;
        root.addProperty(category_property);
    }
    
    return root;
}

String Config::getCategoryComment(const String &category) const
{
    const String category_name = ::getCategory(this, category);
    const auto it              = categories.find(category_name);
    
    if (it != categories.end())
    {
        return it->second.comment;
    }

    return {};
}

void Config::setCategoryComment(const String &category, const String &comment)
{
    const String category_name = ::getCategory(this, category);
    
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

//======================================================================================================================
Config::Property Config::createProperty(const String &name, const var &defaultValue, const String &category) noexcept
{
    Property property(name, defaultValue);
    const String category_name = ::getCategory(this, category);
    const String property_id   = name.removeCharacters(" ").toLowerCase();
    const bool was_added       = categories[category_name].properties.emplace(property_id, property).second;

    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }
        
        property.setConfig(this);
        
        {
            PropertyAddedHandler handler = PropertyAdded;
            handler(property);
        }
        
        return property;
    }

    return categories.at(category_name).properties.at(property_id);
}

OperationResult Config::addProperty(Property property, const String &category) noexcept
{
    if(property.isValid() && property.data->config)
    {
        return ErrorCodes::PropertyOwned;
    }
    
    const String category_name = ::getCategory(this, category);
    const String property_id   = property.getName().removeCharacters(" ").toLowerCase();
    const bool was_added       = categories[category_name].properties.emplace(property_id, property).second;
    
    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }
        
        property.setConfig(this);
        
        {
            PropertyAddedHandler handler = PropertyAdded;
            handler(property);
        }
        
        return true;
    }
    
    return ErrorCodes::PropertyExists;
}

void Config::resetCategory(const String &category) noexcept
{
    const String category_name = ::getCategory(this, category);
    const auto it              = categories.find(category_name);
    
    if(it != categories.end())
    {
        for(auto &[_, property] : it->second.properties)
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
#pragma endregion Config
}
