#include "connection/connection.hpp"
#include "httplib_json.hpp"
#include "queue/queue.hpp"


namespace ProxyQueue {
    void CreateDirectory(const std::string &path) {
        if (std::filesystem::create_directories(path)) {
            spdlog::info("create {}", path);
        } else {
            spdlog::error("failed create {}", path);
        }
    }

    void CreateDefaultDirectory() {
        CreateDirectory(QUEUE_REQ_DIR);
        CreateDirectory(QUEUE_RES_DIR);
    }

    std::optional<httplib::Response> LoadResponse(const std::string &filename) {
        std::ifstream resfile(filename);
        if (!resfile) {
            return std::nullopt;
        }
        spdlog::debug("response file exists:");
        //std::cout << resfile.rdbuf() << std::endl;
        httplib::Response p = nlohmann::json::parse(resfile);
        return p;
    }


    std::string CreateFileUniqueID(const httplib::Request &req) {
        // TODO: ヘッダー/REMOTE_PORTなどがuniqueにならない
        // filter fields
        httplib::Request req2;
        req2.method = req.method;
        req2.target = req.target;
        req2.body = req.body;

        nlohmann::json j = req2;
        std::stringstream ss;
        ss << j;
        std::cout << ss.str() << std::endl;
        return CreateHash(ss.str());
    }


    bool SaveAsRequestQueue(const std::string &requestFile, const httplib::Request &req) {
        nlohmann::json j = req;
        if (std::filesystem::exists(requestFile)) {
            return false;
        }
        std::ofstream ofs(requestFile);
        ofs << j;
        return true;
    }
}// namespace ProxyQueue

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
    // TODO : [&] わからない
    svr.Get(".*", [&](const httplib::Request &req, httplib::Response &res) {
        std::string uniqueID = CreateFileUniqueID(req);
        const std::string responseFile = QUEUE_RES_DIR + "/" + uniqueID;
        if (auto result = LoadResponse(responseFile)) {
            // delete response file
            res = result.value();
            std::filesystem::remove(responseFile);
            spdlog::debug("delete " + responseFile);
            return;
        }

        const std::string requestFile = QUEUE_REQ_DIR + "/" + uniqueID;
        if (!std::filesystem::exists(requestFile)) {
            SaveAsRequestQueue(uniqueID, req);
        }

        res.status = 302;
        res.body = "please wait for access";
    });

    // TODO : [&] にすると動かないのはなぜか調査
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

    // TODO: シンプルに書く or 関数を作って呼び出す
    svr.set_logger([](const httplib::Request &req, const httplib::Response &res) {
        std::string s;
        char buf[BUFSIZ];

        s += "================================\n";

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
        s += "--------------------------------\n";
        snprintf(buf, sizeof(buf), "%d\n", res.status);
        s += buf;
        spdlog::error(s);
    });

    svr.listen(PROXY_ADDRESS.c_str(), PROXY_PORT);

    //    const int resultQueueManager = futureQueueManager.get();
    //    const int resultConnectionManager = futureConnectionManager.get();
}