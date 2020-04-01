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
    @file   nativetags.h
    @date   06, March 2020

    ===============================================================
 */

#pragma once

namespace jaut::reflect
{
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
/**
 *  The types of targets a tag can be attached to.
 */
enum JAUT_API TargetType : unsigned
{
    /** Determines that tags can be attached to class constructors. */
    Constructor = 1,
    /** Determines that tags can be attached to class variables. */
    Field       = 2,
    /** Determines that tags can be attached to class functions. */
    Function    = 4,
    /** Determines that tags can be attached to tag declarations. */
    Tag         = 8,
    /** Determines that tags can be attached to classes. */
    Type        = 16
};

/**
 *  Defines the target-type the tag can be attached to.
 *  If a tag is attached to an unsupported context, this will, depending on the configuration, either throw an exception
 *  or just print out a warning and will be ignored.
 *
 *  If a tag does not specify any targets, this will default to all targets.
 *  @see jaut::reflect::TargetType
 */
struct JAUT_API Target final : public ReflTag
{
    /**
     *  The types of targets this tag can be attached to.
     *  @see jaut::reflect::TargetType
     */
    unsigned target { Constructor | Field | Function | Tag | Type };
};



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
/**
 *  This enum defines the life-scope of a tag.
 *
 *  <table>
 *      <caption>Storage types:</caption>
 *      <tr>
 *          <th>Type</th>
 *          <th>Summary</th>
 *          <th>Reflective</th>
 *      <tr>
 *      <tr>
 *          <td>Class</td>
 *          <td>Class specified tags are unique for each class declaration and its members,
 *              every class declaration and its declared members will have one tag object across instances.</td>
 *          <td>Yes</td>
 *      <tr>
 *      <tr>
 *          <td>Object&nbsp;</td>
 *          <td>
 *              Object specified tags are unique for each instance of a class and its members,
 *              every instance of a class and its members will have its own tag object.
 *          </td>
 *          <td>Yes</td>
 *      <tr>
 *      <tr>
 *          <td>Source</td>
 *          <td>Source specified tags only serve a descriptive purpose and will be completely ignored.</td>
 *          <td>No</td>
 *      <tr>
 *  </table>
 */
enum JAUT_API StorageType
{
    /**
     *  Tags will be unique for each class declaration and its members, every class declaration and its declared members
     *  will have one tag object across all instances.
     */
    Class,

    /**
     *  Tags will be unique for each instance of a class and its members, every instance of a class and its members will
     *  have its own tag object.
     */
    Object,

    /**
     *  Tags will only serve a descriptive purpose and will be completely ignored.
     */
    Source
};

/**
 *  The StoragePolicy class is a tag-class only tag.
 *  This will determine the life-time of a tag and its scope of use.
 *  If no StoragePolicy is specified, it will default to StorageType::Class.
 *
 *  @see jaut::reflect::StorageType
 */
struct JAUT_API StoragePolicy final : public ReflTag
{
    /**
     *  The storage scope of the refltag.
     *  @see jaut::reflect::StorageType
     */
    StorageType storage { Class };
};



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
}