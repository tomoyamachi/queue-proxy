#include "queue/queue.h"
#include "connection/connection.h"

std::string createHash(std::string host, std::string body) {
    return std::to_string(std::hash<std::string>{}(host+body));
};

void createDir(std::string path) {
    try{
        if(std::filesystem::create_directory(path))
            std::cout << "created " << path << std::endl;
        else
            std::cerr << "failed create" << path << std::endl;
    } catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }
}

void createDefaultDir() {
    createDir(ROOT_DIR);
    createDir(QUEUE_DIR);
    createDir(QUEUE_REQ_DIR);
    createDir(QUEUE_RES_DIR);
}

int main()
{
    std::filesystem::remove_all(ROOT_DIR);
    createDefaultDir();
    QueueManager queueManager;
    ConnectionManager connectionManager;
    std::future<int> futureQueueManager = std::async(std::launch::async, &QueueManager::run, &queueManager);
    std::future<int> futureConnectionManager = std::async(std::launch::async, &ConnectionManager::run, &connectionManager);


    for (;;)
    {
        std::string uri;
        std::cin >> uri;
        if (uri == "exit")
        {
            queueManager.stop();
            break;
        }

        auto hashed = createHash(uri, "");
        std::ofstream ofs(QUEUE_REQ_DIR + "/" + hashed);
        proxy::request p = {
                "GET",
                uri,
                "body"
        };
        nlohmann::json j = p;
        ofs << j;
        ofs.close();
    }
    const int resultQueueManager = futureQueueManager.get();
    const int resultConnectionManager = futureConnectionManager.get();
    std::cout << "queue: " << resultQueueManager << " connection: " << resultConnectionManager << "\n";
    std::cout << "End main()\n";
}