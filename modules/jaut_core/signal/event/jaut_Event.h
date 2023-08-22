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
    @file   jaut_Event.h
    @date   21, March 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_DefUtils.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_core/util/jaut_TypeTraits.h>
#include <jaut_core/util/jaut_TypeContainer.h>

#include <juce_core/juce_core.h>

#include <functional>
#include <type_traits>
#include <variant>



//**********************************************************************************************************************
// region Header
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    /**
     *  A type representing whether a function is a free function pointer complying to the specified jaut::EventHandler
     *  type.
     *  
     *  @tparam Fn      The function pointer type
     *  @tparam Handler The corresponding jaut::EventHandler type
     */
    template<class Fn, class Handler>
    JAUT_API struct isHandlerFreeFunction
        : std::bool_constant<std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>
                                                   && std::is_constructible_v<typename Handler::FreeFunction_t, Fn>>
    {};
    
    /**
     *  Determines whether a function is a free function pointer complying to the specified jaut::EventHandler type.
     *  
     *  @tparam Fn      The function pointer type
     *  @tparam Handler The corresponding jaut::EventHandler type
     */
    template<class Fn, class Handler>
    JAUT_API inline constexpr bool isHandlerFreeFunction_v = isHandlerFreeFunction<Fn, Handler>::value;
    
    /**
     *  A type representing whether a type is either a valid lambda-closure or a functor type complying to the specified
     *  jaut::EventHandler type.
     *  
     *  @tparam T       The functor or closure type
     *  @tparam Handler The corresponding jaut::EventHandler type
     */
    template<class T, class Handler>
    JAUT_API struct isHandlerLambdaOrFunctor
        : std::bool_constant<std::is_class_v<std::remove_reference_t<T>>
                             && Handler::EventArgs_t::template prepend<T>::template to<std::is_invocable>::value
                             && std::is_copy_constructible_v<T>>
    {};
    
    /**
     *  Determines whether a type is either a valid lambda-closure or a functor type complying to the specified
     *  jaut::EventHandler type.
     *
     *  @tparam T       The functor or closure type
     *  @tparam Handler The corresponding jaut::EventHandler type
     */
    template<class T, class Handler>
    JAUT_API inline constexpr bool isHandlerLambdaOrFunctor_v = isHandlerLambdaOrFunctor<T, Handler>::value;
    
    //==================================================================================================================
    /**
     *  The Event class provides a simple way for listeners to subscribe to events and changes that may occur.
     *  A listener is determined by a given EventHandler object.
     *
     *  These listeners will be registered in a vector of EventHandlers which can all be raised by the event.
     *
     *  Example:
     *  @code
     *  // Creating a handler (which takes functions that only have one int parameter)
     *  using AnIntHandler = EventHandler<int>;
     *
     *  // Now we declare an Event with this handler
     *  Event<AnIntHandler> SomethingHappened;
     *
     *  // Now we need to subscribe our listeners to the event
     *  void subscribe()
     *  {
     *      // for free functions, we do it like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += makeHandler(&onSomethingHappened);
     *
     *      // for member functions, like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += makeHandler(&SomeClass::onSomethingHappened,
     *                                                                       someClassInstance);
     *
     *      // for lambdas like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += makeHandler([](int value){});
     *
     *      // and for functors like this
     *      instanceOfClassHoldingTheEvent.SomethingHappened += makeHandler(FunctorType{ functorArgs... });
     *
     *      // for lambdas, obviously since they are non-comparable, it is preferable to do it like this
     *      // (if it needs to be removable)
     *      instanceOfClassHoldingTheEvent.SomethingHappened += makeHandler("callback_name", [](int value){});
     *      // this way, the lambda can be referred to by an id
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
     *  @tparam Handler         The handler type this event is defined by
     *  @tparam CriticalSection An optional critical section to guard changes to the handler list
     */
    template<class, class = juce::DummyCriticalSection>
    class Event;
    
    //==================================================================================================================
    /**
     *  The EventHandler class stores a callback to a function and makes it invocable and storable in an Event object.
     *  An EventHandler can have 4 different types of callable, member functions, free functions, lambdas and functors.
     *  
     *  Since member functions are handled differently than free functions, this will keep a pointer to the member
     *  function internally and also the class object to which it belongs.
     *  This makes sure that different classes with the same function pointer don't equal to functions of other classes.
     *  
     *  Due to how lambdas work, you can not unsubscribe from them once subscribed because there is no way to check
     *  for equality.
     *  However, EventHandler allows to name such handlers, which can then be unsubscribed from through
     *  their id.
     *  
     *  @tparam HandlerArgs The arguments of the registrable callback
     */
    template <class ...HandlerArgs>
    class JAUT_API EventHandler
    {
    public:
        /**
         *  The callback type of member functions.
         *  @tparam Owner The owning class of the member function
         */
        template<class Owner>
        using MemberFunction_t = MemberFunctionPointer_t<Owner, void, HandlerArgs...>;
        
        /** The callback type of free functions. */
        using FreeFunction_t = FunctionPointer_t<void, HandlerArgs...>;
        
        /** The callback type of lambda functions. */
        using LambdaFunction_t = FreeFunction_t;
        
        /** The callback type of functors. */
        using Functor_t = FreeFunction_t;
        
        /** A type array containing the event arguments. */
        using EventArgs_t = TypeArray<HandlerArgs...>;
        
        //==============================================================================================================
        /**
         *  Constructs a new EventHandler from a free function.
         *  If the callback is a lambda, it will not be associative when constructed with this constructor.
         *  
         *  @tparam Fn The function type
         *  @param callback The function or lambda
         */
        template<class Fn, std::enable_if_t<isHandlerFreeFunction_v<Fn, EventHandler>>* = nullptr>
        explicit EventHandler(Fn &&function);
        
        /**
         *  Constructs a new EventHandler from any invocable that is not a non-static member and not a
         *  free function.
         *  
         *  @tparam T
         *  @param invocable
         */
        template<class T, std::enable_if_t<isHandlerLambdaOrFunctor_v<T, EventHandler>>* = nullptr>
        explicit EventHandler(T &&invocable);
        
        /**
         *  Constructs a new EventHandler from another Event, allowing to chain events.
         *  <br><br>
         *  Be aware that this could be dangerous if the chained event is destroyed before this one,
         *  since it only points to the chained event. So make sure to remove it again before it gets destroyed.
         *
         *  @param chainedEvent The event that refers to this handler
         */
        template<class CriticalSection>
        explicit EventHandler(Event<EventHandler<HandlerArgs...>, CriticalSection> &chainedEvent);
        
        /**
         *  Constructs a new EventHandler from a member function.
         *
         *  @tparam Owner   The type containing the function
         *  @param callback The member function
         *  @param owner    An instance of the type holding the member function
         */
        template<class Owner>
        EventHandler(MemberFunction_t<Owner> memberFunction, Owner &memberObject);
        
        /**
         *  Constructs a new EventHandler from any invocable with an id.
         *
         *  @tparam Fn The function type
         *  @param id       The id of the handler
         *  @param callback The function or lambda
         */
        template<class T>
        EventHandler(juce::String id, T &&invocable);
        
        /**
         *  Constructs a new EventHandler from a member function with an id.
         *
         *  @tparam Owner The type containing the function
         *  @param id       The id of the handler
         *  @param callback The member function
         *  @param owner    An instance of the type holding the member function
         */
        template<class Owner>
        EventHandler(juce::String id, MemberFunction_t<Owner> memberFunction, Owner &memberObject);
        
        EventHandler(const EventHandler &other) noexcept;
        EventHandler(EventHandler &&other) noexcept;
        
        //==============================================================================================================
        EventHandler &operator=(EventHandler right) noexcept;
        
        //==============================================================================================================
        /**
         *  Calls the stored callback.
         *  This does not do a null-check, so be sure to check it yourself before calling it.
         *  
         *  @param args The arguments to pass to the callback
         */
        template<class ...Args>
        void operator()(Args &&...args) const;
        
        //==============================================================================================================
        /**
         *  Compares two EventHandler and returns whether their callbacks are equal, or, in associative mode,
         *  their ids.
         *  
         *  Note that when this handler is a closure or functor, this will always return false,
         *  as there is no reliable way of comparing them. (technically there is, but since they can
         *  be rvalues, their addresses would differ)
         *  
         *  @param handler The handler to compare this handler with
         *  @return True if both handlers point to the same function, false otherwise
         */
        JAUT_NODISCARD
        bool operator==(const EventHandler &handler) const noexcept;
        
        /**
         *  Compares two EventHandler and returns whether their callbacks are not equal.
         *
         *  @param handler The handler to compare this handler with
         *  @return True if both handlers point to different functions, false if not
         */
        JAUT_NODISCARD
        bool operator!=(const EventHandler &handler) const noexcept;
        
        //==============================================================================================================
        /**
         *  Determines whether the function this handler is pointing to is a member function.
         *  @return True if it is a member function, otherwise false
         */
        JAUT_NODISCARD
        bool isMemberFunction() const noexcept;
        
        /**
         *  Determines whether the function this handler is pointing to is a free function.
         *  @return True if it is a free function, otherwise false
         */
        JAUT_NODISCARD
        bool isFreeFunction() const noexcept;
        
        /**
         *  Determines whether the function this handler is pointing to is a functor or closure.
         *  @return True if it is a closure or functor, otherwise false
         */
        JAUT_NODISCARD
        bool isClosureOrFunctor() const noexcept;
        
        /**
         *  Determines whether this handler is a chained handler, that binds another event to this one.
         *  @return True if this is a chained handler
         */
        JAUT_NODISCARD
        bool isChained() const noexcept;
        
        /**
         *  Determines whether this handler is an associative handler or not.
         *  @return True if the handler possesses an id, otherwise false
         */
        JAUT_NODISCARD
        bool isAssociative() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets the id of the handler if it is associative, otherwise an empty string.
         *  @return The id of the handler
         */
        JAUT_NODISCARD
        juce::String getId() const noexcept;
        
        //==============================================================================================================
        friend void swap(EventHandler &left, EventHandler &right) noexcept
        {
            using std::swap;
        
            swap(left.data,     right.data);
            swap(left.callback, right.callback);
            swap(left.owner,    right.owner);
            swap(left.funcMode, right.funcMode);
        }
        
    private:
        enum class FuncMode
        {
            Free,
            Member,
            ClosureOrFunctor,
            Chained
        };
        
        //==============================================================================================================
        using MemberDataArray = std::array<char, sizeof(MemberFunction_t<EventHandler>)>;
        using DataVar         = std::variant<FreeFunction_t, MemberDataArray, juce::String>;
        
        //==============================================================================================================
        template<class Owner>
        JAUT_NODISCARD
        static MemberDataArray getMemberData(MemberFunction_t<Owner> memberFunctionPointer);
        
        //==============================================================================================================
        DataVar                             data;
        std::function<void(HandlerArgs...)> callback;
        const void                          *owner   { nullptr };
        FuncMode                            funcMode {};
        
        //==============================================================================================================
        JUCE_LEAK_DETECTOR(EventHandler)
    };
    
    template<class ...Args>
    EventHandler(FunctionPointer_t<void, Args...>) -> EventHandler<Args...>;
    
    //==================================================================================================================
    /**
     *  A simple RAII event subscriber interface, that let's you automatically subscribe to and unsubscribe from an
     *  event.<br><br>
     *  Example:
     *  @code{.cpp}
     *  void function()
     *  {
     *      Event<MyHandler> myEvent;
     *      
     *      {
     *          // Here the event will be subscribed to
     *          ScopedSubscriber subscriber(myEvent, jaut::makeHandler(...));
     *          
     *      } // Here the event will be unsubscribed from
     *  }
     *  @endcode
     *  
     *  Note that, when the ScopedSubscriber exceeds the Event's lifetime, the behaviour of the destructor is undefined.
     *  @tparam Handler The handler type
     */
    template<class Handler>
    class JAUT_API ScopedSubscriber
    {
    public:
        using EventHandler_t = Handler;
        using Event_t        = Event<EventHandler_t>;
        
        //==============================================================================================================
        ScopedSubscriber(Event_t &eventToManage, EventHandler_t handler); // NOLINT
        ScopedSubscriber(ScopedSubscriber &&other) noexcept;
        
        ~ScopedSubscriber();
        
        //==============================================================================================================
        ScopedSubscriber& operator=(ScopedSubscriber &&other) noexcept;
        
        //==============================================================================================================
        friend void swap(ScopedSubscriber &left, ScopedSubscriber &right) noexcept
        {
            using std::swap;
            
            swap(left.event,   right.event);
            swap(left.handler, right.handler);
        }
        
    private:
        Event_t        *event;
        EventHandler_t handler;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE(ScopedSubscriber)
    };
    
    //==================================================================================================================
    template<class Handler, class CriticalSection>
    class JAUT_API Event
    {
    public:
        /** The EventHandler type this Event is managing. */
        using Handler_t = Handler;
        
        using HandlerList_t = std::vector<Handler>;
        
        /**
         *  The event argument list of this Event's handlers.
         *  @see jaut::TypeArray
         */
        using EventArgs_t = typename Handler_t::EventArgs_t;
        
        /** The type of callback for additional actions when adding or removing a handler. */
        using AddRemoveCallback_t = std::function<void(const Handler_t&)>;
        
        //==============================================================================================================
        /**
         *  Constructs a new Event object with optional add/remove callbacks.
         *
         *  @param add    The callback to call before a new handler is subscribed
         *  @param remove The callback to call before a handler is unsubscribed
         */
        explicit Event(AddRemoveCallback_t add    = [](const Handler_t&){},
                       AddRemoveCallback_t remove = [](const Handler_t&){});
        
        Event(Event &&other) noexcept;
        
        //==============================================================================================================
        Event& operator=(Event &&right) noexcept;
        
        //==============================================================================================================
        /**
         *  Subscribe a new handler to this event if it doesn't exist already.
         *
         *  @param handler The handler to subscribe
         *  @return This event object
         */
        Event& operator+=(Handler_t handler);
        
        /**
         *  Unsubscribe a handler if it already existed.
         *  Unsubscribing from lambdas is not possible, it's better to use associative handlers for this purpose.
         *
         *  @param handler The handler to unsubscribe
         *  @return This event object
         */
        Event& operator-=(Handler_t handler);
        
        /**
         *  Subscribe multiple new handler to this event if they don't exist already.
         *
         *  @param handlers The handler list to subscribe
         *  @return This event object
         */
        Event& operator+=(HandlerList_t handlers);
        
        /**
         *  Unsubscribe multiple handler if they already existed.
         *  Unsubscribing from lambdas is not possible, it's better to use associative handlers for this purpose.
         *
         *  @param handler The handler list to unsubscribe
         *  @return This event object
         */
        Event& operator-=(HandlerList_t handlers);
        
        //==============================================================================================================
        /**
         *  Invokes the event and runs all registered event handlers.
         *  This is the same as calling Event::invoke().
         *
         *  @param args The event arguments
         */
        template<class ...Args>
        void operator()(Args &&...args) const;
        
        //==============================================================================================================
        /**
         *  Invokes the event and runs all registered event handlers.
         *  This is the same as calling Event::operator().
         *
         *  @param args The event arguments
         */
        template<class ...Args>
        void invoke(Args &&...args) const;
        
        //==============================================================================================================
        friend void swap(Event &left, Event &right) noexcept
        {
            using std::swap;
        
            swap(left.addCallback,    right.addCallback);
            swap(left.removeCallback, right.removeCallback);
            swap(left.handlers,       left.handlers);
        }
        
    private:
        static_assert(sameTypeIgnoreTemplate_v<EventHandler, Handler_t>, JAUT_ASSERT_EVENT_NOT_A_HANDLER);
        
        //==============================================================================================================
        using Iterator_t      = typename HandlerList_t::iterator;
        using ConstIterator_t = typename HandlerList_t::const_iterator;
        
        //==============================================================================================================
        CriticalSection criticalSection;
        
        AddRemoveCallback_t addCallback;
        AddRemoveCallback_t removeCallback;
        HandlerList_t       handlers;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Event)
    };
    
    //==================================================================================================================
    /**
     *  An EventHandler with no arguments.
     *  Use this instead of EventHandler\<\> to better express your intent.
     */
    using TrivialHandler = EventHandler<>;
    
    //==================================================================================================================
    namespace detail
    {
        template<class>
        struct handlerResolver {};
        
        template<class Fn>
        inline auto handlerResolverHelper(Fn &&fn)
        {
            using Func_t = decltype(std::function(fn));
            return handlerResolverHelper(std::forward<Fn>(fn), handlerResolver<Func_t>{});
        }
        
        template<class Fn, class ...Args>
        inline auto handlerResolverHelper(Fn &&fn, handlerResolver<std::function<void(Args...)>>)
        {
            return EventHandler<Args...>(std::forward<Fn>(fn));
        }
        
        template<class Fn>
        inline auto handlerResolverHelper(juce::String id, Fn &&fn)
        {
            using Func_t = decltype(std::function(fn));
            return handlerResolverHelper(std::move(id), std::forward<Fn>(fn), handlerResolver<Func_t>{});
        }
        
        template<class Fn, class ...Args>
        inline auto handlerResolverHelper(juce::String id, Fn &&fn, handlerResolver<std::function<void(Args...)>>)
        {
            return EventHandler<Args...>(std::move(id), std::forward<Fn>(fn));
        }
    }
    
    //==================================================================================================================
    /**
     *  Creates a new EventHandler from a non-static member function.
     *
     *  @tparam Obj The owner type of the member function
     *
     *  @param function The member function to link
     *  @param owner    An instance of the class holding the member function
     *
     *  @return The new EventHandler object
     */
    template<class Obj, class ...Args>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(MemberFunctionPointer_t<Obj, void, Args...> function, Obj &owner)
    {
        jassert(function != nullptr);
        return EventHandler<Args...>{ std::move(function), owner };
    }
    
    /**
     *  Creates a new EventHandler from a non-static member function.
     *
     *  @tparam Obj  The owner type of the member function
     *  @tparam Args The argument types of the handler
     *
     *  @param function The member function to link
     *  @param owner    An instance of the class holding the member function
     *
     *  @return The new EventHandler object
     */
    template<class Obj, class ...Args>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(MemberFunctionPointer_t<Obj, void, Args...> function, Obj *owner)
    {
        // Owner of the member function must point to a valid memory location
        jassert(function != nullptr);
        jassert(owner    != nullptr);
        return EventHandler<Args...>{ std::move(function), *owner };
    }
    
    /**
     *  Creates a new EventHandler from a free function, a lambda or a functor object.
     *
     *  @tparam Args The argument types of the handler
     *
     *  @param invocable The invocable to create the handler from
     *
     *  @return The new EventHandler object
     */
    template<class Fn>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(Fn &&invocable)
    {
        return detail::handlerResolverHelper(std::forward<Fn>(invocable));
    }
    
    /**
     *  Creates a new associative EventHandler from a non-static member function.
     *
     *  @tparam Obj The owner type of the member function
     *
     *  @param id       The id of the handler
     *  @param function The member function to link
     *  @param owner    An instance of the class holding the member function
     *
     *  @return The new EventHandler object
     */
    template<class Obj, class ...Args>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(juce::String id, MemberFunctionPointer_t<Obj, void, Args...> function, Obj &owner)
    {
        jassert(function != nullptr);
        return EventHandler<Args...>{ std::move(id), std::move(function), owner };
    }
    
    /**
     *  Creates a new associative EventHandler from a non-static member function.
     *
     *  @tparam Obj The owner type of the member function
     *
     *  @param id       The id of the handler
     *  @param function The member function to link
     *  @param owner    An instance of the class holding the member function
     *
     *  @return The new EventHandler object
     */
    template<class Obj, class ...Args>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(juce::String id, MemberFunctionPointer_t<Obj, void, Args...> function, Obj *owner)
    {
        jassert(function != nullptr);
        jassert(owner    != nullptr);
        return EventHandler<Args...>{ std::move(id), std::move(function), *owner };
    }
    
    /**
     *  Creates a new associative EventHandler from a free function, a lambda or a functor object.
     *
     *  @tparam Args The argument types of the handler
     *
     *  @param id        The id of the handler
     *  @param invocable The invocable to create the handler from
     *
     *  @return The new EventHandler object
     */
    template<class Fn, class ...Args>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(juce::String id, Fn &&invocable)
    {
        return detail::handlerResolverHelper(std::move(id), std::forward<Fn>(invocable));
    }
    
    /**
     *  Creates a new chained EventHandler from another Event.
     *
     *  @tparam Handler The handler of the other event
     *
     *  @param chainedEvent The chained event to create the handler from
     *
     *  @return The new EventHandler object
     */
    template<class Handler, class CriticalSection>
    JAUT_NODISCARD
    JAUT_API inline auto makeHandler(Event<Handler, CriticalSection> &chainedEvent)
    {
        return Handler(chainedEvent);
    }
}
//======================================================================================================================
// endregion Header
//**********************************************************************************************************************
// region Implementation (EventHandler)
//======================================================================================================================
namespace jaut
{
    template<class ...HandlerArgs>
    template<class Fn, std::enable_if_t<isHandlerFreeFunction_v<Fn, EventHandler<HandlerArgs...>>>*>
    inline EventHandler<HandlerArgs...>::EventHandler(Fn &&parFunction) // NOLINT
        : data    (parFunction),
          callback(std::forward<Fn>(parFunction)),
          funcMode(FuncMode::Free)
    {}
    
    template<class ...HandlerArgs>
    template<class T, std::enable_if_t<isHandlerLambdaOrFunctor_v<T, EventHandler<HandlerArgs...>>>*>
    inline EventHandler<HandlerArgs...>::EventHandler(T &&parInvocable) // NOLINT
        : callback(std::forward<T>(parInvocable)),
          funcMode(FuncMode::ClosureOrFunctor)
    {}
    
    
    template<class ...HandlerArgs>
    template<class CS>
    inline EventHandler<HandlerArgs...>::EventHandler(Event<EventHandler<HandlerArgs...>, CS> &chainedEvent)
        : callback([&e = chainedEvent](HandlerArgs ...args) { e.invoke(args...); }),
          owner   (&chainedEvent)
    {}
    
    template<class ...HandlerArgs>
    template<class Owner>
    inline EventHandler<HandlerArgs...>::EventHandler(MemberFunction_t<Owner> parMemberFunction, Owner &parMemberObject)
        : data    (getMemberData(parMemberFunction)),
          callback([parMemberFunction, &parMemberObject](HandlerArgs ...args)
                   {
                       (parMemberObject.*parMemberFunction)(args...);
                   }),
          owner   (&parMemberObject),
          funcMode(FuncMode::Member)
    {}
    
    template<class ...HandlerArgs>
    template<class T>
    inline EventHandler<HandlerArgs...>::EventHandler(juce::String parId, T &&parInvocable)
        : EventHandler(std::forward<T>(parInvocable))
    {
        // Empty ids are dangerous, best to not have them
        jassert(!parId.isEmpty());
        data = DataVar(std::move(parId));
    }
    
    template<class ...HandlerArgs>
    template<class Owner>
    inline EventHandler<HandlerArgs...>::EventHandler(juce::String parId, MemberFunction_t<Owner> parMemberFunction,
                                                      Owner &parMemberObject)
        : EventHandler(parMemberFunction, parMemberObject)
    {
        // Empty ids are dangerous, best to not have them
        jassert(!parId.isEmpty());
        data = DataVar(std::move(parId));
    }
    
    template<class ...HandlerArgs>
    inline EventHandler<HandlerArgs...>::EventHandler(const EventHandler &parOther) noexcept
        : data    (parOther.data),
          callback(parOther.callback),
          owner   (parOther.owner),
          funcMode(parOther.funcMode)
    {}
    
    template<class ...HandlerArgs>
    inline EventHandler<HandlerArgs...>::EventHandler(EventHandler &&parOther) noexcept
    {
        swap(*this, parOther);
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    inline EventHandler<HandlerArgs...>& EventHandler<HandlerArgs...>::operator=(EventHandler parRight) noexcept
    {
        swap(*this, parRight);
        return *this;
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    template<class ...Args>
    void EventHandler<HandlerArgs...>::operator()(Args &&...parArgs) const
    {
        callback(std::forward<Args>(parArgs)...);
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::operator==(const EventHandler &parHandler) const noexcept
    {
        if (isAssociative() && parHandler.isAssociative())
        {
            return (std::get<juce::String>(data) == std::get<juce::String>(parHandler.data));
        }
        
        if (funcMode == parHandler.funcMode)
        {
            switch (funcMode)
            {
                case FuncMode::Free:
                    return (std::get<FreeFunction_t>(data) == std::get<FreeFunction_t>(parHandler.data));
                    
                case FuncMode::Member:
                    return (owner == parHandler.owner
                            && std::get<MemberDataArray>(data) == std::get<MemberDataArray>(parHandler.data));
                
                case FuncMode::Chained:
                    return (owner == parHandler.owner);
                
                case FuncMode::ClosureOrFunctor: break;
            }
        }
        
        return false;
    }
    
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::operator!=(const EventHandler &parHandler) const noexcept
    {
        return !(this->operator==(parHandler));
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::isMemberFunction() const noexcept
    {
        return (funcMode == FuncMode::Member);
    }
    
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::isFreeFunction() const noexcept
    {
        return (funcMode == FuncMode::Free);
    }
    
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::isClosureOrFunctor() const noexcept
    {
        return (funcMode == FuncMode::ClosureOrFunctor);
    }
    
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::isChained() const noexcept
    {
        return (funcMode == FuncMode::Chained);
    }
    
    template<class ...HandlerArgs>
    bool EventHandler<HandlerArgs...>::isAssociative() const noexcept
    {
        return (data.index() == 2);
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    juce::String EventHandler<HandlerArgs...>::getId() const noexcept
    {
        return (isAssociative() ? std::get<juce::String>(data) : "");
    }
    
    //==================================================================================================================
    template<class ...HandlerArgs>
    template<class Owner>
    auto EventHandler<HandlerArgs...>::getMemberData(MemberFunction_t<Owner> parMemberFunctionPointer)
        -> typename EventHandler<HandlerArgs...>::MemberDataArray
    {
        MemberDataArray data_array;
        (void) std::memcpy(data_array.data(), &parMemberFunctionPointer, data_array.size());
        return data_array;
    }
}
//======================================================================================================================
// endregion Implementation (EventHandler)
//**********************************************************************************************************************
// region Implementation (ScopedSubscriber)
//======================================================================================================================
namespace jaut
{
    template<class Handler>
    inline ScopedSubscriber<Handler>::ScopedSubscriber(Event_t &parEventToManage, EventHandler_t parHandler)
        : event  (&parEventToManage),
          handler(std::move(parHandler))
    {
        // closures can not be unsubscribed from, use an assoc handler instead
        jassert(!handler.isClosureOrFunctor());
        event += handler;
    }
    
    template<class Handler>
    inline ScopedSubscriber<Handler>::ScopedSubscriber(ScopedSubscriber &&other) noexcept
        : event  (other.event),
          handler(std::move(other.handler))
    {
        other.event = nullptr;
    }
    
    template<class Handler>
    inline ScopedSubscriber<Handler>::~ScopedSubscriber()
    {
        if (event)
        {
            event -= handler;
        }
    }
    
    //==================================================================================================================
    template<class Handler>
    inline ScopedSubscriber<Handler>& ScopedSubscriber<Handler>::operator=(ScopedSubscriber<Handler> &&other) noexcept
    {
        ScopedSubscriber temp(std::move(other));
        swap(*this, temp);
    }
}
//======================================================================================================================
// endregion Implementation (ScopedSubscriber)
//**********************************************************************************************************************
// region Implementation (Event)
//======================================================================================================================
namespace jaut
{
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>::Event(AddRemoveCallback_t parAdd, AddRemoveCallback_t parRemove)
        : addCallback(std::move(parAdd)), removeCallback(std::move(parRemove))
    {}
    
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>::Event(Event &&parOther) noexcept
    {
        parOther.criticalSection.enter();
        Event temp(std::move(parOther));
        parOther.criticalSection.exit();
        
        swap(*this, temp);
    }
    
    //==================================================================================================================
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>& Event<Handler, CriticalSection>::operator=(Event &&parRight) noexcept
    {
        parRight.criticalSection.enter();
        Event temp(std::move(parRight));
        parRight.criticalSection.exit();
        
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            swap(*this, temp);
        }
        
        return *this;
    }
    
    //==================================================================================================================
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>& Event<Handler, CriticalSection>::operator+=(Handler_t handler)
    {
        Handler_t temp_handler(std::move(handler));
        
        {
            jdscoped typename CriticalSection::ScopedLockType(criticalSection);
            
            if (std::find(handlers.begin(), handlers.end(), temp_handler) == handlers.end())
            {
                addCallback(temp_handler);
                (void) handlers.emplace_back(std::move(temp_handler));
            }
        }
        
        return *this;
    }
    
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>& Event<Handler, CriticalSection>::operator-=(Handler_t handler)
    {
        const Handler_t temp_handler(std::move(handler));
        
        {
            jdscoped typename CriticalSection::ScopedLockType(criticalSection);
            const auto it = std::find(handlers.begin(), handlers.end(), temp_handler);
            
            if (it != handlers.end())
            {
                removeCallback(temp_handler);
                (void) handlers.erase(it);
            }
        }
        
        return *this;
    }
    
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>& Event<Handler, CriticalSection>::operator+=(HandlerList_t parHandlers)
    {
        HandlerList_t temp_handlers;
        std::swap(temp_handlers, parHandlers);
        
        {
            jdscoped typename CriticalSection::ScopedLockType(criticalSection);
            
            for (Handler_t &handler : temp_handlers)
            {
                if (std::find(handlers.begin(), handlers.end(), handler) == handlers.end())
                {
                    addCallback(handler);
                    (void) handlers.emplace_back(std::move(handler));
                }
            }
        }
        
        return *this;
    }
    
    template<class Handler, class CriticalSection>
    inline Event<Handler, CriticalSection>& Event<Handler, CriticalSection>::operator-=(HandlerList_t parHandlers)
    {
        HandlerList_t temp_handlers;
        std::swap(temp_handlers, parHandlers);
        
        {
            jdscoped typename CriticalSection::ScopedLockType(criticalSection);
            
            for (Handler_t &handler : temp_handlers)
            {
                if (const auto it = std::find(handlers.begin(), handlers.end(), handler);
                    it != handlers.end())
                {
                    removeCallback(*it);
                    (void) handlers.erase(it);
                }
            }
        }
        
        return *this;
    }
    
    //==================================================================================================================
    template<class Handler, class CriticalSection>
    template<class ...Args>
    inline void Event<Handler, CriticalSection>::operator()(Args &&...args) const
    {
        invoke(std::forward<Args>(args)...);
    }
    
    //==================================================================================================================
    template<class Handler, class CriticalSection>
    template<class ...Args>
    inline void Event<Handler, CriticalSection>::invoke(Args &&...args) const
    {
        jdscoped typename CriticalSection::ScopedLockType(criticalSection);
        
        for (const Handler_t &handler : handlers)
        {
            handler(std::forward<Args>(args)...);
        }
    }
}
//======================================================================================================================
// endregion Implementation (Event)
//**********************************************************************************************************************
