#pragma once
#include "../main.hpp"
#include <cpr/cpr.h>

namespace ProxyQueue {
    // TODO: 継承時のpublicなどの使い分けは?
    class QueueManager : public RunParallel {
    private:
        ProxyQueue::request loadRequest(std::filesystem::directory_entry entry) {
            ProxyQueue::request p;
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

        cpr::Response requestFromParams(ProxyQueue::request p) {
            auto url = cpr::Url{p.uri};
            auto headers = cpr::Header{{"content-type", "json"}};

            if (p.method == ProxyQueue::METHOD_POST) {
                return cpr::Post(
                        url,
                        cpr::Body{p.body},
                        headers);
            }

            if (p.method == ProxyQueue::METHOD_GET) {
                return cpr::Get(url, headers);
            }
            if (p.method == ProxyQueue::METHOD_PUT) {
                return cpr::Put(url, headers);
            }
            if (p.method == ProxyQueue::METHOD_DELETE) {
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
                    // std::cout << entry.path() << std::endl;
                    spdlog::debug("entry.path = " + entry.path().string());
                    ProxyQueue::request p;
                    try {
                        p = loadRequest(entry);
                    } catch (std::exception &e) {
                        std::cerr << "exception : " << e.what() << std::endl;
                        break;
                    } catch (...) {
                        std::cerr << "another" << std::endl;
                        break;
                    }

                    auto response = requestFromParams(p);

                    // std::cout << response.text << std::endl;
                    // insert response data to file
                    auto file = entry.path().filename();
                    std::ofstream ofs(QUEUE_RES_DIR + "/" + entry.path().filename().string());
                    auto proxyResp = ProxyQueue::response{response.status_code,
                                                          response.text};
                    nlohmann::json j = proxyResp;
                    ofs << j;
                    ofs.close();

                    // TODO: handle error
                    std::cout << "delete " << entry.path() << std::filesystem::remove(entry.path()) << std::endl;
                }

                // TODO: sleep_forがないと、remove後もファイルが残る。なんとかしたい
                std::this_thread::sleep_for(std::chrono::milliseconds(REQUEST_DURATION));
            }
            return 1;
        }
    };

}// namespace ProxyQueue