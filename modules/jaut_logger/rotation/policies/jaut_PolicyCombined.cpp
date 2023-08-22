/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    @file   jaut_PolicyCombined.cpp
    @date   26, January 2023

    ===============================================================
 */

#include <jaut_logger/rotation/policies/jaut_PolicyCombined.h>



//**********************************************************************************************************************
// region PolicyCombined
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    PolicyCombined::PolicyCombined(std::vector<RotationPolicy> parPolicies) noexcept
        : policies(std::move(parPolicies))
    {}
    
    //==================================================================================================================
    bool PolicyCombined::operator()(const RotationPolicyArgs &parRotationArgs) const noexcept
    {
        for (const RotationPolicy &policy : policies)
        {
            if (policy(parRotationArgs))
            {
                return true;
            }
        }
        
        return false;
    }
}
//======================================================================================================================
// endregion PolicyCombined
//**********************************************************************************************************************
