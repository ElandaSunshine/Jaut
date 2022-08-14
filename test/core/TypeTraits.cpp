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
    @file   TypeTraits.cpp
    @date   18, April 2022

    ===============================================================
 */

#include <gtest/gtest.h>

#include <jaut_core/util/jaut_TypeContainer.h>
#include <jaut_core/util/jaut_TypeEvaluator.h>
#include <jaut_core/util/jaut_TypeTraits.h>

#include <mutex>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    template<class T>
    using TestLadder_t = jaut::TypeLadder_t
    <
        jaut::TypeCase<std::is_floating_point_v<T>, std::vector<float>>,
        jaut::TypeCase<std::is_integral_v<T>,       std::array<int, 0>>,
        jaut::DefaultCase<std::void_t<>>
    >;
    
    //==================================================================================================================
    template<class TypeList>
    struct ForEachTestFunctor
    {        
        std::array<bool, TypeList::length> flags {};
        
        template<class T>
        void operator()()
        {
            flags[TypeList::template indexOf<T>] = true;
        }
    };
    
    template<class TypeList>
    struct ForAllTestFunctor
    {        
        std::array<bool, TypeList::length> flags {};
        
        template<class ...Types>
        void operator()()
        {
            ((flags[TypeList::template indexOf<Types>] = true), ...);
        }
    };
    
    //==================================================================================================================
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(TypeTraitSuite, TestTemplateTraits)
{
    EXPECT_TRUE ((jaut::sameTypeIgnoreTemplate_v<std::vector<int>,  std::vector<bool>>));
    EXPECT_FALSE((jaut::sameTypeIgnoreTemplate_v<std::vector<bool>, std::array<double, 3>>));
    
    EXPECT_TRUE ((jaut::isTemplated_v<std::allocator<int>>));
    EXPECT_FALSE((jaut::isTemplated_v<std::mutex>));
    
    EXPECT_TRUE ((jaut::hasArguments_v<std::tuple<int>>));
    EXPECT_FALSE((jaut::hasArguments_v<std::tuple<>>));
    
    ASSERT_TRUE ((jaut::hasSameTypeList_v<std::tuple<int, double>, std::pair<int, double>>));
    ASSERT_FALSE((jaut::hasSameTypeList_v<std::tuple<int>, std::pair<int, double>>));
}

TEST(TypeTraitSuite, TestPredicates)
{
    // True if "int"
    using PredicateTrivial = jaut::Predicate<std::is_same<int, jaut::PType<>>>;
    EXPECT_TRUE ((PredicateTrivial::value<int>));
    EXPECT_FALSE((PredicateTrivial::value<double>));
    
    // True if not "int"
    using PredicateNegative = jaut::PredicateNot<std::is_same<int, jaut::PType<>>>;
    EXPECT_FALSE((PredicateNegative::value<int>));
    EXPECT_TRUE ((PredicateNegative::value<double>));
    
    // True if integral type but not "bool"
    using PredicateAnd = jaut::PredicateAnd<std::is_integral<jaut::PType<>>,
                                            jaut::PredicateNot<std::is_same<jaut::PType<>, bool>>>;
    EXPECT_TRUE ((PredicateAnd::value<int>));
    EXPECT_FALSE((PredicateAnd::value<double>));
    EXPECT_FALSE((PredicateAnd::value<bool>));
    
    // True if is integral or floating point
    using PredicateOr = jaut::PredicateOr<std::is_integral<jaut::PType<>>, std::is_floating_point<jaut::PType<>>>;
    EXPECT_TRUE ((PredicateOr::value<long double>));
    EXPECT_TRUE ((PredicateOr::value<bool>));
    EXPECT_FALSE((PredicateOr::value<std::mutex>));
    
    // True only if either const or volatile, but neithor none nor both
    using PredicateXor = jaut::PredicateXor<std::is_const<jaut::PType<>>, std::is_volatile<jaut::PType<>>>;
    EXPECT_TRUE ((PredicateXor::value<const int>));
    EXPECT_TRUE ((PredicateXor::value<volatile double>));
    EXPECT_FALSE((PredicateXor::value<std::mutex>));
    EXPECT_FALSE((PredicateXor::value<const volatile std::array<long, 0>>));
}

TEST(TypeTraitSuite, TestTypeListTraits)
{
    using TestList = jaut::TypeArray<int, double, int, wchar_t, float, std::array<int, 3>, double>;
    EXPECT_EQ((jaut::getTypeListLength_v<TestList>), 7);
    
    using ResultList1 = jaut::TypeArray<std::mutex, double, std::mutex, wchar_t, float, std::array<int, 3>, double>;
    EXPECT_TRUE((jaut::hasSameTypeList_v<jaut::replaceType_t<TestList, int, std::mutex>, ResultList1>));
    
    EXPECT_TRUE ((jaut::hasTemplateType_v<TestList, int>));
    EXPECT_FALSE((jaut::hasTemplateType_v<TestList, std::vector<bool>>));
    
    EXPECT_EQ((jaut::getTypeListLength_v<TestList>), 7);
    
    EXPECT_TRUE ((std::is_same_v<jaut::getTypeAt_t<TestList, 3>, wchar_t>));
    EXPECT_FALSE((std::is_same_v<jaut::getTypeAt_t<TestList, 3>, int>));
    
    EXPECT_EQ((jaut::getIndexOf_v<TestList, wchar_t>), 3);
    EXPECT_EQ((jaut::getIndexOf_v<TestList, double>),  1);
    
    using ResultList2 = jaut::TypeArray<wchar_t, float, std::array<int, 3>>;
    EXPECT_TRUE((jaut::hasSameTypeList_v<jaut::removeFrom_t<TestList, int, double>, ResultList2>));
    
    using ResultList3 = jaut::TypeArray<double, float, std::array<int, 3>, double>;
    EXPECT_TRUE((jaut::hasSameTypeList_v<jaut::removeIf_t<TestList, std::is_integral<jaut::PType<>>>, ResultList3>));
    
    using ResultList4 = jaut::TypeArray<int, double, int, wchar_t, float, std::array<int, 3>, double, char, unsigned>;
    EXPECT_TRUE((jaut::hasSameTypeList_v<jaut::addIfAbsent_t<TestList, char, double, unsigned>, ResultList4>));
    EXPECT_TRUE((jaut::hasSameTypeList_v<jaut::addIfAbsent_t<TestList, int, float, double>, TestList>));
    
    EXPECT_TRUE((std::is_same_v<::TestLadder_t<double>,       std::vector<float>>));
    EXPECT_TRUE((std::is_same_v<::TestLadder_t<unsigned>,     std::array<int, 0>>));
    EXPECT_TRUE((std::is_same_v<::TestLadder_t<std::tuple<>>, std::void_t<>>));
}

TEST(TypeTraitSuite, TestTypeArray)
{
    using TestArray = jaut::TypeArray<int, double, long, long long, wchar_t, long double, unsigned, char, float>;
    EXPECT_TRUE((TestArray::equals<TestArray>));
    
    using Other = std::tuple<int, double, long, long long, wchar_t, long double, unsigned, char, float>;
    EXPECT_TRUE((TestArray::same<Other>));
    
    EXPECT_EQ(TestArray::length, 9);
    
    EXPECT_FALSE(TestArray::isEmpty);
    
    using CleanArray = TestArray::clear;
    EXPECT_TRUE(CleanArray::isEmpty);
    
    EXPECT_EQ((TestArray::indexOf<wchar_t>), 4);
    
    EXPECT_TRUE (TestArray::contains<long long>);
    EXPECT_FALSE(TestArray::contains<unsigned long>);
    
    EXPECT_TRUE((std::is_same_v<TestArray::front, int>));
    EXPECT_TRUE((std::is_same_v<TestArray::back, float>));
    
    using LoopArray = jaut::TypeArray<int, double, long, float>;
    
    ::ForEachTestFunctor<LoopArray> for_each_test;
    LoopArray::forEach(for_each_test);
    
    for (const auto val : for_each_test.flags)
    {
        EXPECT_TRUE(val);
    }
    
    ::ForAllTestFunctor<LoopArray> for_all_test;
    LoopArray::forAll(for_all_test);
    
    for (const auto val : for_all_test.flags)
    {
        EXPECT_TRUE(val);
    }
        
    using FillArray = jaut::TypeArray<int, double, long, float>;
    const auto filled_array = FillArray::fillArray(12, 34.0, 3404l, 34.0f);
    
    for (std::size_t i = 0; i < filled_array.size(); ++i)
    {
        const FillArray::toVariant &value = filled_array[i];
        EXPECT_TRUE(i == value.index());
    }
    
    const auto f = FillArray::construct<2>(234l);
    EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(f)>, long>));
    EXPECT_EQ(f, 234l);
    
    const auto *fptr = FillArray::allocate<3>(599.0f);
    EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(*fptr)>, float>));
    EXPECT_FLOAT_EQ(*fptr, 599.0f);
    delete fptr;
    
    const auto fuptr = FillArray::makeUnique<1>(4324.0);
    EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(fuptr)>, std::unique_ptr<double>>));
    EXPECT_DOUBLE_EQ(*fuptr, 4324.0);
    
    const auto fsptr = FillArray::makeShared<0>(3428);
    EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(fsptr)>, std::shared_ptr<int>>));
    EXPECT_EQ(*fsptr, 3428);
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
