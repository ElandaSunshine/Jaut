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
    @file   alphabetiterator.cpp
    @date   11, March 2020
    
    ===============================================================
 */
 
namespace jaut
{
std::size_t AlphabetIterator::stringToDecimal(const juce::String &valueString) noexcept
{
    std::size_t size = 0;
    
    if(valueString.containsOnly("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") && !valueString.isEmpty())
    {
        juce::String::CharPointerType data_ptr = valueString.getCharPointer();
        const int data_length = data_ptr.length();
        const int data_last   = data_length - 1;
        
        for(int i = 0; i < data_last; ++i)
        {
            const juce::juce_wchar current_char = data_ptr.getAndAdvance();
            const std::size_t overflows         = (current_char - getStartChar(current_char)) + 1;
            const std::size_t factor            = std::pow(26, data_last - i);
            
            size += overflows * factor;
        }
        
        size += (*data_ptr - getStartChar(*data_ptr)) + 1;
    }
    
    return size;
}

juce::String AlphabetIterator::decimalToString(std::size_t value, juce::String templ)
{
    AlphabetIterator iterator(value);
    iterator.setTemplate(std::move(templ));
    
    return iterator.toString();
}

//======================================================================================================================
AlphabetIterator::AlphabetIterator() noexcept
    : previousSize(0),
      previousLength(0)
{}

AlphabetIterator::AlphabetIterator(const juce::String &startSequence) noexcept
    : Numeric(stringToDecimal(startSequence)),
      previousSize(0), previousLength(0), dataTemplate(startSequence)
{}

AlphabetIterator::AlphabetIterator(std::size_t size) noexcept
    : Numeric(size),
previousSize(0), previousLength(0)
{}

//======================================================================================================================
AlphabetIterator& AlphabetIterator::operator=(const juce::String &data) noexcept
{
    numericValue = stringToDecimal(data);
    return *this;
}

//======================================================================================================================
std::size_t AlphabetIterator::toDecimal() const noexcept
{
    return numericValue;
}

int AlphabetIterator::length() const noexcept
{
    if(previousSize == numericValue)
    {
        return previousLength;
    }
    
    int length = 0;
    std::size_t size = numericValue - 1;
    
    if(numericValue > 0)
    {
        ++length;
        
        while(size >= 26)
        {
            size = (size / 26) - 1;
            ++length;
        }
    }
    
    return length;
}

int AlphabetIterator::length() noexcept
{
    if(numericValue != previousSize)
    {
        const AlphabetIterator &ait = *this;
        previousLength = ait.length();
        previousData   = std::move(ait.toString());
        previousSize   = numericValue;
    }
    
    return previousLength;
}

//======================================================================================================================
juce::String AlphabetIterator::toString() const
{
    if(numericValue == previousSize)
    {
        return previousData;
    }
    
    const int result_len      = length();
    std::size_t result_size   = numericValue - 1;
    juce::String result_templ = getTemplate(result_len);
    juce::String::CharPointerType result_ptr   = result_templ.getCharPointer().findTerminatingNull();
    
    juce::String result;
    result.preallocateBytes(result_len);
    
    for(int i = 0; i < result_len; i++)
    {
        const juce::juce_wchar start_char = *(--result_ptr);
        result = juce::String::charToString(start_char + (result_size % 26)) + result;
        result_size = (result_size / 26) - 1;
    }
    
    return result;
}

juce::String AlphabetIterator::toString()
{
    if(numericValue != previousSize)
    {
        const AlphabetIterator &ait = *this;
        previousLength = ait.length();
        previousData   = std::move(ait.toString());
        previousSize   = numericValue;
    }
    
    return previousData;
}

//======================================================================================================================
void AlphabetIterator::setTemplate(juce::String templ) noexcept
{
    dataTemplate = std::move(templ);
}

//======================================================================================================================
juce::juce_wchar AlphabetIterator::getStartChar(juce::juce_wchar character) noexcept
{
    return juce::CharacterFunctions::isLowerCase(character) ? 'a' : 'A';
}

//======================================================================================================================
juce::String AlphabetIterator::getTemplate(int length) const
{
    // Template data
    const juce::String::CharPointerType templ_ptr = dataTemplate.getCharPointer();
    const int templ_length = templ_ptr.length();
    const int templ_last   = templ_length - 1;
    
    // New data
    juce::String templ_result;
    const int new_last = length - 1;
    juce::juce_wchar prev_char    = 'A';
    
    templ_result.preallocateBytes(length);
    
    for(int i = new_last; i >= 0; --i)
    {
        const int templ_index = templ_last + new_last - i;
        
        if(fit(templ_index, 0, templ_length))
        {
            prev_char = getStartChar(*(templ_ptr + templ_index));
        }
        
        templ_result = juce::String::charToString(prev_char) + templ_result;
    }
    
    return templ_result;
}
}
