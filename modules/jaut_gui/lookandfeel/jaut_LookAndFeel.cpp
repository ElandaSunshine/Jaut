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
    @file   jaut_LookAndFeel.cpp
    @date   09, March 2023

    ===============================================================
 */

#include <jaut_gui/lookandfeel/jaut_LookAndFeel.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    JAUT_NODISCARD
    juce::uint32 getArgbFrom(juce::LookAndFeel_V4                         *const laf,
                             juce::LookAndFeel_V4::ColourScheme::UIColour colour) noexcept
    {
        return laf->getCurrentColourScheme().getUIColour(colour).getARGB();
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LookAndFeel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    LookAndFeel::LookAndFeel() noexcept
    {
        using UICol = ColourScheme::UIColour;
        
        struct CEntry
        {
            int           id;
            std::uint32_t colour;
        };
        
        static const std::array colour_defs {
            /*
            // jaut::CharFormat
            CEntry{ CharFormat::FormatColour0Id, juce::Colours::black                .getARGB() },
            CEntry{ CharFormat::FormatColour1Id, juce::Colours::darkblue             .getARGB() },
            CEntry{ CharFormat::FormatColour2Id, juce::Colours::darkgreen            .getARGB() },
            CEntry{ CharFormat::FormatColour3Id, 0xFF00AAAA                                     },
            CEntry{ CharFormat::FormatColour4Id, juce::Colours::darkred              .getARGB() },
            CEntry{ CharFormat::FormatColour5Id, 0xFFAA00AA                                     },
            CEntry{ CharFormat::FormatColour6Id, juce::Colours::gold                 .getARGB() },
            CEntry{ CharFormat::FormatColour7Id, juce::Colours::grey                 .getARGB() },
            CEntry{ CharFormat::FormatColour8Id, juce::Colours::darkgrey             .getARGB() },
            CEntry{ CharFormat::FormatColour9Id, juce::Colours::blue                 .getARGB() },
            CEntry{ CharFormat::FormatColourAId, juce::Colours::green                .getARGB() },
            CEntry{ CharFormat::FormatColourBId, juce::Colours::aqua                 .getARGB() },
            CEntry{ CharFormat::FormatColourCId, juce::Colours::red                  .getARGB() },
            CEntry{ CharFormat::FormatColourDId, 0xFFFF55FF                                     },
            CEntry{ CharFormat::FormatColourEId, juce::Colours::yellow               .getARGB() },
            CEntry{ CharFormat::FormatColourFId, juce::Colours::white                .getARGB() },
            */
            
            // jaut::ExtraComponent
            CEntry{ ExtraComponent::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground) },
            CEntry{ ExtraComponent::ColourBorderId,     ::getArgbFrom(this, UICol::outline)          },
            
            // jaut::ContentPane
            CEntry{ ContentPane::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground) },
            CEntry{ ContentPane::ColourBorderId,     ::getArgbFrom(this, UICol::outline)          },
            
            // jaut::SplitPanel
            CEntry{ SplitPanel::ColourBackgroundId, ::getArgbFrom(this, UICol::windowBackground) },
            CEntry{ SplitPanel::ColourBorderId,     ::getArgbFrom(this, UICol::outline)          },
            CEntry{ SplitPanel::ColourSeparatorId,  ::getArgbFrom(this, UICol::outline)          },
            
            // jaut::MultiPagePanel
            CEntry{ MultiPagePanel::ColourBackgroundId,           ::getArgbFrom(this, UICol::windowBackground) },
            CEntry{ MultiPagePanel::ColourBorderId,               ::getArgbFrom(this, UICol::outline)          },
            CEntry{ MultiPagePanel::ColourTabBackgroundId,        0x00000000                                   },
            CEntry{ MultiPagePanel::ColourTabBackgroundActiveId,  ::getArgbFrom(this, UICol::widgetBackground) },
            CEntry{ MultiPagePanel::ColourTabBackgroundHoverId,   ::getArgbFrom(this, UICol::highlightedFill)  }
        };
        
        for (const auto &[id, col] : colour_defs)
        {
            setColour(id, juce::Colour(col));
        }
    }
}
//======================================================================================================================
// endregion LookAndFeel
//**********************************************************************************************************************
