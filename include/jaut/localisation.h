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
 * File: localisation.h
 * Time: 4, Mai 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <unordered_map>

namespace jaut
{
/**
    The Localisation class is a simple wrapper around JUCE's inbuilt localisation system.
    You have to specify a list of valid language and country codes to make sure those languages are
    natively supported by this class.

    If you don't want this strictness, you can still use JUCE's default localisation system.
    For that, see the class LocalisedStrings.
 */
class JAUT_API Localisation final
{
public:
    //=================================================================================================================
    using LangCodeMap = std::unordered_map<String, String>;

    //=================================================================================================================
    /**
        Creates an invalid instance of the Localisation class.
     */
    Localisation() = default;

    /**
        Creates a new instance of the Localisation class.

        @param langRootDir The root directory of the language files
        @param codeList    The list of supported country and language codes
     */
    Localisation(const File &langRootDir, const String &codeList);

    /**
        Creates a new instance of the Localisation class.

        @param langRootDir  The root directory of the language files
        @param codeListFile A file to the list of supported country and language codes
     */
    Localisation(const File &langRootDir, const File &codeListFile);

    /**
        Creates a new instance of the Localisation class.

        @param langRootDir         The root directory of the language files
        @param codeListInputStream A stream to the list of supported country and language codes
     */
    Localisation(const File &langRootDir, InputStream &codeListInputStream);

    Localisation(const Localisation &other);
    Localisation(Localisation &&other) noexcept;

    //=================================================================================================================
    Localisation &operator=(const Localisation &other);
    Localisation &operator=(Localisation &&other) noexcept;

    //=================================================================================================================
    /**
        Sets the default fallback language for the global language instance.

        If there is not already a global language specified, this will become the fallback
        and global language instance.

        @param defaultLanguage A pointer to a new created LocalisedStrings object, this must be dynamically allocated
     */
    void setDefault(LocalisedStrings *defaultLanguage) noexcept;

    /**
        Sets the default fallback language for the global language instance.

        If there is not already a global language specified, this will become the fallback
        and global language instance.

        @param languageStringUtf8 A string containing all the language data to create the language object from
     */
    void setDefault(const String &languageStringUtf8);

    /**
        Sets the default fallback language for the global language instance.

        If there is not already a global language specified, this will become the fallback
        and global language instance.

        @param inputStream An InputStream reading all the language data to create the language object from
     */
    void setDefault(InputStream &inputStream);

    /**
        Sets the new current language of this object.

        This will search your language directory for files meeting the language parameter's content.
        Thus, the language parameter has to follow certain criteria.
        It has to be in the format of "yy_xx" where yy is the lang code and xx is the country code.
        These acronyms have to be contained in this class' given lang code file passed on creation.

        @param language The language to set. English in the UK for example: en_gb
     */
    void setCurrentLanguage(const String &language) noexcept;

    //=================================================================================================================
    /**
        Gets the current global LocalisedString object.

        This is just a convenience method, if you want static access to the object returned by this, just call:
        LocalisedStrings::getCurrentMappings()

        @return The current global LocalisedStrings instance if set in advance, else a null pointer
     */
    LocalisedStrings *getLocale() const noexcept;

    //=================================================================================================================
    friend void swap(Localisation &left, Localisation &right) noexcept
    {
        left.defaultLocale.swap(right.defaultLocale);
        left.langCodes.swap(right.langCodes);
        std::swap(left.rootDir, right.rootDir);
    }

private:
    File rootDir;
    LangCodeMap langCodes;
    std::unique_ptr<LocalisedStrings> defaultLocale;
};
}