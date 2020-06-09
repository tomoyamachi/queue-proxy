#include "connection/connection.hpp"
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

    std::optional<response> LoadResponse(const std::string &filename) {
        std::ifstream resfile(filename);
        if (!resfile) {
            return std::nullopt;
        }
        spdlog::debug("response file exists:");
        std::cout << resfile.rdbuf() << std::endl;
        // auto j = nlohmann::json::parse(resfile);
        response p = response{
                .status_code = 200,
                .body = "body",
        };
        return p;
    }
}// namespace ProxyQueue

int main() {
    using namespace ProxyQueue;
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

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

    for (;;) {
        std::string uri;
        std::cin >> uri;
        if (uri == "exit") {
            queueManager.stop();
            break;
        }

        const std::string uniqueID = CreateHash(uri, "");
        const std::string responseFile = QUEUE_RES_DIR + "/" + uniqueID;
        if (auto result = LoadResponse(responseFile)) {
            // delete response file
            std::filesystem::remove(responseFile);
            spdlog::debug("delete " + responseFile);
            spdlog::debug("response " + result->body);
            continue;
        }

        const std::string requestFile = QUEUE_REQ_DIR + "/" + uniqueID;
        if (std::filesystem::exists(requestFile)) {
            continue;
        }

        std::ofstream ofs(requestFile);
        request p = {
                .method = "GET",
                .uri = uri,
                .body = "body"};
        nlohmann::json j = p;
        ofs << j;
    }
    //    const int resultQueueManager = futureQueueManager.get();
    //    const int resultConnectionManager = futureConnectionManager.get();
}