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
    @file   expandables.h
    @date   11, March 2020
    
    ===============================================================
 */
 
#pragma once

namespace jaut
{
class JAUT_API ExpandableIterator final : public IExpandable
{
public:
    juce::String getName() const override;
    juce::String getSummary() const override;
    juce::String getRangeStartIfEmpty(const juce::String &value) const override;
    juce::String getInstruction() const override;
    std::size_t getRangeSize(const juce::String &start, const juce::String &end) const override;
    
    //==================================================================================================================
    bool inputMatches(const juce::String &start, const juce::String &end) const override;
    
    //==================================================================================================================
    bool isRanged() const override;
    
    //==================================================================================================================
    juce::String expand(std::size_t counter, const juce::String &start, const juce::String &end,
                        ExhaustionBehaviour behaviour) const override;
};

class JAUT_API ExpandableMultiplier final : public IExpandable
{
public:
    juce::String getName() const override;
    juce::String getSummary() const override;
    juce::String getRangeStartIfEmpty(const juce::String &value) const override;
    juce::String getInstruction() const override;
    std::size_t getRangeSize(const juce::String &start, const juce::String &end) const override;
    
    //==================================================================================================================
    bool inputMatches(const juce::String &start, const juce::String &end) const override;
    
    //==================================================================================================================
    bool isRanged() const override;
    
    //==================================================================================================================
    juce::String expand(std::size_t counter, const juce::String &start, const juce::String &end,
                        ExhaustionBehaviour behaviour) const override;

private:
    ExpandableIterator iterator;
};
}
