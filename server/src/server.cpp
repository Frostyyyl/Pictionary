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
    // Read current flags
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1)
        return -1;
    // Set non block
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;

    return 0;
}

template <typename T>
void Server::DisplaySendError(const T &message)
{
    if (errno != EWOULDBLOCK)
    {
        std::cerr << "ERROR: Failed to send message of type: \"" << message << "\"" << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Send buffor is full, message: \"" << message << "\" NOT handled" << std::endl;
    }
}

template <typename T>
void Server::DisplayRecvError(const T &message)
{
    if (errno != EWOULDBLOCK)
    {
        std::cerr << "ERROR: Failed to recv message of type: \"" << message << "\"" << std::endl;
    }
    // else
    // {
    //     std::cout << "INFO: Recv would block, message: \"" << message << "\"" << std::endl;
    // }
}

void Server::SendIncorrectLobbyName(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_LOBBY_NAME));

    // Send information about non-unique/incorrect lobby name
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError(MessageToClient::INCORRECT_LOBBY_NAME);
    }
}

void Server::SendIncorrectPlayerName(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PLAYER_NAME));

    // Send information about incorrect player name
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError(MessageToClient::INCORRECT_PLAYER_NAME);
    }
}

void Server::SendIncorrectPassword(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PASSWORD));

    // Send information about incorrect player name
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError(MessageToClient::INCORRECT_PLAYER_NAME);
    }
}

void Server::ConfirmConnect(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::CONNECT));

    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError(MessageToClient::CONNECT);
    }
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
        DisplaySendError(MessageToClient::UPLOAD_LOBBIES);
        return;
    }

    // Send the lobbies list to client
    rv = send(socket, &list, sizeof(list), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError("LobbyInfoList");
        return;
    }
}

void Server::SendPlayerList(int socket)
{
    PlayerInfoList list;
    std::string lobby = clients.GetClient(socket).GetCurrentLobby();

    // List player names
    for (auto i : lobbies.GetLobby(lobby).GetNames())
    {
        PlayerInfo info = PlayerInfo(i);

        list.AddPlayerInfo(info);
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_PLAYERS), sizeof(list));

    // Send the message type
    int rv = send(socket, &message, sizeof(Message), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError(MessageToClient::UPLOAD_PLAYERS);
        return;
    }

    // Send the lobbies list to client
    rv = send(socket, &list, sizeof(list), MSG_NOSIGNAL | MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplaySendError("PlayerInfoList");
        return;
    }
}

void Server::CreateLobby(int socket, int message_size)
{
    ConnectInfo info;

    // Read the lobby info
    int rv = recv(socket, &info, message_size, MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplayRecvError(MessageToServer::CREATE_LOBBY);
        return;
    }

    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();
    std::string password = info.GetPassword();

    // Check if the lobby name is unique/correct
    if (lobbies.hasLobby(lobby) || lobby.empty() || lobby.size() > ConnectInfo::MAX_LOBBY_NAME_SIZE)
    {
        SendIncorrectLobbyName(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Make sure that player name is correct
    if (name.empty() || name.size() > ConnectInfo::MAX_CLIENT_NAME_SIZE)
    {
        SendIncorrectPlayerName(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Make sure that password is correct
    if (password.size() > ConnectInfo::MAX_LOBBY_PASSWORD_SIZE)
    {
        SendIncorrectPassword(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Create lobby
    lobbies.AddLobby(lobby, password);
    std::cout << "Created lobby with name: " << lobby << std::endl;

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully creating a lobby
    ConfirmConnect(socket);

    // Remove from other structers to handle
    clients.GetClient(socket).SetMessageToHandle(Message());
}

void Server::ConnectToLobby(int socket, int message_size)
{
    ConnectInfo info;

    // Read the client info
    int rv = recv(socket, &info, message_size, MSG_DONTWAIT);

    // Handle errors
    if (rv == -1)
    {
        DisplayRecvError(MessageToServer::CONNECT_TO_LOBBY);
        return;
    }

    std::string password = info.GetPassword();
    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();

    // Make sure that lobby name is correct
    if (!lobbies.hasLobby(lobby) || lobby.size() > ConnectInfo::MAX_LOBBY_NAME_SIZE)
    {
        SendIncorrectLobbyName(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Check if password is correct
    if (lobbies.GetLobby(lobby).GetPassword() != password || password.size() > ConnectInfo::MAX_LOBBY_PASSWORD_SIZE)
    {
        SendIncorrectPassword(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Check if player name is unique/correct
    if (lobbies.GetLobby(lobby).hasPlayerName(name) || name.empty() || name.size() > ConnectInfo::MAX_CLIENT_NAME_SIZE)
    {
        SendIncorrectPlayerName(socket);
        clients.GetClient(socket).SetMessageToHandle(Message());
        return;
    }

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully adding to lobby
    ConfirmConnect(socket);

    // Remove from other structers to handle
    clients.GetClient(socket).SetMessageToHandle(Message());
}

void Server::Read(int socket)
{
    // Check if need to read a structure other than Message
    Message altMessage = clients.GetClient(socket).GetMessageToHandle();

    switch (static_cast<MessageToServer>(altMessage.GetMessageType()))
    {
    case MessageToServer::CREATE_LOBBY:
        CreateLobby(socket, altMessage.GetSize());
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        ConnectToLobby(socket, altMessage.GetSize());
        break;
    
    case MessageToServer::INVALID: // Nothing to handle, continue
        break;
    default:
        std::cerr << "ERROR: Added unexpected message to handle" << std::endl;
        Exit();
        break;
    }




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
        clients.GetClient(socket).SetMessageToHandle(message);
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        clients.GetClient(socket).SetMessageToHandle(message);
        break;
    case MessageToServer::REQUEST_PLAYERS:
        SendPlayerList(socket);
        break;
    case MessageToServer::UPLOAD_CANVAS:
        break;
    case MessageToServer::REQUEST_CANVAS:
        break;
    case MessageToServer::UPLOAD_TEXT:
        break;
    case MessageToServer::REQUEST_CHAT:
        break;
    case MessageToServer::START_ROUND:
        break;

    // Remove from descriptors and lobby, close socket
    case MessageToServer::INVALID:
        std::cout << "Client: " << clients.GetClient(socket).GetAddress() << ":"
                  << clients.GetClient(socket).GetPort() << " closed connection" << std::endl;
        Disconnect(socket);
        break;
    // Unexpected behaviour
    default:
        std::cerr << "ERROR: Received unexpected message type: " << message.GetMessageType() << std::endl;
        // Disconnect(socket);
        break;
    }
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

void Server::EnterLobby(int socket, const std::string &lobby, const std::string &name)
{
    clients.GetClient(socket).SetCurrentLobby(lobby);
    lobbies.GetLobby(lobby).AddPlayer(socket, name);

    std::cout << "Client: " << clients.GetClient(socket).GetAddress()
              << ":" << clients.GetClient(socket).GetPort()
              << ", connected as: \"" << name << "\", with lobby: \"" << lobby << "\"" << std::endl;
}

void Server::Disconnect(int socket)
{
    if (!clients.GetClient(socket).GetCurrentLobby().empty())
    {
        ExitLobby(socket);
    }
    std::cout << "Closed connection with: " << clients.GetClient(socket).GetAddress() << ":"
              << clients.GetClient(socket).GetPort() << std::endl;
    clients.RemoveClient(socket);
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

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
}

void Server::Init()
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
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

    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) == -1)
    {
        std::cerr << "ERROR: Listen failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    maxSocket = serverSocket;
    isInit = true;
}

void Server::Run()
{
    std::cout << "Server listening on port: " << PORT << std::endl;

    while (true)
    {
        FD_ZERO(&reading_list);
        FD_ZERO(&writing_list);
        FD_ZERO(&exception_list);
        
        FD_SET(serverSocket, &reading_list); // Set server socket for accepting connections

        // Set all client descriptors to handle messages
        for (auto& socket : clients.GetSockets())
        {
            FD_SET(socket, &writing_list);
        }

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int rc = select(maxSocket + 1, &reading_list, &writing_list, &exception_list, &timeout);
        if (rc < 0)
        {
            std::cerr << "ERROR: Select failed" << std::endl;
            continue;
        }
        else if (rc == 0)
        {
            std::cout << "Timed out" << std::endl;
            continue;
        }

        // Accept connections if server socket is reading
        if (FD_ISSET(serverSocket, &reading_list))
            Accept();

        // Handle set sockets
        for (int socket = serverSocket + 1; socket <= maxSocket && rc > 0; socket++)
        {
            // Handle exceptions
            if (FD_ISSET(socket, &exception_list))
            {
                rc--;

                FD_CLR(socket, &reading_list);
                FD_CLR(socket, &writing_list);
                Disconnect(socket);
                
                if (socket == maxSocket)
                {
                    while (maxSocket > serverSocket && !clients.hasClient(maxSocket))
                    {
                        maxSocket--;
                    }
                }
            }
            // Handle clients that are writing
            else if (FD_ISSET(socket, &writing_list))
            {
                rc--;

                Read(socket);

                // Update max socket if necessary (socket was closed)
                if (socket == maxSocket)
                {
                    while (maxSocket > serverSocket && !clients.hasClient(maxSocket))
                    {
                        maxSocket--;
                    }
                }
            }
        }
    }
}

void Server::Exit()
{
    std::cout << "\nExit by interrupt, closing all connections" << std::endl;
    for (auto &socket : clients.GetSockets())
    {
        close(socket);
    }
    if (isInit)
    {
        close(serverSocket);
    }

    exit(EXIT_SUCCESS);
}