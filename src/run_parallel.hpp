#pragma once
#include <atomic>

namespace ProxyQueue {
    class RunParallel {
    protected:
        std::mutex m_mutex;
        std::condition_variable m_cv;
        bool m_stop = false;

    public:
        virtual ~RunParallel() = default;
        virtual int run() = 0;
        void notify() {
            m_cv.notify_one();
        }
        void stop() {
            {
                std::lock_guard guard(m_mutex);
                m_stop = true;
            }
            m_cv.notify_one();
        };
    };
}// namespace ProxyQueue