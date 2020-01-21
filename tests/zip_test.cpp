#include <gtest/gtest.h>

#include "zippp/zip.h"

#include <vector>
#include <string>
#include <iostream>
#include <array>

TEST(ZipppTests, basicTest)
{
    const std::vector<int> v{1,2,3};
    //const std::array<int, 3> vs ={1,2,3};
    int vs[3] ={1,2,3};
    //const std::vector<std::string> vs{"a","b","c"};

    for(auto&&[i,s] : zippp::zip(v,vs))
    {
        std::cout << i <<','<< s <<std::endl;
    }
}
