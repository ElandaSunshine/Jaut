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
    @file   expandables.cpp
    @date   11, March 2020
    
    ===============================================================
 */


#include "expandables.h"

using namespace juce;

namespace
{
using namespace jaut;

enum EvaluationType
{
    Numeric,
    Alphabetic,
    Mismatch
};

inline bool isValidNumber(const String &input)
{
    String::CharPointerType char_ptr = input.getCharPointer();
    
    for(int i = 0; i < char_ptr.length(); ++i)
    {
        const juce_wchar ch = char_ptr.getAndAdvance();
        
        if(i > 0)
        {
            if(!CharacterFunctions::isDigit(ch))
            {
                return false;
            }
        }
        else if(!CharacterFunctions::isDigit(ch) && ch != '-')
        {
            return false;
        }
    }
    
    return true;
}

inline EvaluationType getEvaluationType(const String &start, const String &end)
{
    if(isValidNumber(start) && isValidNumber(end))
    {
        return Numeric;
    }
    else if(start.containsOnly("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") &&
            end.containsOnly  ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
    {
        return Alphabetic;
    }
    
    return Mismatch;
}
}

namespace jaut
{
// region ExpandableIterator

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
juce::String ExpandableIterator::getName() const
{
    return "Iterate";
}

juce::String ExpandableIterator::getSummary() const
{
    return R"(Iterates the alphanumerical value by the specified range.
              If no range is specified this will either start by 0 or A, depending on the specified value.
              Numbers and letters can not be combined!')";
}

juce::String ExpandableIterator::getRangeStartIfEmpty(const juce::String &value) const
{
    if(value.containsOnly("-0123456789"))
    {
        return "0";
    }
    
    return CharacterFunctions::isLowerCase(*(--value.getCharPointer().findTerminatingNull())) ? "a" : "A";
}

juce::String ExpandableIterator::getInstruction() const
{
    return "it";
}

std::size_t ExpandableIterator::getRangeSize(const juce::String &start, const juce::String &end) const
{
    const EvaluationType type = ::getEvaluationType(start, end);
    
    if(type == Numeric)
    {
        return static_cast<int>(std::abs(start.getLargeIntValue() - end.getLargeIntValue()) + 1);
    }
    else if(type == Alphabetic)
    {
        AlphabetIterator start_it(start);
        AlphabetIterator end_it(end);
        return std::abs(static_cast<int>(start_it.toDecimal()) - static_cast<int>(end_it.toDecimal())) + 1;
    }
    
    return 0;
}

//======================================================================================================================
bool ExpandableIterator::inputMatches(const juce::String &start, const juce::String &end) const
{
    return ::getEvaluationType(start, end) != Mismatch;
}

//======================================================================================================================
bool ExpandableIterator::isRanged() const
{
    return true;
}

//======================================================================================================================
juce::String ExpandableIterator::expand(std::size_t counter, const juce::String &start, const juce::String &end,
                                        IExpandable::ExhaustionBehaviour behaviour) const
{
    const EvaluationType type = ::getEvaluationType(start, end);
    const bool reset = (behaviour == Reset);
    bool exhausted   = false;
    String result;
    
    if(type == Numeric)
    {
        const long long end_i   = end.getLargeIntValue();
        const long long start_i = start.getLargeIntValue();
        long long result_i;
    
        if(start_i > end_i)
        {
            result_i  = start_i - static_cast<long long>(reset ? counter : counter % getRangeSize(start, end));
            exhausted = (result_i < end_i);
        }
        else
        {
            result_i  = start_i + static_cast<long long>(reset ? counter : counter % getRangeSize(start, end));
            exhausted = (result_i > end_i);
        }
    
        result = String(result_i);
    }
    else if(type == Alphabetic)
    {
        const std::size_t end_i   = AlphabetIterator::stringToDecimal(end);
        const std::size_t start_i = AlphabetIterator::stringToDecimal(start);
        std::size_t result_i;
    
        if(start_i > end_i)
        {
            result_i  = start_i - (reset ? counter : counter % getRangeSize(start, end));
            exhausted = (result_i < end_i);
        }
        else
        {
            result_i  = start_i + (reset ? counter : counter % getRangeSize(start, end));
            exhausted = (result_i > end_i);
        }
    
        result = AlphabetIterator::decimalToString(result_i, end);
    }
    
    if (exhausted)
    {
        switch (behaviour)
        {
            case Static: return end;
            case Omit:   return "";
            default:;
        }
    }
    
    return result;
}
// endregion ExpandableIterator
// region Multiplier

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
juce::String ExpandableMultiplier::getName() const
{
    return "Multiply";
}

juce::String ExpandableMultiplier::getSummary() const
{
    return R"(Multiplies the alphanumerical value by the specified range and expands its value over each line.
              If no range is specified this will either start by 0 or A, depending on the specified value.
              Numbers and letters can not be combined!')";
}

juce::String ExpandableMultiplier::getRangeStartIfEmpty(const juce::String &value) const
{
    return iterator.getRangeStartIfEmpty(value);
}

juce::String ExpandableMultiplier::getInstruction() const
{
    return "mp";
}

std::size_t ExpandableMultiplier::getRangeSize(const juce::String &start, const juce::String &end) const
{
    return iterator.getRangeSize(start, end);
}

//======================================================================================================================
bool ExpandableMultiplier::inputMatches(const juce::String &start, const juce::String &end) const
{
    return iterator.inputMatches(start, end);
}

//======================================================================================================================
bool ExpandableMultiplier::isRanged() const
{
    return true;
}

//======================================================================================================================
juce::String ExpandableMultiplier::expand(std::size_t counter, const juce::String &start, const juce::String &end,
                                          IExpandable::ExhaustionBehaviour behaviour) const
{
    const EvaluationType type = ::getEvaluationType(start, end);
    const bool reset = (behaviour == Reset);
    bool exhausted   = false;
    StringArray result;
    
    if(type == Numeric)
    {
        const long long end_i   = end.getLargeIntValue();
        const long long start_i = start.getLargeIntValue();
        long long result_i      = start_i;
        
        if(start_i > end_i)
        {
            const long long length = start_i - static_cast<long long>(reset ? counter : counter % getRangeSize(start, end));
        
            for (; result_i >= length; --result_i)
            {
                result.add("%%prefx%%" + String(result_i) + "%%suffx%%");
            }
    
            exhausted = (length < end_i);
        }
        else if(start_i < end_i)
        {
            const long long length = start_i + static_cast<long long>(reset ? counter : counter % getRangeSize(start, end));
        
            for (; result_i <= length; ++result_i)
            {
                result.add("%%prefx%%" + String(result_i) + "%%suffx%%");
            }
    
            exhausted = (length > end_i);
        }
    }
    else if(type == Alphabetic)
    {
        const AlphabetIterator end_i  (end);
        const AlphabetIterator start_i(start);
        std::size_t result_i = start_i.toDecimal();
    
        if(start_i > end_i)
        {
            const std::size_t length = start_i.toDecimal() - (reset ? counter : counter % getRangeSize(start, end));
        
            for (; result_i >= length; --result_i)
            {
                result.add("%%prefx%%" + AlphabetIterator::decimalToString(result_i) + "%%suffx%%");
            }
        
            exhausted = (length < end_i.toDecimal());
        }
        else if(start_i < end_i)
        {
            const std::size_t length = start_i.toDecimal() + (reset ? counter : counter % getRangeSize(start, end));
        
            for (; result_i <= length; ++result_i)
            {
                result.add("%%prefx%%" + AlphabetIterator::decimalToString(result_i) + "%%suffx%%");
            }
        
            exhausted = (length > end_i.toDecimal());
        }
    }
    
    if (exhausted)
    {
        switch (behaviour)
        {
            case Static: return result.joinIntoString("", 0, getRangeSize(start, end));
            case Omit:   return "";
            default: ;
        }
    }
    
    return result.joinIntoString("");
}
// endregion Multiplier
}
