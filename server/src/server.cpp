#include "server.hpp"

Server::Server() {}

Server &Server::getInstance()
{
    static Server INSTANCE;
    return INSTANCE;
}

Server::~Server() 
{
    for (auto& socket : clients.GetSockets())
    {
        close(socket);
    }
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

void Server::SendLobbyList(int socket)
{
    LobbyInfoList list;

    // List MAX_LOBBIES_PER_PAGE lobbies
    for (auto i : lobbies.GetLobbyNames(LobbyInfoList::MAX_LOBBIES_PER_PAGE))
    {
        LobbyInfo info = LobbyInfo(i, lobbies.GetLobby(i).GetSize(), lobbies.GetLobby(i).GetPassword());
        
        list.AddLobbyInfo(info);
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_LOBBIES), sizeof(list));

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
    rv = send(socket, &list, sizeof(list), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send: LobbyInfoList" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, LobbyInfoList NOT handled" << std::endl;
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
    LobbyConnectInfo info;

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

    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();
    std::string password = info.GetPassword();

    // Check if the lobby name is unique/correct
    if (lobbies.hasLobby(lobby) || lobby.empty() || lobby.size() > LobbyConnectInfo::MAX_LOBBY_NAME_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_LOBBY_NAME));

        // Send information about non-unique/incorrect lobby name
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

    // Make sure that player name is correct
    if (name.empty() || name.size() > LobbyConnectInfo::MAX_CLIENT_NAME_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PLAYER_NAME));

        // Send information about incorrect player name
        int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

        // Handle errors
        if (rv == -1)
        {
            if (errno != EWOULDBLOCK){
                std::cerr << "ERROR: Failed to send message of type: INCORRECT_PLAYER_NAME" << std::endl;
            } else {
                std::cerr << "INFO: The send buffer is full, INCORRECT_PLAYER_NAME NOT handled" << std::endl;
            }
            return;
        }
        return;
    }

    // Make sure that password is correct
    if (password.size() > LobbyConnectInfo::MAX_LOBBY_PASSWORD_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PASSWORD));

        // Send information about incorrect password
        int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

        // Handle errors
        if (rv == -1)
        {
            if (errno != EWOULDBLOCK){
                std::cerr << "ERROR: Failed to send message of type: INCORRECT_PASSWORD" << std::endl;
            } else {
                std::cerr << "INFO: The send buffer is full, INCORRECT_PASSWORD NOT handled" << std::endl;
            }
            return;
        }
        return;
    }

    // Create lobby
    lobbies.AddLobby(lobby, password);
    std::cout << "Created lobby with name: " << lobby << std::endl;

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully creating a lobby 
    Message message = Message(static_cast<int>(MessageToClient::CONNECT));

    rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send message of type: CONNECT" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, CONNECT NOT handled" << std::endl;
        }
        return;
    }
}

void Server::ConnectToLobby(int socket, int message_size)
{
    LobbyConnectInfo info;

    // Read the client info
    int rv = recv(socket, &info, message_size, MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to receive message of type: CONNECT_TO_LOBBY" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, CONNECT_TO_LOBBY NOT handled" << std::endl;
        }
        return;
    }

    std::string password = info.GetPassword();
    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();


    // Make sure that lobby name is correct
    if (!lobbies.hasLobby(lobby) || lobby.size() > LobbyConnectInfo::MAX_LOBBY_NAME_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_LOBBY_NAME));

        // Send information about incorrect lobby name
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

    // Check if password is correct
    if (lobbies.GetLobby(lobby).GetPassword() != password || password.size() > LobbyConnectInfo::MAX_LOBBY_PASSWORD_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PASSWORD));

        // Send information about incorrect password
        int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

        // Handle errors
        if (rv == -1)
        {
            if (errno != EWOULDBLOCK){
                std::cerr << "ERROR: Failed to send message of type: INCORRECT_PASSWORD" << std::endl;
            } else {
                std::cerr << "INFO: The send buffer is full, INCORRECT_PASSWORD NOT handled" << std::endl;
            }
            return;
        }
        return;
    }

    // Check if player name is unique/correct
    if (lobbies.GetLobby(lobby).hasPlayerName(name) || name.empty() || name.size() > LobbyConnectInfo::MAX_CLIENT_NAME_SIZE){
        Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PLAYER_NAME));

        // Send information about non-unique/incorrect player name
        int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

        // Handle errors
        if (rv == -1)
        {
            if (errno != EWOULDBLOCK){
                std::cerr << "ERROR: Failed to send message of type: INCORRECT_PLAYER_NAME" << std::endl;
            } else {
                std::cerr << "INFO: The send buffer is full, INCORRECT_PLAYER_NAME NOT handled" << std::endl;
            }
            return;
        }
        return;
    }

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully adding to lobby 
    Message message = Message(static_cast<int>(MessageToClient::CONNECT));

    rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        if (errno != EWOULDBLOCK){
            std::cerr << "ERROR: Failed to send message of type: CONNECT" << std::endl;
        } else {
            std::cerr << "INFO: The send buffer is full, CONNECT NOT handled" << std::endl;
        }
        return;
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
    switch (static_cast<MessageToServer>(message.GetMessageType()))
    {
    case MessageToServer::REQUEST_LOBBIES:
        SendLobbyList(socket);
        break;
    case MessageToServer::CREATE_LOBBY:
        CreateLobby(socket, message.GetSize());
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        ConnectToLobby(socket, message.GetSize());
        break;
    case MessageToServer::START_ROUND:
        break;
    case MessageToServer::UPLOAD_CANVAS:
        break;
    case MessageToServer::UPLOAD_TEXT:
        break;

    // Remove from descriptors and lobby, close socket
    case MessageToServer::INVALID:
        Disconnect(socket);
        break;
    // Unexpected behaviour
    default:
        std::cerr << "ERROR: Received unexpected message type: " << message.GetMessageType() << std::endl;
        Disconnect(socket);
        break;
    }
}

void Server::Write(int socket)
{
    
}

void Server::ExitLobby(int socket)
{
    std::string lobby = clients.GetClient(socket).GetCurrentLobby();
    std::cout << "Client: " << clients.GetClient(socket).GetAddress() 
              << ":" << clients.GetClient(socket).GetPort() 
              << " (\"" << lobbies.GetLobby(lobby).GetPlayerName(socket) 
              << "\"), disconnected from lobby: \"" << lobby << "\"" << std::endl;

    lobbies.GetLobby(lobby).RemovePlayer(socket);

    if (lobbies.GetLobby(lobby).GetSize() == 0)
    {
        lobbies.RemoveLobby(lobby);
        std::cout << "Closed lobby: \"" << lobby << "\"" << std::endl; 
    }

    clients.GetClient(socket).SetCurrentLobby("");
}

void Server::EnterLobby(int socket, const std::string& lobby, const std::string& name)
{
    clients.GetClient(socket).SetCurrentLobby(lobby);
    lobbies.GetLobby(lobby).AddPlayer(socket, name);

    std::cout << "Client: " << clients.GetClient(socket).GetAddress() 
              << ":" << clients.GetClient(socket).GetPort() 
              << ", connected as: \"" << name << "\", with lobby: \"" << lobby << "\"" << std::endl;
}

void Server::Disconnect(int socket)
{
    if (!clients.GetClient(socket).GetCurrentLobby().empty()){
        ExitLobby(socket);
    }
    std::cout << "Closed connection with: " << clients.GetClient(socket).GetAddress() 
                << clients.GetClient(socket).GetPort() << std::endl;
    clients.RemoveClient(socket);
    FD_CLR(socket, &descriptors);
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void Server::Accept()
{
    sockaddr_in clientAddr;
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

    Client client = Client(clientAddr);
    clients.AddClient(clientSocket, client);

    // Log information about connecting
    std::cout << "Connected with " << client.GetAddress() << ":" << client.GetPort() << std::endl;

    // Add the client to the descriptor set
    FD_SET(clientSocket, &descriptors);

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
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

        timeout.tv_sec = 5;
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