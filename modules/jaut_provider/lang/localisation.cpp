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
    @file   localisation.cpp
    @date   04, May 2019
    
    ===============================================================
 */

namespace jaut
{
bool Localisation::isValidLanguageFile(const juce::File &file)
{
    if(file.exists() && !file.isDirectory() && file.getFileName().matchesWildcard("??_??.lang", true))
    {
        juce::FileInputStream input_stream(file);
        juce::String language_name;
        juce::StringArray countries;

        while(!input_stream.isExhausted())
        {
            if(!language_name.isEmpty() && !countries.isEmpty())
            {
                return true;
            }

            const juce::String line = input_stream.readNextLine();

            if(line.startsWith("language:"))
            {
                const juce::String definition = line.substring(9).trim();

                if(definition.containsNonWhitespaceChars() && definition.length() > 2)
                {
                    language_name = definition;
                    continue;
                }
            }
            else if(line.startsWith("countries:"))
            {
                const juce::String definition = line.substring(10).trim();
                countries.addTokens(definition, true);

                if(!countries.isEmpty())
                {
                    continue;
                }
            }
            else if(line.isEmpty())
            {
                continue;
            }

            break;
        }
    }

    return false;
}

Localisation::LanguageHeader Localisation::getLanguageFileData(const juce::File &file)
{
    LanguageHeader result;

    if(file.exists() && !file.isDirectory())
    {
        juce::FileInputStream input_stream(file);
        juce::String language;
        juce::StringArray countries;

        while(!input_stream.isExhausted())
        {
            if(!language.isEmpty() && countries.size() > 0)
            {
                result = std::make_pair(language, countries);
                break;
            }

            const juce::String line = input_stream.readNextLine();

            if(line.startsWith("language:") && language.isEmpty())
            {
                language = line.substring(9).trim();
                continue;
            }
            else if(line.startsWith("countries:") && countries.size() == 0)
            {
                const juce::String countries_string = line.substring(10).trim().toUpperCase();
                countries.addTokens(countries_string, true);

                if(countries.isEmpty() && countries_string.containsNonWhitespaceChars()
                                       && countries_string.length() == 2)
                {
                    countries.add(countries_string);
                }

                continue;
            }
            else if(line.isEmpty())
            {
                continue;
            }

            break;
        }
    }

    return result;
}

//=====================================================================================================================
Localisation::Localisation(juce::File langRootDir, const juce::LocalisedStrings &defaultLocale)
    : rootDir(std::move(langRootDir)), currentLocale(defaultLocale), defaultLocale(defaultLocale)
{}

Localisation::Localisation(const Localisation &other)
    : rootDir(other.rootDir),
      fileName(other.fileName),
      currentLocale(other.currentLocale),
      defaultLocale(other.defaultLocale)
{}

Localisation::Localisation(Localisation &&other) noexcept
    : rootDir(std::move(other.rootDir)),
      fileName(std::move(other.fileName)),
      currentLocale(std::move(other.currentLocale)),
      defaultLocale(std::move(other.defaultLocale))
{}

//=====================================================================================================================
Localisation &Localisation::operator=(const Localisation &other)
{
    Localisation temp(other);
    swap(*this, temp);

    return *this;
}

Localisation &Localisation::operator=(Localisation &&other) noexcept
{
    swap(*this, other);
    return *this;
}

//=====================================================================================================================
void Localisation::setDefault(const juce::LocalisedStrings &fallbackLanguage)
{
    defaultLocale = fallbackLanguage;
}

void Localisation::setDefault(const juce::String &languageStringUtf8)
{
    if(languageStringUtf8.isEmpty())
    {
        return;
    }

    defaultLocale = juce::LocalisedStrings(languageStringUtf8, true);
}

void Localisation::setDefault(juce::InputStream &inputStream)
{
    if(inputStream.isExhausted())
    {
        return;
    }

    defaultLocale = juce::LocalisedStrings(inputStream.readEntireStreamAsString(), true);
}

bool Localisation::setCurrentLanguage(const juce::String &language)
{
    const juce::File lang_file = rootDir.getChildFile(language + ".lang");

    if(Localisation::isValidLanguageFile(lang_file))
    {
        fileName      = lang_file.getFileNameWithoutExtension();
        currentLocale = juce::LocalisedStrings(lang_file.loadFileAsString(), true);
        
        return true;
    }

    return false;
}

void Localisation::setCurrentLanguage(const juce::LocalisedStrings &localisedStrings)
{
    currentLocale = localisedStrings;
    fileName      = "";
}

void Localisation::setCurrentLanguage(const Localisation &locale)
{
    currentLocale = locale.currentLocale;
    fileName      = locale.fileName;
}

//=====================================================================================================================
juce::File Localisation::getRootDirectory() const noexcept
{
    return rootDir;
}

juce::File Localisation::getLanguageFile() const
{
    return rootDir.exists() && !fileName.isEmpty() ? rootDir.getChildFile(fileName + ".lang") : juce::File();
}

//=====================================================================================================================
const juce::LocalisedStrings& Localisation::getInternalLocalisation() const noexcept
{
    return currentLocale;
}

//======================================================================================================================
const juce::String Localisation::translate(const juce::String &name) const
{
    if(currentLocale.getMappings().containsKey(name))
    {
        return currentLocale.translate(name);
    }

    return defaultLocale.translate(name);
}

const juce::String Localisation::translate(const juce::String &name, const juce::String &fallbackValue) const
{
    if(currentLocale.getMappings().containsKey(name))
    {
        return currentLocale.translate(name);
    }

    return defaultLocale.translate(name, fallbackValue);
}
}
