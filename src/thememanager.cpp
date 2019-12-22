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
    @file   thememanager.cpp
    @date   19, August 2019
    
    ===============================================================
 */

#include <jaut/thememanager.h>

#include <jaut/exception.h>
#include <jaut/imetadata.h>
#include <jaut/imetareader.h>
#include <jaut/ithemedefinition.h>

namespace jaut
{
namespace
{
inline String toIdString(const String &stringToConvert)
{
    return stringToConvert.removeCharacters(" ").toLowerCase();
}

inline IMetadata *getThemeMetadata(const File &metaFile, IMetaReader *reader)
{
    FileInputStream input_stream(metaFile);

    if(!input_stream.isExhausted())
    {
        return reader->parseMetadata(input_stream);
    }

    return nullptr;
}

inline bool isDefaultValidAndEquals(const ThemeManager *themeManager, const String &stringToCompare) noexcept
{
    const ThemePointer &theme = themeManager->getOptions().defaultTheme;
    return theme.isValid() && theme.getId() == stringToCompare
                           || toIdString(theme->getThemeMeta()->getName()) == stringToCompare;
}

inline bool isHigherVersion(const String &expectedHigher, const String &expectedLower)
{
    if(!(expectedHigher.matchesWildcard("*.*", true) && expectedHigher.containsOnly("0123456789.") &&
         expectedLower .matchesWildcard("*.*", true) && expectedLower .containsOnly("0123456789.")))
    {
        return false;
    }

    int major1 = 0;
    int major2 = 0;
    int minor1 = 0;
    int minor2 = 0;

    std::scanf(expectedHigher.toRawUTF8(), "%d.%d", &major1, &minor1);
    std::scanf(expectedLower.toRawUTF8(),  "%d.%d", &major2, &minor2);

    if(major2 < major1)
    {
        return true;
    }
    else if(minor2 < minor1)
    {
        return true;
    }
    
    return false;
}

inline NamedValueSet peakThemeMeta(const File &metaFile)
{
    NamedValueSet data;

    if(metaFile.exists())
    {
        var json;

        if(JSON::parse(metaFile.loadFileAsString(), json).wasOk())
        {
            DynamicObject *jsonroot = json.getDynamicObject();

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
//=============================================== ThemePointer =========================================================
//======================================================================================================================
#pragma region ThemePointer
struct ThemePointer::ThemeData
{
    ThemeManager *manager;
    bool cached;
    String id;
    std::unique_ptr<IThemeDefinition> theme;

    //==================================================================================================================
    ThemeData(const String &id, IThemeDefinition *theme)
        : manager(nullptr), cached(false), id(id),
          theme(std::unique_ptr<IThemeDefinition>(theme))
    {}

    ~ThemeData() {}
};



//======================================================================================================================
ThemePointer::ThemePointer(std::nullptr_t) noexcept {}

ThemePointer::ThemePointer(const String &id, IThemeDefinition *theme)
    : data(!theme ? nullptr : new ThemeData(id.trim().toLowerCase(), theme))
{
    jassert(theme != nullptr);
}

ThemePointer::~ThemePointer() {}

//======================================================================================================================
IThemeDefinition &ThemePointer::operator*() const
{
    if(data)
    {
        return *data->theme;
    }

    throw jaut::exception::NullDereference("ThemePointer '" + getId() + "' (*)");
}

IThemeDefinition *ThemePointer::operator->() const
{
    if(data)
    {
        return data->theme.get();
    }

    throw jaut::exception::NullDereference("ThemePointer '" + getId() + "' (->)");
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
String ThemePointer::getId() const noexcept
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
#pragma endregion



//======================================================================================================================
//=============================================== ThemeManager =========================================================
//======================================================================================================================
#pragma region ThemeManager
ThemeManager::ThemeManager(const File &themeRoot, f_ThemeInit initializationCallback, p_MetaReader metadataReader,
                           const Options &options)
    : initFunc(initializationCallback), metadataReader(std::move(metadataReader)), options(options),
      themeRoot(themeRoot)
{
    /** 
     *  You have to supply your own MetaReader, as themes need meta-information.
     *  If you don't pass a valid MetaReader, themes can't be loaded.
     */
    jassert(this->metadataReader != nullptr);

    /**
     *  Make sure the root directory of your theme-packs exists.
     */
    jassert(themeRoot.exists());

    /**
     *  You have to define an initialization callback for it to work.
     */
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

ThemeManager::ThemeManager(ThemeManager &&other) noexcept
    : currentThemeId(std::move(other.currentThemeId)),
      initFunc      (std::move(other.initFunc)),
      metadataReader(std::move(other.metadataReader)),
      options       (std::move(other.options)),
      themeCache    (std::move(other.themeCache)),
      themeRoot     (std::move(other.themeRoot))
{
    other.initFunc             = nullptr;
    other.metadataReader       = nullptr;
    other.options.defaultTheme = nullptr;

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
    swap(*this, right);
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
    if(currentThemeId.isEmpty())
    {
        return options.defaultTheme;
    }

    ThemePointer theme = getTheme(currentThemeId);
    return !theme.isValid() ? options.defaultTheme : theme;
}

bool ThemeManager::setCurrentTheme(const String &themeId, bool setToDefaultOnFail)
{
    const String theme_id = jaut::toIdString(themeId);

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

    DirectoryIterator iterator (themeRoot, false, options.themePrefix + "*", File::findDirectories);
    clearThemes();

    while(iterator.next())
    {
        const File &theme_folder = iterator.getFile();
        const File meta_file     = theme_folder.getChildFile(options.themeMetaId);
        const auto theme_meta    = jaut::peakThemeMeta(meta_file);

        if(!theme_meta.isEmpty())
        {
            String theme_id = jaut::toIdString(theme_meta["name"].toString());

            if(jaut::isDefaultValidAndEquals(this, theme_id))
            {
                continue;
            }

            bool insert = true;
            const auto theme_iterator = themeCache.find(theme_id);

            if(options.duplicateBehaviour != Options::KeepDuplicates && theme_iterator != themeCache.end())
            {
                if(options.duplicateBehaviour == Options::KeepLatest)
                {
                    insert = jaut::isHigherVersion(theme_meta["version"].toString(), theme_iterator->second
                                                                                     ->getThemeMeta()->getVersion());
                }
                else
                {
                    insert = options.duplicateBehaviour == Options::KeepLast;
                }
            }

            if(insert)
            {
                IMetadata *metadata = jaut::getThemeMetadata(meta_file, metadataReader.get());
                auto *theme_def     = initFunc(theme_folder, std::unique_ptr<IMetadata>(metadata));

                if(metadata && theme_def)
                {
                    if(theme_iterator != themeCache.end() && options.duplicateBehaviour == Options::KeepDuplicates)
                    {
                        bool ran_out_of_indices = true;

                        for(int i = 1; i < 100; ++i)
                        {
                            const String new_id = theme_id + "_" + String(i);

                            if(themeCache.find(new_id) == themeCache.end())
                            {
                                theme_id = new_id;
                                ran_out_of_indices = false;
                                break;
                            }
                        }

                        if(ran_out_of_indices)
                        {
                            continue;
                        }
                    }
                    
                    ThemePointer theme = ThemePointer(theme_id, theme_def);

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

void ThemeManager::reloadTheme(const String &themeId)
{
    if(!initFunc || !metadataReader || !options.cacheThemes)
    {
        return;
    }

    String theme_id = jaut::toIdString(themeId);

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
ThemePointer ThemeManager::loadTheme(const String &themeId) const
{
    if(!initFunc || !metadataReader)
    {
        return nullptr;
    }

    const String needed_theme_id = jaut::toIdString(themeId);

    if(jaut::isDefaultValidAndEquals(this, needed_theme_id))
    {
        return options.defaultTheme;
    }

    DirectoryIterator iterator (themeRoot, false, options.themePrefix + "*", File::findDirectories);
    File last_theme_file;
    String last_theme_version;

    while(iterator.next())
    {
        const File &theme_folder = iterator.getFile();
        const auto theme_meta    = jaut::peakThemeMeta(theme_folder.getChildFile(options.themeMetaId));

        if(!theme_meta.isEmpty())
        {
            String theme_id      = jaut::toIdString(theme_meta["name"].toString());
            String theme_version = theme_meta["version"].toString();

            if(theme_id != needed_theme_id || theme_version.isEmpty() ||
               !theme_version.matchesWildcard("*.*", true) || !theme_version.containsOnly("0123456789."))
            {
                continue;
            }

            bool end_search = options.duplicateBehaviour == Options::KeepFirstFound;

            if(options.duplicateBehaviour == Options::KeepLatest)
            {
                if(!last_theme_version.isEmpty() &&
                   !jaut::isHigherVersion(theme_meta["version"].toString(), last_theme_version))
                {
                    continue;
                }
            }
            
            last_theme_file    = theme_folder;
            last_theme_version = theme_version;

            if(end_search)
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

ThemePointer ThemeManager::loadExternalTheme(const File &themeFolder) const
{
    if(!initFunc || !metadataReader || !themeFolder.isDirectory() ||
       !themeFolder.getFileName().startsWith(options.themePrefix))
    {
        return nullptr;
    }

    const NamedValueSet peak_meta = jaut::peakThemeMeta(themeFolder);

    if(!peak_meta.isEmpty())
    {
        const String theme_id = jaut::toIdString(peak_meta["name"].toString());

        if(IMetadata *metadata = jaut::getThemeMetadata(themeFolder.getChildFile(options.themeMetaId),
                                                                                 metadataReader.get()))
        {
            IThemeDefinition *theme_def = initFunc(themeFolder, std::unique_ptr<IMetadata>(metadata));

            if(theme_def)
            {
                ThemePointer theme (theme_id, theme_def);
                return theme;
            }
        }
    }

    return nullptr;
}

ThemePointer ThemeManager::loadExternalTheme(const File &themeFolder, bool override)
{
    ThemePointer theme    = loadExternalTheme(themeFolder);
    const String theme_id = theme.getId();

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
ThemePointer ThemeManager::getTheme(const String &themeId) const
{
    if(!options.cacheThemes)
    {
        return loadTheme(themeId);
    }

    if(jaut::isDefaultValidAndEquals(this, themeId.trim().toLowerCase()))
    {
        return options.defaultTheme;
    }

    String theme_id = jaut::toIdString(themeId);

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
        DirectoryIterator it(themeRoot, false, options.themePrefix + "*", File::findDirectories);

        while(it.next())
        {
            const ThemePointer theme = loadExternalTheme(it.getFile());
            
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
bool ThemeManager::contains(const String &themeId) const
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
ThemeManager::t_ThemeIterator ThemeManager::begin() const noexcept
{
    return themeCache.cbegin();
}

ThemeManager::t_ThemeIterator ThemeManager::end() const noexcept
{
    return themeCache.cend();
}
#pragma endregion
}
