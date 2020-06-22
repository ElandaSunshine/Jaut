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

    @author Elanda (elanda@elandasunshine.xyz)
    @file   tag.h
    @date   06, March 2020

    ===============================================================
 */

#pragma once

namespace jaut::reflect
{
/**
 *  The ReflTag class is an interface for custom tags that can be attached to different targets.
 *  Tags usually don't contain any function but only fields that give information of the target.
 */
struct JAUT_API ReflTag { virtual ~ReflTag() = default; };

/**
 *  The ServiceTag class is a special tag interface that can execute tasks when encountered.
 *  This are only useful in combination with tag declarations, as these define special behaviour of ReflTags.
 */
struct JAUT_API ServiceTag
{
    //==================================================================================================================
    virtual ~ServiceTag() = default;

    //==================================================================================================================
    virtual bool perform(const ReflTag &tag) = 0;

    //==================================================================================================================
    virtual juce::String getName() const = 0;
};
}
