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
    @file   iexpandable.h
    @date   10, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  Represents an IExpandable object which defines the structure of an "placeholder replacer" that, internal then,
 *  can be expanded and propagated by an IExpansionUnit.
 *
 *  Note that this class' documentation mainly focuses on the default jaut::ExpansionUnit class, if you use any other
 *  IExpansionUnit implementation, these class' rules may not apply.
 *  In this case, refer to the implementation to find out what rules apply.
 *
 *  @see jaut::IExpansionUnit
 *  @see jaut::ExpansionUnit
 */
class JAUT_API IExpandable
{
public:
    /**
     *  If the IExpansionUnit's count exceeds an IExpandable's expansion range, then this will define the behaviour of
     *  the values it will expand next.
     *
     *  <table>
     *      <caption>Types</caption>
     *      <tr>
     *          <th>Type</th>
     *          <th>Description</th>
     *          <th>Flag</th>
     *      </tr>
     *      <tr>
     *          <td>Dynamic&nbsp;</td>
     *          <td>Continues the range even though it was already exceeded.</td>
     *          <td>d</td>
     *      </tr>
     *      <tr>
     *          <td>Omit</td>
     *          <td>Entirely removes the value from the result.</td>
     *          <td>o</td>
     *      </tr>
     *      <tr>
     *          <td>Reset</td>
     *          <td>Resets the value and lets it start from the beginning.</td>
     *          <td>r</td>
     *      </tr>
     *      <tr>
     *          <td>Static</td>
     *          <td>Inserts the end value of the range over and over again.</td>
     *          <td>s</td>
     *      </tr>
     *  </table>
     */
    enum JAUT_API ExhaustionBehaviour
    {
        Dynamic,
        Omit,
        Reset,
        Static
    };
    
    //==================================================================================================================
    virtual ~IExpandable() = default;
    
    //==================================================================================================================
    /**
     *  Gets the textual representation of this IExpandable object.
     *  @return The name
     */
    virtual juce::String getName() const = 0;
    
    /**
     *  Returns a short description of the IExpandable object.
     *  @return A brief description
     */
    virtual juce::String getSummary() const = 0;
    
    /**
     *  Gets whether the input matches this IExpandable's requirements for the range.
     *
     *  @param start The range start data to check (may be empty)
     *  @param end   The range end data to check
     *  @return True if it matches, false if not
     */
    virtual bool inputMatches(const juce::String &start, const juce::String &end) const = 0;
    
    /**
     *  Determines whether this specifies a range or a single value.
     *  If this returns true, you can specify ranges delimited by ':', else it only accepts single values.
     *
     *  @return True if ranged, false if single-valued
     */
    virtual bool isRanged() const = 0;
    
    /**
     *  If the value is not ranged, this will be the substitute for range start depending on the value given.
     *
     *  @param value The current single range value
     *  @return The start of the range
     */
    virtual juce::String getRangeStartIfEmpty(const juce::String &value) const = 0;
    
    /**
     *  Gets the instruction this IExpandable should be represented by.
     *  Allowed are alphanumerical values and underscores.
     *
     *  @return The symbol
     */
    virtual juce::String getInstruction() const = 0;
    
    /**
     *  Calculates the size of the given range.
     *
     *  @param start The start of the range (may be empty)
     *  @param end   The end of the range
     *  @return The size of the range
     */
    virtual std::size_t getRangeSize(const juce::String &start, const juce::String &end) const = 0;
    
    /**
     *  Expands the given range into the desired text and returns it for the IExpansionUnit.
     *
     *  @param counter The current loop count
     *  @param start   Range start (may be empty)
     *  @param end     Range end
     *  @return The expanded string
     */
    virtual juce::String expand(std::size_t counter, const juce::String &start, const juce::String &end,
                                ExhaustionBehaviour behaviour) const = 0;
};
}
