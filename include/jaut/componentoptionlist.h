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
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   checkboxboard.h
    @date   14, October 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <bitset>

namespace jaut
{
class JAUT_API OptionList final : public Component, Timer
{
public:
    class JAUT_API OptionBox final : public Rectangle<int>
    {
    public:
        struct JAUT_API Listener
        {
            enum JAUT_API OptionStateType
            {
                Activation,
                Availability,
                Visibility
            };

            virtual ~Listener() {}
            virtual void stateChanged(OptionBox &optionBox, OptionStateType stateType) = 0;
        };

        OptionBox() noexcept;
        explicit OptionBox(const String &label, bool checked = false) noexcept;
        OptionBox(const OptionBox &other) = delete;
        OptionBox(OptionBox &&other) noexcept;

        //==============================================================================================================
        OptionBox &operator=(const OptionBox &right) = delete;
        OptionBox &operator=(OptionBox &&right) noexcept;

        //==============================================================================================================
        void repaint() noexcept;
        void repaint(int x, int y, int width, int height) noexcept;

        //==============================================================================================================
        void setEnabled(bool enabled) noexcept;
        void setVisible(bool visible) noexcept;
        void setChecked(bool checked) noexcept;
        void setLabel(const String &label) noexcept;
        void setBounds(const Rectangle<int> &bounds) noexcept;

        //==============================================================================================================
        const bool isEnabled() const noexcept;
        const bool isVisible() const noexcept;
        const bool isChecked() const noexcept;
        const String getLabel() const noexcept;
        const bool isMouseOver() const noexcept;
        const bool isMouseDown() const noexcept;

        //==============================================================================================================
        void addListener(Listener *listener) noexcept;
        void removeListener(Listener *listener) noexcept;

        //==============================================================================================================
        friend void swap(OptionBox &left, OptionBox &right) noexcept
        {
            std::swap(left.label, right.label);
            std::swap(left.optionFlags, right.optionFlags);
            std::swap(left.list, right.list);
        }        

    private:
        friend class OptionList;

        ListenerList<Listener> listeners;
        String label;
        std::bitset<3> optionFlags;
        OptionList *list;
        bool mouseOver;
        bool mouseDown;
    };

    struct JAUT_API Listener
    {
        virtual ~Listener() {}
        virtual void optionBoxStateChanged(OptionBox &optionBox) noexcept = 0;
        virtual void optionBoxAdded(OptionBox &optionBox) noexcept {}
    };

    enum JAUT_API ColourIds
    {
        ColourListBackgroundId    = JAUT_COLOUR_ID<0>,
        ColourOptionBoxEnabledId  = JAUT_COLOUR_ID<1>,
        ColourOptionBoxDisabledId = JAUT_COLOUR_ID<2>,
        ColourOptionLabelId       = JAUT_COLOUR_ID<3>
    };

    enum JAUT_API LabelDirection
    {
        Left,
        Right,
        None
    };

    enum JAUT_API TickType
    {
        Single,
        Multiple
    };
    
    struct JAUT_API LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() = default;

        virtual int getOptionListLabelWidth(const String &label) = 0;
        virtual int getOptionListSpaceBetweenLabelAndBox() = 0;
        virtual void drawOptionListBackground(Graphics &g, int x, int y, int width, int height) = 0;
        virtual void drawOptionListOptionBox(Graphics &g, Rectangle<int> bounds, bool isCheckBox, bool checked,
                                            bool enabled, bool isMouseOver, bool isMouseDown)  = 0;
        virtual void drawOptionListOptionLabel(Graphics &g, const String &label, Rectangle<int> bounds, bool isCheckBox,
                                            bool isRightAligned, bool checked, bool enabled,  bool isMouseOver,
                                            bool isMouseDown) = 0;
    };

    //==================================================================================================================
    OptionList(const String &componentName, TickType tickType, LabelDirection labelDirection = Right);

    //==================================================================================================================
    void add(OptionBox &optionBox, int index = -1) noexcept;
    void add(const Rectangle<int> &bounds, const String &label, bool checked = false, bool visible = true,
             int index = -1) noexcept;

    //==================================================================================================================
    const OptionBox &get(int index) const noexcept;
    OptionBox &get(int index) noexcept;
    const int getFirstCheckedIndex() const noexcept;
    const int getLastCheckedIndex() const noexcept;
    const bool isCapturingBoxMouseEvents() const noexcept;

    //==================================================================================================================
    void shouldCaptureBoxMouseEvents(bool capture) noexcept;

    //==================================================================================================================
    void paint(Graphics &g) override;

    //==================================================================================================================
    void mouseDown(const MouseEvent &e) override;
    void mouseMove(const MouseEvent &e) override;
    
private:
    friend class OptionBox;

    ListenerList<Listener> listeners;
    TickType tickType;
    LabelDirection labelDirection;
    std::vector<OptionBox> optionBoxes;
    bool hasLookAndFeel;
    bool capturesHoverAndClickEvents;

    //==================================================================================================================
    void timerCallback() override;
    void lookAndFeelChanged() override;

    JUCE_DECLARE_NON_COPYABLE(OptionList)
};
}
