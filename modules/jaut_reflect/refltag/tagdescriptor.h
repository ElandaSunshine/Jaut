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
    @file   statictagdescriptor.h
    @date   07, March 2020

    ===============================================================
 */

#pragma once

// _JTAG(StoragePolicy{CLASS}, (void)(test)(Graphics&))
// void test(const Graphics&, const int);

#define _JTAG(TAG, TARGET) \
struct refl_INST_paint_graphics
{
    using _refl_Func = void(refl_info::ReflType::*)(Graphics&);
    
    refl_INST_paint_graphics()
        : func(&refl_info::ReflType::paint)
    {}
    
    DaFunc func;
} refl_paint_graphics;

//#define _JREFL(FULL_CLASS_PATH) JAUT_MAKE_CLASS_REFLECTABLE(FULL_CLASS_PATH)

namespace jaut::reflect
{
class TagDescriptor
{
    JAUT_UTIL_TYPE_TRAITS_DEFINE_TYPE_CHECK(hasReftype, refltype_ObjectDeclaration)

public:
    const int mode;
    const char *const targetId;
    void *const target;
    const std::bitset<8> flags;

    //==================================================================================================================
    template<class Type, class Object>
    inline static constexpr TagDescriptor getTag(const char *id, Type &type, Object *object)
    {
        static_assert(hasReftype_v<Object>, "Could not find reflection declaration for class, "
                                            "you will need to declare it reflectable with the \"_JREFL\" macro!");



        // Gather the defined reflection instance of the class.
        auto clazz = Object::refltype_ObjectInstance;

        return {0, id, };
    }
};

#define _JTAG(x) x

//#define JAUT_MAKE_CLASS_REFLECTABLE(NAMESPACE, CLASS)
inline static struct refltype_ObjectDeclaration final
{
    inline static constexpr const char *NAMESPACE = "", *CLASS = "";
    const char *const classPath, *const className;
    std::vector<TagDescriptor> staticFunctions;
    std::vector<TagDescriptor> staticVariables;
    std::unordered_map<
    
    constexpr refltype_ObjectDeclaration() noexcept : classPath(NAMESPACE), className(CLASS)
    {
        numberOfReflectableMembers.template set<5>();
    }
} refltype_ObjectInstance;

struct test
{
    template<class Ret, class ...Args>
    struct test2
    {
        test2(Ret(*function)(Args...))
        {
        
        }
    };
    
    _JTAG
    (
    
    void testfunc(int num));
};

}


