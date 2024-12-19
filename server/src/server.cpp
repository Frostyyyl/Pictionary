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

void Server::SendLobbiesList(int socket)
{
    LobbiesList lobbiesList;
    int counter = 1;

    // List MAX_LOBBIES_PER_PAGE lobbies
    for (auto i : lobbies)
    {
        LobbyInfo info = LobbyInfo(i.first, i.second.clients.size(), (i.second.password.size() == 0) ? false : true );
        
        lobbiesList.lobbies.push_back(info);

        if (counter > LobbiesList::MAX_LOBBIES_PER_PAGE) { break; }

        counter++; 
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_LOBBIES), sizeof(lobbiesList));

    // Send the prepared lobbies list to client
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send message type: UPLOAD_LOBBIES" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, resend of message type: UPLOAD_LOBBIES NOT handled" << std::endl;
        }
        return;
    }

    int rv = send(socket, &lobbiesList, sizeof(lobbiesList), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send list of lobbies" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, resend of lobbies list NOT handled" << std::endl;
        }
    }
    
}

void Server::CreateLobby(int socket, int message_size)
{
    LobbyCreateInfo info;

    int rv = recv(socket, &info, message_size, MSG_DONTWAIT);

    lobbies.insert(std::pair<std::string, Lobby>(info.name, Lobby(info.password, socket)));

    // TODO:
}

void Server::Accept()
{
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &size);
    if (clientSocket == -1)
    {
        std::cerr << "ERROR: Accept failed" << std::endl;
        return;
    }

    if (SetNonBlocking(clientSocket) == -1)
    {
        std::cerr << "ERROR: Set non block failed" << std::endl;
        close(clientSocket);
        return;
    }

    // Log connection
    std::cout << "Connected with " << inet_ntoa(clientAddr.sin_addr) << ":" 
              << ntohs(clientAddr.sin_port) << std::endl;

    // Add the client to the descriptor set
    FD_SET(clientSocket, &descriptors);

    SendLobbiesList(clientSocket);

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
}

void Server::Read(int socket)
{
    Message message;

    int rv = recv(socket, &message, sizeof(MessageToServer), MSG_DONTWAIT);

    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to receive message type" << std::endl;
        } else {
            std::cout << "INFO: The receive buffer is full" << std::endl;
        }
        return;
    }

    switch (static_cast<MessageToServer>(message.type))
    {
    case MessageToServer::ENTER_HUB:
        SendLobbiesList(socket);
        break;
    case MessageToServer::CREATE_LOBBY:
        CreateLobby(socket, message.size);
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        break;
    case MessageToServer::START_ROUND:
        break;
    case MessageToServer::UPLOAD_CANVAS:
        break;
    case MessageToServer::UPLOAD_TEXT:
        break;

    default:
        std::cout << "INFO: Received unhandled message type" << std::endl;
        break;
    }

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

        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        int rc = select(maxSocket++, &reading, &writing, NULL, &timeout);
        if (rc < 0){
            std::cerr << "ERROR: Select failed" << std::endl;
            continue;
        } else if (rc == 0){
            continue;
        }

        // Accept connections if server socket is reading
        if (FD_ISSET(serverSocket, &reading))
        {
            Accept();

            // Remove from reading to not get catched in the next step
            FD_CLR(serverSocket, &reading);
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

    close(serverSocket);
    exit(EXIT_SUCCESS);
}