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
    @file   localisation.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include "localisation.h"

namespace jaut
{
//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
constexpr const char *Pattern_Key = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-";

//======================================================================================================================
juce::String getStringWithOrWithoutQuotes(juce::String input)
{
    input = input.trim();
    
    if (input.startsWith("\"") && input.endsWith("\""))
    {
        input = input.substring(1, input.length() - 1);
    }
    
    return input;
}

auto getLocaleData(const juce::String &filePath)
{
    const juce::File lang_file(filePath);
    std::unordered_map<juce::String, juce::String> translations;
    
    if (lang_file.existsAsFile())
    {
        juce::FileInputStream file_stream(lang_file);
        
        while (!file_stream.isExhausted())
        {
            const juce::String line = file_stream.readNextLine();
    
            if (line.startsWith("#") || !line.contains("="))
            {
                continue;
            }
            
            const juce::String key   = getStringWithOrWithoutQuotes(line.upToFirstOccurrenceOf("=", false, false));
            const juce::String value = getStringWithOrWithoutQuotes(line.fromFirstOccurrenceOf("=", false, false));
            
            if (key.length() > 2 && key.containsOnly(Pattern_Key) && value.length() > 0)
            {
                translations.emplace(key.toLowerCase(), value);
            }
        }
    }
    
    return translations;
}

auto getLocaleDataFromString(const juce::String &languageString)
{
    std::unordered_map<juce::String, juce::String> translations;
    
    juce::StringArray lines;
    lines.addLines(languageString);
    
    for (const auto &line : lines)
    {
        if (line.startsWith("#") || !line.contains("="))
        {
            continue;
        }
        
        const juce::String key   = getStringWithOrWithoutQuotes(line.upToFirstOccurrenceOf("=", false, false));
        const juce::String value = getStringWithOrWithoutQuotes(line.fromFirstOccurrenceOf("=", false, false));
    
        if (key.length() > 2 && key.containsOnly(Pattern_Key) && value.length() > 0)
        {
            translations.emplace(key.toLowerCase(), value);
        }
    }
    
    return translations;
}

auto getLocaleDataFromStream(juce::InputStream &inputStream)
{
    std::unordered_map<juce::String, juce::String> translations;
    
    while (!inputStream.isExhausted())
    {
        const juce::String line = inputStream.readNextLine();
    
        if (line.startsWith("#") || !line.contains("="))
        {
            continue;
        }
        
        const juce::String key   = getStringWithOrWithoutQuotes(line.upToFirstOccurrenceOf("=", false, false));
        const juce::String value = getStringWithOrWithoutQuotes(line.fromFirstOccurrenceOf("=", false, false));
    
        if (key.length() > 2 && key.containsOnly(Pattern_Key) && value.length() > 0)
        {
            translations.emplace(key.toLowerCase(), value);
        }
    }
    
    return translations;
}
}

//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Localisation
//======================================================================================================================
Localisation Localisation::fromStream(juce::InputStream &stream)
{
    Localisation locale;
    
    auto temp = jaut::getLocaleDataFromStream(stream);
    std::swap(locale.translations, temp);
    
    return locale;
}

juce::String Localisation::getSingleTranslation(const juce::File &file, const juce::String &transKey)
{
    return getSingleTranslation(file, transKey, transKey);
}

juce::String Localisation::getSingleTranslation(juce::InputStream &inputStream, const juce::String &transKey)
{
    return getSingleTranslation(inputStream, transKey, transKey);
}

juce::String Localisation::getSingleTranslation(const juce::File &file, const juce::String &transKey,
                                                const juce::String &defaultValue)
{
    juce::FileInputStream stream(file);
    return getSingleTranslation(stream, transKey, defaultValue);
}

juce::String Localisation::getSingleTranslation(juce::InputStream &inputStream, const juce::String &transKey,
                                                const juce::String &defaultValue)
{
    while (!inputStream.isExhausted())
    {
        const juce::String line = inputStream.readNextLine().trim();
        
        if (line.startsWith("#") || !line.contains("="))
        {
            continue;
        }
        
        const juce::String key   = getStringWithOrWithoutQuotes(line.upToFirstOccurrenceOf("=", false, false));
        const juce::String value = getStringWithOrWithoutQuotes(line.fromFirstOccurrenceOf("=", false, false));
        
        if (key.equalsIgnoreCase(transKey))
        {
            return value;
        }
    }
    
    return defaultValue;
}

//======================================================================================================================
Localisation::Localisation(const juce::File &langOrRoot, std::unique_ptr<Localisation> fallback)
    : fallback(std::move(fallback)),
      fileName(langOrRoot.existsAsFile() ? langOrRoot.getFileNameWithoutExtension() : ""),
      rootDir (langOrRoot.existsAsFile() ? langOrRoot.getParentDirectory()          : langOrRoot)
{
    if (!fileName.isEmpty())
    {
        auto temp = jaut::getLocaleData(langOrRoot.getFullPathName());
        std::swap(translations, temp);
    }
}

Localisation::Localisation(const juce::File &langOrRoot)
    : fileName(langOrRoot.existsAsFile() ? langOrRoot.getFileNameWithoutExtension() : ""),
      rootDir (langOrRoot.existsAsFile() ? langOrRoot.getParentDirectory()          : langOrRoot)
{
    if (!fileName.isEmpty())
    {
        auto temp = jaut::getLocaleData(langOrRoot.getFullPathName());
        std::swap(translations, temp);
    }
}

Localisation::Localisation(const Localisation &other) noexcept
    : translations(other.translations),
      fallback(other.fallback ? std::make_unique<Localisation>(*other.fallback) : nullptr),
      fileName(other.fileName),
      rootDir(other.rootDir)
{}

Localisation::Localisation(Localisation &&other) noexcept
    : Localisation()
{
    swap(*this, other);
}

//======================================================================================================================
Localisation &Localisation::operator=(const Localisation &locale) noexcept
{
    auto temp(locale);
    swap(*this, temp);
    return *this;
}

Localisation &Localisation::operator=(Localisation &&locale) noexcept
{
    auto temp(std::move(locale));
    swap(*this, temp);
    return *this;
}

//======================================================================================================================
void Localisation::setFallback(const Localisation *newFallback)
{
    if (!newFallback)
    {
        fallback.reset();
        return;
    }
    
    if (!fallback)
    {
        fallback.reset(new Localisation);
    }
    
    auto temp = newFallback->translations;
    std::swap(fallback->translations, temp);
}

void Localisation::setFallback(const juce::String &languageString)
{
    if (languageString.isEmpty())
    {
        return;
    }
    
    if (!fallback)
    {
        fallback.reset(new Localisation);
    }
    
    auto temp = jaut::getLocaleDataFromString(languageString);
    std::swap(fallback->translations, temp);
}

void Localisation::setFallback(juce::InputStream &inputStream)
{
    if (inputStream.isExhausted())
    {
        return;
    }
    
    if (!fallback)
    {
        fallback.reset(new Localisation);
    }
    
    auto temp = jaut::getLocaleDataFromStream(inputStream);
    std::swap(fallback->translations, temp);
}

bool Localisation::setCurrentLanguageFromDirectory(const juce::String &languageName)
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

void Localisation::setCurrentLanguage(const Localisation *newLocale)
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

void Localisation::setCurrentLanguage(const juce::String &languageString)
{
    auto temp = jaut::getLocaleDataFromString(languageString);
    std::swap(translations, temp);
    fileName = "";
}

void Localisation::setCurrentLanguage(juce::InputStream &inputStream)
{
    auto temp = jaut::getLocaleDataFromStream(inputStream);
    std::swap(translations, temp);
    fileName = "";
}

void Localisation::setFallbackToCurrent()
{
    if (!fallback)
    {
        return;
    }
    
    auto temp = fallback->translations;
    std::swap(translations, temp);
}

//======================================================================================================================
juce::File Localisation::getRootDirectory() const noexcept
{
    return rootDir;
}

juce::File Localisation::getLanguageFile() const
{
    return rootDir.exists() && !fileName.isEmpty() ? rootDir.getChildFile(fileName + ".lang") : juce::File();
}

//======================================================================================================================
const juce::String &Localisation::translate(const juce::String &key) const
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

const juce::String &Localisation::translate(const juce::String &name, const juce::String &fallbackValue) const
{
    const juce::String key_id = name.removeCharacters(" ").toLowerCase();
    const auto it = translations.find(key_id);
    
    if (it != translations.end())
    {
        return it->second;
    }
    
    return fallbackValue;
}
//======================================================================================================================
// endregion Localisation
//**********************************************************************************************************************
}
