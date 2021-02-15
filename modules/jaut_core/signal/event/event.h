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
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   event.h
    @date   21, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  The EventHandler class stores a callback to a function and makes it invokable and storeable in an Event object.
     *  An EventHandler can have 3 different types of callable, member functions, free functions and lambdas.
     *
     *  Since member functions are handled differently than free functions, this will keep a pointer to the member
     *  function internally and also the class object to which it belongs.
     *  This makes sure that different classes with the same function pointer don't equal to functions of other classes.
     *
     *  Due to how lambdas work, you can not unsubscribe from them once subscribed.
     *
     *  @tparam EventArgs The arguments of the registrable callback
     */
    template <class ...TEventArgs>
    class JAUT_API EventHandler
    {
    public:
        /**
         *  The callback type of member functions.
         *  @tparam Owner The owning class of the member function
         */
        template<class Owner>
        using MemberFunction = void(Owner::*)(TEventArgs...);
        
        /** The callback type of free functions. */
        using FunctionOrLambda = void(*)(TEventArgs...);
    
        //==============================================================================================================
        /**
         *  Constructs a new callback-handler from a free function or a lambda.
         *
         *  @tparam Fn The function type
         *  @param callback The function or lambda
         */
        explicit EventHandler(FunctionOrLambda callback)
            : callback(std::move(callback))
        {}
        
        /**
         *  Constructs a new callback-handler from a member function.
         *
         *  @tparam Owner   The type containing the function
         *  @param callback The member function
         *  @param owner    An instance of the type holding the member function
         */
        template<class Owner>
        EventHandler(MemberFunction<Owner> eventCallback, Owner &owner) noexcept
            : callback([&owner, eventCallback](TEventArgs ...args) { (owner.*eventCallback)(args...); }),
              owner(&owner), memberData(EventHandler::getRelativeFunctionID(eventCallback))
        {}
        
        EventHandler(EventHandler &&other) noexcept
        {
            swap(*this, other);
        }
    
        //==============================================================================================================
        EventHandler &operator=(EventHandler &&right) noexcept
        {
            auto temp(std::move(right));
            swap(*this, temp);
            return *this;
        }
    
        //==============================================================================================================
        /**
         *  Calls the stored callback.
         *  This does not do a null-check, so be sure to check it yourself before calling it.
         *
         *  @param args The arguments to pass to the callback
         */
        void operator()(TEventArgs ...args) const
        {
            callback(args...);
        }
    
        //==============================================================================================================
        /**
         *  Compares two EventHandler and returns whether their callbacks are equal.
         *
         *  @param handler The handler to compare this handler with
         *  @return True if both handlers point to the same function, false if not
         */
        bool operator==(const EventHandler &handler) const noexcept
        {
            // This is just for the last line to fit into one line
            using FuncT = FunctionOrLambda;
            
            if (isMemberFunction())
            {
                return owner == handler.owner && memberData == handler.memberData;
            }
            
            return *callback.template target<FuncT>() == *handler.callback.template target<FuncT>();
        }
        
        /**
         *  Compares two EventHandler and returns whether their callbacks are not equal.
         *
         *  @param handler The handler to compare this handler with
         *  @return True if both handlers point to different functions, false if not
         */
        bool operator!=(const EventHandler &handler) const noexcept
        {
            return !((*this) == handler);
        }
    
        //==============================================================================================================
        /**
         *  Determines whether the function this handler is pointing to is a member function.
         *  @return True if it is a member function, false if not
         */
        bool isMemberFunction() const noexcept { return owner; }
        
        /**
         *  Determines whether the function this handler is pointing to is a free function or a lambda.
         *  @return True if it is a free function, false if not
         */
        bool isFreeFunctionOrLambda() const noexcept { return !owner; }
        
        //==============================================================================================================
        friend void swap(EventHandler &left, EventHandler &right)
        {
            using std::swap;
            
            swap(left.callback,   right.callback);
            swap(left.owner,      right.owner);
            swap(left.memberData, right.memberData);
        }
        
    private:
        template<typename Return, typename Type, typename... Args>
        static juce::String getRelativeFunctionID(Return(Type::*function)(Args...))
        {
            constexpr size_t ptr_size = sizeof(function);
            
            std::array<char, ptr_size> output;
            std::memcpy(output.begin(), &function, ptr_size);
            
            return juce::String(output.cbegin(), ptr_size);
        }
        
        //==============================================================================================================
        std::function<void(TEventArgs...)> callback { nullptr };
        void *owner { nullptr };
        juce::String memberData;
        
        JUCE_DECLARE_NON_COPYABLE(EventHandler)
    };
    
    /**
     *  The Event class provides a simple way for listeners to subscribe to events and changes that may occur.
     *  A listener is determined by a given EventHandler object.
     *
     *  These listeners will be registered in a vector of EventHandlers which can be raised by a control-handler.
     *
     *  Example:
     *  @code
     *  // Creating a handler (which takes functions that only have one int parameter)
     *  using AnIntHandler = EventHandler<int>;
     *
     *  // Now we declare an Event with this handler (by convention, event names are upper-camelcase)
     *  Event<AnIntHandler> SomethingHappened;
     *
     *  // Now we need a listener function (this can either be a member or free function)
     *  void onSomethingHappened(int value)
     *  {
     *      // Do stuff
     *  }
     *
     *  // And we need to subscribe our listeners to the event, preferably in the constructor of a class
     *  SomeClass()
     *  {
     *      // for free functions, we do it like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += make_handler(onSomethingHappened);
     *
     *      // for member functions, like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += make_handler(&SomeClass::onSomethingHappened, this);
     *  }
     *
     *  // But wait, how does this event get triggered?
     *  // It is our duty to raise the event inside the class holding it
     *  void raiseEvent()
     *  {
     *      // Either via functor
     *      SomethingHappened(420);
     *
     *      // Or via invoke
     *      SomethingHappened.invoke(420);
     *  }
     *  @endcode
     *
     *  @tparam THandler        The handler type this event is defined by
     *  @tparam CriticalSection An optional critical section to guard any subscription operation
     */
    template<class THandler, class TCriticalSection = juce::DummyCriticalSection>
    class JAUT_API Event final
    {
    public:
        static_assert(sameTypeIgnoreTemplate_v<EventHandler, THandler>, JAUT_ASSERT_EVENT_NOT_A_HANDLER);
    
        //==============================================================================================================
        /*class JAUT_API AsyncOperation
        {
        public:
            bool isDone() const { return finished->load(); }
            void waitToFinish() { while (!isDone()); }
            
        private:
            std::shared_ptr<std::atomic<bool>> finished;
    
            //==========================================================================================================
            explicit AsyncOperation(std::shared_ptr<std::atomic_bool> flag) : finished(flag) {}
        };*/
        
        //==============================================================================================================
        /** The type of callback for additional actions when adding or removing a handler. */
        using AddRemoveCallback = std::function<void(const THandler&)>;
    
        //==============================================================================================================
        /**
         *  Constructs a new Event object with optional add/remove callbacks.
         *
         *  @param add    The callback to call before a new handler is subscribed
         *  @param remove The callback to call before a handler is unsubscribed
         */
        explicit Event(AddRemoveCallback add    = [](const THandler&){},
                       AddRemoveCallback remove = [](const THandler&){}) noexcept
            : addCallback(std::move(add)), removeCallback(std::move(remove))
        {}
    
        //==============================================================================================================
        /**
         *  Subscribe a new handler to this event if it doesn't exist already.
         *
         *  @param handler The handler to subscribe
         *  @return This event object
         */
        Event& operator+=(THandler &&handler)
        {
            auto temp(std::move(handler));
            
            {
                typename TCriticalSection::ScopedLockType lock(criticalSection);
                
                if (std::find(handlers.begin(), handlers.end(), temp) == handlers.end())
                {
                    addCallback(temp);
                    handlers.emplace_back(std::move(temp));
                }
            }
            
            return *this;
        }
    
        Event& operator+=(typename THandler::FunctionOrLambda callback)
        {
            return operator+=(Handler(callback));
        }
        
        /**
         *  Unsubscribe a handler if it already existed.
         *  Unsubscribing from lambdas is undefined.
         *
         *  @param handler The handler to unsubscribe
         *  @return This event object
         */
        Event& operator-=(THandler &&handler)
        {
            auto temp(std::move(handler));
        
            {
                typename TCriticalSection::ScopedLockType lock(criticalSection);
                
                if (const auto it = std::find(handlers.begin(), handlers.end(), temp); it != handlers.end())
                {
                    removeCallback(temp);
                    handlers.erase(it);
                }
            }
            
            return *this;
        }
        
        Event& operator-=(typename THandler::FunctionOrLambda callback)
        {
            return operator-=(Handler(callback));
        }
    
        //==============================================================================================================
        /**
         *  Invokes the event and runs all registered event handlers on the same thread.
         *
         *  @tparam TArgs The event arguments
         *  @param  args  The event arguments
         */
        template<class ...TArgs>
        void invoke(TArgs &&...args)
        {
            typename TCriticalSection::ScopedLockType lock(criticalSection);
            
            for (THandler &handler : handlers)
            {
                handler(std::forward<TArgs>(args)...);
            }
        }
        
        /**
         *  Invokes the event and runs all registered events on the message thread.
         *  If this is called from the message thread, this will be the same as calling invoke().
         *
         *  Whenever you pick this over invoke(), make sure the registered events all really only concern the
         *  message thread or provide some sort of safety mechanism that things done on other threads don't have
         *  any negative side-effects.
         *
         *  @tparam TArgs The event arguments
         *  @param  args  The event arguments
         *  @return An AsyncOperation object to check when or whether an event has finished its job
         *
        template<class ...TArgs>
        AsyncOperation beginInvoke(TArgs &&...args)
        {
            if (juce::MessageManager::getInstance()->isThisTheMessageThread())
            {
                invoke(std::forward<TArgs>(args)...);
                return;
            }
            
            auto shared = std::make_shared<std::atomic<bool>>(false);
            auto func   = [this, shared, vargs = std::make_tuple(std::forward<TArgs>(args)...)](void*) -> void*
            {
                auto flag = std::move(shared);
    
                std::apply([this, &flag](auto &&...args)
                {
                    {
                        typename TCriticalSection::ScopedLockType locker(criticalSection);

                        for (THandler &handler : handlers)
                        {
                            handler(std::forward<TArgs>(args)...);
                        }
                    }

                    flag->store(true);
               }, std::move(vargs));
    
                return nullptr;
            };
            
            juce::MessageManager::callAsync(func);
            
            return { shared };
        }*/
        
        template<class ...TArgs> void operator()(TArgs &&...args) { invoke(std::forward<TArgs>(args)...); }
        
    private:
        template<class...> friend class EventHandler;
        
        //==============================================================================================================
        TCriticalSection criticalSection;
        AddRemoveCallback addCallback, removeCallback;
        std::vector<THandler> handlers;
        
        JUCE_DECLARE_NON_COPYABLE(Event)
    };
    
    //==================================================================================================================
    /**
     *  An EventHandler with no arguments.
     *  Use this instead of EventHandler<> to express intent.
     */
    using TrivialHandler = EventHandler<>;
    
    //==================================================================================================================
    /**
     *  Creates a new handler from a non-static member function.
     *
     *  @tparam Owner    The owner type of the member function
     *  @tparam Args     The argument types of the member function
     *  @param  function The member function to link
     *  @param  owner    An instance of the class holding the member function
     *  @return The new handler object
     */
    template<class TOwner, class ...TArgs>
    JAUT_API inline auto makeHandler(TOwner &owner, MemberFunctionPointer_t<TOwner, void, TArgs...> callback)
        -> EventHandler<TArgs...>
    {
        return EventHandler<TArgs...>(std::move(callback), owner);
    }
    
    /**
     *  Creates a new handler from a non-static member function.
     *
     *  @tparam Owner    The owner type of the member function
     *  @tparam Args     The argument types of the handler
     *  @param  function The member function to link
     *  @param  owner    An instance of the class holding the member function
     *  @return The new handler object
     */
    template<class TOwner, class ...TArgs>
    JAUT_API inline auto makeHandler(TOwner *owner, MemberFunctionPointer_t<TOwner, void, TArgs...> callback)
        -> EventHandler<TArgs...>
    {
        // Owner of the member function must point to a valid memory location
        jassert(owner != nullptr);
        return EventHandler<TArgs...>(std::move(callback), *owner);
    }
    
    /**
     *  Creates a new handler from a free function or a lambda.
     *
     *  @tparam Args     The argument types of the handler
     *  @param  function The function to link
     *  @return The new handler object
     */
    template<class ...TArgs>
    JAUT_API inline auto makeHandler(FunctionPointer_t<void, TArgs...> callback)
        -> EventHandler<TArgs...>
    {
        return EventHandler<TArgs...>(std::move(callback));
    }
}
