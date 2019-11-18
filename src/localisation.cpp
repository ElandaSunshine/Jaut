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

#include <jaut/localisation.h>

namespace jaut
{
Localisation::Localisation(const File &langRootDir, const LocalisedStrings &defaultLocale)
    : rootDir(langRootDir), currentLocale(defaultLocale), defaultLocale(defaultLocale)
{}

Localisation::Localisation(const Localisation &other)
    : rootDir(other.rootDir),
      currentLocale(other.currentLocale),
      defaultLocale(other.defaultLocale)
{}

Localisation::Localisation(Localisation &&other) noexcept
    : rootDir(std::move(other.rootDir)),
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
    currentLocale.setFallback(new LocalisedStrings(defaultLocale));
}

void Localisation::setDefault(const String &languageStringUtf8)
{
    if(languageStringUtf8.isEmpty())
    {
        return;
    }

    defaultLocale = LocalisedStrings(languageStringUtf8, true);
    currentLocale.setFallback(new LocalisedStrings(defaultLocale));
}

void Localisation::setDefault(InputStream &inputStream)
{
    if(inputStream.isExhausted())
    {
        return;
    }

    defaultLocale = LocalisedStrings(inputStream.readEntireStreamAsString(), true);
    currentLocale.setFallback(new LocalisedStrings(defaultLocale));
}

void Localisation::setCurrentLanguage(const String &language)
{
    const File langfile = rootDir.getChildFile(language + ".lang");

    if(langfile.exists())
    {
        StringArray lines;
        langfile.readLines(lines);

        if(lines > 2 && lines[0].startsWithIgnoreCase("language:") && lines[1].startsWithIgnoreCase("regions:"))
        {
            const String regions  = lines[1].trim().substring(8);
            const String language = lines[0].trim().substring(9);

            if(regions.isEmpty() || language.isEmpty())
            {
                /* Lang files must include language:{language} on the first line and
                   regions:[regions] on the second line!
                 */
                jassertfalse;
                return;
            }

            FileInputStream fis(langfile);

            if(!fis.isExhausted())
            {
                currentLocale = LocalisedStrings(fis.readEntireStreamAsString(), true);
                currentLocale.setFallback(new LocalisedStrings(defaultLocale));
            }
        }
    }
}

void Localisation::setCurrentLanguage(const LocalisedStrings &localisedStrings) noexcept
{
    currentLocale = localisedStrings;
}

void Localisation::setCurrentLanguage(const Localisation &locale) noexcept
{
    currentLocale = locale.currentLocale;
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
