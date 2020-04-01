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
    
    @author Elanda
    @file   expansionunit.h
    @date   10, March 2020
    
    ===============================================================
 */
 
#pragma once

namespace jaut
{
/**
 *  The ExpansionUnit class provides a simple class for expanding placeholders in their respective text appearance.
 *  It contains a collection of IExpandable(s) which the ExpansionUnit can sift through to find the corresponding
 *  IExpandable define and then expands as many statements as it can.
 *
 *  @see jaut::IExpandable
 */
class JAUT_API ExpansionUnit : public IExpansionUnit
{
public:
    /**
     *  This struct defines the behaviour of an ExpansionUnit instance.
     */
    struct JAUT_API Options final
    {
        /**
         *  Determines the default behaviour once the count exceeds the limit of the range of an IExpandable.
         *  This can be set individually for each placeholder, default is Static.
         *  @see jaut::IExpandable::ExhaustionBehaviour
         */
        IExpandable::ExhaustionBehaviour exhaustionBehaviour { IExpandable::Static };
        
        /**
         *  If this is set to true, the counter will generate statements of the amount of the highest range,
         *  otherwise will count only until the lowest range was satisfied.
         */
        bool countToHighest { true };
        
        /**
         *  If this is set to true, full specified but invalid placeholders will be removed, otherwise will be left
         *  as a whole.
         */
        bool deleteInvalid { true };
    };
    //==================================================================================================================
    using Expandable     = std::unique_ptr<IExpandable>;
    using ExpandableMap  = std::unordered_map<juce::String, Expandable>;
    using Iterator       = ExpandableMap::iterator;
    using ConstInterator = ExpandableMap::const_iterator;
    
    //==================================================================================================================
    /**
     *  Constructs a new ExpansionUnit object with default options.
     */
    ExpansionUnit() noexcept = default;
    
    /**
     *  Constructs a new ExpansionUnit object.
     *  @param options The options to apply to this ExpansionUnit object
     */
    explicit ExpansionUnit(Options options) noexcept;
    
    //==================================================================================================================
    /**
     *  Adds a new IExpandable to the ExpansionUnit.
     *  @param The new ITranslationObject
     */
    void addExpandable(Expandable expandable) override;
    
    //==================================================================================================================
    /**
     *  Calculates and returns the amount of expansions to generate.
     *
     *  @param The input string to process
     *  @return The number of expansions
     */
    int getNumberOfRepetitions(const juce::String &input) const override;
    
    /**
     *  Gets the Regex sequence which evaluates the placeholder syntax.
     *  @return The Regex pattern
     */
    std::string getValidatorSequence() const override;
    
    //==================================================================================================================
    /**
     *  Expands the input into their respective substitutes.
     *
     *  @param input The input string to process
     *  @return The array of string containing all expanded strings
     */
    juce::StringArray expandTranslations(const juce::String &input) const override;
    
    //==================================================================================================================
    /**
     *  Gets the full map of IExpandable(s)
     *  @return The map
     */
    const ExpandableMap& getExpandables() const override;
    
    /**
     *  Gets this object's options.
     *  @return The options
     */
    const Options& getOptions() const noexcept;
    
private:
    Options options;
    ExpandableMap expandables;
    
    JUCE_DECLARE_NON_COPYABLE(ExpansionUnit)
};
}
