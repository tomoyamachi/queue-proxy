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
