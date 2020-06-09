#pragma once
#include "../common.hpp"
#include "../run_parallel.hpp"
#include <cpr/cpr.h>

namespace ProxyQueue {
    class QueueManager : public RunParallel {
    private:
        std::optional<Request> loadRequest(const std::string &filePath) {
            std::ifstream reqfile(filePath);
            if (!reqfile) {
                return std::nullopt;
            }
            Request p = nlohmann::json::parse(reqfile);
            if (p.uri == "") {
                throw std::runtime_error("request uri is empty.");
            }
            return p;
        }

        cpr::Response requestFromParams(ProxyQueue::Request p) {
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
                    const std::string requestFile = entry.path().string();
                    const std::string responseFile = QUEUE_RES_DIR + "/" + entry.path().filename().string();

                    spdlog::debug("entry.path = " + requestFile);
                    ProxyQueue::Request p;
                    try {
                        if (const auto op = loadRequest(requestFile)) {
                            p = op.value();
                        } else {
                            continue;
                        }
                    } catch (std::exception &e) {
                        spdlog::error("requestFile {} : {}", requestFile, e.what());
                        break;
                    }

                    cpr::Response response = requestFromParams(p);

                    if (std::filesystem::exists(responseFile)) {
                        continue;
                    }

                    std::ofstream ofs(responseFile);
                    auto proxyResp = ProxyQueue::Response{response.status_code,
                                                          response.text};
                    nlohmann::json j = proxyResp;
                    ofs << j;

                    if (const bool removeResult = std::filesystem::remove(requestFile)) {
                        spdlog::info("delete request file {}, result {}", requestFile, removeResult);
                    } else {
                        spdlog::error("failed to delete request file {}, result {}", requestFile, removeResult);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(REQUEST_DURATION));
            }
            return 1;
        }
    };

}// namespace ProxyQueue