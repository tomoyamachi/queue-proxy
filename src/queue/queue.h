#pragma once
#include "../main.h"
class QueueManager: public RunParallel {
public:
    int run() override {
        std::cout << "Start ConnectionManager()\n";

        for (;;)
        {
            if (QueueManager::m_stop) {
                break;
            }
        }

        std::cout << "End ConnectionManager()\n";
        return 1;
    }
};