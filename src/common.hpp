#pragma once
#include <algorithm>// find
#include <chrono>
#include <cstdlib>// stdlib(c言語)のc++ version
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace ProxyQueue {
    template<class T>
    inline T GetEnvOrDefault(const std::string &variable_name, const T &default_value) {
        if (const char *value = std::getenv(variable_name.c_str())) {
            std::istringstream is(value);
            T t;
            if (is >> t) {
                return t;
            }
        }
        return default_value;
    }

    const std::string PROXY_ADDRESS = GetEnvOrDefault<std::string>("PROXY_ADDRESS", "localhost");
    const int PROXY_PORT = GetEnvOrDefault("PROXY_PORT", 1234);
    const std::string ROOT_DIR = GetEnvOrDefault<std::string>("ROOT_DIR", "tmp");
    const std::string QUEUE_DIR = ROOT_DIR + "/queue";
    const std::string QUEUE_REQ_DIR = QUEUE_DIR + "/req";
    const std::string QUEUE_RES_DIR = QUEUE_DIR + "/response";
    const std::string CONNECTION_PATH = ROOT_DIR + "/connection.json";
    const int CONNECTION_DURATION = GetEnvOrDefault("CONNECTION_DURATION", 10000);
    const int REQUEST_DURATION = GetEnvOrDefault("REQUEST_DURATION", 1000);

    const std::string METHOD_POST = "POST";
    const std::string METHOD_GET = "GET";
    const std::string METHOD_PUT = "PUT";
    const std::string METHOD_DELETE = "DELETE";

    // 307 Temporary Redirect https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/307
    constexpr int STATUS_TEMPRARY_REDIRECT = 307;

    inline std::string CreateHash(const std::string &unique) {
        return std::to_string(std::hash<std::string>{}(unique));
    };
}// namespace ProxyQueue