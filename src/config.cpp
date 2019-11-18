/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: AppData.cpp
 * Time: 4, Mai 2019
 *
 * ===============================================================
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
 * ================================ SharedObject ====================================
 * ================================================================================== */

struct Config::Property::SharedObject final
{
    String comment;
    var defaultValue;
    String name;
    t_sub_map properties;
    var value;

    SharedObject(const String &name, const var &defaultValue) noexcept
        : defaultValue(defaultValue), name(name), value(defaultValue)
    {}
};



/* ==================================================================================
 * ================================== Property ======================================
 * ================================================================================== */

Config::Property::Property() noexcept
    : eventConfig(nullptr)
{}

Config::Property::Property(const String &name, const var &defaultValue)
    : data(new SharedObject(name.trim(), defaultValue)), eventConfig(nullptr)
{
    // Always make sure the Property object's name is set and valid!
    // Also, properties can't have the name "value" as this may break internal routines!
    jassert(!name.isEmpty() && !name.equalsIgnoreCase("value"));
}

Config::Property::Property(const Property &other) noexcept
    : data(other.data), eventConfig(other.eventConfig)
{
    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }
}

Config::Property::Property(Property &&other) noexcept
    : data(std::move(other.data)),
      eventConfig(std::move(other.eventConfig))
{
    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }

    other.data        = nullptr;
    other.eventConfig = nullptr;
    other.listeners.clear();
}

//======================================================================================================================
Config::Property &Config::Property::operator=(const Property &other) noexcept
{
    Property temp(other);
    swap(*this, temp);

    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }

    return *this;
}

Config::Property &Config::Property::operator=(Property &&other) noexcept
{
    swap(*this, other);

    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
    }

    other.data        = nullptr;
    other.eventConfig = nullptr;
    other.listeners.clear();

    return *this;
}

//======================================================================================================================
Config::Property Config::Property::operator[](const String &name) noexcept
{
    if (data)
    {
        return data->properties[name.trim().toLowerCase()];
    }

    return Property();
}

//======================================================================================================================
Config::Property Config::Property::createProperty(const String &name, const var &defaultValue) noexcept
{
    if (data)
    {
        Property prop(name, defaultValue);
        data->properties.set(name.trim().toLowerCase(), prop);
        prop.addConfig(eventConfig);

        return prop;
    }

    return Property();
}

void Config::Property::addProperty(Property prop) noexcept
{
    if (!data)
    {
        return;
    }

    String lname = prop.getName().trim().toLowerCase();

    if (!hasProperty(lname))
    {
        data->properties.set(lname, prop);
        postAddedListener(prop);
        prop.addConfig(eventConfig);
    }
}

Config::Property Config::Property::getProperty(const String &name) const noexcept
{
    String lname = name.trim().toLowerCase();

    if (data && hasProperty(lname))
    {
        return data->properties.getReference(lname);
    }

    return Property();
}

//======================================================================================================================
const int Config::Property::size() const noexcept
{
    return data ? static_cast<int>(data->properties.size()) : 0;
}

const bool Config::Property::hasValid() const noexcept
{
    if (data)
    {
        for (auto it : data->properties)
        {
            if (it.isValid())
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
    return data && data->properties.contains(name.trim().toLowerCase());
}

//======================================================================================================================
const String Config::Property::getName() const noexcept
{
    return data ? data->name : "";
}

const String Config::Property::getComment() const noexcept
{
    return data ? data->comment : "";
}

void Config::Property::setComment(const String &comment) noexcept
{
    if (data)
    {
        data->comment = comment;
    }
}

void Config::Property::setValue(const var &value) noexcept
{
    if (data)
    {
        var oldvalue = data->value;

        /**
            Always make sure that the value you are passing has the same object type than the stored value.
            This design decision makes type conversion a lot easier to maintain and, thus, helps in
            parsing with multiple configuration languages.
         */
        jassert(oldvalue.hasSameTypeAs(value));

        data->value = value;
        postValueChangedListener(data->name, oldvalue, value);

        if (eventConfig)
        {
            eventConfig->postValueChangedListener(data->name, oldvalue, value);
        }
    }
}

const var &Config::Property::getValue() noexcept
{
    static var empty;
    return data ? data->value : empty;
}

//======================================================================================================================
const String Config::Property::toString() const noexcept
{
    return data ? data->value.toString() : "";
}

Config::Property::t_iterator Config::Property::begin() const noexcept
{
    return !data ? t_empty_it() : data->properties.begin();
}

Config::Property::t_iterator Config::Property::end() const noexcept
{
    return !data ? t_empty_it() : data->properties.end();
}

//======================================================================================================================
void Config::Property::addListener(Listener *listener) noexcept
{
    listeners.add(listener);
}

void Config::Property::removeListener(Listener *listener) noexcept
{
    listeners.remove(listener);
}

//======================================================================================================================
void Config::Property::addConfig(Config *config) noexcept
{
    if (config && data)
    {
        eventConfig = config;
    }
}

void Config::Property::postAddedListener(Property prop) noexcept
{
    listeners.call([=, &prop] (Listener &l)
    {
        l.onPropertyAdded(prop);
    });
}

void Config::Property::postValueChangedListener(const String &name, var oldValue, var newValue) noexcept
{
    listeners.call([=, &name, &oldValue, &newValue] (Listener &l)
    {
        l.onValueChanged(name, oldValue, newValue);
    });
}



/* ==================================================================================
 * =================================== Options ======================================
 * ================================================================================== */

Config::Options::Options() noexcept
    : autoSave(false),
      processSynced(false),
      defaultCategory("general")
{}



/* ==================================================================================
 * =================================== Config =======================================
 * ================================================================================== */

Config::Config(const String &root, const Options &options, std::unique_ptr<IConfigParser> configParser)
    : autoSaveActive(true), lock(nullptr),
      parser(configParser.release(), [](IConfigParser *parser) { delete parser; }), options(options)
{
    const String filepath = fixPathString(root + "/" + options.fileName);
    File file(filepath);

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

    fullPath = filepath;
    ipMutex.reset(options.processSynced ? new InterProcessLock(filepath) : nullptr);
    (void) categories[options.defaultCategory.trim().toLowerCase()]; // @noret
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

    other.lock    = nullptr;
    other.parser  = nullptr;
    other.ipMutex = nullptr;
    other.listeners.clear();
}

//======================================================================================================================
Config &Config::operator=(Config &&other) noexcept
{
    swap(*this, other);

    for(auto *listener : other.listeners.getListeners())
    {
        listeners.add(listener);
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
        p_proc_scp scl(createIpcLock());

        if (scl && !scl->isLocked())
        {
            return false;
        }

        // Loading the config file
        Property root ("config", 0);

        for (auto &[name, category] : categories)
        {
            Property cat (name, 0);

            if (comments.find(name) != comments.end())
            {
                cat.setComment(comments.at(name));
            }

            for (auto &[p0, prop] : category)
            {
                cat.addProperty(prop);
            }

            root.addProperty(cat);
        }

        return parser->parseConfig(config, root);
    }

    return false;
}

const bool Config::save() const
{
    File config(fullPath);

    if (config.getParentDirectory().exists())
    {
        // Try to lock for no other threads and/or processes to modify the
        // file at the same time!
        p_proc_scp scl(createIpcLock());

        if (scl && !scl->isLocked())
        {
            return false;
        }

        // Saving the config file
        Property root ("config", 0);

        if (!options.configNotice.isEmpty())
        {
            root.setComment(options.configNotice);
        }

        for (auto &[name,category] : categories)
        {
            Property cat (name, 0);

            if (comments.find(name) != comments.end())
            {
                cat.setComment(comments.at(name));
            }

            for (auto &[p0, prop] : category)
            {
                cat.addProperty(prop);
            }

            root.addProperty(cat);
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

Config::Property Config::getProperty(const String &name, const String &category) noexcept
{
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();

    if (categories.find(catname) != categories.end())
    {
        auto &propmap = categories.at(catname);
        String propname = name.trim().toLowerCase();

        if (propmap.find(propname) != propmap.end())
        {
            return propmap.at(propname);
        }
    }

    return Property();
}

const String Config::getCategoryComment(const String &category) const noexcept
{
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();

    if (catname != options.defaultCategory && comments.find(catname) != comments.end())
    {
        return comments.at(catname);
    }

    return "";
}

void Config::setCategoryComment(const String &category, const String &comment) noexcept
{
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();

    if (catname != options.defaultCategory && categories.find(catname) != categories.end())
    {
        String &cmt = comments[catname];
        cmt = comment;

        if (comment != cmt && shouldAutosave())
        {
            (void) save(); // @noret
        }
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
    Property prop (name, defaultValue);
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();
    String lname   = name.trim().toLowerCase();
    bool added     = categories[catname].emplace(lname, prop).second;

    if (added)
    {
        if (shouldAutosave())
        {
            (void)save(); // @noret
        }

        postAddedListener(prop);
    }

    return prop;
}

void Config::addProperty(Property prop, const String &category) noexcept
{
    String catname = !category.isEmpty() ? category.trim().toLowerCase() : options.defaultCategory.trim().toLowerCase();
    String lname   = prop.getName().trim().toLowerCase();
    bool added     = categories[catname].emplace(lname, prop).second;

    if (added)
    {
        if (shouldAutosave())
        {
            (void)save(); // @noret
        }

        postAddedListener(prop);
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

Config::t_proc_scp *Config::createIpcLock() const
{
    return options.processSynced ? new t_proc_scp(*ipMutex) : nullptr;
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
}
