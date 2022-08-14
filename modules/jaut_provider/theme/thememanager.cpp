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
    @file   thememanager.cpp
    @date   19, August 2019
    
    ===============================================================
 */

namespace jaut
{
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
juce::String toIdString(const juce::String &stringToConvert)
{
    return stringToConvert.removeCharacters(" ").toLowerCase();
}

IMetadata *getThemeMetadata(const juce::File &metaFile, IMetaReader *reader)
{
    juce::FileInputStream input_stream(metaFile);

    if(!input_stream.isExhausted())
    {
        return reader->parseMetadata(input_stream);
    }

    return nullptr;
}

bool isDefaultValidAndEquals(const ThemeManager *themeManager, const juce::String &stringToCompare) noexcept
{
    const ThemePointer &theme = themeManager->getOptions().defaultTheme;
    return theme.isValid() &&
           (theme.getId() == stringToCompare || toIdString(theme->getThemeMeta()->getId()) == stringToCompare);
}

inline juce::NamedValueSet peakThemeMeta(const juce::File &metaFile)
{
    juce::NamedValueSet data;

    if(metaFile.exists())
    {
        juce::var json;

        if(juce::JSON::parse(metaFile.loadFileAsString(), json).wasOk())
        {
            juce::DynamicObject *jsonroot = json.getDynamicObject();

            if(!jsonroot)
            {
                return data;
            }
            
            if(jsonroot->hasProperty("name"))
            {
                data.set("name", jsonroot->getProperty("name").toString());
            }
            else
            {
                return data;   
            }
            if(jsonroot->hasProperty("version"))
            {
                data.set("version", jsonroot->getProperty("version").toString());
            }
            else
            {
                data.clear();
            }
        }
    }

    return data;
}
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region ThemePointer
//======================================================================================================================
struct ThemePointer::ThemeData
{
    juce::String id;
    ThemeManager *manager;
    std::unique_ptr<IThemeDefinition> theme;
    bool cached;

    //==================================================================================================================
    ThemeData(juce::String id, IThemeDefinition *theme)
        : id(std::move(id)), manager(nullptr),
          theme(theme), cached(false)
    {}

    ~ThemeData() = default;
};

//======================================================================================================================
ThemePointer::ThemePointer(std::nullptr_t) noexcept {}

ThemePointer::ThemePointer(const juce::String &id, IThemeDefinition *theme)
    : data(!theme ? nullptr : new ThemeData(id.trim().toLowerCase(), theme))
{
    jassert(theme != nullptr);
}

ThemePointer::~ThemePointer() = default;

//======================================================================================================================
IThemeDefinition &ThemePointer::operator*() const
{
    return *data->theme;
}

IThemeDefinition *ThemePointer::operator->() const
{
    return data->theme.get();
}

ThemePointer::operator bool() const noexcept
{
    return data != nullptr;
}

bool ThemePointer::operator==(const ThemePointer &right) const noexcept
{
    return data == right.data;
}

bool ThemePointer::operator!=(const ThemePointer &right) const noexcept
{
    return data != right.data;
}

bool ThemePointer::operator==(const IThemeDefinition &right) const noexcept
{
    return data && data->theme.get() == &right;
}

bool ThemePointer::operator!=(const IThemeDefinition &right) const noexcept
{
    return data && data->theme.get() != &right;
}

//======================================================================================================================
IThemeDefinition *ThemePointer::get() const noexcept
{
    return data->theme.get();
}

//======================================================================================================================
bool ThemePointer::isCached() const noexcept
{
    return data && data->cached;
}

bool ThemePointer::isValid() const noexcept
{
    return data && get()->isValid();
}

//======================================================================================================================
juce::String ThemePointer::getId() const noexcept
{
    return data ? data->id : "";
}

//======================================================================================================================
void ThemePointer::setThemeManager(ThemeManager *manager) noexcept
{
    if(data)
    {
        data->manager = manager;
        data->cached  = manager;
    }
}
//======================================================================================================================
// endregion ThemePointer
//**********************************************************************************************************************
// region ThemeManager
//======================================================================================================================
ThemeManager::ThemeManager(juce::File themeRoot, ThemeInitFunc initializationCallback, MetaReaderPtr metadataReader,
                           Options options)
    : options(std::move(options)), themeRoot(std::move(themeRoot)), initFunc(std::move(initializationCallback)),
      metadataReader(std::move(metadataReader))
{
    // You have to supply your own MetaReader, as themes need meta-information.
    // If you don't pass a valid MetaReader, themes can't be loaded.
    jassert(this->metadataReader != nullptr);

    // Make sure the root directory of your theme-packs exists.
    jassert(this->themeRoot.exists());

    // You have to define an initialization callback for it to work.
    jassert(initFunc != nullptr);

    if(this->options.themeMetaId.length() < 3)
    {
        this->options.themeMetaId = "packdata.json";
    }

    if(!options.defaultTheme.isValid())
    {
        this->options.defaultTheme = nullptr;
    }
}

ThemeManager::ThemeManager(juce::File themeRoot, ThemeInitFunc initializationCallback, MetaReaderPtr metadataReader)
    : ThemeManager(std::move(themeRoot), std::move(initializationCallback), std::move(metadataReader), {})
{}

ThemeManager::ThemeManager(ThemeManager &&other) noexcept
{
    swap(*this, other);

    for(auto &[id, theme] : themeCache)
    {
        theme.setThemeManager(this);
    }
}

ThemeManager::~ThemeManager()
{
    for(auto &[key, theme] : themeCache)
    {
        theme.setThemeManager(nullptr);
    }
}

//======================================================================================================================
ThemeManager& ThemeManager::operator=(ThemeManager &&right) noexcept
{
    ThemeManager temp(std::move(right));
    swap(*this, temp);
    
    right.initFunc             = nullptr;
    right.metadataReader       = nullptr;
    right.options.defaultTheme = nullptr;

    for(auto &[id, theme] : themeCache)
    {
        theme.setThemeManager(this);
    }

    return *this;
}

//======================================================================================================================
ThemePointer ThemeManager::getCurrentTheme() const
{
    if (currentThemeId.isEmpty())
    {
        return options.defaultTheme;
    }

    ThemePointer theme = getTheme(currentThemeId);
    return !theme.isValid() ? options.defaultTheme : theme;
}

bool ThemeManager::setCurrentTheme(const juce::String &themeId, bool setToDefaultOnFail)
{
    const juce::String theme_id = jaut::toIdString(themeId);

    if(currentThemeId == theme_id)
    {
        return true;
    }

    if(!options.cacheThemes)
    {
        const ThemePointer theme = loadTheme(themeId);

        if(theme.isValid())
        {
            currentThemeId = theme.getId();
            return true;
        }
    }
    else if(contains(theme_id) || jaut::isDefaultValidAndEquals(this, theme_id))
    {
        currentThemeId = theme_id;
        return true;
    }
    else if(setToDefaultOnFail)
    {
        currentThemeId = "";
    }

    return false;
}

//======================================================================================================================
int ThemeManager::numThemes() const noexcept
{
    return themeCache.size();
}

//======================================================================================================================
void ThemeManager::reloadThemes()
{
    if(!initFunc || !metadataReader || !options.cacheThemes)
    {
        return;
    }
    
    const juce::RangedDirectoryIterator iterator(themeRoot, false, options.themePrefix + "*",
                                                 juce::File::findDirectories);
    clearThemes();

    for (const auto &file : iterator)
    {
        const juce::File theme_folder = file.getFile();
        const juce::File meta_file    = theme_folder.getChildFile(options.themeMetaId);
        const auto theme_meta         = jaut::peakThemeMeta(meta_file);

        if(!theme_meta.isEmpty())
        {
            juce::String theme_id = jaut::toIdString(theme_meta["name"].toString());
            const juce::String version  = theme_meta["version"];
            
            if(jaut::isDefaultValidAndEquals(this, theme_id) || !Version::isValidVersionString(version))
            {
                continue;
            }

            bool insert = true;
            const auto theme_iterator = themeCache.find(theme_id);

            if(options.duplicateBehaviour != Options::DuplicateMode::KeepDuplicates &&
               theme_iterator != themeCache.end())
            {
                if (options.duplicateBehaviour == Options::DuplicateMode::KeepLatest)
                {
                    const Version newVersion(version);
                    const Version oldVersion = theme_iterator->second->getThemeMeta()->getVersion();
                    
                    insert = newVersion > oldVersion;
                }
                else
                {
                    insert = options.duplicateBehaviour == Options::DuplicateMode::KeepLast;
                }
            }

            if(insert)
            {
                IMetadata *const metadata = jaut::getThemeMetadata(meta_file, metadataReader.get());
                auto *const theme_def     = initFunc(theme_folder, std::unique_ptr<IMetadata>(metadata));

                if(metadata && theme_def)
                {
                    if(theme_iterator != themeCache.end() &&
                       options.duplicateBehaviour == Options::DuplicateMode::KeepDuplicates)
                    {
                        bool ran_out_of_indices = true;

                        for(int i = 1; i < 100; ++i)
                        {
                            const juce::String new_id = theme_id + "_" + juce::String(i);

                            if(themeCache.find(new_id) == themeCache.end())
                            {
                                theme_id           = new_id;
                                ran_out_of_indices = false;
                                break;
                            }
                        }
                        
                        if(ran_out_of_indices)
                        {
                            continue;
                        }
                    }
                    
                    ThemePointer theme(theme_id, theme_def);

                    if(theme.isValid())
                    {
                        theme.setThemeManager(this);
                        std::exchange(themeCache[theme_id], theme).setThemeManager(nullptr);
                    }
                }
            }
        }
    }

    if(contains(currentThemeId))
    {
        currentThemeId = "";
    }
}

void ThemeManager::clearThemes()
{
    for(auto &[key, theme] : themeCache)
    {
        theme.setThemeManager(nullptr);
    }

    themeCache.clear();
}

void ThemeManager::reloadTheme(const juce::String &themeId)
{
    if(!initFunc || !metadataReader || !options.cacheThemes)
    {
        return;
    }
    
    const juce::String theme_id = jaut::toIdString(themeId);

    if(jaut::isDefaultValidAndEquals(this, theme_id))
    {
        return;
    }

    const auto theme_iterator = themeCache.find(theme_id);

    if(theme_iterator != themeCache.end())
    {
        ThemePointer theme = loadExternalTheme(theme_iterator->second->getThemeRootPath());

        if(!theme.isValid())
        {
            if(!(theme = loadTheme(themeId)).isValid())
            {
                themeCache.erase(theme_iterator)->second.setThemeManager(nullptr);
                return;
            }
        }

        theme.setThemeManager(this);
        std::exchange(themeCache.at(theme_id), theme).setThemeManager(nullptr);
    }
}

//======================================================================================================================
ThemePointer ThemeManager::loadTheme(const juce::String &themeId) const
{
    if(!initFunc || !metadataReader)
    {
        return nullptr;
    }

    const juce::String needed_theme_id = jaut::toIdString(themeId);

    if(jaut::isDefaultValidAndEquals(this, needed_theme_id))
    {
        return options.defaultTheme;
    }
    
    const juce::RangedDirectoryIterator iterator(themeRoot, false, options.themePrefix + "*",
                                                 juce::File::findDirectories);
    juce::File last_theme_file;
    juce::String last_theme_version;

    for (auto &file : iterator)
    {
        const juce::File &theme_folder = file.getFile();
        const auto theme_meta          = jaut::peakThemeMeta(theme_folder.getChildFile(options.themeMetaId));

        if(!theme_meta.isEmpty())
        {
            const juce::String theme_id      = jaut::toIdString(theme_meta["name"].toString());
            const juce::String theme_version = theme_meta["version"].toString();

            if(theme_id != needed_theme_id || !Version::isValidVersionString(theme_version))
            {
                continue;
            }
            
            if(options.duplicateBehaviour == Options::DuplicateMode::KeepLatest)
            {
                const Version newVersion(theme_version);
                const Version oldVersion(last_theme_version);
                
                if(!last_theme_version.isEmpty() && newVersion > oldVersion)
                {
                    continue;
                }
            }
            
            last_theme_file    = theme_folder;
            last_theme_version = theme_version;

            if(options.duplicateBehaviour == Options::DuplicateMode::KeepFirstFound)
            {
                break;
            }
        }
    }

    if(last_theme_file.exists())
    {
        ThemePointer theme = loadExternalTheme(last_theme_file);

        if(theme.isValid())
        {
            return theme;
        }
    }

    return nullptr;
}

ThemePointer ThemeManager::loadExternalTheme(const juce::File &themeFolder) const
{
    if(!initFunc || !metadataReader || !themeFolder.isDirectory() ||
       !themeFolder.getFileName().startsWith(options.themePrefix))
    {
        return nullptr;
    }

    const juce::NamedValueSet peak_meta = jaut::peakThemeMeta(themeFolder);

    if(!peak_meta.isEmpty())
    {
        const juce::String theme_id = jaut::toIdString(peak_meta["name"].toString());

        if(IMetadata *const metadata = jaut::getThemeMetadata(themeFolder.getChildFile(options.themeMetaId),
                                                                                 metadataReader.get()))
        {
            IThemeDefinition *const theme_def = initFunc(themeFolder, std::unique_ptr<IMetadata>(metadata));

            if(theme_def)
            {
                ThemePointer theme (theme_id, theme_def);
                return theme;
            }
        }
    }

    return nullptr;
}

ThemePointer ThemeManager::loadExternalTheme(const juce::File &themeFolder, bool override)
{
    ThemePointer theme          = loadExternalTheme(themeFolder);
    const juce::String theme_id = theme.getId();

    if(override && options.cacheThemes && theme.isValid())
    {
        if(contains(theme_id))
        {
            std::exchange(themeCache.at(theme_id), theme).setThemeManager(nullptr);
        }
        else
        {
            themeCache.emplace(theme_id, theme);   
        }
    }

    return theme;
}

//======================================================================================================================
ThemePointer ThemeManager::getTheme(const juce::String &themeId) const
{
    if(!options.cacheThemes)
    {
        return loadTheme(themeId);
    }

    if(jaut::isDefaultValidAndEquals(this, themeId.trim().toLowerCase()))
    {
        return options.defaultTheme;
    }
    
    const juce::String theme_id = jaut::toIdString(themeId);

    if(contains(theme_id))
    {
        return themeCache.at(theme_id);
    }

    return nullptr;
}

std::vector<ThemePointer> ThemeManager::getAllThemes() const
{
    std::vector<ThemePointer> pointers;

    if(options.defaultTheme.isValid())
    {
        if(options.cacheThemes)
        {
            pointers.reserve(themeCache.size() + 1);
        }

        pointers.emplace_back(options.defaultTheme);
    }

    if(!options.cacheThemes)
    {
        const juce::RangedDirectoryIterator iterator(themeRoot, false, options.themePrefix + "*",
                                                     juce::File::findDirectories);

        for(auto &file : iterator)
        {
            const ThemePointer theme = loadExternalTheme(file.getFile());
            
            if(theme.isValid())
            {
                pointers.emplace_back(theme);
            }
        }
    }
    else
    {
        for(auto &[key, pointer] : themeCache)
        {
            pointers.emplace_back(pointer);
        }
    }

    return pointers;
}

const ThemeManager::Options &ThemeManager::getOptions() const noexcept
{
    return options;
}

//==================================================================================================================
bool ThemeManager::contains(const juce::String &themeId) const
{
    return themeCache.find(themeId.trim().toLowerCase()) != themeCache.end();
}

bool ThemeManager::contains(const ThemePointer &themePointer) const
{
    for(auto &[key, pointer] : themeCache)
    {
        if(pointer == themePointer)
        {
            return true;
        }
    }

    return false;
}

bool ThemeManager::contains(const IThemeDefinition &theme) const
{
    for(auto &[key, pointer] : themeCache)
    {
        if(pointer == theme)
        {
            return true;
        }
    }

    return false;
}

//======================================================================================================================
ThemeManager::ThemeIterator ThemeManager::begin() const noexcept
{
    return themeCache.cbegin();
}

ThemeManager::ThemeIterator ThemeManager::end() const noexcept
{
    return themeCache.cend();
}
//======================================================================================================================
// endregion ThemeManager
//**********************************************************************************************************************
}
