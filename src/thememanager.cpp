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
ThemeManager::ThemePointer::ThemePointer() noexcept
    : manager(nullptr)
{}

ThemeManager::ThemePointer::ThemePointer(std::nullptr_t) noexcept
    : manager(nullptr)
{}

ThemeManager::ThemePointer::ThemePointer(const String &name, IThemeDefinition *theme) noexcept
    : manager(nullptr), name(name), pointer(theme)
{
    jassert(pointer != 0);
}

ThemeManager::ThemePointer::ThemePointer(const ThemePointer &other) noexcept
    : manager(other.manager), name(other.name),
      pointer(other.pointer)
{}

ThemeManager::ThemePointer::ThemePointer(ThemePointer &&other) noexcept
    : manager(std::move(other.manager)), name(std::move(other.name)),
      pointer(std::move(other.pointer))
{
    other.manager = nullptr;
}

ThemeManager::ThemePointer::~ThemePointer()
{}

//======================================================================================================================
ThemeManager::ThemePointer &ThemeManager::ThemePointer::operator=(const ThemePointer &right) noexcept
{
    ThemePointer temp(right);
    swap(*this, temp);

    return *this;
}

ThemeManager::ThemePointer &ThemeManager::ThemePointer::operator=(ThemePointer &&right) noexcept
{
    swap(*this, right);
    right.manager = nullptr;

    return *this;
}

//======================================================================================================================
IThemeDefinition &ThemeManager::ThemePointer::operator*() const
{
    if(pointer)
    {
        return *pointer;
    }

    throw jaut::exception::NullDereference("ThemePointer object -> " + name + " (*)");
}

IThemeDefinition *ThemeManager::ThemePointer::operator->() const
{
    if(pointer)
    {
        return pointer.get();
    }

    throw jaut::exception::NullDereference("ThemePointer object -> " + name + " (->)");
}

ThemeManager::ThemePointer::operator const bool() const noexcept
{
    return pointer != nullptr;
}

bool ThemeManager::ThemePointer::operator==(const ThemePointer &right) const noexcept
{
    return pointer.get() == right.pointer.get();
}

bool ThemeManager::ThemePointer::operator!=(const ThemePointer &right) const noexcept
{
    return pointer.get() != right.pointer.get();
}

//======================================================================================================================
IThemeDefinition *ThemeManager::ThemePointer::get() const noexcept
{
    return pointer.get();
}

//======================================================================================================================
bool ThemeManager::ThemePointer::isCached() const noexcept
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

//======================================================================================================================
String ThemeManager::ThemePointer::getName() const noexcept
{
    return name;
}

//======================================================================================================================
void ThemeManager::ThemePointer::setThemeManager(ThemeManager *manager) noexcept
{
    this->manager = manager;
}
#endif //Region: ThemePointer



//======================================================================================================================
//=============================================== ThemeManager =========================================================
//======================================================================================================================
#if (1) //Region: ThemeManager
ThemeManager::ThemeManager(const File &themeRoot, f_pack_init initializationCallback,
                           std::unique_ptr<IMetaReader> metadataReader, const Options &options)
    : initFunc(initializationCallback), metadataReader(metadataReader.release()),
      options(options), themeRoot(themeRoot)
{
    /**
     * You have to supply your own MetaReader, as themes need meta-information.
     * If you don't pass a valid MetaReader, themes can't be loaded.
     */
    jassert(this->metadataReader != nullptr);

    /**
     * Make sure the root directory of your theme-packs exists.
     */
    jassert(themeRoot.exists());

    if(this->options.themeMetaId.length() < 3)
    {
        this->options.themeMetaId = "packdata.json";
    }
}

ThemeManager::ThemeManager(ThemeManager &&other) noexcept
    : initFunc(std::move(other.initFunc)),
      metadataReader(std::move(other.metadataReader)),
      options(std::move(other.options)),
      themeCache(std::move(other.themeCache)),
      themeRoot(std::move(other.themeRoot))
{
    other.initFunc = nullptr;
}

ThemeManager::~ThemeManager() {}

//======================================================================================================================
ThemeManager& ThemeManager::operator=(ThemeManager &&right) noexcept
{
    swap(*this, right);
    right.initFunc = nullptr;

    return *this;
}

//======================================================================================================================
void ThemeManager::reloadThemePacks()
{
    if(!options.cacheThemes)
    {
        return;
    }

    t_pack_map tempmap;
    DirectoryIterator it(themeRoot, false, options.themePrefix + "*", File::findDirectories);

    while(it.next())
    {
        ThemePointer themeptr(std::move(loadExternalThemePack(it.getFile())));

        if(themeptr)
        {
            themeptr.setThemeManager(this);
            tempmap.emplace(themeptr.getName().trim().toLowerCase(), themeptr);
        }
    }

    for(auto &[key, pack] : themeCache)
    {
        File themeroot (pack->getThemeRootPath());

        if(themeroot.exists() && themeroot.getParentDirectory() != this->themeRoot)
        {
            ThemePointer themeptr(std::move(loadExternalThemePack(themeroot)));

            if(themeptr)
            {
                themeptr.setThemeManager(this);
                tempmap.emplace(themeptr.getName().trim().toLowerCase(), themeptr);
            }
        }
    }

    themeCache.swap(tempmap);
}

void ThemeManager::clearThemePacks()
{
    themeCache.clear();
}

void ThemeManager::reloadThemePack(const String &themeId)
{
    if(!options.cacheThemes)
    {
        return;
    }

    String id = themeId.trim().toLowerCase();

    if(themeCache.find(id) != themeCache.end())
    {
        File folderpath(themeCache.at(id)->getThemeRootPath());

        if(folderpath.exists())
        {
            File meta = folderpath.getChildFile(options.themeMetaId);

            if(!meta.exists())
            {
                return;
            }

            FileInputStream fis(meta);

            if(!fis.isExhausted())
            {
                auto prop = MetadataHelper::readMetaToNamedValueSet(fis)["name"];

                if(prop.isString() && prop.toString().trim().equalsIgnoreCase(themeId))
                {
                    (void)loadExternalThemePack(folderpath, true);
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
}

//======================================================================================================================
ThemeManager::ThemePointer ThemeManager::loadThemePack(const String &themeId) const
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

ThemeManager::ThemePointer ThemeManager::loadExternalThemePack(const File &themeFolder) const
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

ThemeManager::ThemePointer ThemeManager::loadExternalThemePack(const File &themeFolder, bool override)
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

                        ThemePointer themeptr(name, theme);
                        themeptr.setThemeManager(this);
                        (void)themeCache.emplace(id, themeptr);

                        return themeptr;
                    }
                }
            }
        }
    }

    return 0;
}

//======================================================================================================================
ThemeManager::ThemePointer ThemeManager::getThemePack(const String &themeId) const
{
    if(!options.cacheThemes)
    {
        return loadThemePack(themeId);
    }

    String id = themeId.trim().toLowerCase();

    if(themeCache.find(id) != themeCache.end())
    {
        return themeCache.at(id);
    }

    return nullptr;
}

std::vector<ThemeManager::ThemePointer> ThemeManager::getAllThemePacks() const
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

        for(auto &[key, tptr] : themeCache)
        {
            pointers.emplace_back(tptr);
        }
    }

    return pointers;
}

const ThemeManager::Options &ThemeManager::getOptions() const noexcept
{
    return options;
}
#endif //Region: ThemeManager
}
