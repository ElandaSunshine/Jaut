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
    @file   alphabetiterator.h
    @date   11, March 2020
    
    ===============================================================
 */
 
#pragma once

namespace jaut
{
class JAUT_API AlphabetIterator final : public Integral<std::size_t>
{
public:
    static std::size_t stringToDecimal(const juce::String &valueString) noexcept;
    static juce::String decimalToString(std::size_t value, juce::String templ = juce::String());
    
    //==================================================================================================================
    AlphabetIterator() noexcept;
    explicit AlphabetIterator(const juce::String &startSequence) noexcept;
    explicit AlphabetIterator(std::size_t size) noexcept;
    
    //==================================================================================================================
    AlphabetIterator& operator=(const juce::String &data) noexcept;
    
    //==================================================================================================================
    std::size_t toDecimal() const noexcept;
    int length() const noexcept;
    int length() noexcept;
    
    //==================================================================================================================
    juce::String toString() const;
    juce::String toString();
    
    //==================================================================================================================
    void setTemplate(juce::String templ) noexcept;

private:
    static juce::juce_wchar getStartChar(juce::juce_wchar character) noexcept;
    
    //==================================================================================================================
    // Data
    juce::String dataTemplate;
    
    // Cache
    std::size_t previousSize;
    juce::String previousData;
    int previousLength;
    
    //==================================================================================================================
    juce::String getTemplate(int length) const;
};
}
