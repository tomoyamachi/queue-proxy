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

class RunParallel {
protected:
    std::atomic<bool> m_stop{ false };
public:
    virtual int run() = 0;
    void stop() {
        m_stop = true;
    };
};

const std::string QUEUE_DIR = "tmp/queue";
const std::string CONNECTION_PATH = "tmp/connection.json";
const int CONNECTION_DURATION = 10000;
