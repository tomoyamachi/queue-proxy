#pragma once
#include <atomic>

namespace ProxyQueue {
    class RunParallel {
    protected:
        std::atomic<bool> m_stop{false};

    public:
        virtual ~RunParallel() = default;
        virtual int run() = 0;
        void stop() {
            m_stop = true;
        };
    };
}// namespace ProxyQueue