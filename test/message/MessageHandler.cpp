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
    @file   MessageHandler.cpp
    @date   17, Septembre 2022

    ===============================================================
 */

#include <gtest/gtest.h>

#include <jaut_message/thread/jaut_MessageHandler.h>
#include <juce_events/juce_events.h>


//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    //******************************************************************************************************************
    // region Fixture Setup
    //==================================================================================================================
    class MessageHandlerFixture : public testing::Test, public juce::Thread, public juce::Timer
    {
    public:
        static constexpr int start_val  = 20;
        static constexpr int expect_val = 20 + std::numeric_limits<std::int16_t>::max();
    
        //==============================================================================================================
        // all thread data
        std::unique_ptr<jaut::MessageHandler<>> handler;
        std::atomic<bool> keepRunning { true };
        
        // second thread data
        int result { 0 };
        
        // message thread data
        int l_result { 0 };
    
        //==============================================================================================================
        MessageHandlerFixture()
            : juce::Thread("calc")
        {}
        
        //==============================================================================================================
        class MessageSumNumber : public jaut::IMessage
        {
        public:
            MessageHandlerFixture &fixture;
            int num;
            
            //==========================================================================================================
            MessageSumNumber(MessageHandlerFixture &fixture, int number)
                : fixture(fixture),
                  num(number)
            {}
            
            //==========================================================================================================
            void handleMessage(jaut::IMessageHandler*, jaut::MessageDirection) override
            {
                for (int i = 0; i < std::numeric_limits<std::int16_t>::max(); ++i)
                {
                    ++num;
                    juce::Thread::sleep(1);
                }
                
                fixture.result = num;
            }
        };
        
        class MessageSendResult : public jaut::IMessage
        {
        public:
            // context data
            int &intToSet;
            
            // send data
            int result { 0 };
            
            //==========================================================================================================
            explicit MessageSendResult(int &intToSet) noexcept
                : intToSet(intToSet)
            {}
            
            //==========================================================================================================
            void handleMessage(jaut::IMessageHandler*, jaut::MessageDirection) override
            {
                intToSet = result;
            }
    
            //==========================================================================================================
            int getDeferId() override
            {
                return 0;
            }
            
            //==========================================================================================================
            void writeData(int parResult) noexcept
            {
                result = parResult;
            }
        };
        
        //==============================================================================================================
        void SetUp() override
        {
            juce::MessageManager::getInstance()->setCurrentThreadAsMessageThread();
            handler = std::make_unique<jaut::MessageHandler<>>();
            
            startThread();
            startTimer(5000);
        }
        
        void TearDown() override
        {
            juce::MessageManager::deleteInstance();
        }
        
        //==============================================================================================================
        void run() override
        {
            ASSERT_EQ(result, 0);
            
            while (keepRunning)
            {
                handler->processNextMessage();
            }
        }
        
        void timerCallback() override
        {
            static int count = 0;
            
            if (l_result == expect_val || (count++ == 10))
            {
                keepRunning = false;
                juce::MessageManager::getInstance()->stopDispatchLoop();
                
                return;
            }
        }
    };
    //==================================================================================================================
    // endregion Fixture Setup
    //******************************************************************************************************************
    // region Testing Facilities
    //==================================================================================================================
    
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST_F(MessageHandlerFixture, TestHandler)
{
    handler->deferredMessageInitHandler = [this](jaut::IMessage *message)
    {
        switch (message->getDeferId())
        {
            case 0:
            {
                static_cast<MessageSendResult*>(message)->writeData(this->result); // NOLINT
                break;
            }
        }
        
        return true;
    };
    
    handler->send(std::make_unique<MessageHandlerFixture::MessageSumNumber>(*this, start_val));
    handler->send(std::make_unique<MessageHandlerFixture::MessageSendResult>(l_result));
    
    juce::MessageManager::getInstance()->runDispatchLoop();
    
    ASSERT_EQ(l_result, expect_val);
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
