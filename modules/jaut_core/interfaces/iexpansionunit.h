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
    @file   iexpansionunit.h
    @date   10, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class JAUT_API IExpansionUnit
{
public:
    virtual ~IExpansionUnit() = default;
    
    //==================================================================================================================
    /**
     *  Adds a new IExpandable to the ExpansionUnit.
     *  @param The new ITranslationObject
     */
    virtual void addExpandable(std::unique_ptr<IExpandable> expandable) = 0;
    
    //==================================================================================================================
    /**
     *  Calculates and returns the amount of expansions to generate.
     *
     *  @param The input string to process
     *  @return The number of expansions
     */
    virtual int getNumberOfRepetitions(const juce::String &input) const = 0;
    
    /**
     *  Gets the Regex sequence which evaluates the placeholder syntax.
     *  @return The Regex pattern
     */
    virtual std::string getValidatorSequence() const = 0;
    
    //==================================================================================================================
    /**
     *  Expands the input into their respective substitutes.
     *
     *  @param input The input string to process
     *  @return The array of string containing all expanded strings
     */
    virtual juce::StringArray expandTranslations(const juce::String &input) const = 0;
    
    //==================================================================================================================
    /**
     *  Gets the full map of IExpandable(s)
     *  @return The map
     */
    virtual const std::unordered_map<juce::String, std::unique_ptr<IExpandable>>& getExpandables() const = 0;
};
}
