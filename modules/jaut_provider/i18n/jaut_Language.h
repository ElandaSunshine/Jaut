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
    @file   jaut_Language.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <juce_core/juce_core.h>

#include <optional>


namespace jaut
{
    //==================================================================================================================
    /**
    *  The localisation class parses language files and maps them to their key values.
    *  A language file consists of per-line translations which keys and values are seperated by an '='.
    *  The strings are unquoted, case-insensitive and can be dotted if needed.
    *  Lines that are not correctly formatted will be ignored but comments start with a '#'.
    */
    class JAUT_API Language final
    {
    public:
        using TranslationMap = std::unordered_map<juce::String, juce::String>;
    
        //==============================================================================================================
        static bool isValidLanguageCode(const juce::String &code);
        static bool isValidCountryCode(const juce::String &code);
        
        //==============================================================================================================
        /**
         *  Creates a language object from an input stream.
         *  
         *  @param stream The stream providing the language object
         *  @return The new language object
         */
        static std::unique_ptr<Language> fromStream(juce::InputStream &stream);
    
        /**
         *  Creates a language object from a file.
         *
         *  @param file The file providing the language object
         *  @return The new language object
         */
        static std::unique_ptr<Language> fromFile(const juce::File &file);
        
        /**
         *  Gets a single translation line from a translation file.
         * 
         *  @param file     The file to read the line from
         *  @param key      The key of the translation 
         *  @param fallback The fallback value if no such line was found or std::nullopt if the key should be returned
         *  @return The translated or the fallback string
         */
        static juce::String getSingleTranslation(const juce::File               &file,
                                                 const juce::String             &key,
                                                 std::optional<juce::StringRef> fallback = {});
        
        /**
         *  Gets a single translation line from a translation stream.
         *  
         *  @param stream   The stream to read the line from
         *  @param key      The key of the translation
         *  @param fallback The fallback value if no such line was found or std::nullopt if the key should be returned
         *  @return The translated or the fallback string
         */
        static juce::String getSingleTranslation(juce::InputStream              &inputStream,
                                                 const juce::String             &key,
                                                 std::optional<juce::StringRef> fallback = {});
        
        //==============================================================================================================
        /**
         *  Creates a new instance of the Localisation class.
         *
         *  @param langOrRoot The root directory of the language files or the to be parsed language file
         *  @param fallback   The default language object to fallback to if a key wasn't found in the current set one
         */
        Language(juce::File langOrRoot, std::unique_ptr<Language> fallback);
        
        /**
         *  Creates a new instance of the Localisation class.
         *  @param langRootDir The root directory of the language files or the to be parsed language file
         */
        explicit Language(juce::File langOrRoot);
        
        //==============================================================================================================
        /**
         *  Sets the default fallback language.
         *  @param fallback A pointer to an existing Localisation object to copy the translations of or nullptr if no
         *                  fallback Localisation should be used
         */
        void setFallback(std::unique_ptr<Language> fallback) noexcept;
        
        /**
         *  Sets the default fallback language.
         *  @param languageString A string containing all the language data to create the language object from
         *  @return True if the operation was successful, false if the string was empty or couldn't be parsed
         */
        bool setFallbackFromData(const juce::String &languageString);
        
        /**
         *  Sets the default fallback language.
         *  @param languageFile A path to the file containing all the language data to create the language object from
         */
        bool setFallbackFromData(const juce::File &languageFile);

        /**
         *  Sets the default fallback language.
         *  @param inputStream A stream containing all the language data to create the language object from
         */
        bool setFallbackFromData(juce::InputStream &inputStream);
        
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
        void setCurrentLanguage(const Language *newLocale);
        
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
        
        //==============================================================================================================
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

        //==============================================================================================================
        /**
         *  Gets the internal translation map.
         *  @return The map containing all translations
         */
        const TranslationMap& getTranslations() const noexcept;

        //==============================================================================================================
        /**
         *  Gets a translation value for a specific name if one exists.
         *
         *  If no such key was found the value of the default localisation will be returned and if that doesn't exist
         *  either the key itself will be returned.
         *
         *  @param key The name of the localised element
         *  @return The string mapped to that name
         */
        const juce::String& translate(const juce::String &key) const;
        
        /**
         *  Gets a translation value for a specific name if one exists.
         *  If no such key was found, fallbackValue will be returned.
         *
         *  @param key           The name of the localised element
         *  @param fallbackValue The value to return if no such name was found in the current localisation
         *  @return The string mapped to that name
         */
        const juce::String& translate(const juce::String &key, const juce::String &fallbackValue) const;
        
        //==============================================================================================================
        friend void swap(Language &left, Language &right) noexcept
        {
            using std::swap;
            
            swap(left.translations, right.translations);
            swap(left.fallback,     right.fallback);
            swap(left.languagePath, right.languagePath);
        }
        
    private:
        Language() = default;
        Language(TranslationMap &&translations);
        
        //==============================================================================================================
        TranslationMap            translations;
        std::unique_ptr<Language> fallback;
        juce::File                languagePath;
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Language)
    };
}
