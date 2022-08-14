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
    @file   jaut_Language.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut_provider/lang/jaut_Language.h>
#include <regex>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    using OptLine = std::optional<std::pair<juce::String, juce::String>>;
    
    //==================================================================================================================
    constexpr const char *Pattern_Key = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-";
    
    //==================================================================================================================    
    OptLine getLineText(const juce::String &lineText)
    {
        std::regex  regex(R"(^[ ]*([a-zA-Z0-9._-]+)[ ]*=[ ]*(.*?)[ ]*$)");
        std::smatch match;
        std::string input = lineText.toStdString();
        
        if (std::regex_match(input, match, regex))
        {
            return std::make_pair(match.str(1), match.str(2));
        }
        
        return std::nullopt;
    }
    
    jaut::Language::TranslationMap getLocaleDataFromString(const juce::String &languageString)
    {
        const juce::StringArray            lines        = juce::StringArray::fromLines(languageString);
        jaut::Language::TranslationMap translations;
        
        for (const auto &line : lines)
        {
            if (const OptLine line_match = ::getLineText(line); line_match.has_value())
            {
                translations.emplace(line_match.value());
            }
        }
        
        return translations;
    }
    
    jaut::Language::TranslationMap getLocaleData(const juce::File &langFile)
    {
        const juce::String content = langFile.loadFileAsString();
        return getLocaleDataFromString(content);
    }
    
    jaut::Language::TranslationMap getLocaleDataFromStream(juce::InputStream &inputStream)
    {
        const juce::String content = inputStream.readEntireStreamAsString();
        return getLocaleDataFromString(content);
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Language
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    Language Language::fromStream(juce::InputStream &parStream)
    {
        return { ::getLocaleDataFromStream(parStream) };
    }
    
    juce::String Language::getSingleTranslation(const juce::File               &parFile,
                                                    const juce::String             &parKey,
                                                    std::optional<juce::StringRef> parFallback)
    {
        juce::FileInputStream stream(parFile);
        return getSingleTranslation(stream, parKey, parFallback);
    }
    
    juce::String Language::getSingleTranslation(juce::InputStream              &parStream,
                                                    const juce::String             &parKey,
                                                    std::optional<juce::StringRef> parFallback)
    {
        while (!parStream.isExhausted())
        {
            const juce::String line = parStream.readNextLine();
            
            if (const OptLine line_match = ::getLineText(line);
                line_match.has_value() && line_match->first.equalsIgnoreCase(parKey))
            {
                return line_match->second;
            }
        }
        
        return parFallback.value_or(parKey);
    }
    
    //==================================================================================================================
    Language::Language(juce::File parLangOrRoot, std::unique_ptr<Language> parFallback)
        : fallback    (std::move(parFallback)),
          languagePath(std::move(parLangOrRoot))
    {
        if (languagePath.existsAsFile())
        {
            translations = ::getLocaleData(parLangOrRoot.getFullPathName());
        }
    }

    Language::Language(juce::File parLangOrRoot)
        : Language(std::move(parLangOrRoot), nullptr)
    {}

    Language::Language(const Language &parOther) noexcept
        : translations(parOther.translations),
          fallback    (parOther.fallback ? std::make_unique<Language>(*parOther.fallback) : nullptr),
          languagePath(parOther.languagePath)
    {}

    Language::Language(Language &&parOther) noexcept
        : Language()
    {
        swap(*this, parOther);
    }

    //==================================================================================================================
    Language &Language::operator=(Language parRight) noexcept
    {
        swap(*this, parRight);
        return *this;
    }

    //==================================================================================================================
    void Language::setFallback(std::unique_ptr<Language> parFallback) noexcept
    {
        std::swap(fallback, parFallback);
    }

    bool Language::setFallbackFromData(const juce::String &parLanguageString)
    {
        if (parLanguageString.isEmpty())
        {
            return false;
        }
        
        fallback = std::make_unique<Language>();
    }

    void Language::setFallbackFromData(const juce::File &parLanguageFile)
    {
        if (inputStream.isExhausted())
        {
            return;
        }
        
        if (!fallback)
        {
            fallback.reset(new Language);
        }
        
        auto temp = jaut::getLocaleDataFromStream(inputStream);
        std::swap(fallback->translations, temp);
    }

    bool Language::setCurrentLanguageFromDirectory(const juce::String &languageName)
    {
        if (!rootDir.isDirectory())
        {
            // Must be a valid directory
            jassertfalse;
            return false;
        }
        
        juce::String lang_id = languageName.removeCharacters(" ").toLowerCase();
        const juce::File lang_file = rootDir.getChildFile(lang_id + ".lang");
        
        if (lang_file.existsAsFile())
        {
            auto temp = jaut::getLocaleData(lang_file.getFullPathName());
            std::swap(translations, temp);
            std::swap(fileName, lang_id);
            
            return true;
        }
        
        return false;
    }

    void Language::setCurrentLanguage(const Language *newLocale)
    {
        if (!newLocale)
        {
            translations.clear();
            return;
        }
        
        auto temp = newLocale->translations;
        std::swap(translations, temp);
        fileName = newLocale->fileName;
    }

    void Language::setCurrentLanguage(const juce::String &languageString)
    {
        auto temp = jaut::getLocaleDataFromString(languageString);
        std::swap(translations, temp);
        fileName = "";
    }

    void Language::setCurrentLanguage(juce::InputStream &inputStream)
    {
        auto temp = jaut::getLocaleDataFromStream(inputStream);
        std::swap(translations, temp);
        fileName = "";
    }

    void Language::setFallbackToCurrent()
    {
        if (!fallback)
        {
            return;
        }
        
        auto temp = fallback->translations;
        std::swap(translations, temp);
    }

    //==================================================================================================================
    juce::File Language::getRootDirectory() const noexcept
    {
        return rootDir;
    }

    juce::File Language::getLanguageFile() const
    {
        return rootDir.exists() && !fileName.isEmpty() ? rootDir.getChildFile(fileName + ".lang") : juce::File();
    }

    //==================================================================================================================
    const juce::String &Language::translate(const juce::String &key) const
    {
        const juce::String key_id = key.removeCharacters(" ").toLowerCase();
        const auto it = translations.find(key_id);
        
        if (it != translations.end())
        {
            return it->second;
        }
        else if (fallback)
        {
            const auto it_f = fallback->translations.find(key_id);
            
            if (it_f != fallback->translations.end())
            {
                return it_f->second;
            }
        }
        
        return key;
    }

    const juce::String &Language::translate(const juce::String &name, const juce::String &fallbackValue) const
    {
        const juce::String key_id = name.removeCharacters(" ").toLowerCase();
        const auto it = translations.find(key_id);
        
        if (it != translations.end())
        {
            return it->second;
        }
        
        return fallbackValue;
    }
    
    //==================================================================================================================
    Language::Language(TranslationMap &&parTranslations)
        : translations(std::move(parTranslations))
    {}
}
//======================================================================================================================
// endregion Language
//**********************************************************************************************************************
