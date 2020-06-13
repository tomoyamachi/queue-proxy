#include <gtest/gtest.h>

#include "../src/uri.hpp"

class TestUri : public ::testing::Test {
};

namespace {
    TEST(TestUri, Parse) {
        ProxyQueue::Uri url = ProxyQueue::Uri::Parse("http://test.com/test/test");
        ASSERT_EQ(url.Protocol, "http");
        ASSERT_EQ(url.Host, "test.com");
        ASSERT_EQ(url.Path, "/test/test");
        ASSERT_EQ(url.QueryString, "");
    }
}