#pragma once
#include "../main.h"
#include <cpr/cpr.h>

class QueueManager: public RunParallel {
public:
    int run() override {
        for (;;)
        {

            if (QueueManager::m_stop) {
                break;
            }

            std::string path = QUEUE_REQ_DIR + "/";
            for (const auto & entry : std::filesystem::directory_iterator(path)) {

                // TODO: handler file cannot load
                std::ifstream reqfile(entry.path(), std::ifstream::in);
                if (reqfile.is_open()) {
                    std::string line;
                    while (std::getline(reqfile, line)) {
                        std::cout << line << std::endl;
                    }
                }
                reqfile.close();


                // TODO : transaction
                auto response = cpr::Get(
                        cpr::Url{"https://httpbin.org/get"},
                        cpr::Body{"hello"},
                        cpr::Header{}
                        );

                auto file = entry.path().filename();
                std::ofstream ofs(QUEUE_RES_DIR + "/" + entry.path().filename().string());
                ofs << "status:" << response.status_code << std::endl;
                //ofs << "header:" << response.header << std::endl;
                ofs << "body:" << response.text << std::endl;
                ofs.close();

                // TODO: handle error
                std::cout << "delete " << entry.path() << std::filesystem::remove(entry.path()) << std::endl;
            }
        }
        return 1;
    }
};

