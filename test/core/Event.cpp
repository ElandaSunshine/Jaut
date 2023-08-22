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
    @file   Event.cpp
    @date   08, April 2022
    
    ===============================================================
 */

#include <jaut_core/signal/event/jaut_Event.h>

#include <gtest/gtest.h>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    //******************************************************************************************************************
    // region Fixture Setup
    //==================================================================================================================
    class EventFixture : public ::testing::Test
    {
    public:
        enum class HandlerMode
        {
            Free,
            Closure,
            Member,
            Functor
        };
        
        //==============================================================================================================
        using TestHandler = jaut::EventHandler<int>;
        
        //==============================================================================================================
        jaut::Event<TestHandler> TestEventST;
        HandlerMode              handlerMode {};
        
        bool freeHandlerAdded    { false };
        bool closureHandlerAdded { false };
        bool memberHandlerAdded  { false };
        bool functorHandlerAdded { false };
        
        //==============================================================================================================
        EventFixture()
            : TestEventST (
                // Add policies
                [this](const TestHandler &handler)
                {
                    setHandlerMethod(handler, true);
                },
                
                // Remove policies
                [this](const TestHandler &handler)
                {
                    setHandlerMethod(handler, false);
                }
            )
        {}
        
    private:
        void setHandlerMethod(const TestHandler &handler, bool value)
        {
            switch (handlerMode)
            {
                case HandlerMode::Free:    if (handler.isFreeFunction())     { freeHandlerAdded    = value; break; }
                case HandlerMode::Closure: if (handler.isClosureOrFunctor()) { closureHandlerAdded = value; break; }
                case HandlerMode::Member:  if (handler.isMemberFunction())   { memberHandlerAdded  = value; break; }
                case HandlerMode::Functor: if (handler.isClosureOrFunctor()) { functorHandlerAdded = value; break; }
            }
        }
    };
    //==================================================================================================================
    // endregion Fixture Setup
    //******************************************************************************************************************
    // region Testing Facilities
    //==================================================================================================================
    int freeHandlerValue    = 0;
    int closureHandlerValue = 0;
    int memberHandlerValue  = 0;
    int functorHandlerValue = 0;
    
    //==================================================================================================================
    struct MemberHandler
    {
        void memberHandler(int value) // NOLINT
        {
            memberHandlerValue = value;
        }
    };
    
    struct Functor
    {
        void operator()(int value)
        {
            functorHandlerValue = value;
        }
    };
    
    //==================================================================================================================
    void freeHandler(int value)
    {
        freeHandlerValue = value;
    }
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(HandlerTest, TestCallbackTypes)
{
    using TestHandler = jaut::EventHandler<int>;
    
    ::MemberHandler member;
    const auto      lambda = [](int value)
    {
        ::closureHandlerValue = value;
    };
    
    const TestHandler free_handler    = jaut::makeHandler("free", &::freeHandler);
    const TestHandler closure_handler = jaut::makeHandler("closure", lambda);
    const TestHandler member_handler  = jaut::makeHandler("member", &::MemberHandler::memberHandler, member);
    const TestHandler functor_handler = jaut::makeHandler("functor", ::Functor{});
    
    EXPECT_TRUE(free_handler   .isFreeFunction());
    EXPECT_TRUE(closure_handler.isClosureOrFunctor());
    EXPECT_TRUE(member_handler .isMemberFunction());
    EXPECT_TRUE(functor_handler.isClosureOrFunctor());
}

TEST_F(EventFixture, TestFreeHandler)
{
    const TestHandler handler = jaut::makeHandler(&::freeHandler);
    
    // Handler registration
    handlerMode = HandlerMode::Free;
    TestEventST += handler;
    EXPECT_TRUE(freeHandlerAdded);
    
    // Post event
    TestEventST.invoke(420);
    EXPECT_EQ(freeHandlerValue, 420);
    
    // Remove policies
    TestEventST -= handler;
    EXPECT_FALSE(freeHandlerAdded);
}

TEST_F(EventFixture, TestClosureHandler)
{
    const TestHandler handler = jaut::makeHandler([](int value)
    {
        ::closureHandlerValue = value;
    });
    
    // Handler registration
    handlerMode = HandlerMode::Closure;
    TestEventST += handler;
    
    EXPECT_TRUE(closureHandlerAdded);
    
    // Post event
    TestEventST.invoke(69);
    EXPECT_EQ(closureHandlerValue, 69);
    
    TestEventST -= handler;
    EXPECT_TRUE(closureHandlerAdded);
    
    closureHandlerAdded = false;
}

TEST_F(EventFixture, TestMemberHandler)
{
    ::MemberHandler member_handler_obj;
    const TestHandler handler = jaut::makeHandler(&::MemberHandler::memberHandler, &member_handler_obj);
    
    // Handler registration
    handlerMode = HandlerMode::Member;
    TestEventST += handler;
    
    EXPECT_TRUE(memberHandlerAdded);
    
    // Post event
    TestEventST.invoke(666);
    EXPECT_EQ(memberHandlerValue, 666);
    
    TestEventST -= handler;
    EXPECT_FALSE(memberHandlerAdded);
}

TEST_F(EventFixture, TestFunctorHandler)
{
    const TestHandler handler = jaut::makeHandler( ::Functor{});
    
    // Handler registration
    handlerMode = HandlerMode::Functor;
    TestEventST += handler;
    
    EXPECT_TRUE(functorHandlerAdded);
    
    // Post event
    TestEventST.invoke(42);
    EXPECT_EQ(functorHandlerValue, 42);
    
    TestEventST -= handler;
    EXPECT_TRUE(functorHandlerAdded);
    
    functorHandlerAdded = false;
}

TEST_F(EventFixture, TestAssocHandlers)
{
    ::MemberHandler member;
    const auto      lambda = [](int value)
    {
        ::closureHandlerValue = value;
    };
    
    const TestHandler free_handler    = jaut::makeHandler("free", &::freeHandler);
    const TestHandler closure_handler = jaut::makeHandler("closure", lambda);
    const TestHandler member_handler  = jaut::makeHandler("member", &::MemberHandler::memberHandler, member);
    const TestHandler functor_handler = jaut::makeHandler("functor", ::Functor{});
    
    handlerMode = HandlerMode::Free;
    TestEventST += free_handler;
    EXPECT_TRUE(freeHandlerAdded);
    
    handlerMode = HandlerMode::Closure;
    TestEventST += closure_handler;
    EXPECT_TRUE(closureHandlerAdded);
    
    handlerMode = HandlerMode::Member;
    TestEventST += member_handler;
    EXPECT_TRUE(memberHandlerAdded);
    
    handlerMode = HandlerMode::Functor;
    TestEventST += functor_handler;
    EXPECT_TRUE(functorHandlerAdded);
    
    TestEventST.invoke(42069666);
    EXPECT_EQ(freeHandlerValue,    42069666);
    EXPECT_EQ(closureHandlerValue, 42069666);
    EXPECT_EQ(memberHandlerValue,  42069666);
    EXPECT_EQ(functorHandlerValue, 42069666);
    
    // try to unsubscribe
    handlerMode = HandlerMode::Free;
    TestEventST -= free_handler;
    EXPECT_FALSE(freeHandlerAdded);
    
    handlerMode = HandlerMode::Closure;
    TestEventST -= closure_handler;
    EXPECT_FALSE(closureHandlerAdded);
    
    handlerMode = HandlerMode::Member;
    TestEventST -= member_handler;
    EXPECT_FALSE(memberHandlerAdded);
    
    handlerMode = HandlerMode::Functor;
    TestEventST -= functor_handler;
    EXPECT_FALSE(functorHandlerAdded);
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
