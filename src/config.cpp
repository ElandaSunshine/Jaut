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

#include <jaut/config.h>
#include <jaut/iconfigparser.h>

namespace jaut
{
namespace
{
const String fixPathString(const String &currentPath) noexcept
{
    String newpath = currentPath;

    if (currentPath.isEmpty())
    {
        return "";
    }

    if (currentPath.containsChar('\\'))
    {
        newpath = currentPath.replaceCharacters("\\", "/");

        if (newpath.contains("//"))
        {
            CharPointer_UTF8 charptr = newpath.getCharPointer();
            juce_wchar currchar;
            String newuri;

            for (;;)
            {
                currchar = charptr.getAndAdvance();

                if (currchar == 0)
                {
                    break;
                }

                if (currchar == '/' && *charptr == '/')
                {
                    continue;
                }

                newuri += currchar;
            }

            newpath = newuri;
        }
    }

    return newpath;
}
}



/* ==================================================================================
 * ================================== Property ======================================
 * ================================================================================== */
#pragma region Property
struct Config::Property::SharedObject
{
    String comment;
    var defaultValue;
    String name;
    t_ChildMap properties;
    Property *parent;
    var value;
    Config *eventConfig;
    ListenerList<Listener> listeners;

    //==================================================================================================================
    SharedObject(const String &name, const var &defaultValue) noexcept
        : defaultValue(defaultValue), name(name), value(defaultValue),
          parent(nullptr), eventConfig(nullptr)
    {}
};



//======================================================================================================================
Config::Property::Property(const String &name, const var &defaultValue)
    : data(new SharedObject(name.trim(), defaultValue))
{
    // Always make sure the Property object's name is set and valid!
    // Also, properties can't have the name "value" as this may break internal routines!
    jassert(!name.isEmpty() && !name.equalsIgnoreCase("value"));
}

Config::Property::~Property() noexcept {}

//======================================================================================================================
Config::Property Config::Property::operator[](const String &name) noexcept
{
    if (isValid())
    {
        return data->properties[name.trim().toLowerCase()];
    }

    return Property();
}

//======================================================================================================================
Config::Property Config::Property::createProperty(const String &name, const var &defaultValue) noexcept
{
    if (isValid())
    {
        Property prop(name, defaultValue);
        prop.data->parent = this;
        prop.addConfig(data->eventConfig);
        data->properties.emplace(name.trim().toLowerCase(), prop);
        postAddedListener(prop);

        return prop;
    }

    return Property();
}

void Config::Property::addProperty(Property prop) noexcept
{
    if (!isValid())
    {
        return;
    }

    String property_id = prop.getName().trim().toLowerCase();

    if (!hasProperty(property_id))
    {
        data->properties.emplace(property_id, prop);
        prop.data->parent = this;
        prop.addConfig(data->eventConfig);
        postAddedListener(prop);
    }
}

Config::Property Config::Property::getProperty(const String &name) const noexcept
{
    String property_id = name.trim().toLowerCase();

    if (isValid() && hasProperty(property_id))
    {
        return data->properties.at(property_id);
    }

    return Property();
}

//======================================================================================================================
const int Config::Property::size() const noexcept
{
    return isValid() ? static_cast<int>(data->properties.size()) : 0;
}

const bool Config::Property::hasValid() const noexcept
{
    if (isValid())
    {
        for (auto &[key, value] : data->properties)
        {
            if (value.isValid())
            {
                return true;
            }
        }
    }

    return false;
}

const bool Config::Property::isValid() const
{
    return data != nullptr;
}

const bool Config::Property::hasProperty(const String &name) const noexcept
{
    return isValid() && data->properties.find(name.trim().toLowerCase()) != data->properties.end();
}

//======================================================================================================================
const String Config::Property::getName() const noexcept
{
    return isValid() ? data->name : "";
}

const String Config::Property::getComment() const noexcept
{
    return isValid() ? data->comment : "";
}

void Config::Property::setComment(const String &comment) noexcept
{
    if (isValid())
    {
        data->comment = comment;
    }
}

void Config::Property::setValue(const var &value) noexcept
{
    if (isValid() && value != data->value)
    {
        var old_value = data->value;

        /**
            Always make sure that the value you are passing has the same object type than the stored value.
            This design decision makes type conversion a lot easier to maintain and thus, helps in
            parsing with multiple configuration languages.
         */
        jassert(old_value.hasSameTypeAs(value));

        data->value = value;
        postValueChangedListener(data->name, old_value, value);

        if (data->eventConfig)
        {
            String category;
            String name      = data->name;
            Property *parent = data->parent;

            if(parent)
            {
                for(;;)
                {
                    name = parent->data->name + "." + name;

                    if(!parent->data->parent)
                    {
                        break;
                    }

                    parent = parent->data->parent;
                }
            }
            
            for(auto &cat : data->eventConfig->categories)
            {
                if(cat.first.equalsIgnoreCase(parent->data->name))
                {
                    category = cat.first;
                    break;
                }
            }

            data->eventConfig->postValueChangedListener(category + "." + name, old_value, value);
        }
    }
}

const var &Config::Property::getValue() const noexcept
{
    static var empty;
    return isValid() ? data->value : empty;
}

void Config::Property::reset(bool recursive) noexcept
{
    if(!isValid())
    {
        return;
    }

    setValue(data->defaultValue);

    if(recursive)
    {
        for(auto &[key, property] : *this)
        {
            if(property.isValid())
            {
                property.reset(true);
            }
        }
    }
}

//======================================================================================================================
const String Config::Property::toString() const noexcept
{
    return getValue().toString();
}

//======================================================================================================================
Config::Property::t_Iterator Config::Property::begin() noexcept
{
    return !isValid() ? t_Iterator() : data->properties.begin();
}

Config::Property::t_Iterator Config::Property::end() noexcept
{
    return !isValid() ? t_Iterator() : data->properties.end();
}

Config::Property::t_CIterator Config::Property::begin() const noexcept
{
    return !isValid() ? t_CIterator() : data->properties.begin();
}

Config::Property::t_CIterator Config::Property::end() const noexcept
{
    return !isValid() ? t_CIterator() : data->properties.end();
}

Config::Property::t_CIterator Config::Property::cbegin() const noexcept
{
    return !isValid() ? t_CIterator() : data->properties.begin();
}

Config::Property::t_CIterator Config::Property::cend() const noexcept
{
    return !isValid() ? t_CIterator() : data->properties.end();
}

//======================================================================================================================
void Config::Property::addListener(Listener *listener) noexcept
{
    if(isValid())
    {
        data->listeners.add(listener);
    }
}

void Config::Property::removeListener(Listener *listener) noexcept
{
    if(isValid())
    {
        data->listeners.remove(listener);
    }
}

//======================================================================================================================
void Config::Property::addConfig(Config *config) noexcept
{
    if(isValid())
    {
        data->eventConfig = config;

        if(hasValid())
        {
            for(auto property : *this)
            {
                property.second.addConfig(config);
            }
        }
    }
}

void Config::Property::postAddedListener(Property prop) noexcept
{
    if(isValid())
    {
        data->listeners.call([=, &prop](Listener &l)
        {
            l.onPropertyAdded(prop);
        });
    }
}

void Config::Property::postValueChangedListener(const String &name, var oldValue, var newValue) noexcept
{
    if(isValid())
    {
        data->listeners.call([=, &name, &oldValue, &newValue](Listener &l)
        {
            l.onValueChanged(name, oldValue, newValue);
        });
    }
}
#pragma endregion Property



/* ==================================================================================
 * =================================== Config =======================================
 * ================================================================================== */
#pragma region Config
Config::Options::Options() noexcept
    : autoSave(false),
      processSynced(false),
      defaultCategory("general")
{}



//======================================================================================================================
Config::Config(const String &root, const Options &options, std::unique_ptr<IConfigParser> configParser)
    : autoSaveActive(true), lock(nullptr),
      parser(configParser.release(), [](IConfigParser *parser) { delete parser; }), options(options)
{
    const String file_path = fixPathString(root + "/" + options.fileName);
    File file(file_path);

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

    fullPath = file_path;
    ipMutex.reset(options.processSynced ? new InterProcessLock(file_path) : nullptr);
    (void) categories[options.defaultCategory.trim().toLowerCase()];
}

Config::Config(Config &&other) noexcept
    : autoSaveActive(std::move(other.autoSaveActive)),
      categories(std::move(other.categories)),
      comments(std::move(other.comments)),
      fullPath(std::move(other.fullPath)),
      ipMutex(std::move(other.ipMutex)),
      lock(std::move(other.lock)),
      parser(std::move(other.parser)),
      options(std::move(other.options))
{
    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }

    for(auto &[p0, category_map] : categories)
    {
        for(auto &[p1, property] : category_map)
        {
            property.addConfig(this);
        }
    }

    other.lock    = nullptr;
    other.parser  = nullptr;
    other.ipMutex = nullptr;
    other.listeners.clear();
}

Config::~Config()
{
    for(auto &[p0, category_map] : categories)
    {
        for(auto &[p1, property] : category_map)
        {
            property.addConfig(nullptr);
        }
    }
}

//======================================================================================================================
Config &Config::operator=(Config &&other) noexcept
{
    swap(*this, other);

    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }

    for(auto &[p0, category_map] : categories)
    {
        for(auto &[p1, property] : category_map)
        {
            property.addConfig(this);
        }
    }

    other.lock    = nullptr;
    other.parser  = nullptr;
    other.ipMutex = nullptr;
    other.listeners.clear();

    return *this;
}

//======================================================================================================================
const bool Config::load()
{
    File config(fullPath);

    if (config.exists())
    {
        // Try to lock for no other threads and/or processes to modify the
        // file at the same time!
        p_ProcessLock lock (createIpcLock());

        if (lock && !lock->isLocked())
        {
            return false;
        }

        return parser->parseConfig(config, getAllProperties());
    }

    return false;
}

const bool Config::save() const
{
    File config (fullPath);

    if (config.getParentDirectory().exists())
    {
        // Try to lock for no other threads and/or processes to modify the
        // file at the same time!
        p_ProcessLock lock (createIpcLock());

        if (lock && !lock->isLocked())
        {
            return false;
        }

        // Saving the config file
        Property root = getAllProperties();

        if (!options.configNotice.isEmpty())
        {
            root.setComment(options.configNotice);
        }

        return parser->writeConfig(config, root);
    }

    return false;
}

//======================================================================================================================
const String Config::getConfigName(bool withoutExtension) const noexcept
{
    File config(fullPath);

    if (config.getParentDirectory().exists())
    {
        return withoutExtension ? config.getFileNameWithoutExtension() : config.getFileName();
    }

    return "";
}

Config::Property Config::getProperty(const String &name, const String &category) const noexcept
{
    String category_name = !category.isEmpty() ? category.trim().toLowerCase()
                                               : options.defaultCategory.trim().toLowerCase();

    if (categories.find(category_name) != categories.end())
    {
        const t_PropertyMap &property_map = categories.at(category_name);
        String property_id = name.trim().toLowerCase();

        if (property_map.find(property_id) != property_map.end())
        {
            return property_map.at(property_id);
        }
    }

    return Property();
}

Config::Property Config::getAllProperties()
{
    Property root("config", var());

    for (auto &[name, category_map] : categories)
    {
        Property category(name, var());

        if (comments.find(name) != comments.end())
        {
            category.setComment(comments.at(name));
        }

        for (auto &[p0, property] : category_map)
        {
            category.addProperty(property);
        }

        root.addProperty(category);
    }

    return root;
}

const Config::Property Config::getAllProperties() const
{
    Property root("config", var());

    for (auto &[name, category_map] : categories)
    {
        Property category(name, var());

        if (comments.find(name) != comments.end())
        {
            category.setComment(comments.at(name));
        }

        for (auto &[p0, property] : category_map)
        {
            category.addProperty(property);
        }

        root.addProperty(category);
    }

    return root;
}

String Config::getCategoryComment(const String &category) const noexcept
{
    String category_name = !category.isEmpty() ? category.trim().toLowerCase()
                                               : options.defaultCategory.trim().toLowerCase();

    if (category_name != options.defaultCategory && comments.find(category_name) != comments.end())
    {
        return comments.at(category_name);
    }

    return "";
}

void Config::setCategoryComment(const String &category, const String &comment) noexcept
{
    String category_name = !category.isEmpty() ? category.trim().toLowerCase()
                                               : options.defaultCategory.trim().toLowerCase();

    if (category_name != options.defaultCategory && comments.find(category_name) != comments.end())
    {
        comments.at(category_name) = comment;
    }
}

const IConfigParser *Config::getParser() const noexcept
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
    Property property (name, defaultValue);
    String category_name = !category.isEmpty() ? category.trim().toLowerCase()
                                               : options.defaultCategory.trim().toLowerCase();
    String property_id = name.trim().toLowerCase();
    bool was_added     = categories[category_name].emplace(property_id, property).second;

    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }

        property.addConfig(this);
        postAddedListener(property);
    }

    return property;
}

void Config::addProperty(Property property, const String &category) noexcept
{
    if(property.isValid() && property.data->eventConfig)
    {
        return;
    }

    String category_name = !category.isEmpty() ? category.trim().toLowerCase()
                                               : options.defaultCategory.trim().toLowerCase();
    String property_id   = property.getName().trim().toLowerCase();
    bool was_added       = categories[category_name].emplace(property_id, property).second;

    if (was_added)
    {
        if (shouldAutosave())
        {
            (void) save();
        }

        property.addConfig(this);
        postAddedListener(property);
    }
}

void Config::resetCategory(const String &category) noexcept
{
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();
    auto it        = categories.find(catname);

    if(it != categories.end())
    {
        for(auto proppair : it->second)
        {
            proppair.second.reset(true);
        }
    }
}

//======================================================================================================================
void Config::addListener(Listener *listener) noexcept
{
    if (listener)
    {
        listeners.add(listener);
    }
}

void Config::removeListener(Listener *listener) noexcept
{
    listeners.remove(listener);
}

//======================================================================================================================
const bool Config::shouldAutosave() const noexcept
{
    return autoSaveActive && options.autoSave;
}

Config::t_ProcessLock *Config::createIpcLock() const
{
    return options.processSynced ? new t_ProcessLock(*ipMutex) : nullptr;
}

void Config::postAddedListener(Property prop) noexcept
{
    listeners.call([=, &prop](Listener &l)
    {
        l.onPropertyAdded(prop);
    });
}

void Config::postValueChangedListener(const String &name, var oldValue, var newValue) noexcept
{
    listeners.call([=, &name, &oldValue, &newValue](Listener &l)
    {
        l.onValueChanged(name, oldValue, newValue);
    });
}
#pragma endregion Config
}
