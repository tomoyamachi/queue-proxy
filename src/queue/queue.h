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

            std::string path = QUEUE_DIR + "/";
            for (const auto & entry : std::filesystem::directory_iterator(path))
                std::cout << entry.path() << std::endl;

        }

        std::cout << "End ConnectionManager()\n";
        return 1;
    }
};

