#include <gtest/gtest.h>

#include "zippp/zip.h"

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <array>


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