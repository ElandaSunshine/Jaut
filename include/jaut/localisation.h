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
    @file   localisation.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <unordered_map>

namespace jaut
{
/**
    The Localisation class is a simple wrapper around JUCE's inbuilt localisation system.
 */
class JAUT_API Localisation final
{
public:
    static bool isValidLanguageFile(const File &file);
    static std::pair<String, StringArray> getLanguageFileData(const File &file);

    //==================================================================================================================
    /**
        Creates an invalid instance of the Localisation class.
     */
    Localisation() = default;

    /**
        Creates a new instance of the Localisation class.

        @param langRootDir The root directory of the language files
     */
    Localisation(const File &langRootDir, const LocalisedStrings &defaultLocale);

    Localisation(const Localisation &other);
    Localisation(Localisation &&other) noexcept;

    //==================================================================================================================
    Localisation &operator=(const Localisation &other);
    Localisation &operator=(Localisation &&other) noexcept;

    //==================================================================================================================
    /**
        Sets the default fallback language.

        @param defaultLanguage A pointer to a new created LocalisedStrings object, this must be dynamically allocated
     */
    void setDefault(const LocalisedStrings &defaultLanguage) noexcept;

    /**
        Sets the default fallback language.

        @param languageStringUtf8 A string containing all the language data to create the language object from
     */
    void setDefault(const String &languageStringUtf8);

    /**
        Sets the default fallback language.

        @param inputStream An InputStream reading all the language data to create the language object from
     */
    void setDefault(InputStream &inputStream);

    /**
        Sets the new current language of this object or does nothing if no file with that name was found.

        @param language The language to set. English in the UK for example: en_gb
        @return True if setting the new language was successfull, false if not
     */
    bool setCurrentLanguage(const String &language);

    /**
        Sets the new current language from another LocalisedStrings object.
        This won't affect the default language!

        @param localisedStrings The language object to set
     */
    void setCurrentLanguage(const LocalisedStrings &localisedStrings) noexcept;

    /**
        Sets the new current language from another Localisation object.
        This won't affect the default language!

        @param localisedStrings The language object to set
     */
    void setCurrentLanguage(const Localisation &locale) noexcept;

    //==================================================================================================================
    /**
        Gets the current language file.

        @return The language file
     */
    File getLanguageFile() const noexcept;

    //==================================================================================================================
    /**
        Gets the internal LocalisedStrings object.

        @return The current LocalisedString object
     */
    const LocalisedStrings &getInternalLocalisation() const noexcept;

    //==================================================================================================================
    const String translate(const String &name) const noexcept;
    const String translate(const String &name, const String &fallbackValue) const noexcept;

    //==================================================================================================================
    friend void swap(Localisation &left, Localisation &right) noexcept
    {
        std::swap(left.rootDir, right.rootDir);
        std::swap(left.currentLocale, right.currentLocale);
        std::swap(left.defaultLocale, right.defaultLocale);
    }

private:
    File rootDir;
    String fileName;
    LocalisedStrings currentLocale;
    LocalisedStrings defaultLocale;
};
}
