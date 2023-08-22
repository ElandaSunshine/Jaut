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
    @file   jaut_LogRotationManager.cpp
    @date   30, March 2022
    
    ===============================================================
 */

#include <jaut_logger/rotation/jaut_LogRotationManager.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region LogRotationManager
//======================================================================================================================
namespace jaut
{
    LogRotationManager::LogRotationManager(juce::File                parLogFile,
                                           NonNull<RotationPolicy>   parRotationPolicy,
                                           NonNull<RotationStrategy> parRotationStrategy)
        : policy  (std::move(parRotationPolicy.get())),
          strategy(std::move(parRotationStrategy.get())),
          logFile (std::move(parLogFile))
    {}
    
    //==================================================================================================================
    const RotationPolicy& LogRotationManager::getPolicy() const noexcept
    {
        return policy;
    }

    const RotationStrategy& LogRotationManager::getStrategy() const noexcept
    {
        return strategy;
    }
    
    const juce::File& LogRotationManager::getLogFile() const noexcept
    {
        return logFile;
    }
    
    //==================================================================================================================
    bool LogRotationManager::tryRotateLogs(const LogMessage &parLogMessage, const juce::String &parMessageRendered)
    {
        if (!logFile.exists())
        {
            return false;
        }
        
        const RotationPolicyArgs args {
            logFile,
            parLogMessage,
            parMessageRendered
        };
        
        if (policy(args))
        {
            forceRotateLogs();
            return true;
        }
        
        return false;
    }
    
    void LogRotationManager::forceRotateLogs()
    {
        if (!logFile.exists())
        {
            return;
        }
        
        eventBeforeRotation.invoke(logFile);
        
        const juce::File rotated_file = strategy(*this);
        (void) logFile.deleteFile();
        (void) logFile.create();
        
        eventAfterRotation.invoke(rotated_file);
    }
}
//======================================================================================================================
// endregion LogRotationManager
//**********************************************************************************************************************
