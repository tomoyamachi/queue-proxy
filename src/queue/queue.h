#pragma once
#include "../main.h"
#include <cpr/cpr.h>

// TODO: 継承時のpublicなどの使い分けは?
class QueueManager : public RunParallel {
private:
    proxy::request loadRequest(std::filesystem::directory_entry entry) {
        proxy::request p;
        // TODO: handler file cannot load
        std::ifstream reqfile(entry.path(), std::ifstream::in);
        if (reqfile.is_open()) {
            auto j = nlohmann::json::parse(reqfile);
            p = j;
        }
        reqfile.close();
        if (p.uri == "") {
            throw "load proxy::request";
        }
        return p;
    }

    cpr::Response requestFromParams(proxy::request p) {
        auto url = cpr::Url{p.uri};
        auto headers = cpr::Header{{"content-type", "json"}};

        if (p.method == proxy::METHOD_POST) {
            return cpr::Post(
                    url,
                    cpr::Body{p.body},
                    headers);
        }

        if (p.method == proxy::METHOD_GET) {
            return cpr::Get(url, headers);
        }
        if (p.method == proxy::METHOD_PUT) {
            return cpr::Put(url, headers);
        }
        if (p.method == proxy::METHOD_DELETE) {
            return cpr::Delete(url, headers);
        }

        throw "http method : " + p.method;
    }

public:
    int run() override {
        for (;;) {

            if (QueueManager::m_stop) {
                break;
            }

            std::string path = QUEUE_REQ_DIR + "/";
            for (const auto &entry : std::filesystem::directory_iterator(path)) {
                std::cout << entry.path() << std::endl;

                proxy::request p;
                try {
                    p = loadRequest(entry);
                } catch (std::exception &e) {
                    std::cerr << "exception" << e.what();
                    break;
                } catch (...) {
                    std::cerr << "another";
                    break;
                }

                auto response = requestFromParams(p);

                std::cout << response.text << std::endl;
                // insert response data to file
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
