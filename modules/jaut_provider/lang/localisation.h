/**
    ===============================================================
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
/**
 *  The localisation class parses language files and maps them to their key values.
 *  A language file consists of per-line translations which keys and values are seperated by an '='.
 *  The strings are unquoted, case-insensitive and can be dotted if needed.
 *  Lines that are not correctly formatted will be ignored but comments start with a '#'.
 */
class JAUT_API Localisation final
{
public:
    static Localisation fromStream(juce::InputStream &stream);
    static juce::String getSingleTranslation(const juce::File &file, const juce::String &transKey);
    static juce::String getSingleTranslation(juce::InputStream &inputStream, const juce::String &transKey);
    static juce::String getSingleTranslation(const juce::File &file, const juce::String &transKey,
                                             const juce::String &defaultValue);
    static juce::String getSingleTranslation(juce::InputStream &inputStream, const juce::String &transKey,
                                             const juce::String &defaultValue);
    
    //==================================================================================================================
    /**
     *  Creates a new instance of the Localisation class.
     *
     *  @param langOrRoot The root directory of the language files or the to be parsed language file
     *  @param fallback   The default language object to fallback to if a key wasn't found in the current set one
     */
    Localisation(const juce::File &langOrRoot, std::unique_ptr<Localisation> fallback);
    
    /**
     *  Creates a new instance of the Localisation class.
     *  @param langRootDir The root directory of the language files or the to be parsed language file
     */
    explicit Localisation(const juce::File &langOrRoot);

    Localisation(const Localisation &other) noexcept;
    Localisation(Localisation &&other) noexcept;
    
    //==================================================================================================================
    Localisation &operator=(const Localisation &locale) noexcept;
    Localisation &operator=(Localisation &&locale) noexcept;
    
    //==================================================================================================================
    /**
     *  Sets the default fallback language.
     *  @param newFallback A pointer to an exisitn Localisation object to copy the translations of or nullptr if no
     *                     fallback Localisation should be used
     */
    void setFallback(const Localisation *newFallback);

    /**
     *  Sets the default fallback language.
     *  @param languageString A string containing all the language data to create the language object from
     */
    void setFallback(const juce::String &languageString);

    /**
     *  Sets the default fallback language.
     *  @param inputStream An InputStream reading all the language data to create the language object from
     */
    void setFallback(juce::InputStream &inputStream);
    
    /**
     *  Sets the new current language of this object or does nothing if no file with that name was found.
     *
     *  @param languageName The name of the language file to parse
     *  @return True if setting the new language was successful, false if not
     */
    bool setCurrentLanguageFromDirectory(const juce::String &languageName);
    
    /**
     *  Sets the new current language from another LocalisedStrings object.
     *  This won't affect the default language!
     *
     *  @param localisedStrings The language object to set
     */
    void setCurrentLanguage(const Localisation *newLocale);
    
    /**
     *  Sets the new current language from another Localisation object.
     *  This won't affect the default language!
     *
     *  @param languageString The string to read the language data from
     */
    void setCurrentLanguage(const juce::String &languageString);
    
    /**
     *  Sets the new current language of this object or does nothing if no file with that name was found.
     *
     *  @param language The language to set. English in the UK for example: en_gb
     *  @return True if setting the new language was successfull, false if not
     */
    void setCurrentLanguage(juce::InputStream &inputStream);
    
    /**
     *  Sets this' Localisation object's fallback localisation to the current used one.
     *  If no fallback was set, this won't do anything.
     */
    void setFallbackToCurrent();
    
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
     *  Gets the internal translation map.
     *  @return The map containing all translations
     */
    const auto &getTranslations() const noexcept { return translations; }

    //==================================================================================================================
    /**
     *  Gets a translation value for a specific name if one exists.
     *
     *  If no such key was found the value of the default localisation will be returned and if that doesn't exist
     *  either the key itself will be returned.
     *
     *  @param key The name of the localised element
     *  @return The string mapped to that name
     */
    const juce::String &translate(const juce::String &key) const;
    
    /**
     *  Gets a translation value for a specific name if one exists.
     *  If no such key was found, fallbackValue will be returned.
     *
     *  @param key           The name of the localised element
     *  @param fallbackValue The value to return if no such name was found in the current localisation
     *  @return The string mapped to that name
     */
    const juce::String &translate(const juce::String &key, const juce::String &fallbackValue) const;
    
    //==================================================================================================================
    friend void swap(Localisation &left, Localisation &right) noexcept
    {
        using std::swap;
        
        swap(left.translations, right.translations);
        swap(left.fallback,     right.fallback);
        swap(left.fileName,     right.fileName);
        swap(left.rootDir,      right.rootDir);
    }
    
private:
    Localisation() = default;
    
    //==================================================================================================================
    std::unordered_map<juce::String, juce::String> translations;
    std::unique_ptr<Localisation> fallback;
    juce::String fileName;
    juce::File rootDir;
};
}
