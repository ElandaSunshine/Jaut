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
    
    @author elanda
    @file   classtype.h
    @date   18.03.20
    
    ===============================================================
 */
#pragma once

namespace jaut::reflect
{
class ClassType
{
private:
    struct FunctionInfo
    {
        const String name;
        const String returnType;
        std::vector<ClassType&> parameters;
    };
    
    struct FieldInfo
    {
        const String name;
    };
    
    std::unordered_map<String, FunctionInfo> functions;
    std::unordered_map<String, FieldInfo> fields;
};
}
