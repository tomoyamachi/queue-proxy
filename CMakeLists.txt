cmake_minimum_required(VERSION 3.16)
project(simple_clion)

set(CMAKE_CXX_STANDARD 20)
set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")

add_subdirectory(include)

add_executable(queue-proxy
        src/main.cpp
)

target_link_libraries(queue-proxy ${CPR_LIBRARIES})
include_directories(${CPR_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS} ${LOG_INCLUDE_DIRS})