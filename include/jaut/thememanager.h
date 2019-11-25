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
    @file   thememanager.h
    @date   19, August 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{

class IMetadata;
class IMetaReader;
class IThemeDefinition;
class ThemeManager;

class JAUT_API ThemePointer final
{
public:
    using p_SharedTheme = std::shared_ptr<IThemeDefinition>;

    ThemePointer() noexcept;
    ThemePointer(std::nullptr_t) noexcept;
    ThemePointer(const String &name, IThemeDefinition *theme) noexcept;
    ThemePointer(const ThemePointer &other) noexcept;
    ThemePointer(ThemePointer &&other) noexcept;
    ~ThemePointer();

    //==============================================================================================================
    ThemePointer &operator=(const ThemePointer &right) noexcept;
    ThemePointer &operator=(ThemePointer &&right) noexcept;

    //==============================================================================================================
    IThemeDefinition &operator*() const;
    IThemeDefinition *operator->() const;
    operator const bool() const noexcept;
    bool operator==(const ThemePointer &right) const noexcept;
    bool operator!=(const ThemePointer &right) const noexcept;

    //==============================================================================================================
    IThemeDefinition *get() const noexcept;

    //==============================================================================================================
    bool isCached() const noexcept;
    bool isValid() const noexcept;

    //==============================================================================================================
    String getName() const noexcept;

    //==============================================================================================================
    friend void swap(ThemePointer &left, ThemePointer &right) noexcept
    {
        std::swap(left.manager, right.manager);
        std::swap(left.name, right.name);
        left.pointer.swap(right.pointer);
    }

private:
    friend class ThemeManager;

    mutable ThemeManager *manager;
    mutable String name;
    p_SharedTheme pointer;

    //==============================================================================================================
    void setThemeManager(ThemeManager *manager) noexcept;
};

class JAUT_API ThemeManager final
{
public:
    struct JAUT_API Options final
    {
        bool cacheThemes;
        String themeMetaId;
        String themePrefix;

        Options() noexcept
            : cacheThemes(true),
              themeMetaId("packdata.json")
        {}
    };

    using p_SharedTheme   = std::shared_ptr<IThemeDefinition>;
    using p_MetaReader    = std::unique_ptr<IMetaReader>;
    using p_Theme         = std::shared_ptr<IThemeDefinition>;
    using f_ThemeInit     = std::function<IThemeDefinition*(const juce::File&, IMetadata *meta)>;
    using t_ThemeMap      = std::map<juce::String, ThemePointer>;
    using t_ThemeIterator = t_ThemeMap::const_iterator;
    using t_ThemeVector   = std::vector<ThemePointer>;

    //==================================================================================================================
    ThemeManager(const File &themeRoot, f_ThemeInit initializationCallback, p_MetaReader metadataReader,
                 const Options &options = Options());
    ThemeManager(ThemeManager &&other) noexcept;
    ~ThemeManager();

    //==================================================================================================================
    ThemeManager &operator=(ThemeManager &&right) noexcept;

    //==================================================================================================================
    ThemePointer getCurrentTheme() const;
    bool setCurrentTheme(const String &themeId);

    //==================================================================================================================
    int numThemePacks() const noexcept;

    //==================================================================================================================
    void reloadThemePacks();
    void clearThemePacks();
    void reloadThemePack(const String &themeId);

    //==================================================================================================================
    ThemePointer loadThemePack(const String &themeId) const;
    ThemePointer loadExternalThemePack(const File &themeFolder) const;
    ThemePointer loadExternalThemePack(const File &themeFolder, bool override);

    //==================================================================================================================
    ThemePointer  getThemePack(const String &themeId) const;
    t_ThemeVector getAllThemePacks() const;
    const Options &getOptions() const noexcept;

    //==================================================================================================================
    t_ThemeIterator begin() const noexcept;
    t_ThemeIterator end()   const noexcept; 

    //==================================================================================================================
    friend void swap(ThemeManager &left, ThemeManager &right) noexcept
    {
        std::swap(left.currentThemeId, right.currentThemeId);
        std::swap(left.initFunc, right.initFunc);
        std::swap(left.themeRoot, right.themeRoot);
        std::swap(left.options, right.options);
        left.metadataReader.swap(right.metadataReader);
        left.themeCache.swap(right.themeCache);
    }

private:
    friend class ThemePointer;

    String currentThemeId;
    f_ThemeInit initFunc;
    p_MetaReader metadataReader;
    Options options;
    t_ThemeMap themeCache;
    File themeRoot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeManager)
};
}
