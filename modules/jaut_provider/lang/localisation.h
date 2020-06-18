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
    
    @author Elanda
    @file   localisation.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/** The Localisation class is a quick and dirty wrapper around JUCEs' inbuilt LocalisedStrings class. */
class JAUT_API Localisation final
{
public:
    using LanguageHeader = std::pair<juce::String, juce::StringArray>;
    
    //==================================================================================================================
    /**
     *  Gets whether the given file is a valid localised file.
     *
     *  @param file The file to check
     *  @return Return true if the given file is a valid language file
     */
    static bool isValidLanguageFile(const juce::File &file);
    
    /**
     *  Returns a pair with the language name and a list of countries it applies to.
     *
     *  @param file The file to parse
     *  @return The pair containing the language name and country list
     */
    static LanguageHeader getLanguageFileData(const juce::File &file);

    //==================================================================================================================
    /**
     *  Creates a new instance of the Localisation class.
     *  @param langRootDir   The root directory of the language files
     *  @param defaultLocale The default language object to fallback to if a key wasn't found in the current set one
     */
    Localisation(juce::File langRootDir, const juce::LocalisedStrings &defaultLocale);
    Localisation(const Localisation &other);
    Localisation(Localisation &&other) noexcept;

    //==================================================================================================================
    Localisation &operator=(const Localisation &other);
    Localisation &operator=(Localisation &&other) noexcept;

    //==================================================================================================================
    /**
     *  Sets the default fallback language.
     *  @param defaultLanguage A pointer to a new created LocalisedStrings object, this must be dynamically allocated
     */
    void setDefault(const juce::LocalisedStrings &defaultLanguage);

    /**
     *  Sets the default fallback language.
     *  @param languageStringUtf8 A string containing all the language data to create the language object from
     */
    void setDefault(const juce::String &languageStringUtf8);

    /**
     *  Sets the default fallback language.
     *  @param inputStream An InputStream reading all the language data to create the language object from
     */
    void setDefault(juce::InputStream &inputStream);

    /**
     *  Sets the new current language of this object or does nothing if no file with that name was found.
     *
     *  @param language The language to set. English in the UK for example: en_gb
     *  @return True if setting the new language was successfull, false if not
     */
    bool setCurrentLanguage(const juce::String &language);

    /**
     *  Sets the new current language from another LocalisedStrings object.
     *  This won't affect the default language!
     *
     *  @param localisedStrings The language object to set
     */
    void setCurrentLanguage(const juce::LocalisedStrings &localisedStrings);

    /**
     *  Sets the new current language from another Localisation object.
     *  This won't affect the default language!
     *
     *  @param localisedStrings The language object to set
     */
    void setCurrentLanguage(const Localisation &locale);

    //==================================================================================================================
    /**
     *  Gets the locale root directory.
     *  @return The root directory
     */
    juce::File getRootDirectory() const noexcept;

    /**
     *  Gets the current language file.
     *  @return The language file
     */
    juce::File getLanguageFile() const;

    //==================================================================================================================
    /**
     *  Gets the internal LocalisedStrings object.
     *  @return The current LocalisedString object
     */
    const juce::LocalisedStrings &getInternalLocalisation() const noexcept;

    //==================================================================================================================
    /**
     *  Gets a translation value for a specific name if one exists.
     *
     *  If no such key was found the value of the default localisation will be returned and if that doesn't exist
     *  either the key itself will be returned.
     *
     *  @param name The name of the localised element
     *  @return The string mapped to that name
     */
    const juce::String translate(const juce::String &name) const;
    
    /**
     *  Gets a translation value for a specific name if one exists.
     *  If no such key was found, fallbackValue will be returned.
     *
     *  @param name          The name of the localised element
     *  @param fallbackValue The value to return if no such name was found in the current localisation
     *  @return The string mapped to that name
     */
    const juce::String translate(const juce::String &name, const juce::String &fallbackValue) const;

    //==================================================================================================================
    friend void swap(Localisation &left, Localisation &right) noexcept
    {
        std::swap(left.rootDir, right.rootDir);
        std::swap(left.currentLocale, right.currentLocale);
        std::swap(left.defaultLocale, right.defaultLocale);
    }

private:
    juce::LocalisedStrings currentLocale;
    juce::LocalisedStrings defaultLocale;
    juce::String fileName;
    juce::File rootDir;
};
}
