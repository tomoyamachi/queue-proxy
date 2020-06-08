#include "queue/queue.h"
#include "connection/connection.h"
int main()
{
    QueueManager queueManager;
    ConnectionManager connectionManager;
    std::future<int> futureQueueManager = std::async(std::launch::async, &QueueManager::run, &queueManager);
    std::future<int> futureConnectionManager = std::async(std::launch::async, &ConnectionManager::run, &connectionManager);

    for (;;)
    {
        std::string request;
        std::cin >> request;
        if (request == "exit")
        {
            queueManager.stop();
            break;
        }

        std::ofstream ofs(QUEUE_DIR + "/" + request);
        ofs << "test" << std::endl;
        ofs.close();
    }
    const int resultQueueManager = futureQueueManager.get();
    const int resultConnectionManager = futureConnectionManager.get();
    std::cout << "queue: " << resultQueueManager << " connection: " << resultConnectionManager << "\n";
    std::cout << "End main()\n";
}