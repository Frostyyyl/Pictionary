#pragma once

#include <map>
#include <vector>
#include <array>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

class Client
{
public:
    Client(sockaddr_in address) : address(address) {};
    Client() = default;
    ~Client() noexcept = default;

    int getPort() { return ntohs(address.sin_port); }
    std::string getAddress() { return inet_ntoa(address.sin_addr); }
    std::string getLobby() { return lobby; }
    void setLobby(const std::string& lobby) { this->lobby = lobby; }

private:
    std::string lobby = {};
    sockaddr_in address = {};
};

class ClientManager
{
public:
    ClientManager() = default;
    ~ClientManager() noexcept = default;

    Client getClient(int socket){ return clients[socket]; }
    void addClient(int socket, Client client) { clients.insert({socket, client}); }
    void removeClient(int socket) { clients.erase(socket); }

private:
    std::map<int, Client> clients = {};
} ;