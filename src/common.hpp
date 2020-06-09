#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT

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

    inline std::string CreateHash(const std::string &host, const std::string &body) {
        return std::to_string(std::hash<std::string>{}(host + body));
    };

    struct Request {
        std::string method;
        std::string uri;
        std::string body;
        // int64_t 確実にサイズが保証。 int, long サイズがコンパイラ依存
        std::int64_t content_length;
        std::string headers;
        std::string remote_address;
    };

    inline void to_json(nlohmann::json &j, const Request &p) {
        j = nlohmann::json{
                {"method", p.method},
                {"uri", p.uri},
                {"body", p.body},
                {"content_length", p.content_length},
                {"headers", p.headers},
                {"remote_address", p.remote_address}};
    }

    inline void from_json(const nlohmann::json &j, Request &p) {
        j.at("method").get_to(p.method);
        j.at("uri").get_to(p.uri);
        j.at("body").get_to(p.body);
        j.at("content_length").get_to(p.content_length);
        j.at("headers").get_to(p.headers);
        j.at("remote_address").get_to(p.remote_address);
    }

    struct Response {
        int status_code;
        std::string body;
    };
    inline void to_json(nlohmann::json &j, const Response &p) {
        j = nlohmann::json{
                {"code", p.status_code},
                {"body", p.body}};
    }

    inline void from_json(const nlohmann::json &j, Response &p) {
        j.at("code").get_to(p.status_code);
        j.at("body").get_to(p.body);
    }
}// namespace ProxyQueue