#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <future>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include <nlohmann/json.hpp>

class RunParallel {
protected:
    std::atomic<bool> m_stop{ false };
public:
    virtual int run() = 0;
    void stop() {
        m_stop = true;
    };
};

const std::string ROOT_DIR = "tmp";
const std::string QUEUE_DIR = ROOT_DIR + "/queue";
const std::string QUEUE_REQ_DIR = QUEUE_DIR + "/req";
const std::string QUEUE_RES_DIR = QUEUE_DIR + "/response";
const std::string CONNECTION_PATH = ROOT_DIR + "/connection.json";
const int CONNECTION_DURATION = 100000;

namespace ProxyRequest {
    struct request {
        std::string host;
        std::string uri;
        std::string method;
        int64_t content_length;
        std::string headers;
        std::string body;
        std::string remote_address;
    };

    void to_json(nlohmann::json &j, const request &p) {
        j = nlohmann::json{{"host",   p.host},
                           {"uri",    p.uri},
                           {"method", p.method}};
    }

    void from_json(const nlohmann::json &j, request &p) {
        j.at("host").get_to(p.host);
    }
}
