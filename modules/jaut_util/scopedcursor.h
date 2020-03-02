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
    @file   scopedcursor.h
    @date   29, February 2020

    ===============================================================
 */

namespace jaut
{
struct JAUT_API ScopedCursor
{
    const MouseCursor resetCursor;

    explicit ScopedCursor(const MouseCursor &cursor, MouseCursor resetCursor = MouseCursor::NormalCursor)
        : resetCursor(std::move(resetCursor))
    {
        Desktop::getInstance().getMainMouseSource().showMouseCursor(cursor);
    }

    ~ScopedCursor()
    {
        Desktop::getInstance().getMainMouseSource().showMouseCursor(resetCursor);
    }
};

struct ScopedCursorWait : ScopedCursor
{
    ScopedCursorWait() : ScopedCursor(MouseCursor::WaitCursor) {}
};
};