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
    @file   jaut_LanguageRegistry.h
    @date   10, August 2022
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_provider/lang/jaut_Language.h>



namespace jaut
{
    //==================================================================================================================
    /** A simple registry container for Language objects. */
    class JAUT_API LanguageRegistry
    {
    public:
        struct LangKey
        {
            juce::String language;
            juce::String country;
        };
    
        //==============================================================================================================
        using PairType = std::pair<LangKey, std::shared_ptr<Language>>;

    private:
        using LanguageVec = std::vector<PairType>;
        
    public:
        using value_type             = std::shared_ptr<Language>;
        using reference              = std::shared_ptr<Language>;
        using const_reference        = std::shared_ptr<const Language>;
        using iterator               = LanguageVec::iterator;
        using const_iterator         = LanguageVec::const_iterator;
        using reverse_iterator       = LanguageVec::reverse_iterator;
        using const_reverse_iterator = LanguageVec::const_reverse_iterator;
        using difference_type        = std::ptrdiff_t;
        using size_type              = std::size_t;
        
        //==============================================================================================================
        /**
         *  Adds a new language object to the registry if it does not exist already.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @param languageObj The new language object
         *  @return A pair with the iterator to the inserted element or, if it already existed, the existent element
         *          and a bool determining whether the element has been inserted
         */
        std::pair<iterator, bool> add(juce::String langCode,
                                      juce::String countryCode,
                                      std::unique_ptr<Language> languageObj);
        
        /**
         *  Removes a language object by its codes.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @return Whether the object has been removed
         */
        bool remove(const juce::String &langCode, const juce::String &countryCode);
        
        /**
         *  Removes an entire batch of objects that reference a certain language code.
         *
         *  @param langCode The code of the language
         *  @return The number of objects removed
         */
        int removeLanguage(const juce::String &code);
    
        /**
         *  Removes an entire batch of objects that reference a certain country code.
         *
         *  @param countryCode The code of the country
         *  @return The number of objects removed
         */
        int removeCountry(const juce::String &code);
        
        //==============================================================================================================
        /**
         *  Gets a language registry object by its language and country code. (e.g. en, GB for UK english)
         *  Names are case-sensitive.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @return The language object if found, otherwise nullptr
         */
        JAUT_NODISCARD
        reference getLanguage(const juce::String &langCode, const juce::String &countryCode);
        
        /**
         *  Gets a language registry object by its language and country code. (e.g. en, GB for UK english)
         *  Names are case-sensitive.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *  @return The language object if found, otherwise nullptr
         */
        JAUT_NODISCARD
        const_reference getLanguage(const juce::String &langCode, const juce::String &countryCode) const;
    
        /**
         *  Finds all language-country-variants that satisfy the given language code.
         *
         *  @param langCode The code of the language
         *  @return A collection of found language objects consisting of the country name and the object
         */
        JAUT_NODISCARD
        std::vector<std::pair<juce::String, reference>> findForLanguage(const juce::String &langCode);
        
        /**
         *  Finds all language-country-variants that satisfy the given language code.
         *
         *  @param langCode The code of the language
         *  @return A collection of found language objects consisting of the country name and the object
         */
        JAUT_NODISCARD
        std::vector<std::pair<juce::String, const_reference>> findForLanguage(const juce::String &langCode) const;
    
        /**
         *  Finds all languages that satisfy the given country code.
         *
         *  @param countryCode The code of the country
         *  @return A collection of found language objects consisting of the language name and the object
         */
        JAUT_NODISCARD
        std::vector<std::pair<juce::String, reference>> findForCountry(const juce::String &countryCode);
    
        /**
         *  Finds all languages that satisfy the given country code.
         *
         *  @param countryCode The code of the country
         *  @return A collection of found language objects consisting of the language name and the object
         */
        JAUT_NODISCARD
        std::vector<std::pair<juce::String, const_reference>> findForCountry(const juce::String &countryCode) const;
    
        //==============================================================================================================
        /**
         *  Get whether a given language exists.
         *
         *  @param langCode    The code of the language
         *  @param countryCode The code of the country
         *
         *  @return True if a language was found, false if not
         */
        JAUT_NODISCARD
        bool hasLanguage(const juce::String &langCode, const juce::String &countryCode) const;
    
        /**
         *  Get whether a given language exists regardless the country code.
         *
         *  @param langCode The code of the language
         *  @return True if a language was found, false if not
         */
        JAUT_NODISCARD
        bool hasAnyLanguage(const juce::String &code) const;
    
        /**
         *  Get whether a given country exists regardless the language code.
         *
         *  @param countryCode The code of the country
         *  @return True if a country was found, false if not
         */
        JAUT_NODISCARD
        bool hasAnyCountry(const juce::String &code) const;
        
        //==============================================================================================================
        /** Removes all language objects. */
        void clear();
        
        //==============================================================================================================
        /**
         *  Get the number of registered language objects.
         *  @return The number of objects
         */
        JAUT_NODISCARD
        size_type size() const noexcept;
    
        /**
         *  Gets the maximum allocatable size depending on the system implementation.
         *  @return The maximum size possible
         */
        JAUT_NODISCARD
        size_type maxSize() const noexcept;
    
        /**
         *  Gets whether this container is empty or not.
         *  @return Whether it's empty
         */
        JAUT_NODISCARD
        bool isEmpty() const noexcept;
        
        //==============================================================================================================
        JAUT_NODISCARD iterator         begin()        noexcept;
        JAUT_NODISCARD iterator         end()          noexcept;
        JAUT_NODISCARD const_iterator   begin()  const noexcept;
        JAUT_NODISCARD const_iterator   end()    const noexcept;
        JAUT_NODISCARD const_iterator   cbegin() const noexcept;
        JAUT_NODISCARD const_iterator   cend()   const noexcept;
        
        JAUT_NODISCARD reverse_iterator       rbegin()        noexcept;
        JAUT_NODISCARD reverse_iterator       rend()          noexcept;
        JAUT_NODISCARD const_reverse_iterator rbegin()  const noexcept;
        JAUT_NODISCARD const_reverse_iterator rend()    const noexcept;
        JAUT_NODISCARD const_reverse_iterator crbegin() const noexcept;
        JAUT_NODISCARD const_reverse_iterator crend()   const noexcept;
        
        //==============================================================================================================
        friend void swap(LanguageRegistry &left, LanguageRegistry &right) noexcept
        {
            using std::swap;
            swap(left.languages, right.languages);
        }
        
    private:
        template<class Self>
        using ReferenceVariant = std::conditional_t<std::is_const_v<Self>, const_reference, reference>;
    
        //==============================================================================================================
        template<class Self>
        JAUT_NODISCARD
        static auto impl_getLanguage(Self &obj, const juce::String &langCode, const juce::String &countryCode)
            -> ReferenceVariant<Self>;
    
        template<bool SearchByLanguage, class Self>
        JAUT_NODISCARD
        static auto impl_findFor(Self &obj, const juce::String &code)
            -> std::vector<std::pair<juce::String, ReferenceVariant<Self>>>;
        
        //==============================================================================================================
        LanguageVec languages;
    
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(LanguageRegistry)
    };
    
    //==================================================================================================================
    // LanguageRegistry DRY impl funcs
    template<class Self>
    auto LanguageRegistry::impl_getLanguage(Self               &parObj,
                                            const juce::String &parLangCode,
                                            const juce::String &parCountryCode)
        -> ReferenceVariant<Self>
    {
        for (auto it = parObj.languages.begin(); it != parObj.languages.end(); ++it)
        {
            if (it->first.language == parLangCode && it->first.country == parCountryCode)
            {
                return it->second;
            }
        }
    
        return nullptr;
    }
    
    template<bool SearchByLanguage, class Self>
    auto LanguageRegistry::impl_findFor(Self &parObj, const juce::String &parCode)
        -> std::vector<std::pair<juce::String, ReferenceVariant<Self>>>
    {
        std::vector<std::pair<juce::String, ReferenceVariant<Self>>> output;
    
        for (const auto &[key, value] : parObj.languages)
        {
            if constexpr(SearchByLanguage)
            {
                if (key.language == parCode)
                {
                    output.emplace_back(key.country, value);
                }
            }
            else
            {
                if (key.country == parCode)
                {
                    output.emplace_back(key.language, value);
                }
            }
        }
        
        return output;
    }
}
