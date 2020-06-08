#include "queue/queue.h"
#include <fstream>
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


        std::ofstream ofs(QUEUE_PATH+"/"+request);
        ofs << "test" << std::endl;
        ofs.close();
    }
    const int resultQueueManager = futureQueueManager.get();
    std::cout << "resultQueueManager: " << resultQueueManager << "\n";
    std::cout << "End main()\n";
}