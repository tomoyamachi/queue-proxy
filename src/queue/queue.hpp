#pragma once
#include "../common.hpp"
#include "../run_parallel.hpp"
#include "../uri.hpp"
#include <httplib.h>

namespace ProxyQueue {
    bool DeleteFile(std::string filePath) {
        if (const bool removeResult = std::filesystem::remove(filePath)) {
            spdlog::debug("delete request file {}, result {}", filePath, removeResult);
            return true;
        }
        spdlog::error("failed to delete request file {]", filePath);
        return false;
    }

    class QueueManager : public RunParallel {
    private:
        std::optional<httplib::Request> loadRequest(const std::string &filePath) {
            std::ifstream reqfile(filePath);
            if (!reqfile) {
                return std::nullopt;
            }
            httplib::Request p = nlohmann::json::parse(reqfile);
            if (p.target == "") {
                throw std::runtime_error("request uri is empty.");
            }
            return p;
        }


        std::optional<httplib::Response> requestFromParams(const httplib::Request p) {
            Uri url = Uri::Parse(p.target);
            // TODO: client作成部分は httplib::Client か httplib::SSLClient のどちらかを返す関数をつくって、autoで設定したい
            httplib::Client cli(url.Host);
            if (url.Protocol == "https") {
                httplib::SSLClient cli(url.Host);
            }

            if (!cli.is_valid()) {
                throw std::runtime_error("invalid url format : " + p.target);
            }
            std::shared_ptr<httplib::Response> res;
            if (p.method == ProxyQueue::METHOD_POST) {
                res = cli.Post(url.Path.c_str(), p.headers, p.params);
            } else if (p.method == ProxyQueue::METHOD_GET) {
                res = cli.Get(url.Path.c_str());
            } else if (p.method == ProxyQueue::METHOD_PUT) {
                res = cli.Put(url.Path.c_str(), p.headers, p.params);
            } else if (p.method == ProxyQueue::METHOD_DELETE) {
                res = cli.Delete(url.Path.c_str(), p.headers);
            } else {
                throw std::runtime_error("not supported http method : " + p.method);
            }
            if (!res) {
                return std::nullopt;
            }
            return *res;
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
                    httplib::Request p;
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

                    if (std::filesystem::exists(responseFile)) {
                        continue;
                    }

                    httplib::Response proxyResp;
                    try {
                        if (auto r = requestFromParams(p)) {
                            proxyResp = r.value();
                        }
                        // TODO: responseがない場合の処理をどうするか考える
                    } catch (std::exception &e) {
                        spdlog::error("request {}", e.what());
                        DeleteFile(requestFile);
                        continue;
                    } catch (...) {
                        spdlog::error("something wrong");
                        continue;
                    }

                    std::ofstream ofs(responseFile);
                    nlohmann::json j = proxyResp;
                    ofs << j;
                    DeleteFile(requestFile);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(REQUEST_DURATION));
            }
            return 1;
        }
    };

}// namespace ProxyQueue