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
    @file   jaut_LogRotationManager.h
    @date   30, March 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/exception/jaut_LogRotationException.h>
#include <jaut_logger/rotation/policies/jaut_RotationPolicy.h>
#include <jaut_logger/rotation/strategies/jaut_RotationStrategy.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/signal/event/jaut_Event.h>



namespace jaut
{
    //==================================================================================================================
    namespace detail
    {
        JAUT_UTIL_TYPE_TRAITS_DEFINE_METHOD_CHECK(setup)
    }
    
    //==================================================================================================================
    /**
     *  This class helps managing log files and how they should be rotated.
     *  It is not meant to be derived from but rather to be added as a type, but nothing stops you from doing so
     *  if you need to.
     *
     *  The passed in rotation-policies is a predicate that will evaluate whether it is necessary to rotate the logs
     *  now or not.
     *  The specification for such a policies is as follows:
     *  @code
     *  [](const jaut::RotationPolicyArgs &policyArgs) -> bool
     *  {
     *      // mommy, do the logs need to be rotated now?
     *      return true;
     *  }
     *  @endcode
     *  
     *  Additionally, there is the rotation strategy that will determine how the logs are rotated,
     *  these will determine the target place as well as the target name and format.<br>
     *  At the end of such a call, the result is a file that points to the newly generated archive, or if possible,
     *  an invalid file if for some reason it couldn't be rotated.
     *  @code
     *  [](const jaut::LogRotationManager &rotationManager) -> juce::File
     *  {
     *      // move log
     *      // blabla
     *      // return newArchive
     *  }
     *  @endcode
     */
    class JAUT_API LogRotationManager
    {
    public:
        using BeforeRotationHandler = EventHandler<const juce::File&>;
        using AfterRotationHandler  = BeforeRotationHandler;
        
        //==============================================================================================================
        /**
         *  This event will be raised before the rotation happens.
         *  This is useful for closing the stream first if it is open.
         *
         *  @param par1 The current log file
         */
        Event<BeforeRotationHandler> eventBeforeRotation;
        
        /**
         *  This event will be raised after the rotation happened.
         *  This is useful for opening the new log file.
         *
         *  @param par1 The rotated log file or invalid file if it was deleted
         */
        Event<AfterRotationHandler> eventAfterRotation;
        
        //==============================================================================================================
        /**
         *  Creates a new instance of the LogRotationManager class.
         *
         *  @param logFile                 The log file to manage, the extension of this file will also determine the
         *                                 to-be-used log extension for the manager
         *  @param rotationPolicy          The invocable that determines WHEN logs should be rotated
         *  @param rotationStrategy        The invocable that determines HOW logs should be rotated
         *  @param usingReplacingFormatter If the owning sink of this log manager is in replace mode
         */
        LogRotationManager(juce::File                logFile,
                           NonNull<RotationPolicy>   rotationPolicy,
                           NonNull<RotationStrategy> rotationStrategy);
        
        //==============================================================================================================
        /**
         *  Gets the rotation policy.
         *  @return The rotation policy
         */
        JAUT_NODISCARD
        const RotationPolicy& getPolicy() const noexcept;
        
         /**
          *  Gets the rotation strategy.
          *  @return The rotation strategy
          */
        JAUT_NODISCARD
        const RotationStrategy& getStrategy() const noexcept;
        
        /**
         *  Gets the currently managed log file.
         *  @return The current log file
         */
        JAUT_NODISCARD
        const juce::File& getLogFile() const noexcept;
        
        //==============================================================================================================
        /**
         *  Tries to rotate the log file if the current rotation policy evaluates to true, otherwise does nothing.
         *  If you want to force a rotation, you can use forceRotateLogs() instead.
         *
         *  @param logMessage      The message that should get logged
         *  @param messageRendered The rendered message that was created from the formatter
         *  
         *  @return True if the log was successfully rotated, false if not
         */
        bool tryRotateLogs(const LogMessage &logMessage, const juce::String &messageRendered);
        
        /** 
         *  Forces an unchecked rotation of the current log file.
         *  <br><br>
         *  This will rotate the log, delete and create a new empty file,
         *  so it would make sense to first close the stream whichever handles the file and open it afterwards.<br>
         *  You can do so by subscribing to the events eventBeforeRotation and eventAfterRotation.
         */
        void forceRotateLogs();
        
    private:
        RotationPolicy   policy;
        RotationStrategy strategy;
        juce::File       logFile;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogRotationManager)
    };
}
