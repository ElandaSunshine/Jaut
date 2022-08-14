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
    @file   jaut_LanguageManager.cpp
    @date   31, July 2022
    
    ===============================================================
 */

#include <jaut_provider/lang/jaut_LanguageManager.h>

#include <jaut_provider/lang/jaut_LanguageException.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LanguageManager
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LanguageManager &LanguageManager::global(const juce::File &parLanguageRootFolder)
    {
        static LanguageManager instance{ parLanguageRootFolder };
        return instance;
    }
    
    //==================================================================================================================
    LanguageManager::LanguageManager(juce::File parLanguageRootFolder, jaut::LanguageManager::Options parOptions)
        : options(parOptions),
          languageRoot(std::move(parLanguageRootFolder))
    {
        if (!languageRoot.isDirectory())
        {
            throw LangManagerIOException("Language root folder '" + languageRoot.getFullPathName() + "' does not exist "
                                         "or is not a directory");
        }
    }
    
    LanguageManager::LanguageManager(juce::File parLanguageRootFolder)
        : LanguageManager(std::move(parLanguageRootFolder), Options{})
    {}
    
    //==================================================================================================================
    LanguageRegistry::reference LanguageManager::getLanguage(const juce::String                &parLangCode,
                                                             const juce::String                &parCountryCode,
                                                             jaut::LanguageManager::SearchMode parMode)
    {
        return impl_getLanguage(*this, parLangCode, parCountryCode, parMode);
    }
    
    LanguageRegistry::const_reference LanguageManager::getLanguage(const juce::String                &parLangCode,
                                                                   const juce::String                &parCountryCode,
                                                                   jaut::LanguageManager::SearchMode parMode) const
    {
        return impl_getLanguage(*this, parLangCode, parCountryCode, parMode);
    }
}
//======================================================================================================================
// endregion LanguageManager
//**********************************************************************************************************************
