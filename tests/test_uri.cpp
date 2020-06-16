#include <gtest/gtest.h>
#include <tuple>

#include "../src/uri.hpp"

class TestUri : public ::testing::Test {
};

namespace {
    TEST(TestUri, TupleParse) {
        const std::vector<std::tuple<std::string, std::string, ProxyQueue::Uri>> tests = {
                {"http",
                 "http://test.com/test/test",
                 ProxyQueue::Uri{
                         "",
                         "/test/test",
                         "http",
                         "test.com",
                         ""}},
                {"https",
                 "https://test.com",
                 ProxyQueue::Uri{
                         "",
                         "",
                         "https",
                         "test.com",
                         ""}},
                {"with query",
                 "http://test.com/test?test=1",
                 ProxyQueue::Uri{
                         "?test=1",
                         "/test",
                         "http",
                         "test.com",
                         ""}}};

//        for (const auto &test : tests) {
//            auto actual = ProxyQueue::Uri::Parse(std::get<1>(test));
//            ASSERT_EQ(actual, test.second);
//        }
        // structured bindings c++17以降
        for (auto[name, uri, expect]:tests) {
            auto actual = ProxyQueue::Uri::Parse(uri);
            ASSERT_EQ(actual, expect) << " : " << name;
        }
    }
}// namespace