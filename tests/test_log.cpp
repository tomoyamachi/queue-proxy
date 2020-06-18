#include <gtest/gtest.h>

#include "../src/log.cpp"

class TestLog : public ::testing::Test {
};

namespace {
    TEST(TestLog, log) {
        std::stringstream actual;
        log(actual, "test");
        ASSERT_EQ(actual.str(), "test\n");
    }
    TEST(TestLog, notextlog) {
        std::stringstream actual;
        log(actual);
        ASSERT_EQ(actual.str(), "notext\n");
    }
}