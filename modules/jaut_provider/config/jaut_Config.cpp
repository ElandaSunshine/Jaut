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
    @file   jaut_Config.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut_provider/config/jaut_Config.h>

#include <jaut_core/util/jaut_VarUtil.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_provider/config/jaut_ConfigException.h>

#include <jaut_provider/config/parser/jaut_XmlParser.h>
#include <jaut_provider/config/parser/jaut_JsonParser.h>
#include <jaut_provider/config/parser/jaut_YamlParser.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    const juce::String& getCategoryId(const jaut::Config &config, const juce::String &category) noexcept
    {
        return (category.isEmpty() ? config.getOptions().defaultCategory : category);
    }
    
    void validateId(const juce::String &id)
    {
        if (id.isEmpty())
        {
            jaut::throwAssertFalse<jaut::ConfigPropertyIdException>("", "can't be empty");
        }
    }
    
    juce::StringArray processExpression(const juce::String &expression)
    {
        juce::StringArray hierarchy;
        
        if (expression.contains("."))
        {
            if (expression.endsWithChar('.') || expression.startsWithChar('.') || expression.contains(".."))
            {
                jaut::throwAssertFalse<jaut::ConfigPropertyIdException>(expression,
                                                                        "property-path expression is not valid");
            }
            
            hierarchy = juce::StringArray::fromTokens(expression, ".", "");
        }
        else
        {
            hierarchy.add(expression);
        }
        
        return hierarchy;
    }
    
    bool isValidType(const juce::var &defaultVar, const juce::var &var) noexcept
    {
        return (!defaultVar.isVoid() && (defaultVar.hasSameTypeAs(var) || var.isVoid()));
    }
    
    std::string_view deduceConfigName(const jaut::IConfigParser *const parser) noexcept
    {
        if (dynamic_cast<const jaut::JsonParser*>(parser))
        {
            return "jsonc";
        }
        
        static constexpr std::array guess_list { "json", "xml", "yaml", "env", "toml", "ini", "hcl" };
        const juce::String className = typeid(*parser).name();
        
        for (const auto &lang : guess_list)
        {
            if (className.containsIgnoreCase(lang))
            {
                return lang;
            }
        }
        
        return "conf";
    }
    
    juce::String processConfigName(const juce::String               &directory,
                                   const jaut::IConfigParser *const parser,
                                   const jaut::Config::Options      &options)
    {
        juce::File config_file(directory);
        
        if (!config_file.exists() || !config_file.isDirectory())
        {
            jaut::throwAssertFalse<jaut::ConfigIOException>("invalid file path '" + directory
                                                            + "', directory does not exist or is not a directory");
        }
        
        if (!options.fileName.isEmpty())
        {
            config_file = config_file.getChildFile(options.fileName);
        }
        else
        {
            const juce::String config_name = juce::String("config.") + ::deduceConfigName(parser).data();
            config_file = config_file.getChildFile(config_name);
        }
        
        if (config_file.exists() && config_file.isDirectory())
        {
            jaut::throwAssertFalse<jaut::ConfigIOException>("invalid file path '" + config_file.getFullPathName()
                                                            + "', file exists but is a directory");
        }
        
        return config_file.getFullPathName();
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Config
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T>
    auto Config::getCategory_impl(T &parConfig, const juce::String &parCategoryId)
    {
        const juce::String &category_id = ::getCategoryId(parConfig, parCategoryId);
        ::validateId(category_id);
    
        decltype(parConfig.getCategory("")) prop = nullptr;
        
        if (auto cat_it = parConfig.properties.find(category_id); cat_it != parConfig.properties.end())
        {
            prop = &cat_it->second;
        }
        
        return prop;
    }
    
    template<class T>
    auto Config::getProperty_impl(T &parConfig, const juce::String &parId, const juce::String &parCategoryId)
    {
        decltype(parConfig.getProperty("")) prop = nullptr;
    
        if (auto *const category = parConfig.getCategory(parCategoryId))
        {
            prop = category->getProperty(parId);
        }
        
        return prop;
    }
    
    template<class T>
    auto Config::findProperty_impl(T &parConfig, const juce::String &parExpression)
    {
        decltype(parConfig.findProperty("")) prop = nullptr;
        
        if (!parExpression.contains(".") || parExpression.endsWith("."))
        {
            return prop;
        }
    
        const juce::String property_exp = parExpression.fromFirstOccurrenceOf(".", false, true);
        const juce::String category_id  = (parExpression.startsWith(".")
                                               ? parConfig.options.defaultCategory
                                               : parExpression.upToFirstOccurrenceOf(".", false, true));
        
        if (auto *const category = parConfig.getCategory(category_id))
        {
            prop = category->findProperty(property_exp);
        }
        
        return prop;
    }
    
    //==================================================================================================================
    Config Config::backendJson(juce::File parDirectory, Options parOptions)
    {
        return { std::move(parDirectory), std::make_unique<JsonParser>(), std::move(parOptions) };
    }
    
    Config Config::backendXml(juce::File parDirectory, Options parOptions)
    {
        return { std::move(parDirectory), std::make_unique<XmlParser>(), std::move(parOptions) };
    }
    
    #if JAUT_OPTLIB_YAML_CPP
    Config Config::backendYaml(juce::File parDirectory, Options parOptions)
    {
        return { std::move(parDirectory), std::make_unique<YamlParser>(), std::move(parOptions) };
    }
    #endif
    
    //==================================================================================================================
    Config::Config(juce::File parDirectory, NonNull<ConfigParserPtr> parConfigParser, Options parOptions)
        : options (std::move(parOptions)),
          fullPath(std::move(parDirectory)),
          parser  (std::move(parConfigParser.get()))
    {
        fullPath = ::processConfigName(fullPath.getFullPathName(), parser.get(), options);
        ::validateId(options.defaultCategory);
        
        if (options.autoSavePolicy)
        {
            ValueChanged += makeHandler([](const Config &config, const Property &property, const juce::var&)
            {
                const bool save = config.options.autoSavePolicy(property, Options::PolicyTarget::ValueChanged);
                
                if (save && config.save())
                {
                    config.SavedAutomatically.invoke(config);
                }
            });
    
            PropertyAdded += makeHandler([](const Config &config, const Property&, const Property &newProperty)
            {
                const bool save = config.options.autoSavePolicy(newProperty, Options::PolicyTarget::PropertyAdded);
    
                if (save && config.save())
                {
                    config.SavedAutomatically.invoke(config);
                }
            });
        }
    }
    
    //==================================================================================================================
    bool Config::load()
    {
        const juce::File &config_file = fullPath;
        
        if (!config_file.exists())
        {
            return false;
        }
        
        IConfigParser::ParseAttributes<CategoryMap> attributes { config_file, properties, options };
        parser->parseConfig(std::move(attributes));
        
        return true;
    }
    
    bool Config::save() const
    {
        const juce::File &config_file = fullPath;
        
        if (!config_file.getParentDirectory().exists())
        {
            if (!options.createFileIfNotExists)
            {
                return false;
            }
            
            if (const juce::Result result = config_file.create(); !result.wasOk())
            {
                jaut::throwAssertFalse<jaut::ConfigIOException>("config file could not be created, "
                                                                + result.getErrorMessage()
                                                                + "(" + config_file.getFullPathName() + ")");
            }
        }
    
        IConfigParser::ParseAttributes<const CategoryMap> attributes { config_file, properties, options };
        parser->writeConfig(std::move(attributes));
        
        return true;
    }
    
    //==================================================================================================================
    Config::Property* Config::findProperty(const juce::String &parExpression)
    {
        return findProperty_impl(*this, parExpression);
    }
    
    const Config::Property* Config::findProperty(const juce::String &parExpression) const
    {
        return findProperty_impl(*this, parExpression);
    }
    
    //==================================================================================================================
    juce::String Config::getConfigName(bool parWithoutExtension) const
    {
        const juce::File &config_file = fullPath;
        return (parWithoutExtension ? config_file.getFileNameWithoutExtension() : config_file.getFileName());
    }
    
    Config::Property* Config::getProperty(const juce::String &parId, const juce::String &parCategoryId)
    {
        return getProperty_impl(*this, parId, parCategoryId);
    }
    
    const Config::Property* Config::getProperty(const juce::String &parId, const juce::String &parCategoryId) const
    {
        return getProperty_impl(*this, parId, parCategoryId);
    }
    
    juce::String Config::getCategoryComment(const juce::String &parCategoryId) const
    {
        if (const Property *category = getCategory(parCategoryId))
        {
            return category->getComment();
        }
        
        return "";
    }
    
    const Config::Options& Config::getOptions() const noexcept
    {
        return options;
    }
    
    const juce::File& Config::getConfigFile() const noexcept
    {
        return fullPath;
    }
    
    //==================================================================================================================
    bool Config::setCategoryComment(const juce::String &parCategoryId, const juce::String &parComment)
    {
        if (Property *const category = getCategory(parCategoryId))
        {
            category->setComment(parComment);
            return true;
        }
        
        return false;
    }
    
    //==================================================================================================================
    std::pair<bool, Config::Property&> Config::createProperty(const juce::String     &parId,
                                                              const juce::var        &parDefaultValue,
                                                              const juce::String     &parCategoryId,
                                                              juce::NotificationType parNotify)
    {
        ::validateId(parId);
        
        if (!parCategoryId.isEmpty())
        {
            ::validateId(parCategoryId);
        }
        
        if (options.strictTyping)
        {
            switch (VarUtil::getVarType(parDefaultValue))
            {
                case VarUtil::VarTypeId::Undefined: JAUT_FALLTHROUGH;
                case VarUtil::VarTypeId::Unknown:
                {
                    const auto ex = ConfigIncompatibleTypeException::needsStrictType(parId);
                    dbgassertfalse(juce::String(ex.what()));
                    throw ex; // NOLINT
                }
                
                default: break;
            }
        }
        
        Property *category = getCategory(parCategoryId);
        
        if (!category)
        {
            const juce::String &cat_id = (parCategoryId.isEmpty() ? options.defaultCategory : parCategoryId);
            category = &PropertyBuilder::constructInMap(cat_id, *this, properties);
        }
        
        return category->createProperty(parId, parDefaultValue, parNotify);
    }
    
    void Config::resetCategory(const juce::String &parCategoryId, juce::NotificationType parNotify)
    {
        if (Property *const category = getCategory(parCategoryId))
        {
            category->reset(true, parNotify);
        }
    }
    
    void Config::resetAll(juce::NotificationType parNotify)
    {
        for (auto &[_, category] : properties)
        {
            category.reset(true, parNotify);
        }
    }
    
    //==================================================================================================================
    Config::Property* Config::getCategory(const juce::String &parCategoryId)
    {
        return getCategory_impl(*this, parCategoryId);
    }
    
    const Config::Property* Config::getCategory(const juce::String &parCategoryId) const
    {
        return getCategory_impl(*this, parCategoryId);
    }
}
//======================================================================================================================
// endregion Config
//**********************************************************************************************************************
// region Config::PropertyBuilder
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    Config::Property& Config::PropertyBuilder::constructInMap(const juce::String &parId, const Config &parConfig,
                                                              Config::CategoryMap &parMap)
    {
        return parMap.try_emplace(parId, Property::internalDelegate_v, parId, juce::var(),
                                  parConfig, nullptr).first->second;
    }
}
//======================================================================================================================
// endregion Config::PropertyBuilder
//**********************************************************************************************************************
// region Config::Property
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T>
    T* Config::Property::getProperty_impl(T &parProperty, const juce::String &parId)
    {
        ::validateId(parId);
        
        auto it = parProperty.properties.find(parId);
        return (it != parProperty.properties.end() ? &it->second : nullptr);
    }
    
    template<class T>
    T* Config::Property::findProperty_impl(T &parProperty, const juce::StringArray &parHierarchy, int parIndex)
    {
        if (parIndex >= parHierarchy.size())
        {
            return &parProperty;
        }
        
        const juce::String &id = parHierarchy.getReference(parIndex);
        ::validateId(id);
        
        auto it = parProperty.properties.find(id);
        
        if (it == parProperty.properties.end())
        {
            return nullptr;
        }
        
        return findProperty_impl(it->second, parHierarchy, parIndex + 1);
    }
    
    //==================================================================================================================
    Config::Property::Property(InternalDelegate_t, juce::String parId, juce::var parDefaultValue,
                               const Config &parConfig, const Property *parParent)
        : defaultValue(parDefaultValue),
          value(std::move(parDefaultValue)),
          id(std::move(parId)),
          config(parConfig),
          parent(parParent)
    {
        ::validateId(id);
    }
    
    //==================================================================================================================
    Config::Property& Config::Property::operator[](const juce::String &parId)
    {
        return createProperty(parId).second;
    }
    
    //==================================================================================================================
    std::pair<bool, Config::Property&> Config::Property::createProperty(const juce::String     &parId,
                                                                        const juce::var        &parDefaultValue,
                                                                        juce::NotificationType parNotify)
    {
        ::validateId(parId);
        
        if (config.options.strictTyping)
        {
            switch (VarUtil::getVarType(parDefaultValue))
            {
                case VarUtil::VarTypeId::Undefined: JAUT_FALLTHROUGH;
                case VarUtil::VarTypeId::Unknown:
                {
                    const auto ex = ConfigIncompatibleTypeException::needsStrictType(parId);
                    dbgassertfalse(juce::String(ex.what()));
                    throw ex; // NOLINT
                }
                    
                default: break;
            }
        }
        
        const auto &[it, inserted] = properties.try_emplace(parId, internalDelegate_v, parId,
                                                            parDefaultValue, config, this);
        
        if (inserted && parNotify != juce::dontSendNotification)
        {
            postPropertyAdded(it->second);
        }
        
        return { inserted, it->second };
    }
    
    Config::Property* Config::Property::findProperty(const juce::String &parExpression)
    {
        return findProperty_impl(*this, ::processExpression(parExpression), 0);
    }
    
    const Config::Property* Config::Property::findProperty(const juce::String &parExpression) const
    {
        return findProperty_impl(*this, ::processExpression(parExpression), 0);
    }
    
    //==================================================================================================================
    int Config::Property::size() const noexcept
    {
        return static_cast<int>(properties.size());
    }
    
    bool Config::Property::hasProperty(const juce::String &parId) const
    {
        ::validateId(parId);
        return (properties.find(parId) != properties.end());
    }
    
    bool Config::Property::hasChildren() const noexcept
    {
        return !properties.empty();
    }
    
    //==================================================================================================================
    Config::Property* Config::Property::getProperty(const juce::String &parId)
    {
        return getProperty_impl(*this, parId);
    }
    
    const Config::Property* Config::Property::getProperty(const juce::String &parId) const
    {
        return getProperty_impl(*this, parId);
    }
    
    const juce::String& Config::Property::getId() const noexcept
    {
        return id;
    }
    
    // NOLINTNEXTLINE
    const juce::String& Config::Property::getCategory() const noexcept
    {
        if (!parent)
        {
            return id;
        }
        
        return parent->getCategory();
    }
    
    juce::String Config::Property::getAbsoluteName() const
    {
        juce::StringArray names;
        names.ensureStorageAllocated(2);
        recurseResolveHierarchy(names);
        
        return names.joinIntoString(".");
    }
    
    const juce::String& Config::Property::getComment() const noexcept
    {
        return comment;
    }
    
    const juce::var& Config::Property::getValue() const noexcept
    {
        return value;
    }
    
    const juce::var& Config::Property::getDefaultValue() const noexcept
    {
        return defaultValue;
    }
    
    //==================================================================================================================
    void Config::Property::setComment(juce::String parComment) noexcept
    {
        std::swap(comment, parComment);
    }
    
    bool Config::Property::setValue(const juce::var &parNewValue, juce::NotificationType parNotify)
    {
        if (config.options.strictTyping && !::isValidType(defaultValue, parNewValue))
        {
            const auto ex = ConfigIncompatibleTypeException::unexpectedPropertyType(parNewValue, defaultValue);
            dbgassertfalse(juce::String(ex.what()))
            throw ex; // NOLINT
        }
        
        return modifyValue(parNewValue, parNotify);
    }
    
    //==================================================================================================================
    void Config::Property::reset(bool parRecursive, juce::NotificationType parNotify)
    {
        if (parRecursive)
        {
            recurseReset(parNotify);
        }
        else
        {
            modifyValue(defaultValue, parNotify);
        }
    }
    
    //==================================================================================================================
    Config::Property::PropertyType Config::Property::getPropertyType() const noexcept
    {
        return (isMapProperty() ? PropertyType::Map : PropertyType::Value);
    }
    
    bool Config::Property::isMapProperty() const noexcept
    {
        return (config.options.strictTyping ? defaultValue.isVoid() : hasChildren());
    }
    
    bool Config::Property::isValueProperty() const noexcept
    {
        return !isMapProperty();
    }
    
    bool Config::Property::isCategory() const noexcept
    {
        return !parent;
    }
    
    //==================================================================================================================
    juce::String Config::Property::toString() const
    {
        return value.toString();
    }
    
    //==================================================================================================================
    Config::Property::Iterator      Config::Property::begin()        noexcept { return properties.begin();  }
    Config::Property::Iterator      Config::Property::end()          noexcept { return properties.end();    }
    Config::Property::ConstIterator Config::Property::begin()  const noexcept { return properties.begin();  }
    Config::Property::ConstIterator Config::Property::end()    const noexcept { return properties.end();    }
    Config::Property::ConstIterator Config::Property::cbegin() const noexcept { return properties.cbegin(); }
    Config::Property::ConstIterator Config::Property::cend()   const noexcept { return properties.cend();   }
    
    //==================================================================================================================
    void Config::Property::postPropertyAdded(const Property &parProperty)
    {
        PropertyAdded.invoke(*this, parProperty);
        config.PropertyAdded.invoke(config, *this, parProperty);
    }
    
    void Config::Property::postValueChanged(const juce::var &parOldValue)
    {
        ValueChanged.invoke(*this, parOldValue);
        config.ValueChanged.invoke(config, *this, parOldValue);
    }
    
    //==================================================================================================================
    bool Config::Property::modifyValue(const juce::var &parNewValue, juce::NotificationType parNotify)
    {
        if (parNewValue == value)
        {
            return false;
        }
        
        const juce::var old_value = std::exchange(value, parNewValue);
        
        if (parNotify != juce::dontSendNotification)
        {
            postValueChanged(old_value);
        }
        
        return true;
    }
    
    //==================================================================================================================
    // NOLINTNEXTLINE
    void Config::Property::recurseReset(juce::NotificationType parNotify)
    {
        modifyValue(defaultValue, parNotify);
        
        for (auto &[_, property] : properties)
        {
            property.recurseReset(parNotify);
        }
    }
    
    // NOLINTNEXTLINE
    void Config::Property::recurseResolveHierarchy(juce::StringArray &parNames) const
    {
        if (parent)
        {
            parent->recurseResolveHierarchy(parNames);
        }
        
        parNames.add(id);
    }
}
//======================================================================================================================
// endregion Config::Property
//**********************************************************************************************************************
