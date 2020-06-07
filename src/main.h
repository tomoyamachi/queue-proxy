#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <future>

class RunParallel {
protected:
    std::atomic<bool> m_stop{ false };
public:
    virtual int run() = 0;
    void stop() {
        m_stop = true;
    };
};


