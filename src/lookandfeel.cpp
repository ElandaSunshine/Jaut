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
    @file   lookandfeel.cpp
    @date   16, October 2019
    
    ===============================================================
 */

#include <jaut/lookandfeel.h>
#include <jaut/fontformat.h>

namespace jaut
{
LookAndFeel::LookAndFeel() noexcept
{
    // CharFormat
    setColour(CharFormat::ColourFormat0Id,           Colours::white);
    setColour(CharFormat::ColourFormat1Id,           Colours::red);
    setColour(CharFormat::ColourFormat2Id,           Colours::orange);
    setColour(CharFormat::ColourFormat3Id,           Colours::yellow);
    setColour(CharFormat::ColourFormat4Id,           Colours::green);
    setColour(CharFormat::ColourFormat5Id,           Colours::blue);
    setColour(CharFormat::ColourFormat6Id,           Colours::indigo);
    setColour(CharFormat::ColourFormat7Id,           Colours::violet);
    setColour(CharFormat::ColourFormat8Id,           Colours::purple);
    setColour(CharFormat::ColourFormat9Id,           Colours::pink);
    setColour(CharFormat::ColourFormatAId,           Colours::silver);
    setColour(CharFormat::ColourFormatBId,           Colours::gold);
    setColour(CharFormat::ColourFormatCId,           Colours::beige);
    setColour(CharFormat::ColourFormatDId,           Colours::brown);
    setColour(CharFormat::ColourFormatEId,           Colours::grey);
    setColour(CharFormat::ColourFormatFId,           Colours::black);
}
}
