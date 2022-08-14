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
    @file   jaut_LanguageManager.h
    @date   31, July 2022
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_DefUtils.h>

#include <jaut_core/util/jaut_OperationResult.h>
#include <jaut_provider/lang/jaut_Language.h>
#include <jaut_provider/lang/jaut_LanguageRegistry.h>

#include <juce_core/juce_core.h>

#include <optional>



namespace jaut
{
    //==================================================================================================================
    /**
     *  A registry class for jaut::Language objects.
     *  It will manage all language files inside a directory and allows you to switch between them.
     *
     *  This class follows the standards of ISO, namely ISO 639-1 for language codes
     *  and ISO 3166 Alpha-2 for country codes.
     */
    class JAUT_API LanguageManager final
    {
    public:
        struct Options
        {
            //==========================================================================================================
            /** The model on how to resources should be loaded and unloaded. */
            enum class LoadModel
            {
                /**
                 *  Whenever a new language object is loaded, the others are all unloaded.
                 *
                 *  This mode guarantees the least resource consumption, as always just one object is loaded into
                 *  the buffer.
                 *  However, due to this, switching languages will take some more time, as they first need to be
                 *  read again.
                 */
                Automatic,
                
                /**
                 *  All language objects will be loaded and stick until the class' instance is destroyed.
                 *
                 *  This mode has the most resource consumption, as all languages will be loaded into the buffer.
                 *  This mode, on the other hand, guarantees the best language switching times, as all languages
                 *  have already been loaded.
                 */
                Static,
                
                /**
                 *  Loads languages on demand, but will not unload them once switched.
                 *
                 *  This mode has a dynamic memory consumption model, it will load languages on-demand when switched,
                 *  but will not free them upon the next switch.
                 */
                Dynamic,
            };
    
            //==========================================================================================================
            LoadModel loadModel = LoadModel::Dynamic;
        };
        
        /** Returned whenever languages are loaded from disk, contains the result of the loading operation. */
        struct LoadResult
        {
            /** How many new languages have been added to the buffer. */
            int newInstances;
            
            /** How many old languages have already been in the buffer. */
            int oldInstances;
        };
        
        enum class SearchMode
        {
            DiskOnly,
            BufferOnly,
            Both
        };
    
        //==============================================================================================================
        /**
         *  Gets the global LanguageManager instance if needed.
         *
         *  @param languageRootFolder The root directory containing all the language files.
         *  @return The global instance
         *
         *  @throws LangManagerIOException On creation, if the given root folder is not a directory or does not exist
         */
        static LanguageManager& global(const juce::File &languageRootFolder = {});
        
        //==============================================================================================================
        /**
         *  Constructs a new instance of this class.
         *
         *  @param languageRootFolder The root directory containing all the language files.
         *  @param options            The options object for this instance
         *
         *  @throws LangManagerIOException If the given root folder is not a directory or does not exist
         */
        LanguageManager(juce::File languageRootFolder, Options options);
    
        /**
         *  Constructs a new instance of this class.
         *
         *  @param languageRootFolder The root directory containing all the language files.
         *  @throws LangManagerIOException If the given root folder is not a directory or does not exist
         */
        explicit LanguageManager(juce::File languageRootFolder);
        
        //==============================================================================================================
        /**
         *  Gets a language registry object by its language and country code. (e.g. en, GB for UK english)
         *
         *  If none was found, it will try to load it from disk and if this fails too, because it couldn't be found or
         *  for any other reason, it will return nullptr.
         *
         *  Note that, unless the manager's mode is set to Automatic, the language object will be added to the buffer
         *  if not already there.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @param mode        Where to search for the language
         *
         *  @return The language object if found, otherwise nullptr
         */
        LanguageRegistry::reference getLanguage(const juce::String &langCode,
                                                const juce::String &countryCode,
                                                SearchMode         mode         = SearchMode::Both);
    
        /**
         *  Gets a language registry object by its language and country code. (e.g. en, GB for UK english)
         *  If none was found, it will try to load it from disk and if this fails too, because it couldn't be found or
         *  for any other reason, it will return nullptr.
         *
         *  The const version will not add the disk loaded language to the buffer.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @param mode        Where to search for the language
         *
         *  @return The language object if found, otherwise nullptr
         */
        LanguageRegistry::const_reference getLanguage(const juce::String &langCode,
                                                      const juce::String &countryCode,
                                                      SearchMode         mode         = SearchMode::Both) const;
    
        /**
         *  Finds all language country-variants that satisfy the given language code.
         *  This will effectively load the language objects into the buffer if not in automatic mode.
         *
         *  @param langCode   The code of the language
         *  @param searchMode Where to search for the language
         *
         *  @return A collection of found language objects
         */
        std::vector<LanguageRegistry::reference> findForLanguage(const juce::String &code,
                                                                 SearchMode         mode  = SearchMode::Both);
        
        /**
         *  Finds all language country-variants that satisfy the given language code.
         *  The const version will not add the disk loaded language to the buffer.
         *
         *  @param langCode   The code of the language
         *  @param searchMode Where to search for the language
         *
         *  @return A collection of found language objects
         */
        std::vector<LanguageRegistry::const_reference> findForLanguage(const juce::String &code,
                                                                       SearchMode         mode
                                                                           = SearchMode::Both) const;
    
        /**
         *  Finds all languages that satisfy the given country code.
         *  This will effectively load the language objects into the buffer if not in automatic mode.
         *
         *  @param countryCode The code of the country
         *  @param searchMode  Where to search for the language
         *
         *  @return A collection of found language objects
         */
        std::vector<LanguageRegistry::reference> findForCountry(const juce::String &code,
                                                                SearchMode         mode  = SearchMode::Both);
        
        /**
         *  Finds all languages that satisfy the given country code.
         *  The const version will not add the disk loaded language to the buffer.
         *
         *  @param countryCode The code of the country
         *  @param searchMode  Where to search for the language
         *
         *  @return A collection of found language objects
         */
        std::vector<LanguageRegistry::const_reference> findForCountry(const juce::String &code,
                                                                      SearchMode         mode
                                                                          = SearchMode::Both) const;
        
        //==============================================================================================================
        /**
         *  Gets the current language object, or nullptr if none was set.
         *  @return The current language object
         */
        Language* getCurrentLanguageObject() noexcept;
        
        /**
         *  Gets the current language object, or nullptr if none was set.
         *  @return The current language object
         */
        const Language* getCurrentLanguageObject() const noexcept;
        
        /**
         *  Sets the current language-object by its language and country code. (e.g. en, GB for UK english)
         *  This will effectively load the language objects into the buffer if not in the buffer already.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *
         *  @return True if the language could be set or was already set, false if a problem occurred
         */
        bool setCurrentLanguage(const juce::String &langCode, const juce::String &countryCode);
        
        //==============================================================================================================
        /**
         *  Tries to load a language from a stream and adds it to the buffer.
         *
         *  @param stream The stream to read the language from
         *  @return True if it was successfully read, false if it couldn't be read
         */
        bool loadFromStream(juce::InputStream &stream);
        
        /**
         *  Loads all languages from disk into the buffer.
         *  If the load model is automatic, this will do nothing.
         *
         *  @return The result of the operation
         */
        LoadResult loadAll();
        
        /**
         *  Unloads all language objects in the buffer.
         *  @return The number of unloaded language objects
         */
        int unloadAll();
        
        //==============================================================================================================
        /**
         *  Get whether a given language exists.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @param mode        Where to search for the language
         *
         *  @return True if a language was found, false if not
         */
        bool hasLanguage(const juce::String &langCode,
                         const juce::String &countryCode,
                         SearchMode         mode         = SearchMode::Both) const;
    
        /**
         *  Get whether a given language exists regardless the country code.
         *
         *  @param langCode The code of the language
         *  @param mode     Where to search for the language
         *
         *  @return True if a language was found, false if not
         */
        bool hasAnyLanguage(const juce::String &langCode, SearchMode mode = SearchMode::Both) const;
    
        /**
         *  Get whether a given country exists regardless the language code.
         *
         *  @param countryCode The code of the country
         *  @param mode        Where to search for the country
         *
         *  @return True if a country was found, false if not
         */
        bool hasAnyCountry(const juce::String &countryCode, SearchMode mode = SearchMode::Both) const;
        
        
    private:
        struct LangKey
        {
            juce::String language;
            juce::String country;
        };
    
        //==============================================================================================================
        static std::shared_ptr<Language> readOnDisk();
        
        //==============================================================================================================
        template<class Self>
        static LanguageRegistry::reference impl_getLanguage(Self                              &obj,
                                                            const juce::String                &langCode,
                                                            const juce::String                &countryCode,
                                                            jaut::LanguageManager::SearchMode mode);
        
        //==============================================================================================================
        LanguageRegistry languages;
        Language         *currentLanguage { nullptr };
        Options          options;
        juce::File       languageRoot;
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LanguageManager)
    };
    
    //==================================================================================================================
    // LanguageManager DRY impl funcs
    template<class Self>
    LanguageRegistry::reference LanguageRegistry::impl_getLanguage(Self                              &parObj,
                                                                   const juce::String                &parLangCode,
                                                                   const juce::String                &parCountryCode,
                                                                   jaut::LanguageManager::SearchMode parMode)
    {
        if (parMode == LanguageManager::SearchMode::BufferOnly || parMode == LanguageManager::SearchMode::Both)
        {
            if (LanguageRegistry::reference ref = parObj.languages.getLanguage(parLangCode, parCountryCode))
            {
                return ref;
            }
        }
        
        if (parMode == LanguageManager::SearchMode::DiskOnly || parMode == LanguageManager::SearchMode::Both)
        {
            const juce::File lang_file = parObj.languageRoot.getChildFile(parLangCode + "_" + parCountryCode + ".lang");
            
            if (lang_file.exists())
            {
                try
                {
                    if (std::unique_ptr<Language> object = Language::fromFile(lang_file))
                    {
                    
                    }
                }
                catch (const std::exception &ex) {}
            }
        }
        
        return nullptr;
    }
}
