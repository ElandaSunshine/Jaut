#include <jaut/propertymap.h>

namespace jaut
{
const bool PropertyMap::createProperty(const Identifier &key, const var &defaultValue, bool notifyChange) noexcept
{
    if (!contains(key))
    {
        properties.set(key, defaultValue);

        if (notifyChange)
        {
            postAddedListener(key.toString(), defaultValue);
        }

        return true;
    }

    return false;
}

var PropertyMap::getProperty(const Identifier &key) const noexcept
{
    return contains(key) ? properties[key] : var();
}

void PropertyMap::setProperty(const Identifier &key, const var &value, bool notifyChange) noexcept
{
    if (contains(key))
    {
        var oldvalue = getProperty(key);
        properties.set(key, value);

        if (notifyChange)
        {
            postValueChangedListener(key.toString(), oldvalue, value);
        }
    }
    else
    {
        properties.set(key, value);
        postAddedListener(key.toString(), value);
    }
}

const var *PropertyMap::getPropertyPtr(const Identifier &key) const noexcept
{
    return properties.getVarPointer(key);
}

//=====================================================================================================================
const size_t PropertyMap::size() const noexcept
{
    return properties.size();
}

void PropertyMap::clear() noexcept
{
    properties.clear();
}

const bool PropertyMap::isEmpty() const noexcept
{
    return properties.isEmpty();
}

const bool PropertyMap::contains(const Identifier &name) const noexcept
{
    return properties.contains(name);
}

const bool PropertyMap::contains(const var &value) const noexcept
{
    for (auto val : properties)
    {
        if (val.value == value)
        {
            return true;
        }
    }
    
    return false;
}

PropertyMap::t_iterator PropertyMap::begin() const noexcept
{
    return properties.begin();
}

PropertyMap::t_iterator PropertyMap::end() const noexcept
{
    return properties.end();
}

//=====================================================================================================================
void PropertyMap::readFrom(const ValueTree &valueTree, bool notifyChange) noexcept
{
    if (!valueTree.isValid())
    {
        return;
    }

    for (ValueTree prop : valueTree)
    {
        if (prop.isValid() && prop.hasType("Property"))
        {
            const Identifier name = Identifier(prop["Name"]);
            const var value = prop["Value"];

            if (contains(name))
            {
                const var oldvalue = getProperty(name);
                properties.set(name, value);

                if (notifyChange)
                {
                    postValueChangedListener(name.toString(), oldvalue, value);
                }
            }
        }
    }
}

ValueTree PropertyMap::writeTo(const String &name) const noexcept
{
    ValueTree valueTree(name);

    for (auto value : properties)
    {
        ValueTree prop("Property");
        prop.setProperty("Name", value.name.toString(), nullptr);
        prop.setProperty("Value", value.value, nullptr);
        valueTree.appendChild(prop, nullptr);
    }

    return valueTree;
}

//=====================================================================================================================
void PropertyMap::addListener(Listener *listener) noexcept
{
    if (listener)
    {
        listeners.add(listener);
    }
}

void PropertyMap::removeListener(Listener *listener) noexcept
{
    listeners.remove(listener);
}

//=====================================================================================================================
void PropertyMap::postValueChangedListener(const String &name, var oldValue, var newValue) noexcept
{
    listeners.call([=, &name, &oldValue, &newValue](Listener &l)
    {
        l.onValueChanged(name, oldValue, newValue);
    });
}

void PropertyMap::postAddedListener(const String & name, var value) noexcept
{
    listeners.call([=, &name, &value](Listener &l)
    {
        l.onPropertyAdded(name, value);
    });
}
}