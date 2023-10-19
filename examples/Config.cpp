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

    Copyright (c) 2023 ElandaSunshine
    ===============================================================

    @author Elanda
    @file   Config.cpp
    @date   17, October 2023

    ===============================================================
 */

#include <jaut_provider/config/jaut_Config.h>             // Include our main config
#include <jaut_provider/config/parser/jaut_JsonParser.h>  // Include our in-built JSONC parser
#include <jaut_provider/config/jaut_ConfigException.h>    // Include our config exceptions (for catching purposes)



//======================================================================================================================
/**
 *  This function illustrates the query approach to config management.
 *  You can retrieve the properties you added to the config and, if found, do with them whatever you want.
 */
void queryConfig()
{
    // We specify a name for our example
    jaut::Config::Options options;
    options.fileName = "query.jsonc";
    
    // We create our config instance here, can also be part of a class
    jaut::Config config(juce::File::getCurrentWorkingDirectory(), std::make_unique<jaut::JsonParser>(), options);
    
    // Adding our first integer with default value "0"
    config.createProperty("myInteger", 0);
    
    // Adding our first string with default value "Hello World!"
    config.createProperty("myString", "Hello World!");
    
    // Adding our first array with default values 0, 2349 and 32
    config.createProperty("myArray", juce::Array<juce::var>{ 0, 2349, 32 });
    
    // Adding our first property that starts out as null (juce::var::void)
    config.createProperty("myNull");
    
    // Setting some properties
    config.getProperty("myInteger")->setValue(420);
    config.getProperty("myString") ->setValue("Hello Universe!");
    config.getProperty("myArray")  ->setValue(juce::Array<juce::var>{ 200, 3 });
    
    // Save our config to disk
    config.save();
    
    // And we can also load changes from disk of course
    config.load();
}

/**
 *  This function illustrates the structural approach to config management.
 *  You can cache the properties you added to the config and refer to them at any time.
 *  
 *  This approach is generally preferred, as you don't have to query every time which adds to the cost of finding.
 */
void structuralConfig()
{
    jaut::Config::Options options;
    options.fileName = "structural.jsonc";
    
    // We can also get our in-built json parser with a static factory
    auto config = jaut::Config::backendJson(juce::File::getCurrentWorkingDirectory(), options);
    
    /* 
       Here we instantly retrieve the property object and cache it,
       this can also be done with "getProperty" and "findProperty" if the property already exists.
     */
    jaut::Config::Property &prop_int = config.createProperty("myInteger", 0).second;
    jaut::Config::Property &prop_str = config.createProperty("myString", "Hello World!").second;
    jaut::Config::Property &prop_arr = config.createProperty("myArray", juce::Array<juce::var>{ 0, 2349, 32 }).second;
    
    // We can also add comments to properties (will be outputted if the parser supports it)
    prop_int.setComment("Hello this is an array, hell yeah");
    
    prop_int.setValue(620);
    prop_str.setValue("Hello Multiverse!");
    prop_arr.setValue(juce::Array<juce::var>{ 200, 3 });
    
    config.save();
}

/**
 *  This function illustrates how child properties can be used with config.
 *  If you need nested property-sets, you can add properties to other properties instead of the category root.
 *  
 *  Note that this makes the property automatically a map property, which means it will ignore any value you set it to.
 *  (theoretically, in the end it depends on the parser that is used, if the parser supports it a property can be both)
 */
void childProperties()
{
    jaut::Config::Options options;
    options.fileName = "children.jsonc";
    
    auto config = jaut::Config::backendJson(juce::File::getCurrentWorkingDirectory(), options);
    
    // Some irrelevant properties
    jaut::Config::Property &prop_int = config.createProperty("myInteger", 0).second;
    jaut::Config::Property &prop_str = config.createProperty("myString", "Hello World!").second;
    jaut::Config::Property &prop_arr = config.createProperty("myArray", juce::Array<juce::var>{ 0, 2349, 32 }).second;
    
    // Our map property with children, we can nest this as deep as we want
    jaut::Config::Property &prop_map = config.createProperty("myChildren").second;
    
    // Let's add some children
    prop_map.createProperty("myInteger", 42);
    prop_map.createProperty("myString", "Are you ready kids?");
    
    config.save();
}

/**
 *  This function illustrates how strict typing works with configs.
 *  
 *  Strict typing enables configs to throw exceptions whenever we assign a property any other value than what it was
 *  defaulted with.
 *  For example, if we initialised a property with an int and we assign it a string, it will throw a jaut::Config.
 *  
 *  This also applies if we load a property from the config file with a mismatched type, so you'd want to try-catch
 *  load operations in this case.
 */
void strictTyping()
{
    jaut::Config::Options options;
    options.fileName = "strict.jsonc";
    
    // We enable strict typing
    options.strictTyping = true;
    
    auto config = jaut::Config::backendJson(juce::File::getCurrentWorkingDirectory(), options);
    jaut::Config::Property &prop_int = config.createProperty("myInteger", 0).second;
    
    jaut::Config::Property &prop_map = config.createProperty("myChildren").second;
    jaut::Config::Property &sub_prop_flt = prop_map.createProperty("myFloat", 0.0f).second;
    
    // Here we will deliberately assign a wrong type (string to int property), if it throws we assign 42 instead
    try
    {
        prop_int.setValue("Hello Exception!");
    }
    catch (jaut::ConfigIncompatibleTypeException &ex)
    {
        // We can also ask for the specific error we got from this exception, here we check for only type mismatches
        if (ex.getCode() == jaut::ConfigIncompatibleTypeException::ErrorCode::TypeMismatch)
        {
            // It failed, so we set the value to 42, as it is THE ANSWER!
            prop_int.setValue(42);
        }
    }
    
    // Here we try to assign any value to a map property
    try
    {
        prop_map.setValue("Hello Exception, I missed you <3");
    }
    catch (jaut::ConfigIncompatibleTypeException &ex)
    {
        sub_prop_flt.setValue(42.0f);
    }
    
    config.save();
}

/**
 *  This function illustrates how auto saving works.
 *  We can specify ourselves whenever a config should call "save()" itself.
 *  
 *  In this example, we save whenever a change has occurred
 */
void autoSaving()
{
    jaut::Config::Options options;
    options.fileName = "autosave.jsonc";
    
    // We set the auto save policy to every value change, but not on adding properties
    options.autoSavePolicy = jaut::Config::Options::policyOnValueChange;
    
    auto config = jaut::Config::backendJson(juce::File::getCurrentWorkingDirectory(), options);
    
    // Adding our first integer with default value "0"
    config.createProperty("myInteger", 0);
    config.createProperty("myString", "Hello World!");
    config.createProperty("myArray", juce::Array<juce::var>{ 0, 2349, 32 });
    
    // We set a value now, and it should auto save on its own
    config.getProperty("myInteger")->setValue(420);
    
    // But this will not auto-save (because we tell it to not invoke any event handlers)
    config.getProperty("myInteger")->setValue(666, juce::dontSendNotification);
}

//======================================================================================================================
int main()
{
    queryConfig();
    structuralConfig();
    childProperties();
    strictTyping();
    autoSaving();
    
    return 0;
}
