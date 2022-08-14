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
    @file   scopedcursor.h
    @date   29, February 2020

    ===============================================================
 */

namespace jaut
{
/** A little utility that sets a mouse cursor and resets it to another once this object leaves the scope it is in. */
class JAUT_API ScopedCursor
{
public:
    /**
     *  Creates a new ScopedCursor instance.
     *
     *  @param cursor      The cursor to set
     *  @param resetCursor The cursor to reset once this object leaves its current scope
     */
    explicit ScopedCursor(const juce::MouseCursor &cursor,
                          juce::MouseCursor resetCursor = juce::MouseCursor::NormalCursor)
        : resetCursor(std::move(resetCursor))
    {
        juce::Desktop::getInstance().getMainMouseSource().showMouseCursor(cursor);
    }

    ~ScopedCursor()
    {
        juce::Desktop::getInstance().getMainMouseSource().showMouseCursor(resetCursor);
    }
    
private:
    const juce::MouseCursor resetCursor;
};

/** Sets the current cursor to the wait cursor and resets it back to the normal one. */
struct JAUT_API ScopedCursorWait : ScopedCursor
{
    ScopedCursorWait() : ScopedCursor(juce::MouseCursor::WaitCursor) {}
};
}
