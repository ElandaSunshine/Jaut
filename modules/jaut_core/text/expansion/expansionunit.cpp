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
    @file   expansionunit.cpp
    @date   10, March 2020
    
    ===============================================================
 */

#include <JuceHeader.h>
#include <regex>



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
using namespace juce;

//======================================================================================================================
namespace
{
using namespace jaut;

enum RegexID
{
    RIDExpression  =  1,
    RIDInstruction =  2,
    RIDFlag        =  4,
    RIDPrefix      =  6,
    RIDRangeStart  =  7,
    RIDRangeEnd    =  9,
    RIDSuffix      = 11
};

struct Match final
{
    using Behaviour = jaut::IExpandable::ExhaustionBehaviour;
    
    //==================================================================================================================
    const juce::String fullMatch;
    const juce::String instruction;
    const juce::String prefix;
    const juce::String suffix;
    const juce::String rangeStart;
    const juce::String rangeEnd;
    const Behaviour    behaviour;
    
    //==================================================================================================================
    Match(String match, String inst, String pref, String suff, String rs, String re, Behaviour be)
        : fullMatch(std::move(match)), instruction(std::move(inst)), prefix(std::move(pref)),
          suffix(std::move(suff)), rangeStart(std::move(rs)), rangeEnd(std::move(re)), behaviour(be)
    {}
};

//======================================================================================================================
inline int getSizeInMode(int oldValue, int newValue, bool max)
{
    return max ? std::max(oldValue, newValue) : std::min(oldValue, newValue);
}

inline jaut::IExpandable::ExhaustionBehaviour getBehaviourForFlag(const String &flag,
                                                                  jaut::IExpandable::ExhaustionBehaviour def)
{
    using Behaviour = jaut::IExpandable::ExhaustionBehaviour;
    
    if(flag.equalsIgnoreCase("d"))
    {
        return Behaviour::Dynamic;
    }
    else if(flag.equalsIgnoreCase("o"))
    {
        return Behaviour::Omit;
    }
    else if(flag.equalsIgnoreCase("r"))
    {
        return Behaviour::Reset;
    }
    else if (flag.equalsIgnoreCase("s"))
    {
        return Behaviour::Static;
    }
    
    return def;
}

inline String getMatchResult(int index, const std::smatch &match, const String &defaultValue)
{
    return match[index].matched ? match.str(index) : defaultValue;
}

inline String getMatchResult(int index, const std::smatch &match)
{
    return getMatchResult(index, match, "");
}

inline int prepareMatches(const jaut::ExpansionUnit &eu, const String &input,
                                       std::vector<Match> *matches = nullptr)
{
    std::regex line_pattern (eu.getValidatorSequence());
    std::string std_input   (input.toStdString());
    
    int last_size = eu.getOptions().countToHighest ? 0 : std::numeric_limits<int>::max();
    
    for (std::sregex_iterator it = std::sregex_iterator(std_input.begin(), std_input.end(), line_pattern);
         it != std::sregex_iterator(); ++it)
    {
        const std::smatch line_match = *it;
        const String instruction = String(line_match.str(RIDInstruction));
        const auto iterator      = eu.getExpandables().find(instruction.trim().toLowerCase());
        
        if(iterator != eu.getExpandables().end())
        {
            const jaut::IExpandable &expandable = *iterator->second;
            String range_start = line_match.str(RIDRangeStart);
            String range_end   = ::getMatchResult(RIDRangeEnd, line_match);
            
            if(!expandable.isRanged() && !range_end.isEmpty())
            {
                range_end   = range_start + ":" + range_end;
                range_start = expandable.getRangeStartIfEmpty(range_end);
            }
            else if(range_end.isEmpty())
            {
                range_end = expandable.getRangeStartIfEmpty(range_start);
                std::swap(range_end, range_start);
            }
            
            if(!expandable.inputMatches(range_start, range_end))
            {
                continue;
            }
            
            const int new_size = expandable.getRangeSize(range_start, range_end);
            
            if(new_size > 0)
            {
                last_size = ::getSizeInMode(last_size, new_size, eu.getOptions().countToHighest);
    
                if(matches)
                {
                    matches->emplace_back(String(line_match.str(RIDExpression)), instruction,
                                          ::getMatchResult(RIDPrefix, line_match),
                                          ::getMatchResult(RIDSuffix, line_match), range_start, range_end,
                                          ::getBehaviourForFlag(::getMatchResult(RIDFlag, line_match),
                                                                eu.getOptions().exhaustionBehaviour));
                }
            }
        }
    }
    
    return last_size == std::numeric_limits<int>::max() ? 0 : last_size;
}
}



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
namespace jaut
{
ExpansionUnit::ExpansionUnit(Options options) noexcept
    : options(options)
{}

//======================================================================================================================
void ExpansionUnit::addExpandable(Expandable expandable)
{
    const String id = expandable->getInstruction().trim().toLowerCase();
    
    if (expandables.find(id) == expandables.end())
    {
        expandables.emplace(id, std::move(expandable));
    }
}

//======================================================================================================================
int ExpansionUnit::getNumberOfRepetitions(const String &input) const
{
    return ::prepareMatches(*this, input, nullptr);
}

std::string ExpansionUnit::getValidatorSequence() const
{
    /* Notable groups:
     * 1:  Full expression
     * 2:  Instruction
     * 4:  Flag: d, o, r, s (For more: jaut::Translator::Options::ExhaustionBehaviour)
     * 6:  Prefix
     * 7:  Range start
     * 9:  Range end
     * 11: Suffix */
    
    return R"((\{\s*([a-zA-Z0-9]+?)(\?([dorsDORS]))?\s*=\s*(\<([^><]+?)\>)?)"
           R"(\s*\[\s*([^:[]+?)\s*(\:\s*([^:\]]+?))?\s*\]\s*\s*(\<([^><]+?)\>)?\s*\}))";
}

//======================================================================================================================
StringArray ExpansionUnit::expandTranslations(const juce::String &input) const
{
    std::vector<Match> matches;
    const int repetitions = ::prepareMatches(*this, input, &matches);
    
    StringArray lines;
    lines.ensureStorageAllocated(repetitions);
    
    for(int c = 0; c < repetitions; ++c)
    {
        String result = input;
        
        for(auto &match : matches)
        {
            IExpandable &expandable = *expandables.at(match.instruction.trim().toLowerCase());
            const String expansion  = expandable.expand(c, match.rangeStart, match.rangeEnd, match.behaviour)
                                                .replace("%%prefx%%", match.prefix).replace("%%suffx%%", match.suffix);
            result = result.replace(match.fullMatch, expansion, false);
        }
        
        lines.add(result);
    }
    
    return lines;
}

//======================================================================================================================
const ExpansionUnit::ExpandableMap &ExpansionUnit::getExpandables() const
{
    return expandables;
}

const ExpansionUnit::Options &ExpansionUnit::getOptions() const noexcept
{
    return options;
}
}
