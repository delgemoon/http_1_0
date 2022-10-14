//
// Created by tammd on 10/11/22.
//

#include <gtest/gtest.h>
#include <thread_pool.hpp>
#include <util.hpp>
using namespace std::chrono_literals;
TEST(thrad_pool_test, simple_one)
{
    http10::server::thread_pool tp(5);
    std::this_thread::sleep_for(100s);
}

TEST(util_test, str_to_bool)
{
    auto value = http10::server::str_to_bool("true");
    EXPECT_EQ(true, value);
    value = http10::server::str_to_bool("false");
    EXPECT_EQ(false, value);
    value = http10::server::str_to_bool("FalSE");
    EXPECT_EQ(false, value);
    EXPECT_ANY_THROW(http10::server::str_to_bool("FalSE1"));
}