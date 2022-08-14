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

    Copyright (c) 2022 ElandaSunshine
    ===============================================================

    @author Elanda
    @file   virtualparameter.h
    @date   14, July 2020
    
    ===============================================================
 */

namespace jaut
{
class JAUT_API VirtualParameter : public juce::RangedAudioParameter
{
public:
    VirtualParameter(const juce::String &id, const juce::String &name, const juce::String &label)
        : RangedAudioParameter(id, name, label, juce::AudioProcessorParameter::genericParameter)
    {}
    
    //==================================================================================================================
    void remapTo(juce::AudioProcessorParameter *parameter)
    {
        std::swap(attachedParameter, parameter);
    }
    
    //==================================================================================================================
    float getValue() const override
    {
        return attachedParameter ? attachedParameter->getValue() : getDefaultValue();
    }
    
    void setValue (float newValue) override
    {
        if (attachedParameter)
        {
            attachedParameter->setValue(newValue);
        }
    }
    
    virtual float getDefaultValue() const override
    {
        return 1.0f;
    }
    
    //==================================================================================================================
    const juce::NormalisableRange<float>& getNormalisableRange() const override
    {
        return range;
    }

private:
    juce::NormalisableRange<float> range { 0.0f, 1.0f };
    juce::AudioProcessorParameter *attachedParameter;
};
}
