#include "connection/connection.hpp"
#include "httplib_json.hpp"
#include "queue/queue.hpp"
#include "file.hpp"

namespace ProxyQueue {
    void Log(const httplib::Request &req, const httplib::Response &res) {
        std::string s;
        char buf[BUFSIZ];

        snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
                 req.version.c_str(), req.path.c_str());
        s += buf;

        std::string query;
        for (auto it = req.params.begin(); it != req.params.end(); ++it) {
            const auto &x = *it;
            snprintf(buf, sizeof(buf), "%c%s=%s",
                     (it == req.params.begin()) ? '?' : '&', x.first.c_str(),
                     x.second.c_str());
            query += buf;
        }
        snprintf(buf, sizeof(buf), "%s\n", query.c_str());
        s += buf;
        snprintf(buf, sizeof(buf), "%d\n", res.status);
        s += buf;
        spdlog::debug(s);
    }
}

int main() {
    using namespace ProxyQueue;
    spdlog::set_level(spdlog::level::debug);

    // start other threads
    spdlog::info("const ROOT_DIR {}, CONNECTION_DURATION {}, REQUEST_DURATION {}", ROOT_DIR, CONNECTION_DURATION, REQUEST_DURATION);
    std::filesystem::remove_all(ROOT_DIR);
    try {
        CreateDefaultDirectory();
    } catch (const std::exception &e) {
        spdlog::error("failed create {}", e.what());
        return 1;
    }
    QueueManager queueManager;
    ConnectionManager connectionManager;
    std::future<int> futureQueueManager = std::async(std::launch::async, &QueueManager::run, &queueManager);
    std::future<int> futureConnectionManager = std::async(std::launch::async, &ConnectionManager::run, &connectionManager);

    // Start a proxy server
    httplib::Server svr;
    svr.Get(".*", [](const httplib::Request &req, httplib::Response &res) {
        std::string uniqueID = CreateFileUniqueID(req);
        const std::string responseFile = QUEUE_RES_DIR + "/" + uniqueID;
        if (auto result = LoadResponse(responseFile)) {
            //response using cached data
            spdlog::debug("response found {}", responseFile);
            res = result.value();
            DeleteFile(responseFile);
            return;
        }

        const std::string requestFile = QUEUE_REQ_DIR + "/" + uniqueID;
        if (!std::filesystem::exists(requestFile)) {
            spdlog::debug("save to request file: {}", requestFile);
            SaveAsRequestQueue(requestFile, req);
        }

        res.status = STATUS_TEMPRARY_REDIRECT;
        res.body = "{\"msg\":\"please wait for access\"}";
    });

    svr.Post(".*", [](const httplib::Request &req, httplib::Response &res) {
        res.status = 200;
        res.body = req.body;
    });

    svr.set_error_handler([](const httplib::Request & /*req*/, httplib::Response &res) {
        const char *fmt = "Error %d";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/plain");
    });

    svr.set_logger(Log);
    svr.listen(PROXY_ADDRESS.c_str(), PROXY_PORT);

    //    const int resultQueueManager = futureQueueManager.get();
    //    const int resultConnectionManager = futureConnectionManager.get();
}

