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
    @file   imetareader.h
    @date   27, July 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/** This interface provides the base structure for the theme data reader of your themes-system. */
class IMetaReader
{
public:
    virtual ~IMetaReader() = default;
    
    //==================================================================================================================
    /**
     *  Reads a metadata file and returns new memory for it.
     *
     *  @param stream The stream to read the metadata from
     *  @return The metadata object
     */
    virtual IMetadata* parseMetadata(juce::InputStream &stream) = 0;
};
}
