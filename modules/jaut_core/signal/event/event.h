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
    @file   event.h
    @date   21, March 2020
    
    ===============================================================
 */

#pragma once



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
namespace jaut
{
template<class Handler> class Event;

/**
 *  The EventHandler class stores a callback to a function and makes it invokable and storeable in an Event object.
 *  An EventHandler can have 3 different types of callable, member functions, free functions and lambdas.
 *
 *  Since member functions are handled differently than free functions, this will keep a pointer to the member
 *  function internally and also the class object to which it belongs.
 *  This makes sure that different classes with the same function pointer don't equal to functions of other classes.
 *
 *  There are two major types of event handlers, the callback-handler and the control-handler.
 *  The callback-handler holds a pointer to a function and calls back to it when invoked, whereas the
 *  control-handler invokes an Event.
 *  Control-handler can only be acquired by assigning an event to the handler.
 *
 *  Due to how lambdas are handled, they are not supported yet unfortunately.
 *
 *  @tparam EventArgs The arguments of the registrable callback
 */
template <class...EventArgs>
class JAUT_API EventHandler final
{
public:
    /**
     *  The callback type of member functions.
     *  @tparam Owner The owning class of the member function
     */
    template<class Owner>
    using MemberCallbackType = void(Owner::*)(EventArgs...);
    
    /**
     *  The callback type of free functions.
     */
    using GlobalCallbackType = void(*)(EventArgs...);
    
    /**
     *  The callback type of lambda functions.
     */
    using FunctionCallbackType = std::function<void(EventArgs...)>;
    
    //==================================================================================================================
    /**
     *  Constructs a new callback-handler from either a free function or a lambda.
     *  @param callback The function or lambda
     */
    explicit EventHandler(FunctionCallbackType callback) noexcept
        : owner(nullptr), event(nullptr),
          isLambda(!(callback.template target<GlobalCallbackType>())),
          callback(callback)
    {
        // For the time being, lambdas are not entirely supported yet.
        jassert(!isLambda);
    }
    
    /**
     *  Constructs a new callback-handler from a member function.
     *
     *  @tparam Owner   The type containing the function
     *  @param callback The member function
     *  @param owner    An instance of the type holding the member function
     */
    template<class Owner>
    EventHandler(MemberCallbackType<Owner> callback, Owner &owner) noexcept
        : owner(&owner), event(nullptr), isLambda(false),
          memberData(getRelativeFunctionID(callback)),
          callback([&owner, callback](EventArgs...args) mutable
                   {
                       (owner.*callback)(std::forward<EventArgs>(args)...);
                   })
    {}
    
    /**
     *  Constructs a new control-handler.
     *  @param event The event to control with this handler
     */
    EventHandler(Event<EventHandler> &eventObject) // NOLINT: Allow assignment construction
        : event(&eventObject), isLambda(false),
          owner(nullptr),
          callback([this](EventArgs... args) mutable
          {
              for (auto &handler : event->handlers)
              {
                  handler(std::forward<EventArgs>(args)...);
              }
          })
    {
        event->controlHandlers.emplace_back(this);
    }
    
    EventHandler(EventHandler &&other) noexcept
        : event(std::move(other.event)),
          owner(std::move(other.owner)),
          memberData(std::move(other.memberData)),
          isLambda(std::move(other.isLambda)),
          callback(std::move(other.callback))
    {
        other.event    = nullptr;
        other.owner    = nullptr;
        other.callback = nullptr;
    }
    
    ~EventHandler()
    {
        if (event)
        {
            const auto it = std::find(event->controlHandlers.begin(), event->controlHandlers.end(), this);
            
            if (it != event->controlHandlers.end())
            {
                event->controlHandlers.erase(it);
            }
        }
    }
    
    //==================================================================================================================
    EventHandler &operator=(Event<EventHandler> &eventObject) = delete;
    EventHandler &operator=(EventHandler &&right)             = delete;
    
    //==================================================================================================================
    /**
     *  Calls the stored callback.
     *  This does not do a null-check, so be sure to check it yourself before calling it.
     *  @param args The arguments to pass to the callback
     */
    void operator()(EventArgs... args) const
    {
        callback(std::forward<EventArgs>(args)...);
    }
    
    //==================================================================================================================
    /**
     *  Checks whether the stored callback is valid.
     *  @return True if the callback is valid, false if not
     */
    explicit operator bool() const noexcept
    {
        return callback != nullptr;
    }
    
    //==================================================================================================================
    /**
     *  Compares two EventHandler and returns whether their callbacks are equal.
     *
     *  @param handler The handler to compare this handler with
     *  @return True if both handlers point to the same function, false if not
     */
    bool operator==(const EventHandler &handler) const noexcept
    {
        if(isMemberFunction())
        {
            return owner == handler.owner && memberData == handler.memberData;
        }
        
        return !isLambdaFunction() && ((*callback.template target<GlobalCallbackType>()) ==
                                      (*handler.callback.template target<GlobalCallbackType>()));
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
    
    /**
     *  Checks whether the stored callback is null.
     *  @return True if the callback is null, false if not
     */
    bool operator==(std::nullptr_t) const noexcept
    {
        return callback == nullptr;
    }
    
    /**
     *  Checks whether the stored callback is not null.
     *  @return True if the callback is not null, false if it is
     */
    bool operator!=(std::nullptr_t) const noexcept
    {
        return callback != nullptr;
    }
    
    //==================================================================================================================
    /**
     *  Determines whether this handler object is a callback-handler.
     *  @return True if this handler is a callback-handler, false if not
     */
    bool isCallbackHandler() const noexcept
    {
        return !isControlHandler();
    }
    
    /**
     *  Determines whether this handler object is a control-handler.
     *  @return True if this handler is a control-handler, false if not
     */
    bool isControlHandler() const noexcept
    {
        return event != nullptr;
    }
    
    //==================================================================================================================
    /**
     *  Determines whether the function this handler is pointing to is a member function.
     *  @return True if it is a member function, false if not
     */
    bool isMemberFunction() const noexcept
    {
        return owner;
    }
    
    /**
     *  Determines whether the function this handler is pointing to is a free function.
     *  @return True if it is a free function, false if not
     */
    bool isFreeFunction() const noexcept
    {
        return !event && !owner && !isLambda;
    }
    
    /**
     *  Determines whether the function this handler is holding is a lambda.
     *  @return True if it is a lambda, false if not
     */
    bool isLambdaFunction() const noexcept
    {
        return isLambda;
    }
    
private:
    friend class Event<EventHandler>;
    
    //==================================================================================================================
    template<typename Return, typename Type, typename... Args>
    static juce::String getRelativeFunctionID(Return(Type::*function)(Args...))
    {
        union
        {
            Return(Type::*func)(Args...);
            const unsigned char buffer[sizeof(func)];
        } data = { function };
        
        juce::String id;
        
        for (int i = 0; i < sizeof(data); ++i)
        {
            id << data.buffer[i];
        }
        
        return id;
    }
    
    //==================================================================================================================
    Event<EventHandler> *event;
    void *owner;
    juce::String memberData;
    bool isLambda;
    FunctionCallbackType callback;
    
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
 *      // First we need to get a new control-handler by assigning an event to a new handler object
 *      AnIntHandler control_handler = SomethingHappened;
 *
 *      // Then we can simply post it by calling it with all the needed parameters
 *      control_handler(420);
 *  }
 *  @endcode
 *
 *  @tparam Handler The handler type this event is defined by
 */
template<class Handler>
class JAUT_API Event final
{
public:
    static_assert(is_of_templated_type<EventHandler, Handler>::value, JAUT_ASSERT_EVENT_NOT_A_HANDLER);
    
    //==================================================================================================================
    // The type of callback for additional actions when adding or removing a handler
    using AddRemoveCallback = std::function<void(const Handler&)>;
    
    //==================================================================================================================
    /**
     *  Constructs a new Event object with optional add/remove callbacks.
     *
     *  @param add    The callback to call before a new handler is subscribed
     *  @param remove The callback to call before a handler is unsubscribed
     */
    explicit constexpr Event(AddRemoveCallback add = nullptr, AddRemoveCallback remove = nullptr) noexcept
        : addCallback(add ? add : [](const Handler&){}), removeCallback(remove ? remove : [](const Handler&){})
    {}
    
    ~Event()
    {
        for (auto &handler : controlHandlers)
        {
            handler->event    = nullptr;
            handler->callback = nullptr;
        }
    }
    
    //==================================================================================================================
    /**
     *  Subscribe a new handler to this event if it doesn't exist already.
     *
     *  @param handler The handler to subscribe
     *  @return This event object
     */
    Event &operator+=(Handler &&handler)
    {
        if(handler && !handler.isControlHandler())
        {
            if(std::find(handlers.begin(), handlers.end(), handler) == handlers.end())
            {
                add(handler);
                handlers.emplace_back(std::move(handler));
            }
        }
        
        return *this;
    }
    
    /**
     *  Unsubscribe a handler if it already existed.
     *
     *  @param handler The handler to unsubscribe
     *  @return This event object
     */
    Event &operator-=(Handler &&handler)
    {
        if(handler && !handler.isControlHandler())
        {
            const auto it = std::find(handlers.begin(), handlers.end(), handler);
            
            if(it != handlers.end())
            {
                remove(handler);
                handlers.erase(it);
            }
        }
        
        return *this;
    }
    
private:
    template<class...>
    friend class EventHandler;
    
    //==================================================================================================================
    AddRemoveCallback addCallback, removeCallback;
    std::vector<Handler> handlers;
    std::vector<Handler*> controlHandlers;
    JUCE_DECLARE_NON_COPYABLE(Event)
};

/**
 *  Creates a new handler from a member function.
 *
 *  @tparam Owner   The owner type of the member function
 *  @tparam Args    The argument types of the member function
 *  @param function The member function to link
 *  @param owner    An instance of the class holding the member function
 *  @return The new handler object
 */
template<class Owner, class...Args>
JAUT_API inline auto make_handler(void(Owner::*function)(Args...), Owner &owner)
{
    return EventHandler<Args...>(function, owner);
}

/**
 *  Creates a new handler from a member function.
 *
 *  @tparam Owner   The owner type of the member function
 *  @tparam Args    The argument types of the member function
 *  @param function The member function to link
 *  @param owner    An instance of the class holding the member function
 *  @return The new handler object
 */
template<class Owner, class...Args>
JAUT_API inline auto make_handler(void(Owner::*function)(Args...), Owner *owner)
{
    // Owner of the member function must point to a valid memory location
    jassert(owner != nullptr);
    return EventHandler<Args...>(function, *owner);
}

/**
 *  Creates a new handler from a free function.
 *
 *  @tparam Args    The argument types of the function
 *  @param function The function to link
 *  @return The new handler object
 */
template<class...Args>
JAUT_API inline auto make_handler(void(*function)(Args...))
{
    return EventHandler<Args...>(function);
}
}
