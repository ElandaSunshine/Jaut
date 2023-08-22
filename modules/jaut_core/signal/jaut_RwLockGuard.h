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
    @file   jaut_RwLockGuard.h
    @date   18, December 2022
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    enum class RwLockGuardAction
    {
        Read,
        Write
    };
    
    template<RwLockGuardAction Mode, bool TryLocking = false>
    class JAUT_API RwLockGuard
    {
    public:
        explicit RwLockGuard(juce::ReadWriteLock &lock) noexcept;
        ~RwLockGuard();
        
        //==============================================================================================================
        JAUT_NODISCARD
        operator bool() const noexcept; // NOLINT
        
    private:
        juce::ReadWriteLock &lock;
        bool                succeeded { !TryLocking };
    };
    
    //==================================================================================================================
    // IMPLEMENTATION: RwLockGuard
    template<RwLockGuardAction Mode, bool TryLocking>
    RwLockGuard<Mode, TryLocking>::RwLockGuard(juce::ReadWriteLock &parLock) noexcept
        : lock(parLock)
    {
        if constexpr (Mode == RwLockGuardAction::Read)
        {
            if constexpr (TryLocking)
            {
                succeeded = lock.tryEnterRead();
            }
            else
            {
                lock.enterRead();
            }
        }
        else
        {
            if constexpr (TryLocking)
            {
                succeeded = lock.tryEnterWrite();
            }
            else
            {
                lock.enterWrite();
            }
        }
    }
    
    template<RwLockGuardAction Mode, bool TryLocking>
    RwLockGuard<Mode, TryLocking>::~RwLockGuard()
    {
        if (!succeeded)
        {
            return;
        }
        
        if constexpr (Mode == RwLockGuardAction::Read)
        {
            lock.exitRead();
        }
        else
        {
            lock.exitWrite();
        }
    }
    
    //==================================================================================================================
    template<RwLockGuardAction Mode, bool TryLocking>
    RwLockGuard<Mode, TryLocking>::operator bool() const noexcept
    {
        return succeeded;
    }
}
