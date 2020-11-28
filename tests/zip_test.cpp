#include <gtest/gtest.h>

#include "zippp/zip.h"

#include <vector>
#include <list>
#include <forward_list>
#include <string>
#include <iostream>
#include <array>

TEST(ZipppTests, eqTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();
    auto it2 = col.begin();
    ASSERT_NE(&it, &it2);
    EXPECT_TRUE(it == it2);
    EXPECT_TRUE(it2 == it);
    EXPECT_FALSE(it != it2);
    EXPECT_FALSE(it2 != it);
}

TEST(ZipppTests, notEqualTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();
    auto it2 = col.end();
    ASSERT_NE(&it, &it2);
    EXPECT_TRUE(it != it2);
    EXPECT_TRUE(it2 != it);
    EXPECT_FALSE(it == it2);
    EXPECT_FALSE(it2 == it);
}

TEST(ZipppTests, incInplaceTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();
    auto [val] = *(++it);
    auto [val2] = *it;
    EXPECT_EQ(2, val);
    EXPECT_EQ(2, val2);
}

TEST(ZipppTests, incTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();
    auto [val] = *(it++);
    auto [val2] = *it;
    EXPECT_EQ(1, val);
    EXPECT_EQ(2, val2);
}
TEST(ZipppTests, singleTest)
{
    std::vector<int> v{1,2,3};

    int count = 1;
    for(auto&&[i] : zippp::zip(v))
    {
        static_assert(std::is_same<decltype(v)::value_type, std::decay_t<decltype(i)>>::value,
            "Type of binding doesnt match type of list");
        ASSERT_EQ(count++, i);
    }
}

TEST(ZipppTests, iteratorTypeTest)
{
    std::vector<int> v{1,2,3};

    auto zip_col = zippp::zip(v);
    auto zip_iter = zip_col.begin();

    using col_iter_t = decltype(zip_col)::iterator;
    using iter_t = decltype(zip_iter);
    static_assert(std::is_same_v<col_iter_t, iter_t>, "Iterator types dont match");
    ASSERT_TRUE(true);
}

TEST(ZipppTests, stdArrayTest)
{
    std::array<int,3> v{1,2,3};

    int count = 1;
    for(auto&&[i] : zippp::zip(v))
    {
        static_assert(std::is_same<decltype(v)::value_type, std::decay_t<decltype(i)>>::value,
            "Type of binding doesnt match type of list");
        ASSERT_EQ(count++, i);
    }
}

TEST(ZipppTests, cArrayTest)
{
    int v[3] = {1,2,3};

    int count = 1;
    for(auto&&[i] : zippp::zip(v))
    {
        static_assert(std::is_same<std::decay_t<decltype(*v)>, std::decay_t<decltype(i)>>::value,
            "Type of binding doesnt match type of list");
        ASSERT_EQ(count++, i);
    }
}

TEST(ZipppTests, boolVectTest)
{
    std::vector<bool> v = {false, true, false};
    int count = 0;
    for(auto&&[i] : zippp::zip(v))
    {
        // No static asserts as i is a bool reference
        ASSERT_EQ(count++ % 2 == 1, i) << "bools dont match for idx: "  << count;
    }
}

TEST(ZipppTests, doubleTest)
{
    std::vector<int> v{1,2,3};
    std::vector<int> v2{2,4,6};
    int count = 1;
    for(auto&&[i,j] : zippp::zip(v,v2))
    {
        static_assert(std::is_same<decltype(v)::value_type, std::decay_t<decltype(i)>>::value, 
            "Type of binding of i doesnt match type of list");
        static_assert(std::is_same<decltype(v2)::value_type, std::decay_t<decltype(j)>>::value, 
            "Type of binding of j doesnt match type of list");
        EXPECT_EQ(count, i);
        ASSERT_EQ(count*2, j);
        ++count;
    }
}

TEST(ZipppTests, mismatchListTypeTest)
{
    std::vector<int> v{1,2,3};
    std::list<int> v2{2,4,6};
    int count = 1;
    for(auto&&[i,j] : zippp::zip(v,v2))
    {
        static_assert(std::is_same<decltype(v)::value_type, std::decay_t<decltype(i)>>::value,
            "Type of binding of i doesnt match type of list");
        static_assert(std::is_same<decltype(v2)::value_type, std::decay_t<decltype(j)>>::value,
            "Type of binding of j doesnt match type of list");
        EXPECT_EQ(count, i);
        ASSERT_EQ(count*2, j);
        ++count;
    }
}

TEST(ZipppTests, mismatchValueTypeTest)
{
    std::vector<int> v{1,2,3};
    std::vector<unsigned long long> v2{2,4,6};
    int count = 1;
    for(auto&&[i,j] : zippp::zip(v,v2))
    {
        static_assert(std::is_same<decltype(v)::value_type, std::decay_t<decltype(i)>>::value,
            "Type of binding of i doesnt match type of list");
        static_assert(std::is_same<decltype(v2)::value_type, std::decay_t<decltype(j)>>::value,
            "Type of binding of j doesnt match type of list");
        EXPECT_EQ(count, i);
        ASSERT_EQ(count*2, j);
        ++count;
    }
}

TEST(ZipppTests, forwardTagTest)
{
    std::forward_list<int> l1;
    std::list<int> l2;
    std::vector<int> l3;
    auto col = zippp::zip(l1,l2,l3);
    static_assert(std::is_same_v<std::forward_iterator_tag,decltype(col)::iterator::iterator_category>, 
        "Iterator category is not forward iterator");
    ASSERT_TRUE(true);
}

TEST(ZipppTests, biDirTagTest)
{
    std::list<int> l1;
    std::vector<int> l2;
    auto col = zippp::zip(l1,l2);
    static_assert(std::is_same_v<std::bidirectional_iterator_tag,decltype(col)::iterator::iterator_category>, 
        "Iterator category is not bidirectional iterator");
    ASSERT_TRUE(true);
}

TEST(ZipppTests, randomTagTest)
{
    int l1[4];
    std::vector<int> l2;
    auto col = zippp::zip(l1,l2);
    static_assert(std::is_same_v<std::random_access_iterator_tag, decltype(col)::iterator::iterator_category>, 
        "Iterator category is not random access iterator");
    ASSERT_TRUE(true);
}

TEST(ZipppTests, biDirDecInplaceTest)
{
    std::list<int> l{1,2,3};
    auto col = zippp::zip(l);
    auto it = col.end();
    auto [val] = *(--it);
    auto [val2] = *it;
    EXPECT_EQ(val, 3);
    ASSERT_EQ(val2, 3);
}

TEST(ZipppTests, biDirDecTest)
{
    std::list<int> l{1,2,3};
    auto col = zippp::zip(l);
    auto it = col.end();
    auto orig = it--;
    auto [val] = *it;
    EXPECT_EQ(orig, col.end());
    ASSERT_EQ(val, 3);
}

TEST(ZipppTests, randomDirDecInplaceTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.end();
    auto [val] = *(--it);
    auto [val2] = *it;
    EXPECT_EQ(val, 3);
    ASSERT_EQ(val2, 3);
}

TEST(ZipppTests, randomDirDecTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.end();
    auto orig = it--;
    auto [val] = *it;
    EXPECT_EQ(orig, col.end());
    ASSERT_EQ(val, 3);
}

TEST(ZipppTests, randomInplaceIncTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    it += 2;
    auto [val] = *it;

    ASSERT_EQ(val, 3);
}

TEST(ZipppTests, randomInplaceDecTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.end();

    it -= 2;
    auto [val] = *it;

    ASSERT_EQ(val, 2);
}

TEST(ZipppTests, randomAddTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    auto it2 = it + 2;
    auto [val] = *it2;

    ASSERT_EQ(val, 3);
}

TEST(ZipppTests, randomSubTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.end();

    auto it2 = it - 2;
    auto [val] = *it2;

    ASSERT_EQ(val, 2);
}

// Testing wrapper to track destructions of objects
template<typename T>
struct DestructionLoggingWrapper : public T {
    bool* has_destruted;
    
    template<typename...Args>
    DestructionLoggingWrapper(bool* flag, Args... args) : T(std::forward<Args>(args)...), has_destruted(flag) {
        *has_destruted = false;
    }
    ~DestructionLoggingWrapper() {if (has_destruted) *has_destruted = true;}
    DestructionLoggingWrapper(const DestructionLoggingWrapper&) = delete;
    DestructionLoggingWrapper(DestructionLoggingWrapper&& w) : T(std::move(w)) {
        has_destruted = w.has_destruted;
        w.has_destruted = nullptr;
    }
    DestructionLoggingWrapper& operator=(const DestructionLoggingWrapper&) = delete;
    DestructionLoggingWrapper& operator=(DestructionLoggingWrapper&&) = delete;
};

TEST(ZipppTests, tmpListInput)
{
    std::initializer_list<int> ints = {1,2,3};
    bool has_destructed = false;
    {
        auto col = zippp::zip(DestructionLoggingWrapper<std::vector<int>>(&has_destructed, ints));
        ASSERT_FALSE(has_destructed);
    }
    ASSERT_TRUE(has_destructed);
}

TEST(ZipppTests, tmpListMultiInput)
{
    std::initializer_list<int> ints = {1,2,3};
    bool has_destructed = false;
    {
        auto col = zippp::zip(ints, DestructionLoggingWrapper<std::vector<int>>(&has_destructed, ints));
        ASSERT_FALSE(has_destructed);
    }
    ASSERT_TRUE(has_destructed);
}

TEST(ZipppTests, copyTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    auto [val] = *it;
    val = 5;
    EXPECT_EQ(val, 5);
    EXPECT_EQ(v.front(), 1);
}

TEST(ZipppTests, constCopyTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    const auto [val] = *it;
    static_assert(std::is_const_v<decltype(val)>, "Binding is not const");
    EXPECT_EQ(val, 1);
    v.front() = 5;
    EXPECT_EQ(val, 1);
}

TEST(ZipppTests, refTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    auto& [val] = *it;
    val = 5;
    EXPECT_EQ(val, 5);
    EXPECT_EQ(v.front(), 5);
}

TEST(ZipppTests, rvalueRefTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    auto&& [val] = *it;
    val = 5;
    EXPECT_EQ(val, 5);
    EXPECT_EQ(v.front(), 5);
}

TEST(ZipppTests, constRefTest)
{
    std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    const auto& [val] = *it;
    static_assert(std::is_const_v<decltype(val)>, "Binding is not const");
    EXPECT_EQ(val, 1);
    v.front() = 5;
    EXPECT_EQ(val, 5);
}

TEST(ZipppTests, constRefConstSourceTest)
{
    const std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    const auto& [val] = *it;
    static_assert(std::is_const_v<decltype(val)>, "Binding is not const");
    EXPECT_EQ(val, 1);
}

TEST(ZipppTests, copyConstSourceTest)
{
    const std::vector<int> v{1,2,3};
    auto col = zippp::zip(v);
    auto it = col.begin();

    auto [val] = *it;
    val = 5;
    EXPECT_EQ(val, 5);
    EXPECT_EQ(v.front(), 1);
}