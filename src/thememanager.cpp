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
#include <jaut/metadatahelper.h>

#include <vector>

namespace jaut
{
//======================================================================================================================
//=============================================== ThemePointer =========================================================
//======================================================================================================================
#if (1) //Region: ThemePointer
ThemePointer::ThemePointer() noexcept
    : manager(nullptr)
{}

ThemePointer::ThemePointer(std::nullptr_t) noexcept
    : manager(nullptr)
{}

ThemePointer::ThemePointer(const String &name, IThemeDefinition *theme) noexcept
    : manager(nullptr), name(name), pointer(theme)
{
    jassert(pointer != 0);
}

ThemePointer::ThemePointer(const ThemePointer &other) noexcept
    : manager(other.manager), name(other.name),
      pointer(other.pointer)
{}

ThemePointer::ThemePointer(ThemePointer &&other) noexcept
    : manager(std::move(other.manager)),
      name(std::move(other.name)),
      pointer(std::move(other.pointer))
{
    other.manager = nullptr;
}

ThemePointer::~ThemePointer()
{}

//======================================================================================================================
ThemePointer &ThemePointer::operator=(const ThemePointer &right) noexcept
{
    ThemePointer temp(right);
    swap(*this, temp);

    return *this;
}

ThemePointer &ThemePointer::operator=(ThemePointer &&right) noexcept
{
    swap(*this, right);
    right.manager = nullptr;
    right.pointer = nullptr;

    return *this;
}

//======================================================================================================================
IThemeDefinition &ThemePointer::operator*() const
{
    if(pointer)
    {
        return *pointer;
    }

    throw jaut::exception::NullDereference("ThemePointer object -> " + name + " (*)");
}

IThemeDefinition *ThemePointer::operator->() const
{
    if(pointer)
    {
        return pointer.get();
    }

    throw jaut::exception::NullDereference("ThemePointer object -> " + name + " (->)");
}

ThemePointer::operator const bool() const noexcept
{
    return pointer != nullptr;
}

bool ThemePointer::operator==(const ThemePointer &right) const noexcept
{
    return pointer.get() == right.pointer.get();
}

bool ThemePointer::operator!=(const ThemePointer &right) const noexcept
{
    return pointer.get() != right.pointer.get();
}

//======================================================================================================================
IThemeDefinition *ThemePointer::get() const noexcept
{
    return pointer.get();
}

//======================================================================================================================
bool ThemePointer::isCached() const noexcept
{
    if(manager && manager->options.cacheThemes && pointer.use_count() > 1)
    {        
        for(auto &[key, value] : manager->themeCache)
        {
            if(value.pointer == pointer)
            {
                if(name != key)
                {
                    name = key;
                }

                return true;
            }
        }

        manager = nullptr;
    }

    return false;
}

bool ThemePointer::isValid() const noexcept
{
    return pointer && pointer->isValid();
}

//======================================================================================================================
String ThemePointer::getName() const noexcept
{
    return name;
}

//======================================================================================================================
void ThemePointer::setThemeManager(ThemeManager *manager) noexcept
{
    this->manager = manager;
}
#endif //Region: ThemePointer



//======================================================================================================================
//=============================================== ThemeManager =========================================================
//======================================================================================================================
#if(1) //Region: ThemeManager

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

    if(this->options.themeMetaId.length() < 3)
    {
        this->options.themeMetaId = "packdata.json";
    }
}

ThemeManager::ThemeManager(ThemeManager &&other) noexcept
    : currentThemeId(std::move(other.currentThemeId)),
      initFunc(std::move(other.initFunc)),
      metadataReader(std::move(other.metadataReader)),
      options(std::move(other.options)),
      themeCache(std::move(other.themeCache)),
      themeRoot(std::move(other.themeRoot))
{
    other.initFunc       = nullptr;
    other.metadataReader = nullptr;
}

ThemeManager::~ThemeManager() {}

//======================================================================================================================
ThemeManager& ThemeManager::operator=(ThemeManager &&right) noexcept
{
    swap(*this, right);
    right.initFunc       = nullptr;
    right.metadataReader = nullptr;

    return *this;
}

//======================================================================================================================
ThemePointer ThemeManager::getCurrentTheme() const
{
    return getThemePack(currentThemeId);
}

bool ThemeManager::setCurrentTheme(const String &themeId)
{
    const String theme_id = themeId.trim().toLowerCase();

    if(themeCache.find(theme_id) != themeCache.end())
    {
        currentThemeId = theme_id;
        return true;
    }

    return false;
}

//======================================================================================================================
int ThemeManager::numThemePacks() const noexcept
{
    return themeCache.size();
}

//======================================================================================================================
void ThemeManager::reloadThemePacks()
{
    if(!options.cacheThemes)
    {
        return;
    }

    t_ThemeMap temp_map;
    DirectoryIterator iterator(themeRoot, false, options.themePrefix + "*", File::findDirectories);

    while(iterator.next())
    {
        ThemePointer pointer = std::move(loadExternalThemePack(iterator.getFile()));

        if(pointer)
        {
            pointer.setThemeManager(this);
            temp_map.emplace(pointer.getName().trim().toLowerCase(), pointer);
        }
    }

    for(auto &[key, pack] : themeCache)
    {
        File theme_root = pack->getThemeRootPath();

        if(theme_root.exists() && theme_root.getParentDirectory() != this->themeRoot)
        {
            ThemePointer pointer = std::move(loadExternalThemePack(theme_root));

            if(pointer)
            {
                pointer.setThemeManager(this);
                temp_map.emplace(pointer.getName().trim().toLowerCase(), pointer);
            }
        }
    }

    themeCache.swap(temp_map);

    if(!getCurrentTheme() && numThemePacks() > 0)
    {
        for(auto &[key, theme] : themeCache)
        {
            currentThemeId = key;
            return;
        }
    }
}

void ThemeManager::clearThemePacks()
{
    themeCache.clear();
    currentThemeId = "";
}

void ThemeManager::reloadThemePack(const String &themeId)
{
    if(!options.cacheThemes)
    {
        return;
    }

    String theme_id = themeId.trim().toLowerCase();

    if(themeCache.find(theme_id) != themeCache.end())
    {
        File theme_folder (themeCache.at(theme_id)->getThemeRootPath());

        if(theme_folder.exists())
        {
            File meta_file = theme_folder.getChildFile(options.themeMetaId);

            if(!meta_file.exists())
            {
                return;
            }

            FileInputStream input_stream(meta_file);

            if(!input_stream.isExhausted())
            {
                auto theme_name = MetadataHelper::readMetaToNamedValueSet(input_stream)["name"];

                if(theme_name.isString() && theme_name.toString().trim().equalsIgnoreCase(themeId))
                {
                    (void)loadExternalThemePack(theme_folder, true);
                }
            }
        }
        else
        {
            DirectoryIterator it(themeRoot, false, options.themePrefix + "*", File::findDirectories);

            while(it.next())
            {
                File file = it.getFile();
                File meta = file.getChildFile(options.themeMetaId);

                if(!meta.exists())
                {
                    continue;
                }

                FileInputStream fis(meta);

                if(!fis.isExhausted())
                {
                    auto prop = MetadataHelper::readMetaToNamedValueSet(fis)["name"];

                    if(prop.isString() && prop.toString().trim().equalsIgnoreCase(themeId))
                    {
                        (void)loadExternalThemePack(file, true);
                    }
                }
            }
        }
    }

    if(theme_id == currentThemeId && !getCurrentTheme() && numThemePacks() > 0)
    {
        for(auto &[key, theme] : themeCache)
        {
            currentThemeId = key;
            return;
        }
    }
}

//======================================================================================================================
ThemePointer ThemeManager::loadThemePack(const String &themeId) const
{
    String id = themeId.trim().toLowerCase();
    DirectoryIterator it(themeRoot, false, options.themePrefix + "*", File::findDirectories);

    while(it.next())
    {
        File file = it.getFile();
        File meta = file.getChildFile(options.themeMetaId);

        if(!meta.exists())
        {
            continue;
        }

        FileInputStream fis(meta);

        if(!fis.isExhausted())
        {
            auto prop = MetadataHelper::readMetaToNamedValueSet(fis)["name"];

            if(prop.isString() && prop.toString().trim().equalsIgnoreCase(themeId))
            {
                return loadExternalThemePack(file);
            }
        }
    }

    return 0;
}

ThemePointer ThemeManager::loadExternalThemePack(const File &themeFolder) const
{
    if(themeFolder.exists())
    {
        File metafile = themeFolder.getChildFile(options.themeMetaId);

        if(metafile.exists())
        {
            FileInputStream fis(metafile);

            if(!fis.isExhausted())
            {
                IMetadata *meta = metadataReader->parseMetadata(fis);

                if(meta && initFunc)
                {
                    return ThemePointer(meta->getName(), initFunc(themeFolder, meta));
                }
            }
        }
    }

    return 0;
}

ThemePointer ThemeManager::loadExternalThemePack(const File &themeFolder, bool override)
{
    if(!options.cacheThemes)
    {
        return loadExternalThemePack(themeFolder);
    }

    if(themeFolder.exists())
    {
        File metafile = themeFolder.getChildFile(options.themeMetaId);

        if(!metafile.exists())
        {
            return 0;
        }

        FileInputStream fis(metafile);

        if(!fis.isExhausted())
        {
            IMetadata *meta = metadataReader->parseMetadata(fis);

            if(meta)
            {
                String name = meta->getName();
                String id = name.trim().toLowerCase();
                bool haselement;

                if(themeCache.find(id) != themeCache.end())
                {
                    if(!override)
                    {
                        return themeCache.at(id);
                    }

                    haselement = true;
                }

                if(initFunc)
                {
                    IThemeDefinition *theme = initFunc(themeFolder, meta);

                    if(theme)
                    {
                        if(override && haselement)
                        {
                            (void) themeCache.erase(id);
                        }

                        ThemePointer pointer(name, theme);
                        pointer.setThemeManager(this);
                        (void) themeCache.emplace(id, pointer);

                        return pointer;
                    }
                }
            }
        }
    }

    return 0;
}

//======================================================================================================================
ThemePointer ThemeManager::getThemePack(const String &themeId) const
{
    if(!options.cacheThemes)
    {
        return loadThemePack(themeId);
    }

    String theme_id = themeId.trim().toLowerCase();

    if(themeCache.find(theme_id) != themeCache.end())
    {
        return themeCache.at(theme_id);
    }

    return nullptr;
}

std::vector<ThemePointer> ThemeManager::getAllThemePacks() const
{
    std::vector<ThemePointer> pointers;

    if(!options.cacheThemes)
    {
        DirectoryIterator it(themeRoot, false, options.themePrefix + "*", File::findDirectories);

        while(it.next())
        {
            if(ThemePointer theme = loadExternalThemePack(it.getFile()))
            {
                pointers.emplace_back(theme);
            }
        }
    }
    else
    {
        pointers.reserve(themeCache.size());

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

//======================================================================================================================
ThemeManager::t_ThemeIterator ThemeManager::begin() const noexcept
{
    return themeCache.cbegin();
}

ThemeManager::t_ThemeIterator ThemeManager::end() const noexcept
{
    return themeCache.cend();
}
#endif //Region: ThemeManager
}
