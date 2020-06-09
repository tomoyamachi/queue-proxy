#pragma once
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <string>
#include <thread>
#include <unordered_map>

class RunParallel {
protected:
    std::atomic<bool> m_stop{false};

public:
    virtual int run() = 0;
    void stop() {
        m_stop = true;
    };
};

std::string GetEnvOrDefault(const std::string &variable_name, const std::string &default_value) {
    const char *value = getenv(variable_name.c_str());
    return value ? value : default_value;
}


// 型を指定した状態で環境変数を読み込むことができるライブラリはないか
const std::string ROOT_DIR = GetEnvOrDefault("ROOT_DIR", "tmp");
const std::string QUEUE_DIR = ROOT_DIR + "/queue";
const std::string QUEUE_REQ_DIR = QUEUE_DIR + "/req";
const std::string QUEUE_RES_DIR = QUEUE_DIR + "/response";
const std::string CONNECTION_PATH = ROOT_DIR + "/connection.json";
const int CONNECTION_DURATION = 100000;

namespace proxy {
    // 標準ライブラリにhttpmethodが入っていないか
    const std::string METHOD_POST = "POST";
    const std::string METHOD_GET = "GET";
    const std::string METHOD_PUT = "PUT";
    const std::string METHOD_DELETE = "DELETE";

    struct request {
        std::string method;
        std::string uri;
        std::string body;
        int64_t content_length;
        std::string headers;
        std::string remote_address;
    };

    // TODO : もっとシンプルに書けるか
    void to_json(nlohmann::json &j, const request &p) {
        j = nlohmann::json{
                {"method", p.method},
                {"uri", p.uri},
                {"body", p.body},
                {"content_length", p.content_length},
                {"headers", p.headers},
                {"remote_address", p.remote_address}};
    }

    void from_json(const nlohmann::json &j, request &p) {
        j.at("method").get_to(p.method);
        j.at("uri").get_to(p.uri);
        j.at("body").get_to(p.body);
        j.at("content_length").get_to(p.content_length);
        j.at("headers").get_to(p.headers);
        j.at("remote_address").get_to(p.remote_address);
    }
}// namespace proxy
