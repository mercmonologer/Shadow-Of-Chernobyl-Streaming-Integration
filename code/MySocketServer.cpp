#include "stdafx.h"
#include "MySocketServer.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <winsock2.h>
#include "level.h"
#include "actor.h"
#include "alife_simulator.h"
#include "ai_space.h"
#include "ai_object_location.h"

#pragma comment(lib, "ws2_32.lib")

std::atomic<bool> MySocketServer::running = false;  
std::mutex fileMutex;

void MySocketServer::start(int port) {
    if (running.exchange(true)) {
        std::cout << "Server is already running. Skipping start." << std::endl;
        return;
    }

    serverThread = std::thread([this, port]() {  // Capture 'this' to use member functions
        WSADATA wsaData;
        SOCKET serverSocket, clientSocket;
        sockaddr_in serverAddr, clientAddr;
        int addrLen = sizeof(clientAddr);

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed!" << std::endl;
            running = false;
            return;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed!" << std::endl;
            WSACleanup();
            running = false;
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed! Port may be in use." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            running = false;
            return;
        }

        if (listen(serverSocket, 5) == SOCKET_ERROR) {
            std::cerr << "Listen failed!" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            running = false;
            return;
        }

        std::cout << "Server started on port " << port << std::endl;

        while (running) {
            clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed!" << std::endl;
                continue;
            }

            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::string message(buffer);

                // Write to file safely
                {
                    std::lock_guard<std::mutex> lock(fileMutex);
                    std::ofstream file("received_messages.txt", std::ios_base::app);
                    if (file.is_open()) {
                        file << "Received: " << message << std::endl;
                    }
                }

                std::cout << "Message received: " << message << std::endl;

                if(message.find("cheesed_to_meet_you") != std::string::npos){
                    //spawn 1 bandit

                    for (int i = 1; i <= 10; i++)
                    {
                        spawnEnemyNearPlayer("m_tushkano_normal");
                    }
                    

                } else if (message.find("puppies!") != std::string::npos){
                    //spawn 2 psuedodogs
                    
                     for (int i = 1; i <= 3; i++) {
                        spawnEnemyNearPlayer("pseudodog_normal");
                    }           

                } else if (message.find("bloodsucker_bukkake") != std::string::npos){
                    // spawn 3 bloodsuckers
                      for (int i = 1; i <= 5; i++) {
                        spawnEnemyNearPlayer("bloodsucker_normal");
                    } 
                } else if (message.find("spawn_caseoh") != std::string::npos){
                    //spawn 3 psuedogiant
                    for (int i = 1; i <= 3; i++)
                    {
                        spawnEnemyNearPlayer("gigant_strong");
                    } 
                } else if (message.find("you_got_snorked") != std::string::npos){
                    //spawn 1 snork
                        spawnEnemyNearPlayer("snork_normal");
                }
            }

            closesocket(clientSocket);
        }

        closesocket(serverSocket);
        WSACleanup();
        running = false;
    });

    serverThread.detach();
}

void MySocketServer::stop() {
    if (!running.exchange(false)) {
        std::cout << "Server is not running. Skipping stop." << std::endl;
        return;
    }

    std::cout << "Stopping server..." << std::endl;
}

bool MySocketServer::isRunning() {
    return running.load();
}

void MySocketServer::spawnEnemyNearPlayer(const std::string& enemyType) {
    if (!g_actor || !ai().get_alife()) {
        std::cerr << "Error: Actor or ALife system is not initialized!" << std::endl;
        return;
    }

    Fvector spawnPos = g_actor->Position();
    spawnPos.x += 5.0f;  // Offset to prevent collision
    spawnPos.z += 5.0f;

    // Get AI location details
    u32 levelVertexID = g_actor->ai_location().level_vertex_id();
    GameGraph::_GRAPH_ID gameVertexID = g_actor->ai_location().game_vertex_id();

    // std::cout << "Spawning enemy: " << enemyType << " at " << spawnPos.x << ", " << spawnPos.y << ", " << spawnPos.z << std::endl;

const_cast<CALifeSimulator*>(ai().get_alife())->spawn_item(enemyType.c_str(), spawnPos, levelVertexID, gameVertexID, ALife::_OBJECT_ID(-1));



}
