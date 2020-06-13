cmake_minimum_required(VERSION 3.16)
project(queue-proxy)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(include)
add_executable(queue-proxy
        src/main.cpp
        src/file.cpp
        )

set(CMAKE_CXX_FLAGS "-DCPPHTTPLIB_OPENSSL_SUPPORT")

if (APPLE)
    set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/opt/openssl/)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
    set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
endif()

find_package(OpenSSL COMPONENTS SSL Crypto)
message(OPENSSL_FOUND)
if (OPENSSL_FOUND)
    target_link_libraries(queue-proxy OpenSSL::SSL)
    target_link_libraries(queue-proxy OpenSSL::Crypto)
endif (OPENSSL_FOUND)

include_directories(${JSON_INCLUDE_DIRS} ${LOG_INCLUDE_DIRS} ${HTTPLIB_INCLUDE_DIRS})

add_subdirectory(tests)