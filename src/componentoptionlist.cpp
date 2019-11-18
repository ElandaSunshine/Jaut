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
    @file   componentoptionlist.cpp
    @date   14, October 2019
    
    ===============================================================
 */

#include <jaut/componentoptionlist.h>
#include <jaut/lookandfeel.h>

namespace jaut
{
namespace
{
    constexpr int Const_Timer_Speed = 200;

    inline void pushStateChange(ListenerList<OptionList::OptionBox::Listener> &listeners, OptionList::OptionBox &box,
                                OptionList::OptionBox::Listener::OptionStateType state) noexcept
    {
        listeners.call([&box, &state](OptionList::OptionBox::Listener &listener)
        {
            listener.stateChanged(box, state);
        });
    }

    inline void pushListChange(ListenerList<OptionList::Listener> &listeners, OptionList::OptionBox &box,
                              int mode) noexcept
    {
        listeners.call([&box, mode](OptionList::Listener &listener)
        {
            switch(mode)
            {
                case 1:
                    listener.optionBoxAdded(box);
                    break;
                    
                default:
                    listener.optionBoxStateChanged(box);
            }
        });
    }
}

/* ==================================================================================
 * ==================================== OptionBox ===================================
 * ================================================================================== */

OptionList::OptionBox::OptionBox() noexcept
    : list(nullptr)
{
    optionFlags[0] = false;
    optionFlags[1] = true;
    optionFlags[2] = true;
}

OptionList::OptionBox::OptionBox(const String &label, bool checked) noexcept
    : label(label), list(nullptr)
{
    optionFlags[0] = checked;
    optionFlags[1] = true;
    optionFlags[2] = true;
}

OptionList::OptionBox::OptionBox(OptionBox &&other) noexcept
    : label(std::move(other.label)),
      optionFlags(std::move(other.optionFlags)),
      list(std::move(other.list))
{
    setBounds(other);

    for(Listener *l : other.listeners.getListeners())
    {
        listeners.add(l);
    }

    other.listeners.clear();
    other.list = nullptr;
}

//======================================================================================================================
OptionList::OptionBox &OptionList::OptionBox::operator=(OptionBox &&right) noexcept
{
    swap(*this, right);
    setBounds(right);

    for(Listener *l : right.listeners.getListeners())
    {
        listeners.add(l);
    }

    right.listeners.clear();
    right.list = nullptr;

    return *this;
}

//======================================================================================================================
void OptionList::OptionBox::repaint() noexcept
{
    if(list)
    {
        list->repaint(getX(), getY(), getWidth(), getHeight());
    }
}

void OptionList::OptionBox::repaint(int x, int y, int width, int height) noexcept
{
    if(list)
    {
        list->repaint(jmin(getX() + x, getRight()),     jmin(getY() + y, getBottom()),
                      jmin(getX() + width, getRight()), jmin(getY() + height, getRight()));
    }
}

//======================================================================================================================
void OptionList::OptionBox::setEnabled(bool enabled) noexcept
{
    if(optionFlags[1] != enabled)
    {
        optionFlags[1] = enabled;
        jaut::pushStateChange(listeners, *this, Listener::Availability);

        if(list)
        {
            if(list->tickType == Single)
            {
                if(isChecked())
                {
                    optionFlags[0] = false;
                }

                auto it = std::find(list->optionBoxes.begin(), list->optionBoxes.end(), *this);

                if(it != list->optionBoxes.end())
                {
                    auto box = ++it != list->optionBoxes.end() ? it : list->optionBoxes.begin();

                    while(!box->isEnabled())
                    {
                        ++box;
                    }

                    box->optionFlags[0] = true;
                }

                list->repaint();
            }
            else
            {
                repaint(0, 4, 16, 16);
            }
        }
    }
}

void OptionList::OptionBox::setVisible(bool visible) noexcept
{
    if(optionFlags[2]!= visible)
    {
        optionFlags[2] = visible;
        jaut::pushStateChange(listeners, *this, Listener::Visibility);
        repaint(0, 4, 16, 16);
    }
}

void OptionList::OptionBox::setChecked(bool checked) noexcept
{
    if(!isEnabled())
    {
        return;
    }

    if(list && list->tickType == Single)
    {
        if(isChecked())
        {
            return;
        }

        for(OptionBox &box : list->optionBoxes)
        {
            box.optionFlags[0] = false;
        }

        optionFlags[0] = checked;
        pushStateChange(listeners, *this, Listener::Activation);
        repaint(0, 4, 16, 16);
    }
    else if(optionFlags[0] != checked)
    {
        optionFlags[0] = checked;
        pushStateChange(listeners, *this, Listener::Activation);
        repaint(0, 4, 16, 16);
    }
}

void OptionList::OptionBox::setLabel(const String &label) noexcept
{
    if(this->label != label)
    {
        this->label = label;
        repaint(16, 0, getWidth() - 16, getHeight());
    }
}

void OptionList::OptionBox::setBounds(const Rectangle<int> &bounds) noexcept
{
    if(bounds != *this)
    {
        this->Rectangle<int>::setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

        if(isVisible())
        {
            repaint();
        }
    }
}

//======================================================================================================================
const bool OptionList::OptionBox::isEnabled() const noexcept
{
    return optionFlags[1];
}

const bool OptionList::OptionBox::isVisible() const noexcept
{
    return optionFlags[2];
}

const bool OptionList::OptionBox::isChecked() const noexcept
{
    return optionFlags[0];
}

const String OptionList::OptionBox::getLabel() const noexcept
{
    return label;
}

const bool OptionList::OptionBox::isMouseOver() const noexcept
{
    return mouseOver;
}

const bool OptionList::OptionBox::isMouseDown() const noexcept
{
    return mouseDown;
}

//======================================================================================================================
void OptionList::OptionBox::addListener(Listener *listener) noexcept
{
    listeners.add(listener);
}

void OptionList::OptionBox::removeListener(Listener *listener) noexcept
{
    listeners.remove(listener);
}



/* ==================================================================================
 * =================================== OptionList ===================================
 * ================================================================================== */

OptionList::OptionList(const String &componentName, TickType tickType, LabelDirection labelDirection)
    : Component(componentName),
      tickType(tickType), labelDirection(labelDirection),
      hasLookAndFeel(dynamic_cast<jaut::LookAndFeel*>(&getLookAndFeel())),
      capturesHoverAndClickEvents(true)
{
    startTimer(jaut::Const_Timer_Speed);
}

//======================================================================================================================
void OptionList::add(OptionBox &optionBox, int index) noexcept
{  
    optionBox.list = this;

    if(index < 0 || index >= optionBoxes.size())
    {
        optionBoxes.emplace_back(std::move(optionBox));
        jaut::pushListChange(listeners, optionBoxes.at(optionBoxes.size() - 1), 1);
    }
    else
    {
        optionBoxes.insert(optionBoxes.begin() + index, std::move(optionBox));
        jaut::pushListChange(listeners, optionBoxes.at(index), 1);
    }
}

void OptionList::add(const Rectangle<int> &bounds, const String &label, bool checked, bool visible, int index) noexcept
{
    OptionBox box(label, checked);
    box.setBounds(bounds);
    box.setVisible(visible);
    add(box, index);
}

//======================================================================================================================
const OptionList::OptionBox &OptionList::get(int index) const noexcept
{
    return optionBoxes.at(index);
}

OptionList::OptionBox &OptionList::get(int index) noexcept
{
    return optionBoxes.at(index);
}

const int OptionList::getFirstCheckedIndex() const noexcept
{
    for(int i = 0; i < optionBoxes.size(); ++i)
    {
        if(optionBoxes.at(i).isChecked())
        {
            return i;
        }
    }

    return -1;
}

const int OptionList::getLastCheckedIndex() const noexcept
{
    for(int i = optionBoxes.size() - 1; i >= 0; --i)
    {
        if(optionBoxes.at(i).isChecked())
        {
            return i;
        }
    }

    return -1;
}

const bool OptionList::isCapturingBoxMouseEvents() const noexcept
{
    return capturesHoverAndClickEvents;
}

//======================================================================================================================
void OptionList::shouldCaptureBoxMouseEvents(bool capture) noexcept
{
    capturesHoverAndClickEvents = capture;
}

//======================================================================================================================
void OptionList::paint(Graphics &g)
{
    if(!hasLookAndFeel)
    {
        g.setColour(Colours::black);
        g.drawText("No default LookAndFeel set. Please set jaut::LookAndFeel as default!", 0, 0,
                   getWidth(), getHeight(), Justification::centred);

        return;
    }

    jaut::LookAndFeel *lf = dynamic_cast<jaut::LookAndFeel*>(&getLookAndFeel());
    lf->drawOptionListBackground(g, 0, 0, getWidth(), getHeight());

    for(OptionBox &box : optionBoxes)
    {
        if(!box.isVisible())
        {
            continue;
        }

        const bool mouseover = box.mouseOver;
        const bool mousedown = box.mouseDown;

        {
            Graphics::ScopedSaveState sss(g);
            g.setOrigin(box.getX(), box.getY());
            lf->drawOptionListOptionBox(g, box.withX(0).withY(0), tickType == Multiple,
                                        box.isChecked(), box.isEnabled(), mouseover, mousedown);
        }
        
        if(labelDirection != None)
        {
            Graphics::ScopedSaveState sss(g);
            const int labelw = lf->getOptionListLabelWidth(box.label);
            const int labelx = labelDirection  == Right ? box.getRight()
                             : (labelDirection == Left  ? box.getX() - labelw : 0);
            
            g.setOrigin(labelx, box.getY());
            lf->drawOptionListOptionLabel(g, box.getLabel(), {0, 0, labelw, box.getHeight()},
                                          tickType == Multiple, labelDirection == Right, box.isChecked(),
                                          box.isEnabled(), mouseover, mousedown);
        }
    }
}

//======================================================================================================================
void OptionList::mouseDown(const MouseEvent &e)
{
    if(!hasLookAndFeel)
    {
        return;
    }

    if(capturesHoverAndClickEvents)
    {
        startTimer(jaut::Const_Timer_Speed);
    }

    if(tickType == Single)
    {
        int boxupdate  = false;
        int index      = 0;
        
        for(int i = optionBoxes.size() - 1; i >= 0; --i)
        {
            OptionBox &box        = optionBoxes.at(i);
            const bool boxclicked = box.contains(e.x, e.y);
            const bool boxvalid   = box.isEnabled() && box.isVisible();

            if(boxvalid && boxclicked)
            {
                if(box.isChecked())
                {
                    return;
                }

                index     = i;
                boxupdate = true;
                break;
            }
        }

        if(boxupdate)
        {
            optionBoxes.at(index).setChecked(true);
        }
    }
    else
    {
        for(int i = optionBoxes.size() - 1; i >= 0; --i)
        {
            OptionBox &box = optionBoxes.at(i);

            if(box.isEnabled() && box.isVisible() && box.contains(e.x, e.y))
            {
                box.setChecked(!box.isChecked());
                jaut::pushListChange(listeners, box, 0);
                break;
            }
        }
    }

    repaint();
}

void OptionList::mouseMove(const MouseEvent &e)
{
    if(!isTimerRunning() && hasLookAndFeel && capturesHoverAndClickEvents)
    {
        startTimer(jaut::Const_Timer_Speed);
    }
}

//======================================================================================================================
void OptionList::timerCallback()
{
    bool hasenableds = false;

    if(isEnabled() && isVisible())
    {
        // mouse enters box
        for(int i = optionBoxes.size() - 1; i >= 0; --i)
        {
            OptionBox &box = optionBoxes.at(i);
            bool anychange = false;

            if(box.contains(getMouseXYRelative()))
            {
                if(Desktop::getInstance().getMainMouseSource().isDragging() && !box.isMouseDown())
                {
                    box.mouseDown = true;
                    box.mouseOver = true;
                    anychange     = true;
                }
                else if(!box.isMouseOver())
                {
                    box.mouseOver = true;
                    anychange     = true;
                }
            }
            else
            {
                if(box.isMouseDown())
                {
                    box.mouseDown = false;
                    anychange     = true;
                }

                if(box.isMouseOver())
                {
                    box.mouseOver = false;
                    anychange     = true;
                }
            }

            if(anychange)
            {
                box.repaint();
            }

            if(box.mouseOver || box.mouseDown)
            {
                hasenableds = true;
            }
        }
    }

    if(!hasenableds)
    {
        stopTimer();
    }
}

void OptionList::lookAndFeelChanged()
{
    hasLookAndFeel = dynamic_cast<jaut::LookAndFeel*>(&getLookAndFeel());
}
}
