#include <iostream> // std::cout, std::cin
#include <string> // std::string
#include <thread> // std::thread, std::this_thread::sleep_for
#include <chrono> // std::chrono::milliseconds
#include <atomic> // std::atomic
#include <unordered_map> // std::unordered_map
#include <mutex> // std::mutex, std::lock_guard
#include <future> // std::async, std::future

inline void Sleep(int milliSec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliSec));
}

inline bool IsReady(const std::future<int>& f)
{
    return (f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
}

// g_data 用のミューテックス
std::mutex g_mutex;
// ハッシュテーブル
std::unordered_map<std::string, int> g_data;

int QueueManager(std::atomic<bool>& stop)
{
    std::cout << "Start QueueManager()\n";

    for (;;)
    {
        if (stop)
        {
            break;
        }

        { // このスコープの間ロック
            std::lock_guard lock(g_mutex);
            // g_data に対する操作
        }

        Sleep(1000);
    }

    std::cout << "End QueueManager()\n";

    return 777;
}

class ConnectionManager
{
private:

    std::atomic<bool> m_stop{ false };

public:

    int run()
    {
        std::cout << "Start ConnectionManager()\n";

        for (;;)
        {
            if (m_stop)
            {
                break;
            }

            { // このスコープの間ロック
                std::lock_guard lock(g_mutex);
                // g_data に対する操作
            }

            Sleep(1000);
        }

        std::cout << "End ConnectionManager()\n";

        return 888;
    }

    void stop()
    {
        m_stop = true;
    }
};

int main()
{
    std::cout << "Start main()\n";

    std::atomic<bool> stopQueueManager{ false };
    // QueueManager を別スレッドで立ち上げ
    std::future<int> futureQueueManager = std::async(std::launch::async, QueueManager, std::ref(stopQueueManager));

    ConnectionManager connectionManager;
    std::future<int> futureConnectionManager = std::async(std::launch::async, &ConnectionManager::run, &connectionManager);

    for (;;)
    {
        std::string request;
        std::cin >> request;

        if (request == "exit")
        {
            stopQueueManager = true;
            connectionManager.stop();
            break;
        }

        { // このスコープの間ロック
            std::lock_guard lock(g_mutex);
            g_data[request] = 100;
            std::cout << "SET: g_data[" << request << "] = 100\n";
        }
    }

    // futureQueueManager と futureConnectionManager の終了待機
    std::cout << "スレッド終了待機...\n";
    const int resultQueueManager = futureQueueManager.get();
    const int resultConnectionManager = futureConnectionManager.get();
    std::cout << "スレッド done!\n";

    std::cout << "resultQueueManager: " << resultQueueManager << "\n";
    std::cout << "resultConnectionManager: " << resultConnectionManager << "\n";

    std::cout << "End main()\n";
}