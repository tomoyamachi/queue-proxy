#include "queue/queue.h"

int main()
{
    QueueManager queueManager;
    std::future<int> futureQueueManager = std::async(std::launch::async, &QueueManager::run, &queueManager);
    for (;;)
    {
        std::string request;
        std::cin >> request;
        if (request == "exit")
        {
            queueManager.stop();
            break;
        }
    }
    const int resultQueueManager = futureQueueManager.get();
    std::cout << "resultQueueManager: " << resultQueueManager << "\n";
    std::cout << "End main()\n";
}