#include "server.hpp"

Server::Server() {}

Server &Server::getInstance()
{
    static Server INSTANCE;
    return INSTANCE;
}

Server::~Server() 
{
    // TODO: Add code for closing client sockets
    close(serverSocket);
    std::cout << "Closing server" << std::endl;
}

int Server::SetNonBlocking(int socket)
{
    // Read current flags
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) return -1;
    // Set non block
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    
    return 0;
}

void Server::SendLobbiesList(int socket)
{
    LobbiesList lobbiesList;

    // List MAX_LOBBIES_PER_PAGE lobbies
    for (auto i : lobbies)
    {
        LobbyInfo info = LobbyInfo(i.first, i.second.clients.size(), (i.second.password.size() == 0) ? false : true );
        
        lobbiesList.lobbies[lobbiesList.size] = info;

        lobbiesList.size++;
        if (lobbiesList.size >= LobbiesList::MAX_LOBBIES_PER_PAGE) 
        {
            break; 
        }
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_LOBBIES), sizeof(lobbiesList));

    // Send the message type
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send message of type: UPLOAD_LOBBIES" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, UPLOAD_LOBBIES NOT handled" << std::endl;
        }
        return;
    }

    // Send the lobbies list to client
    rv = send(socket, &lobbiesList, sizeof(lobbiesList), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send: LobbiesList" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, LobbiesList NOT handled" << std::endl;
        }
    }
    
}

void Server::SendPlayerList(int socket)
{
    // TODO: Implement sending client list of a lobby
    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_LOBBIES));
}

void Server::CreateLobby(int socket, int message_size)
{
    LobbyCreateInfo info;

    // Read the lobby info
    int rv = recv(socket, &info, message_size, MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to receive message of type: CREATE_LOBBY" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, CREATE_LOBBY NOT handled" << std::endl;
        }
        return;
    }

    // Check if the lobby name is unique
    std::map<std::string, Lobby>::iterator it = lobbies.find(info.name.data());

    if (it != lobbies.end()){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_LOBBY_NAME));

        // Send information about non-unique lobby name
        int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

        // Handle errors
        if (rv == -1)
        {
            if (errno != EWOULDBLOCK){
                std::cerr << "ERROR: Failed to send message of type: INCORRECT_LOBBY_NAME" << std::endl;
            } else {
                std::cerr << "INFO: The send buffer is full, INCORRECT_LOBBY_NAME NOT handled" << std::endl;
            }
            return;
        }
        return;
    }

    lobbies.insert(std::pair<std::string, Lobby>(info.name.data(), Lobby(info.password.data())));
    lobbies[info.name.data()].clients.push_back(socket);

    std::cout << "Created lobby with name: " << info.name.data() << std::endl;

    // Send information about successfully creating a lobby 
    Message message = Message(static_cast<int>(MessageToClient::CONNECT_TO_LOBBY));

    rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send message of type: CONNECT_TO_LOBBY" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, CONNECT_TO_LOBBY NOT handled" << std::endl;
        }
        return;
    }
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

    // Log information about connecting
    std::cout << "Connected with " << inet_ntoa(clientAddr.sin_addr) << ":" 
              << ntohs(clientAddr.sin_port) << std::endl;

    // Add the client to the descriptor set
    FD_SET(clientSocket, &descriptors);

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
}

void Server::Read(int socket)
{
    Message message;

    int rv = recv(socket, &message, sizeof(Message), MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK)
        {
            std::cerr << "ERROR: Failed to read message type" << std::endl;
        }
        return;
    }

    // Handle based on message type
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

    // Remove from descriptors and close socket
    case MessageToServer::INVALID:
        FD_CLR(socket, &descriptors);
        shutdown(socket, SHUT_RDWR);
        close(socket);
        std::cout << "Closed connection" << std::endl; // TODO: Modify to display who got disconnected
        break;
    // Unexpected behaviour
    default:
        std::cerr << "ERROR: Received unexpected message type: " << message.type << std::endl;
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
    int on = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

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
        sleep(1); // TODO: Remove later

        FD_SET(serverSocket, &reading); // Set server socket for accepting connections
        writing = descriptors; // Set all descriptors to handle messages

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        int rc = select(maxSocket++, &reading, &writing, NULL, &timeout);
        if (rc < 0){
            std::cerr << "ERROR: Select failed" << std::endl;
            continue;
        } else if (rc == 0){
            std::cout << "Timed out" << std::endl;
            continue;
        }

        // Accept connections if server socket is reading
        if (FD_ISSET(serverSocket, &reading))
        {
            Accept();

            // Remove server socket from reading set
            FD_CLR(serverSocket, &reading);
        }

        // Handle all clients that are writing 
        for (int socket = serverSocket + 1; socket <= maxSocket && rc > 0; socket++)
        {
            if (FD_ISSET(socket, &writing))
            {
                rc--;
                
                Read(socket);

                // Update max socket if necessary
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

                // Update max socket if necessary
                if (socket == maxSocket) 
                {
                    while (maxSocket > serverSocket && !FD_ISSET(maxSocket, &descriptors)) {
                        maxSocket--;
                    }
                }
            }
        }
    }
}