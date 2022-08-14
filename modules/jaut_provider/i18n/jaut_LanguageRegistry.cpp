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
    @file   jaut_LanguageRegistry.cpp
    @date   10, August 2022
    
    ===============================================================
 */

#include <jaut_provider/lang/jaut_LanguageRegistry.h>

#include <jaut_provider/jaut_provider_define.h>
#include <jaut_provider/lang/jaut_LanguageException.h>
#include <jaut_datasets/jaut_Dataset.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    bool isValidLanguage(const juce::String &code)
    {
        #if JAUT_PROVIDER_STRICT_LANGUAGE_PROTOCOL
        return jaut::Language::isValidLanguageCode(code);
        #else
        return true;
        #endif
    }
    
    bool isValidCountry(const juce::String &code)
    {
        #if JAUT_PROVIDER_STRICT_LANGUAGE_PROTOCOL
        return jaut::Language::isValidCountryCode(code);
        #else
        return true;
        #endif
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LanguageRegistry
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    auto LanguageRegistry::add(juce::String              parLangCode,
                               juce::String              parCountryCode,
                               std::unique_ptr<Language> parLanguageObj)
        -> std::pair<LanguageRegistry::iterator, bool>
    {
        if (!::isValidLanguage(parLangCode))
        {
            throw LangIsoCodeException::fromLanguageCode(parLangCode);
        }
        
        if (!::isValidCountry(parCountryCode))
        {
            throw LangIsoCodeException::fromCountryCode(parLangCode);
        }
        
        for (auto it = languages.begin(); it != languages.end(); ++it)
        {
            if (it->first.language == parLangCode && it->first.country == parCountryCode)
            {
                return { it, false };
            }
        }
        
        LangKey    key { std::move(parLangCode), std::move(parCountryCode) };
        value_type obj { parLanguageObj.release() };
        auto it = languages.emplace(languages.end(), key, obj);
        
        return { it, true };
    }
    
    //==================================================================================================================
    bool LanguageRegistry::remove(const juce::String &parLangCode, const juce::String &parCountryCode)
    {
        for (auto it = languages.begin(); it != languages.end(); ++it)
        {
            if (it->first.language == parLangCode && it->first.country == parCountryCode)
            {
                languages.erase(it);
                return true;
            }
        }
        
        return false;
    }
    
    int LanguageRegistry::removeLanguage(const juce::String &parCode)
    {
        int count = 0;
        
        for (auto it = languages.begin(); it != languages.end(); ++it)
        {
            if (it->first.language == parCode)
            {
                languages.erase(it);
                ++count;
            }
        }
    
        return count;
    }
    
    int LanguageRegistry::removeCountry(const juce::String &parCode)
    {
        int count = 0;
        
        for (auto it = languages.begin(); it != languages.end(); ++it)
        {
            if (it->first.country == parCode)
            {
                languages.erase(it);
                ++count;
            }
        }
        
        return count;
    }
    
    //==================================================================================================================
    auto LanguageRegistry::getLanguage(const juce::String &parLangCode, const juce::String &parCountryCode)
        -> reference
    {
        return impl_getLanguage(*this, parLangCode, parCountryCode);
    }
    
    auto LanguageRegistry::getLanguage(const juce::String &parLangCode, const juce::String &parCountryCode) const
        -> const_reference
    {
        return impl_getLanguage(*this, parLangCode, parCountryCode);
    }
    
    auto LanguageRegistry::findForLanguage(const juce::String &parCode)
        -> std::vector<std::pair<juce::String, reference>>
    {
        return impl_findFor<true>(*this, parCode);
    }
    
    auto LanguageRegistry::findForLanguage(const juce::String &parCode) const
        -> std::vector<std::pair<juce::String, const_reference>>
    {
        return impl_findFor<true>(*this, parCode);
    }
    
    auto LanguageRegistry::findForCountry(const juce::String &parCode)
        -> std::vector<std::pair<juce::String, reference>>
    {
        return impl_findFor<false>(*this, parCode);
    }
    
    auto LanguageRegistry::findForCountry(const juce::String &parCode) const
        -> std::vector<std::pair<juce::String, const_reference>>
    {
        return impl_findFor<false>(*this, parCode);
    }
    
    //==================================================================================================================
    bool LanguageRegistry::hasLanguage(const juce::String &parLangCode, const juce::String &parCountryCode) const
    {
        for (const auto &[key, value] : languages)
        {
            if (key.language == parLangCode && key.country == parCountryCode)
            {
                return true;
            }
        }
        
        return false;
    }
    
    bool LanguageRegistry::hasAnyLanguage(const juce::String &parCode) const
    {
        for (const auto &[key, value] : languages)
        {
            if (key.language == parCode)
            {
                return true;
            }
        }
    
        return false;
    }
    
    bool LanguageRegistry::hasAnyCountry(const juce::String &parCode) const
    {
        for (const auto &[key, value] : languages)
        {
            if (key.country == parCode)
            {
                return true;
            }
        }
    
        return false;
    }
    
    //==================================================================================================================
    void LanguageRegistry::clear()
    {
        languages.clear();
    }
    
    //==================================================================================================================
    LanguageRegistry::size_type LanguageRegistry::size() const noexcept
    {
        return languages.size();
    }
    
    LanguageRegistry::size_type LanguageRegistry::maxSize() const noexcept
    {
        return languages.max_size();
    }
    
    bool LanguageRegistry::isEmpty() const noexcept
    {
        return languages.empty();
    }
    
    //==================================================================================================================
    LanguageRegistry::iterator LanguageRegistry::begin()              noexcept { return languages.begin();  }
    LanguageRegistry::iterator LanguageRegistry::end()                noexcept { return languages.end();    }
    LanguageRegistry::const_iterator LanguageRegistry::begin()  const noexcept { return languages.begin();  }
    LanguageRegistry::const_iterator LanguageRegistry::end()    const noexcept { return languages.end();    }
    LanguageRegistry::const_iterator LanguageRegistry::cbegin() const noexcept { return languages.cbegin(); }
    LanguageRegistry::const_iterator LanguageRegistry::cend()   const noexcept { return languages.cend();   }
    
    LanguageRegistry::reverse_iterator       LanguageRegistry::rbegin()        noexcept { return languages.rbegin();  }
    LanguageRegistry::reverse_iterator       LanguageRegistry::rend()          noexcept { return languages.rend();    }
    LanguageRegistry::const_reverse_iterator LanguageRegistry::rbegin()  const noexcept { return languages.rbegin();  }
    LanguageRegistry::const_reverse_iterator LanguageRegistry::rend()    const noexcept { return languages.rend();    }
    LanguageRegistry::const_reverse_iterator LanguageRegistry::crbegin() const noexcept { return languages.crbegin(); }
    LanguageRegistry::const_reverse_iterator LanguageRegistry::crend()   const noexcept { return languages.crend();   }
}
//======================================================================================================================
// endregion LanguageRegistry
//**********************************************************************************************************************
