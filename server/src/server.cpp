#include "server.hpp"

Server::Server() {}

Server &Server::getInstance()
{
    static Server INSTANCE;
    return INSTANCE;
}

Server::~Server() {}

int Server::SetNonBlocking(int socket)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) return -1;
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    
    return 0;
}

void Server::Accept()
{
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &SIZE);
    if (clientSocket == -1)
    {
        std::cerr << "ERROR: Accept failed" << std::endl;
        return;
    }

    if (SetNonBlocking(clientSocket) == -1)
    {
        std::cerr << "ERROR: Set non block failed" << std::endl;
        return;
    }

    std::cout << "Connected with " << inet_ntoa(clientAddr.sin_addr) << ":" 
              << ntohs(clientAddr.sin_port) << std::endl;

    // Add the client to the descriptor set and clients map
    FD_SET(clientSocket, &descriptors);
    clients.insert(std::pair<int, Client>(clientSocket, Client(clientAddr)));

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
}

void Server::Read(int socket)
{

}

void Server::Write(int socket)
{
    
}

void Server::Init(int port)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1)
    {
        std::cerr << "ERROR: Could not create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (SetNonBlocking(serverSocket) == -1)
    {
        std::cerr << "ERROR: Set non block failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "ERROR: Bind failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_QUEUE_SIZE) == -1)
    {
        std::cerr << "ERROR: Listen failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&descriptors);
    FD_ZERO(&reading);
    FD_ZERO(&writing);
    
    FD_SET(serverSocket, &descriptors);

    maxSocket = serverSocket;
}

void Server::Run()
{
    std::cout << "Entering main loop" << std::endl;

    while (true)
    {
        FD_SET(serverSocket, &reading);
        writing = descriptors;

        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        int rc = select(maxSocket++, &reading, &writing, NULL, &timeout);
        if (rc < 0){
            std::cerr << "ERROR: Select failed" << std::endl;
            continue;
        } else if (rc == 0){
            continue;
        }

        // Accept connections if server socket is ready
        if (FD_ISSET(serverSocket, &reading))
        {
            Accept();
        }

        // Handle all clients that are writing 
        for (int socket = serverSocket + 1; socket <= maxSocket && rc > 0; socket++)
        {
            if (FD_ISSET(socket, &writing))
            {
                rc--;
                
                Read(socket);

                if (socket == maxSocket) 
                {
                    while (maxSocket > serverSocket && !FD_ISSET(maxSocket, &descriptors)) {
                        maxSocket--;
                    }
                }
            }
        }

        // Handle all clients that are reading 
        for (int socket = serverSocket + 1; socket <= maxSocket && rc > 0; socket++)
        {
            if (FD_ISSET(socket, &reading))
            {
                rc--;
                
                Write(socket);

                if (socket == maxSocket) 
                {
                    while (maxSocket > serverSocket && !FD_ISSET(maxSocket, &descriptors)) {
                        maxSocket--;
                    }
                }
            }
        }
    }

    for (auto client : clients){
        close(client.first);
    }

    close(serverSocket);
    exit(EXIT_SUCCESS);
}