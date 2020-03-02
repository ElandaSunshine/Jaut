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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   localisation.cpp
    @date   04, May 2019
    
    ===============================================================
 */

#include <jaut/provider/localisation.h>

namespace jaut
{
bool Localisation::isValidLanguageFile(const File &file)
{
    if(file.exists() && !file.isDirectory() && file.getFileName().matchesWildcard("??_??.lang", true))
    {
        FileInputStream input_stream(file);
        String language_name;
        StringArray countries;

        while(!input_stream.isExhausted())
        {
            if(!language_name.isEmpty() && !countries.isEmpty())
            {
                return true;
            }

            const String line = input_stream.readNextLine();

            if(line.startsWith("language:"))
            {
                const String definition = line.substring(9).trim();

                if(definition.containsNonWhitespaceChars() && definition.length() > 2)
                {
                    language_name = definition;
                    continue;
                }
            }
            else if(line.startsWith("countries:"))
            {
                const String definition = line.substring(10).trim();
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

std::pair<String, StringArray> Localisation::getLanguageFileData(const File &file)
{
    decltype(std::declval<Localisation>().getLanguageFileData(File())) result;

    if(file.exists() && !file.isDirectory())
    {
        FileInputStream input_stream(file);
        String language;
        StringArray countries;

        while(!input_stream.isExhausted())
        {
            if(!language.isEmpty() && countries.size() > 0)
            {
                result = std::make_pair(language, countries);
                break;
            }

            const String line = input_stream.readNextLine();

            if(line.startsWith("language:") && language.isEmpty())
            {
                language = line.substring(9).trim();
                continue;
            }
            else if(line.startsWith("countries:") && countries.size() == 0)
            {
                const String countries_string = line.substring(10).trim().toUpperCase();
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
Localisation::Localisation(const File &langRootDir, const LocalisedStrings &defaultLocale)
    : rootDir(langRootDir), currentLocale(defaultLocale), defaultLocale(defaultLocale)
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
void Localisation::setDefault(const LocalisedStrings &fallbackLanguage) noexcept
{
    defaultLocale = fallbackLanguage;
}

void Localisation::setDefault(const String &languageStringUtf8)
{
    if(languageStringUtf8.isEmpty())
    {
        return;
    }

    defaultLocale = LocalisedStrings(languageStringUtf8, true);
}

void Localisation::setDefault(InputStream &inputStream)
{
    if(inputStream.isExhausted())
    {
        return;
    }

    defaultLocale = LocalisedStrings(inputStream.readEntireStreamAsString(), true);
}

bool Localisation::setCurrentLanguage(const String &language)
{
    const File langfile = rootDir.getChildFile(language + ".lang");

    if(Localisation::isValidLanguageFile(langfile))
    {
        fileName      = langfile.getFileNameWithoutExtension();
        currentLocale = LocalisedStrings(langfile.loadFileAsString(), true);
        
        return true;
    }

    return false;
}

void Localisation::setCurrentLanguage(const LocalisedStrings &localisedStrings) noexcept
{
    currentLocale = localisedStrings;
    fileName      = "";
}

void Localisation::setCurrentLanguage(const Localisation &locale) noexcept
{
    currentLocale = locale.currentLocale;
    fileName      = locale.fileName;
}

//=====================================================================================================================
File Localisation::getRootDirectory() const noexcept
{
    return rootDir;
}

File Localisation::getLanguageFile() const noexcept
{
    return rootDir.exists() && !fileName.isEmpty() ? rootDir.getChildFile(fileName + ".lang") : File();
}

//=====================================================================================================================
const LocalisedStrings &Localisation::getInternalLocalisation() const noexcept
{
    return currentLocale;
}

//======================================================================================================================
const String Localisation::translate(const String &name) const noexcept
{
    if(currentLocale.getMappings().containsKey(name))
    {
        return currentLocale.translate(name);
    }

    return defaultLocale.translate(name);
}

const String Localisation::translate(const String &name, const String &fallbackValue) const noexcept
{
    if(currentLocale.getMappings().containsKey(name))
    {
        return currentLocale.translate(name);
    }

    return defaultLocale.translate(name, fallbackValue);
}
}
