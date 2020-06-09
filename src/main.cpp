#include "connection/connection.hpp"
#include "queue/queue.hpp"

namespace ProxyQueue {
    void CreateDir(const std::string &path) {
        try {
            if (std::filesystem::create_directory(path))
                std::cout << "created " << path << std::endl;
            else
                std::cerr << "failed create" << path << std::endl;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void CreateDefaultDir() {
        CreateDir(ROOT_DIR);
        CreateDir(QUEUE_DIR);
        CreateDir(QUEUE_REQ_DIR);
        CreateDir(QUEUE_RES_DIR);
    }

    // TODO: loadRequestと似た構造なのでtemplateとしてつくりたい
    // TODO: fileがなければfalseのような場合はどのように実装する? pointerで返してnilなら何もないとかにしたほうがよさそう
    response LoadResponse(const std::string &filename) {
        response p;
        std::ifstream resfile(filename, std::ifstream::in);
        if (resfile.is_open()) {
            spdlog::debug("response file exist:");
            std::cout << resfile.rdbuf() << std::endl;
            //        auto j = nlohmann::json::parse(resfile);
            //        p = j;
        }
        resfile.close();
        return p;
    }

    bool foundResponseFile(const std::string &filename) {
        std::ifstream ifile;
        ifile.open(filename);
        if (ifile) {
            ifile.close();
            return true;
        } else {
            return false;
        }
    }
}// namespace ProxyQueue

int main() {
    using namespace ProxyQueue;
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

    std::filesystem::remove_all(ROOT_DIR);
    CreateDefaultDir();
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

        auto hashed = CreateHash(uri, "");
        auto responseFile = QUEUE_RES_DIR + "/" + hashed;
        if (foundResponseFile(responseFile)) {
            LoadResponse(responseFile);

            // delete response file
            std::filesystem::remove(responseFile);
            spdlog::debug("delete " + responseFile);
            continue;
        }

        std::ofstream ofs(QUEUE_REQ_DIR + "/" + hashed);
        // TODO: keyを指定して設定したい
        request p = {
                "GET",
                uri,
                "body"};
        nlohmann::json j = p;
        ofs << j;
        ofs.close();
    }
    const int resultQueueManager = futureQueueManager.get();
    const int resultConnectionManager = futureConnectionManager.get();
}