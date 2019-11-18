/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: localisation.cpp
 * Time: 4, Mai 2019
 *
 * ===============================================================
 */

#include <jaut/localisation.h>

namespace jaut
{
namespace
{
void createIso639CodeList(const StringArray &dataList, Localisation::LangCodeMap &stringMap) noexcept
{
    for (auto line : dataList)
    {
        if (line.length() < 6)
        {
            continue;
        }

        String langcode = line.trim().substring(0, 2);
        StringArray langnames;
        langnames.addTokens(line.substring(3).trim(), ";", "\"");

        for (auto langname : langnames)
        {
            stringMap.emplace(langname.toLowerCase(), langcode.toLowerCase());
        }
    }
}

bool pushLanguageUpdateToGlobalManager(const File &langFile, LocalisedStrings *locale, LocalisedStrings *sLocale)
{
    sLocale = new LocalisedStrings(langFile, true);

    if (locale)
    {
        sLocale->setFallback(new LocalisedStrings(*locale));
        LocalisedStrings::setCurrentMappings(sLocale);

        return true;
    }

    return false;
}

LocalisedStrings *setDefault(LocalisedStrings *globLocale, LocalisedStrings *locales)
{
    if (!globLocale)
    {
        LocalisedStrings::setCurrentMappings(new LocalisedStrings(*locales));
    }

    LocalisedStrings::getCurrentMappings()->setFallback(new LocalisedStrings(*locales));

    return locales;
}
}

//=====================================================================================================================
Localisation::Localisation(const File &langRootDir, const String &codeList)
    : rootDir(langRootDir)
{
    StringArray listData;
    listData.addLines(codeList);
    createIso639CodeList(listData, langCodes);
}

Localisation::Localisation(const File &langRootDir, const File &codeListFile)
    : rootDir(langRootDir)
{
    StringArray listData;
    codeListFile.readLines(listData);
    createIso639CodeList(listData, langCodes);
}

Localisation::Localisation(const File &langRootDir, InputStream &codeListInputSteam)
    : rootDir(langRootDir)
{
    StringArray listData;

    while (!codeListInputSteam.isExhausted())
    {
        listData.add(codeListInputSteam.readNextLine());
    }

    createIso639CodeList(listData, langCodes);
}

Localisation::Localisation(const Localisation &other)
    : defaultLocale(new LocalisedStrings(*other.defaultLocale)),
    langCodes(other.langCodes)
{}

Localisation::Localisation(Localisation &&other) noexcept
    : defaultLocale(std::move(other.defaultLocale)),
    langCodes(std::move(other.langCodes))
{
    other.defaultLocale = nullptr;
}

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
    other.defaultLocale = nullptr;

    return *this;
}

//=====================================================================================================================
void Localisation::setDefault(LocalisedStrings *fallbackLanguage) noexcept
{
    defaultLocale.reset(jaut::setDefault(getLocale(), fallbackLanguage));
}

void Localisation::setDefault(const String &languageStringUtf8)
{
    defaultLocale.reset(jaut::setDefault(getLocale(), new LocalisedStrings(languageStringUtf8, true)));
}

void Localisation::setDefault(InputStream &inputStream)
{
    defaultLocale.reset(jaut::setDefault(getLocale(), new LocalisedStrings(inputStream.readString(), true)));
}

void Localisation::setCurrentLanguage(const String &language) noexcept
{
    if (!language.matchesWildcard("??_??", true))
    {
        // Make sure the language format is {xx_yy}.
        // Where x: lang code and where y: country code
        // according to the ISO 639 standardisation.
        jassertfalse;
        return;
    }

    LocalisedStrings *slocale = nullptr;
    File langfile = rootDir.getChildFile(language + ".lang");

    if (langfile.exists())
    {
        if (!pushLanguageUpdateToGlobalManager(langfile, defaultLocale.get(), slocale))
        {
            setDefault(slocale);
        }

        return;
    }

    using Types = File::TypesOfFileToFind;
    DirectoryIterator it = DirectoryIterator(rootDir, false, "*.lang", Types::findFiles | Types::ignoreHiddenFiles);

    while (it.next())
    {
        StringArray filelines;
        it.getFile().readLines(filelines);

        if (filelines.size() >= 2)
        {
            String lang = "";
            StringArray regions;

            for (String line : filelines)
            {
                if (!lang.isEmpty() && regions.size() > 0)
                {
                    break;
                }

                if (line.startsWithIgnoreCase("language:"))
                {
                    lang = line.substring(9).trim().toLowerCase();
                }
                else if (line.startsWithIgnoreCase("countries:"))
                {
                    regions.addTokens(line.substring(10).trim().toLowerCase(), true);
                }
                else
                {
                    break;
                }
            }

            if (langCodes.find(lang) != langCodes.end())
            {
                const String &langcode = langCodes.at(lang);
                if (langcode.equalsIgnoreCase(language.trim().substring(0, 2)))
                {
                    if (regions.contains(language.substring(3, 5), true))
                    {
                        if (!pushLanguageUpdateToGlobalManager(it.getFile(), defaultLocale.get(), slocale))
                        {
                            setDefault(slocale);
                        }

                        return;
                    }
                }
            }
        }
    }
}

//=====================================================================================================================
LocalisedStrings *Localisation::getLocale() const noexcept
{
    return LocalisedStrings::getCurrentMappings();
}
}