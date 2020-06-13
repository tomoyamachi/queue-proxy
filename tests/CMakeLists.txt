# 'Google_test' is the subproject name
project(Google_tests)

find_package(OpenSSL COMPONENTS SSL Crypto)
if (OPENSSL_FOUND)
    target_link_libraries(queue-proxy OpenSSL::SSL)
    target_link_libraries(queue-proxy OpenSSL::Crypto)
endif (OPENSSL_FOUND)

add_subdirectory(include/gtest)
message(gtest_SOURCE_DIR)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
add_executable(Google_Tests_run test_main.cpp test_uri.cpp)
target_link_libraries(Google_Tests_run gtest gtest_main)