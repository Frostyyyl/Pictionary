#include "network_connector.hpp"

NetworkConnector::NetworkConnector() {}

NetworkConnector &NetworkConnector::getInstance()
{
    static NetworkConnector INSTANCE;
    return INSTANCE;
}

NetworkConnector::~NetworkConnector() {}

bool NetworkConnector::Init()
{
    struct sockaddr_in serverAddr;

    // Here specify server's address and port

    memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1100);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == -1)
    {
        // std::cerr << "Could not create server socket" << std::endl;
        return false;
    }

    if (connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof serverAddr) == -1)
    {
        // std::cerr << "Could not connect to server" << std::endl;
        close(serverSocket);
        return false;
    }

    return true;
}

void NetworkConnector::Exit()
{
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}

bool NetworkConnector::Connect(std::string address, int port)
{
    struct sockaddr_in lobbyAddr;

    memset(&lobbyAddr, 0, sizeof lobbyAddr);
    lobbyAddr.sin_family = AF_INET;
    lobbyAddr.sin_port = htons(port);
    lobbyAddr.sin_addr.s_addr = inet_addr(address.c_str());

    lobbySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (lobbySocket == -1)
    {
        // std::cerr << "Could not create lobby socket" << std::endl;
        return false;
    }

    if (connect(lobbySocket, (struct sockaddr *)&lobbyAddr, sizeof lobbyAddr) == -1)
    {
        // std::cerr << "Could not connect to lobby" << std::endl;
        close(lobbySocket);
        return false;
    }
} 

void NetworkConnector::Disconnect()
{
    shutdown(lobbySocket, SHUT_RDWR);
    close(lobbySocket);
} 