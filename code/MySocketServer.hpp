#pragma once
#include <string>
#include <thread>
#include <atomic>

class MySocketServer {
public:
    void start(int port); // Starts the server on the given port
    void stop();          // Stops the server
    bool isRunning(); // Check if the server is running

private:
    static std::atomic<bool> running;
    std::thread serverThread;

    void spawnEnemyNearPlayer(const std::string& enemyType);

};
