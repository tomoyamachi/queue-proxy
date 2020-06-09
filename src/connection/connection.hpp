#pragma once
#include "../common.hpp"
#include "../run_parallel.hpp"

namespace ProxyQueue {
    class ConnectionManager : public RunParallel {
    public:
        int run() override {
            int i = 0;
            for (;;) {
                if (ConnectionManager::m_stop) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_DURATION));
                i += 1;
                std::ofstream ofs(CONNECTION_PATH);
                ofs << "i = " << i << std::endl;
                ofs.close();
                std::cout << "insert connection" << std::endl;
            }
            return 1;
        }
    };
}// namespace ProxyQueue