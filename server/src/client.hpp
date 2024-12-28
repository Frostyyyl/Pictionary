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

class ClientManager
{
public:
    ClientManager() = default;
    ~ClientManager() noexcept = default;

    Client &GetClient(int socket) { return clients[socket]; }
    void AddClient(int socket, Client client) { clients.insert({socket, client}); }
    void RemoveClient(int socket) { clients.erase(socket); }
    std::vector<int> GetSockets()
    {
        std::vector<int> tmp;

        for (const auto &pair : clients)
        {
            tmp.push_back(pair.first);
        }

        return tmp;
    }
    bool hasClient(int socket)
    {
        for (const auto &pair : clients)
        {
            if (pair.first == socket)
                return true;
        }

        return false;
    }

private:
    std::map<int, Client> clients = {};
};