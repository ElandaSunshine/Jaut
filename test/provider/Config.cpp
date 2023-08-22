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
    @file   Config.cpp
    @date   25, April 2022

    ===============================================================
 */

#define YAML_CPP_STATIC_DEFINE

// Units
#include <jaut_provider/config/parser/jaut_XmlParser.cpp>
#include <jaut_provider/config/parser/jaut_JsonParser.cpp>
#include <jaut_provider/config/parser/jaut_YamlParser.cpp>
#include <jaut_provider/config/jaut_Config.cpp>

// Headers
#include <gtest/gtest.h>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    constexpr std::string_view defaultCategoryName = "iamTheDefault";
    constexpr std::string_view longString = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
                                            "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
                                            "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
                                            "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
                                            "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat"
                                            " cupidatat non proident, sunt in culpa qui officia deserunt mollit anim"
                                            " id est laborum.";
    
    //******************************************************************************************************************
    // region Fixture Setup
    //==================================================================================================================
    class ConfigFixture : public testing::Test // NOLINT
    {
    public:    
        std::unique_ptr<jaut::Config> config;
        
        // default category
        jaut::Config::Property *propBool;
        jaut::Config::Property *propInt;
        jaut::Config::Property *propSubProps;
            jaut::Config::Property *propDouble;
            jaut::Config::Property *propInt64;
        jaut::Config::Property *propNull;
        jaut::Config::Property *propString;
        jaut::Config::Property *propLongString;
        
        // array category
        jaut::Config::Property *propStringArray;
        jaut::Config::Property *propEmptyArray;
        jaut::Config::Property *propObjectArray;
        
        //==============================================================================================================
        template<class T>
        void setup(std::unique_ptr<T>                    parser,
                   juce::String                          name,
                   bool                                  strictTyping,
                   bool                                  createIfNotExists,
                   jaut::Config::Options::AutoSavePolicy policy            = nullptr)
        {
            static constexpr auto exe_location = juce::File::SpecialLocationType::currentExecutableFile;
            const juce::File      config_path  = juce::File::getSpecialLocation(exe_location).getParentDirectory();
            
            jaut::Config::Options options;
            options.configNotice          = "This is a test notice HELLO THERE!\nHa it even has multiple lines.";
            options.defaultCategory       = defaultCategoryName.data();
            options.strictTyping          = strictTyping;
            options.createFileIfNotExists = createIfNotExists;
            std::swap(name,   options.fileName);
            std::swap(policy, options.autoSavePolicy);
            
            config = std::make_unique<jaut::Config>(config_path, std::move(parser), std::move(options));
            
            propBool       = &config->createProperty("test_bool",        true)             .second;
            propInt        = &config->createProperty("test_int",         45  )             .second;
            propNull       = &config->createProperty("test_null")                          .second;
            propString     = &config->createProperty("test_string",      "a string")       .second;
            propLongString = &config->createProperty("test_long_string", longString.data()).second;

            propSubProps = &config      ->createProperty("test_children")                                       .second;
            propDouble   = &propSubProps->createProperty("test_double", 23.0)                                   .second;
            propInt64    = &propSubProps->createProperty("test_int64",  std::numeric_limits<juce::int64>::max()).second;
            
            config->createProperty("test_null_int", 0);
            config->getProperty("test_null_int")->setValue(juce::var{});
            
            propSubProps->setComment("Test property comment");
            
            const juce::Array<juce::var> array_strings {
                "string 1",
                "string 2",
                "string your mom"
            };
            
            // Taken from: https://www.javatpoint.com/json-example
            const juce::Array<juce::var> array_all_types {
                juce::JSON::parse(R"JSON({
                        "employees":[  
                            {"name":"Shyam", "email":"shyamjaiswal@gmail.com"},  
                            {"name":"Bob", "email":"bob32@gmail.com"},  
                            {"name":"Jai", "email":"jai87@gmail.com"}
                        ],
                        "string": "\ntesthaha\" \\"
                    })JSON"),
                2,
                std::numeric_limits<juce::int64>().max(),
                38921,
                32.03,
                " blbla<&",
                juce::var(),
                true,
                juce::Array<juce::var> {
                    "test\"'>",
                    0,
                    23.0,
                    true
                }
            };
            
            propStringArray = &config->createProperty("test_string_array", array_strings,            "array").second;
            propEmptyArray  = &config->createProperty("test_empty_array",  juce::Array<juce::var>(), "array").second;
            propObjectArray = &config->createProperty("test_all_array",    array_all_types,          "array").second;
            
            config->setCategoryComment(jaut::Config::defaultCategory, "Commenting on a category");
        }
    };
    
    template<class T>
    class ParserFixture : public ConfigFixture
    {
    public:    
        void setup(juce::String name)
        {
            ConfigFixture::setup(std::make_unique<T>(), std::move(name), false, true);
        }
        
        static constexpr std::string_view getExtension()
        {
            if constexpr(std::is_same_v<T, jaut::XmlParser>)
            {
                return "xml";
            }
            else if constexpr (std::is_same_v<T, jaut::JsonParser>)
            {
                return "jsonc";
            }
            else
            {
                return "yaml";
            }
        }
    };
    //==================================================================================================================
    // endregion Fixture Setup
    //******************************************************************************************************************
    // region Testing Facilities
    //==================================================================================================================
    class DummyParser : public jaut::IConfigParser
    {
        void parseConfig(ParseAttributes<jaut::Config::CategoryMap> attributes) final
        {
            jaut::ignore(attributes);
        }
        
        void writeConfig(ParseAttributes<const jaut::Config::CategoryMap> attributes) const final
        {
            jaut::ignore(attributes);
        }
    };
    
    using TestedParsers = testing::Types<jaut::XmlParser, jaut::JsonParser, jaut::YamlParser>;
    //==================================================================================================================
    // endregion Testing Facilities
    //******************************************************************************************************************
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST_F(ConfigFixture, TestConfigClass)
{
    setup(std::make_unique<::DummyParser>(), "", false, false);

    const juce::String def = defaultCategoryName.data();
    
    jaut::Config::Property *prop = config->findProperty(def + "." + "test_children.test_double");
    ASSERT_TRUE (prop != nullptr);
    EXPECT_STREQ(prop->getId().toRawUTF8(), "test_double");
    
    EXPECT_TRUE(config->findProperty(def + "." + "test_nothing_here") == nullptr);
    
    prop = config->getProperty("test_bool");
    ASSERT_TRUE (prop != nullptr);
    EXPECT_STREQ(prop->getId().toRawUTF8(), "test_bool");
    EXPECT_TRUE(config->getProperty("test_bool", "array") == nullptr);
    
    bool prop_cha_hap = false;
    bool prop_add_hap = false;
    
    auto event_value_change = [this, &prop_cha_hap](const jaut::Config&,
                                                    const jaut::Config::Property &obj,
                                                    const juce::var&)
    {
        EXPECT_TRUE(&obj == propDouble);
        EXPECT_DOUBLE_EQ(static_cast<double>(obj.getValue()), 420.0);
        prop_cha_hap = true;
    };
    config->ValueChanged += jaut::makeHandler(event_value_change);
        
    auto event_prop_added = [this, &prop_add_hap](const jaut::Config&,
                                                  const jaut::Config::Property &parent,
                                                  const jaut::Config::Property &child)
    {
        EXPECT_TRUE(&parent == propSubProps);
        EXPECT_STREQ(child.getId().toRawUTF8(), "something");
        prop_add_hap = true;
    };
    config->PropertyAdded += jaut::makeHandler(event_prop_added);
    
    propDouble->setValue(420.0);
    EXPECT_TRUE(prop_cha_hap);
    
    propSubProps->createProperty("something");
    EXPECT_TRUE(prop_add_hap);
}

TEST_F(ConfigFixture, TestConfigProperties)
{
    {
        setup(std::make_unique<::DummyParser>(), "", false, false);
            
        // test names
        const juce::String def  = defaultCategoryName.data();

        const juce::String &id  = propInt->getId();
        const juce::String &cat = propInt->getCategory();
        const juce::String abs  = propInt->getAbsoluteName();
        const juce::String comb = def + "." + id;

        EXPECT_STREQ(cat.toRawUTF8(), defaultCategoryName.data());
        EXPECT_STREQ(id .toRawUTF8(), "test_int");
        EXPECT_STREQ(abs.toRawUTF8(), comb.toRawUTF8());

        const juce::String &cid  = propSubProps->getId();
        const juce::String &ccid = propDouble->getId();
        const juce::String ccabs = propDouble->getAbsoluteName();
        const juce::String ccomb = def + "." + cid + "." + ccid;

        EXPECT_STREQ(cid  .toRawUTF8(), "test_children");
        EXPECT_STREQ(ccid .toRawUTF8(), "test_double");
        EXPECT_STREQ(ccabs.toRawUTF8(), ccomb.toRawUTF8());


        // test property retrieval
        // create and get
        EXPECT_TRUE(propSubProps->getProperty("something") == nullptr);
        std::pair<bool, jaut::Config::Property&> new_prop = propSubProps->createProperty("something", 666);
        EXPECT_TRUE(new_prop.first);
        std::pair<bool, jaut::Config::Property&> new_prop_2 = propSubProps->createProperty("something");
        EXPECT_FALSE(new_prop_2.first);
        EXPECT_FALSE(propSubProps->getProperty("something") == nullptr);

        // subscript
        EXPECT_EQ  (static_cast<int>((*propSubProps)["something"].getValue()), 666);
        EXPECT_TRUE(new_prop.second.getProperty("something2") == nullptr);
        jaut::Config::Property &another_sub_prop = new_prop.second["something2"];
        EXPECT_TRUE(another_sub_prop.getValue().isVoid());

        // find
        jaut::Config::Property *found_prop = propSubProps->findProperty("something");
        ASSERT_TRUE(found_prop != nullptr);
        EXPECT_EQ(static_cast<int>(found_prop->getValue()), 666);
        found_prop = propSubProps->findProperty("something.something2");
        ASSERT_TRUE(found_prop != nullptr);
        EXPECT_TRUE(found_prop->getValue().isVoid());

        // general tests
        EXPECT_EQ   (propSubProps->size(), 3);
        EXPECT_TRUE (propSubProps->hasProperty("test_double"));
        EXPECT_TRUE (propSubProps->hasProperty("test_int64"));
        EXPECT_TRUE (propSubProps->hasProperty("something"));
        EXPECT_TRUE (propSubProps->hasChildren());
        EXPECT_FALSE(propBool->hasChildren());

        EXPECT_TRUE     (new_prop.second.setValue(420));
        EXPECT_EQ       (static_cast<int>(new_prop.second.getValue()), 420);
        EXPECT_DOUBLE_EQ(static_cast<double>(propDouble->getValue()), 23.0);
        EXPECT_TRUE     (propDouble->setValue(42.0));
        EXPECT_DOUBLE_EQ(static_cast<double>(propDouble->getValue()), 42.0);
        
        propSubProps->reset(true);
        EXPECT_EQ       (static_cast<int>(new_prop.second.getValue()), 666);
        EXPECT_DOUBLE_EQ(static_cast<double>(propDouble->getValue()), 23.0);

        EXPECT_TRUE(propSubProps->isMapProperty());
        EXPECT_TRUE(propDouble->isValueProperty());



        const juce::String ocat = "array";
        const juce::String &ocid  = propStringArray->getId();
        const juce::String &occat = propStringArray->getCategory();
        const juce::String ocabs  = propStringArray->getAbsoluteName();
        const juce::String occomb = ocat + "." + ocid;

        EXPECT_STREQ(occat.toRawUTF8(), ocat.toRawUTF8());
        EXPECT_STREQ(ocid .toRawUTF8(), "test_string_array");
        EXPECT_STREQ(ocabs.toRawUTF8(), occomb.toRawUTF8());

    }


    {
        // testing strict typing (without)
        EXPECT_NO_THROW({
            propBool->setValue(24);
        });
        
        // testing strict typing (with)
        setup(std::make_unique<::DummyParser>(), "", true, false);
        EXPECT_THROW({
            propBool->setValue(24);
        }, jaut::ConfigIncompatibleTypeException);
        
        
        
        bool valch_hap    = false;
        bool prev_val_val = false;
        
        // testing change events
        auto value_change = [&valch_hap, &prev_val_val](const jaut::Config::Property &obj, const juce::var &old)
        {
            EXPECT_TRUE(static_cast<bool>(old) == prev_val_val);
            EXPECT_STREQ(obj.getId().toRawUTF8(), "test_bool");
            valch_hap = true;
        };
        
        propBool->ValueChanged += jaut::makeHandler(value_change);
        
        propBool->setValue(true);
        EXPECT_FALSE(valch_hap);
        
        prev_val_val = static_cast<bool>(propBool->getValue());
        propBool->setValue(false, juce::dontSendNotification);
        EXPECT_FALSE(valch_hap);
        
        prev_val_val = static_cast<bool>(propBool->getValue());
        propBool->setValue(true);
        EXPECT_TRUE(valch_hap);
        
        
        
        bool propadd_hap = false;
        
        auto prop_added = [&propadd_hap](const jaut::Config::Property &parent, const jaut::Config::Property &addedProp)
        {
            EXPECT_STREQ(parent   .getId().toRawUTF8(), "test_children");
            EXPECT_STREQ(addedProp.getId().toRawUTF8(), "something");
            propadd_hap = true;
        };
        
        propSubProps->PropertyAdded += jaut::makeHandler(prop_added);
        propSubProps->createProperty("something");
        
        EXPECT_TRUE(propadd_hap);
    }
}

//======================================================================================================================
TYPED_TEST_SUITE_P(ParserFixture);
TYPED_TEST_P(ParserFixture, TestParsers)
{
    static juce::String ext = std::decay_t<decltype(*this)>::getExtension().data();
    
    static constexpr int              test_int              = 91039;
    static constexpr double           test_double           = 89342.4234;
    static constexpr std::string_view test_null_string      = "not so null anymore";
    static constexpr std::string_view test_multiline_string = "haha \ni a'm \nmultiline\n\n";
    
    this->setup(juce::String("config.") + ext);
    this->propBool       ->setValue(false);
    this->propInt        ->setValue(test_int);
    this->propDouble     ->setValue(test_double);
    this->propNull       ->setValue(juce::String(test_null_string     .data()));
    this->propString     ->setValue(juce::String(test_multiline_string.data()));
    this->propInt64      ->setValue({});
        
    const juce::StringArray test_strings {
        "string 1 new",
        "string 2 new",
        "string your mom again",
    };
    this->propStringArray->setValue(test_strings);
    this->config->save();
    
    this->setup(juce::String("config.") + ext);
    
    EXPECT_TRUE     (this->propNull->getValue().isVoid());
    EXPECT_TRUE     (static_cast<bool>        (this->propBool  ->getValue()));
    EXPECT_DOUBLE_EQ(static_cast<double>      (this->propDouble->getValue()), 23.0);
    EXPECT_EQ       (static_cast<int>         (this->propInt   ->getValue()), 45  );
    EXPECT_STREQ    (this->propString->toString().toRawUTF8(),                "a string");
    EXPECT_EQ       (static_cast<juce::int64>(this->propInt64 ->getValue()),  std::numeric_limits<juce::int64>().max());
    EXPECT_STREQ    (this->propObjectArray->getValue().getArray()->getReference(5).toString().toRawUTF8(),
                     R"( blbla<&)");
    EXPECT_STREQ    (this->propObjectArray->getValue().getArray()->getReference(0).getDynamicObject()
                                          ->getProperty("string").toString().toRawUTF8(), "\ntesthaha\" \\");
    EXPECT_STREQ    (this->propLongString->toString().toRawUTF8(), longString.data());
    
    {
        const juce::StringArray strings = *this->propStringArray->getValue().getArray();
        EXPECT_STREQ(strings.getReference(0).toRawUTF8(), "string 1");
        EXPECT_STREQ(strings.getReference(1).toRawUTF8(), "string 2");
        EXPECT_STREQ(strings.getReference(2).toRawUTF8(), "string your mom");
    }
    
    this->config->load();
    
    EXPECT_FALSE    (static_cast<bool>  (this->propBool  ->getValue()));
    EXPECT_EQ       (static_cast<int>   (this->propInt   ->getValue()), test_int);
    EXPECT_DOUBLE_EQ(static_cast<double>(this->propDouble->getValue()), test_double);
    EXPECT_STREQ    (this->propNull  ->toString().toRawUTF8(),          test_null_string     .data());
    EXPECT_STREQ    (this->propString->toString().toRawUTF8(),          test_multiline_string.data());
    EXPECT_TRUE     (this->propInt64 ->getValue().isVoid());
    EXPECT_STREQ    (this->propObjectArray->getValue().getArray()->getReference(5).toString().toRawUTF8(),
                     R"( blbla<&)");
    EXPECT_STREQ    (this->propObjectArray->getValue().getArray()->getReference(0).getDynamicObject()
                                          ->getProperty("string").toString().toRawUTF8(), "\ntesthaha\" \\");
    EXPECT_STREQ    (this->propLongString->toString().toRawUTF8(), longString.data());
    
    {
        const juce::StringArray strings = *this->propStringArray->getValue().getArray();
        EXPECT_STREQ(strings.getReference(0).toRawUTF8(), "string 1 new");
        EXPECT_STREQ(strings.getReference(1).toRawUTF8(), "string 2 new");
        EXPECT_STREQ(strings.getReference(2).toRawUTF8(), "string your mom again");
    }
}

//======================================================================================================================
REGISTER_TYPED_TEST_SUITE_P(ParserFixture, TestParsers);
INSTANTIATE_TYPED_TEST_SUITE_P(ParsingTests, ParserFixture, ::TestedParsers,);
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
