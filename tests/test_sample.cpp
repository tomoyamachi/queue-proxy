#include <gtest/gtest.h>

#include "../src/common.hpp"

class TestSample : public ::testing::Test {
};

TEST_F(TestSample, add) {
    EXPECT_EQ("GET", ProxyQueue::METHOD_GET);
    EXPECT_EQ("POST", ProxyQueue::METHOD_POST);
}