#include "connection/connection.h"
#include "queue/queue.h"

// TODO:このあたりのutility関数はどこにおくときれいになる?
std::string createHash(std::string host, std::string body) {
    return std::to_string(std::hash<std::string>{}(host + body));
};

void createDir(std::string path) {
    try {
        if (std::filesystem::create_directory(path))
            std::cout << "created " << path << std::endl;
        else
            std::cerr << "failed create" << path << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void createDefaultDir() {
    createDir(ROOT_DIR);
    createDir(QUEUE_DIR);
    createDir(QUEUE_REQ_DIR);
    createDir(QUEUE_RES_DIR);
}

// TODO: loadRequestと似た構造なのでtemplateとしてつくりたい
// TODO: fileがなければfalseのような場合はどのように実装する? pointerで返してnilなら何もないとかにしたほうがよさそう
proxy::response loadResponse(std::string filename) {
    proxy::response p;
    std::ifstream resfile(filename, std::ifstream::in);
    if (resfile.is_open()) {
        std::cout << "response file exist:" << std::endl;
        std::cout << resfile.rdbuf() << std::endl;
        //        auto j = nlohmann::json::parse(resfile);
        //        p = j;
    }
    resfile.close();
    return p;
}

bool foundResponseFile(std::string filename) {
    std::ifstream ifile;
    ifile.open(filename);
    if (ifile) {
        ifile.close();
        return true;
    } else {
        return false;
    }
}

int main() {
    std::filesystem::remove_all(ROOT_DIR);
    createDefaultDir();
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

        auto hashed = createHash(uri, "");
        auto responseFile = QUEUE_RES_DIR + "/" + hashed;
        if (foundResponseFile(responseFile)) {
            loadResponse(responseFile);

            // delete response file
            std::cout << "delete " << responseFile << std::filesystem::remove(responseFile) << std::endl;
            continue;
        }

        std::ofstream ofs(QUEUE_REQ_DIR + "/" + hashed);
        // TODO: keyを指定して設定したい
        proxy::request p = {
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