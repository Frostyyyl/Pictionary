#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../global/src/messages.hpp"

class Client
{
public:
    Client(sockaddr_in address) : address(address) {};
    Client() = default;
    ~Client() noexcept = default;

    int GetPort() { return ntohs(address.sin_port); }
    std::string GetAddress() { return inet_ntoa(address.sin_addr); }
    std::string GetCurrentLobby() { return lobby; }
    void SetCurrentLobby(const std::string &lobby) { this->lobby = lobby; }
    Message GetMessageToHandle() { return msgToHandle; }
    void SetMessageToHandle(Message message) { msgToHandle = message; }

private:
    Message msgToHandle = Message();
    std::string lobby = {};
    sockaddr_in address = {};
};

class ClientManager {
public:
    static ClientManager& getInstance() {
        static ClientManager instance;
        return instance;
    }

    std::shared_ptr<Client> GetClient(int socket) {
        if (clients.count(socket)) {
            return clients.at(socket);
        }
        return nullptr; // Return nullptr if not found
    }

    void AddClient(int socket, const Client& client) {
        clients.insert({socket, std::make_shared<Client>(client)});
    }

    void RemoveClient(int socket) {
        clients.erase(socket);
    }

    std::vector<int> GetSockets() {
        std::vector<int> tmp;
        for (const auto& pair : clients) {
            tmp.push_back(pair.first);
        }
        return tmp;
    }

    bool hasClient(int socket) {
        return clients.count(socket) > 0; 
    }

private:
    ClientManager() = default;
    ~ClientManager() noexcept = default;
    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    std::map<int, std::shared_ptr<Client>> clients;
};
