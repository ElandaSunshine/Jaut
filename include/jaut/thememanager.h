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

/**
    The ThemePointer is a wrapper around a IThemeDefinition object, sharing it along all other ThemePointer objects
    pointing to the same memory location.

    It acts like a smart pointer, with the only difference that it has member methods regarding the the underlying
    type like isValid() and getId().

    The jaut::ThemeManager class uses this class to cache themes. 
    
    @see jaut::ThemeManager
 */
class JAUT_API ThemePointer final
{
public:
    ThemePointer() noexcept = default;
    ThemePointer(std::nullptr_t) noexcept;
    ThemePointer(const String &id, IThemeDefinition *theme);
    ~ThemePointer();

    //==============================================================================================================
    IThemeDefinition &operator*() const;
    IThemeDefinition *operator->() const;

    operator bool() const noexcept;

    bool operator==(const ThemePointer &right) const noexcept;
    bool operator!=(const ThemePointer &right) const noexcept;
    bool operator==(const IThemeDefinition &right) const noexcept;
    bool operator!=(const IThemeDefinition &right) const noexcept;
    
    //==============================================================================================================
    IThemeDefinition *get() const noexcept;

    //==============================================================================================================
    bool isCached() const noexcept;
    bool isValid() const noexcept;

    //==============================================================================================================
    String getId() const noexcept;

    //==============================================================================================================
    friend void swap(ThemePointer &left, ThemePointer &right) noexcept
    {
        left.data.swap(right.data);
    }

private:
    friend class ThemeManager;
    struct ThemeData;

    std::shared_ptr<ThemeData> data;

    //==============================================================================================================
    void setThemeManager(ThemeManager *manager) noexcept;
};

class JAUT_API ThemeManager final
{
public:
    struct JAUT_API Options final
    {
        enum JAUT_API DuplicateMode
        {
            KeepLast,
            KeepLatest,
            KeepDuplicates,
            KeepFirstFound
        };

        bool cacheThemes;
        ThemePointer defaultTheme;
        DuplicateMode duplicateBehaviour;
        String themeMetaId;
        String themePrefix;

        Options() noexcept
            : cacheThemes(true),
              duplicateBehaviour(KeepLast),
              themeMetaId("packdata.json")
        {}
    };

    using p_SharedTheme   = std::shared_ptr<IThemeDefinition>;
    using p_MetaReader    = std::unique_ptr<IMetaReader>;
    using p_Theme         = std::shared_ptr<IThemeDefinition>;
    using f_ThemeInit     = std::function<IThemeDefinition*(const juce::File&, std::unique_ptr<IMetadata>)>;
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
    bool setCurrentTheme(const String &themeId, bool setToDefaultOnFail = false);

    //==================================================================================================================
    int numThemes() const noexcept;

    //==================================================================================================================
    void reloadThemes();
    void clearThemes();
    void reloadTheme(const String &themeId);

    //==================================================================================================================
    ThemePointer loadTheme(const String &themeId) const;
    ThemePointer loadExternalTheme(const File &themeFolder) const;
    ThemePointer loadExternalTheme(const File &themeFolder, bool override);

    //==================================================================================================================
    ThemePointer  getTheme(const String &themeId) const;
    t_ThemeVector getAllThemes() const;
    const Options &getOptions() const noexcept;

    //==================================================================================================================
    bool contains(const String &themeId) const;
    bool contains(const ThemePointer &themePointer) const;
    bool contains(const IThemeDefinition &theme)    const;

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
    String currentThemeId;
    f_ThemeInit initFunc;
    p_MetaReader metadataReader;
    Options options;
    t_ThemeMap themeCache;
    File themeRoot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeManager)
};
}
