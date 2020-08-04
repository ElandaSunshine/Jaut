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



//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
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
        : callback(callback), owner(nullptr)
    {
        // For the time being, lambdas are not supported yet
        jassert(!(callback.template target<GlobalCallbackType>()));
    }
    
    /**
     *  Constructs a new callback-handler from a member function.
     *
     *  @tparam Owner   The type containing the function
     *  @param callback The member function
     *  @param owner    An instance of the type holding the member function
     */
    template<class Owner>
    EventHandler(MemberCallbackType<Owner> eventCallback, Owner &owner) noexcept
        : callback([&owner, eventCallback](EventArgs ...args) { (owner.*eventCallback)(args...); }),
          owner(&owner), memberData(getRelativeFunctionID(callback))
    {}
    
    EventHandler(EventHandler &&other) noexcept
    {
        swap(*this, other);
    }
    
    //==================================================================================================================
    EventHandler &operator=(EventHandler &&right) noexcept
    {
        auto temp(std::move(right));
        swap(*this, temp);
        return *this;
    }
    
    //==================================================================================================================
    /**
     *  Calls the stored callback.
     *  This does not do a null-check, so be sure to check it yourself before calling it.
     *
     *  @param args The arguments to pass to the callback
     */
    void operator()(EventArgs ...args) const
    {
        callback(args...);
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
        if (isMemberFunction())
        {
            return owner == handler.owner && memberData == handler.memberData;
        }
        
        return *callback.template target<GlobalCallbackType>() == *handler.callback.template target<GlobalCallbackType>();
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
        return !owner;
    }
    
    //==================================================================================================================
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
    FunctionCallbackType callback { nullptr };
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
 *  @tparam Handler The handler type this event is defined by
 */
template<class Handler, class CriticalSection = juce::DummyCriticalSection>
class JAUT_API Event final
{
public:
    static_assert(sameTypeIgnoreTemplate_v<EventHandler, Handler>, JAUT_ASSERT_EVENT_NOT_A_HANDLER);
    
    //==================================================================================================================
    /** The type of callback for additional actions when adding or removing a handler. */
    using AddRemoveCallback = std::function<void(const Handler&)>;
    
    //==================================================================================================================
    /**
     *  Constructs a new Event object with optional add/remove callbacks.
     *
     *  @param add    The callback to call before a new handler is subscribed
     *  @param remove The callback to call before a handler is unsubscribed
     */
    explicit Event(AddRemoveCallback add    = [](const Handler&){},
                   AddRemoveCallback remove = [](const Handler&){}) noexcept
        : addCallback(add), removeCallback(remove)
    {}
    
    //==================================================================================================================
    /**
     *  Subscribe a new handler to this event if it doesn't exist already.
     *
     *  @param handler The handler to subscribe
     *  @return This event object
     */
    Event& operator+=(Handler &&handler)
    {
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (std::find(handlers.begin(), handlers.end(), handler) == handlers.end())
            {
                addCallback(handler);
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
    Event& operator-=(Handler &&handler)
    {
        auto temp(std::move(handler));
    
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (const auto it = std::find(handlers.begin(), handlers.end(), temp); it != handlers.end())
            {
                removeCallback(temp);
                handlers.erase(it);
            }
        }
        
        return *this;
    }
    
    //==================================================================================================================
    template<class ...Args>
    void invoke(Args &&...args)
    {
        for (auto &handler : handlers)
        {
            handler(std::forward<Args>(args)...);
        }
    }
    
    template<class ...Args>
    void operator()(Args &&...args)
    {
        typename CriticalSection::ScopedLockType lock(criticalSection);
        invoke(std::forward<Args>(args)...);
    }
    
private:
    template<class...> friend class EventHandler;
    
    //==================================================================================================================
    CriticalSection criticalSection;
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
template<class Owner, class ...Args>
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
template<class Owner, class ...Args>
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
template<class ...Args>
JAUT_API inline auto make_handler(void(*function)(Args...))
{
    return EventHandler<Args...>(function);
}
}
