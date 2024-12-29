#include "server.hpp"

Server::Server() {}

Server &Server::getInstance()
{
    static Server INSTANCE;
    return INSTANCE;
}

Server::~Server() {}

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
        for (auto &socket : ClientManager::getInstance().GetSockets())
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
            // std::cout << "Timed out" << std::endl;
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
                    while (maxSocket > serverSocket && !ClientManager::getInstance().hasClient(maxSocket))
                    {
                        maxSocket--;
                    }
                }
            }
            // Handle ClientManager::getInstance() that are writing
            else if (FD_ISSET(socket, &writing_list))
            {
                rc--;

                Read(socket);

                // Update max socket if necessary (socket was closed)
                if (socket == maxSocket)
                {
                    while (maxSocket > serverSocket && !ClientManager::getInstance().hasClient(maxSocket))
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
    for (auto &socket : ClientManager::getInstance().GetSockets())
    {
        std::cout << "Closing connection with: " << ClientManager::getInstance().GetClient(socket)->GetAddress() << ":"
                  << ClientManager::getInstance().GetClient(socket)->GetPort() << std::endl;
        shutdown(socket, SHUT_RDWR);
        close(socket);
    }
    if (isInit)
    {
        close(serverSocket);
    }

    exit(EXIT_SUCCESS);
}

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

void Server::Accept()
{
    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
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
    ClientManager::getInstance().AddClient(clientSocket, clientAddr);

    // Log information about connecting
    std::cout << "Connected with " << client.GetAddress() << ":" << client.GetPort() << std::endl;

    // Update the maximum socket value if necessary
    if (clientSocket > maxSocket)
    {
        maxSocket = clientSocket;
    }
}

void Server::ExitLobby(int socket)
{
    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();
    std::cout << "Client: " << ClientManager::getInstance().GetClient(socket)->GetAddress()
              << ":" << ClientManager::getInstance().GetClient(socket)->GetPort()
              << " (\"" << LobbyManager::getInstance().GetLobby(lobby)->GetPlayer(socket)->GetName()
              << "\"), disconnected from lobby: \"" << lobby << "\"" << std::endl;

    LobbyManager::getInstance().GetLobby(lobby)->RemovePlayer(socket);

    if (LobbyManager::getInstance().GetLobby(lobby)->GetSize() == 0)
    {
        LobbyManager::getInstance().RemoveLobby(lobby);
        std::cout << "Closed lobby: \"" << lobby << "\"" << std::endl;
    }

    ClientManager::getInstance().GetClient(socket)->SetCurrentLobby("");
}

void Server::EnterLobby(int socket, const std::string &lobby, const std::string &name)
{
    ClientManager::getInstance().GetClient(socket)->SetCurrentLobby(lobby);
    LobbyManager::getInstance().GetLobby(lobby)->AddPlayer(socket, name);

    std::cout << "Client: " << ClientManager::getInstance().GetClient(socket)->GetAddress()
              << ":" << ClientManager::getInstance().GetClient(socket)->GetPort()
              << ", connected as: \"" << name << "\", with lobby: \"" << lobby << "\"" << std::endl;
}

void Server::Disconnect(int socket)
{
    if (!ClientManager::getInstance().GetClient(socket)->GetCurrentLobby().empty())
    {
        ExitLobby(socket);
    }
    std::cout << "Closed connection with: " << ClientManager::getInstance().GetClient(socket)->GetAddress() << ":"
              << ClientManager::getInstance().GetClient(socket)->GetPort() << std::endl;
    ClientManager::getInstance().RemoveClient(socket);
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void Server::SendIncorrectLobbyName(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_LOBBY_NAME));

    // Send information about non-unique/incorrect lobby name
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::INCORRECT_LOBBY_NAME))
    {
        Disconnect(socket);
    }
}

void Server::SendIncorrectPlayerName(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PLAYER_NAME));

    // Send information about incorrect player name
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::INCORRECT_PLAYER_NAME))
    {
        Disconnect(socket);
    }
}

void Server::SendIncorrectPassword(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::INCORRECT_PASSWORD));

    // Send information about incorrect player name
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::INCORRECT_PASSWORD))
    {
        Disconnect(socket);
    }
}

void Server::SendConfirmConnect(int socket)
{
    Message message = Message(static_cast<int>(MessageToClient::CONFIRM_CONNECT));

    // Send information about successful connection
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::CONFIRM_CONNECT))
    {
        Disconnect(socket);
    }
}

void Server::SendLobbyList(int socket)
{
    LobbyInfoList list;

    // List MAX_LOBBIES_PER_PAGE LobbyManager::getInstance()
    for (auto i : LobbyManager::getInstance().GetLobbyNames(LobbyInfoList::MAX_LOBBIES_PER_PAGE))
    {
        LobbyInfo info = LobbyInfo(i, LobbyManager::getInstance().GetLobby(i)->GetSize(), LobbyManager::getInstance().GetLobby(i)->GetPassword());

        list.AddLobbyInfo(info);
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_LOBBIES), sizeof(list));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_LOBBIES))
    {
        Disconnect(socket);
        return;
    }

    // Send the lobbies list to client
    if (!WriteWithRetry(socket, &list, sizeof(list), "LobbyInfoList"))
    {
        Disconnect(socket);
        return;
    }
}

void Server::SendPlayerList(int socket)
{
    PlayerInfoList list;
    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();

    // List player names
    for (auto i : LobbyManager::getInstance().GetLobby(lobby)->GetNames())
    {
        PlayerInfo info = PlayerInfo(i);

        list.AddPlayerInfo(info);
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_PLAYERS), sizeof(list));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_PLAYERS))
    {
        Disconnect(socket);
        return;
    }

    // Send the lobbies list to client
    if (!WriteWithRetry(socket, &list, sizeof(list), "PlayerInfoList"))
    {
        Disconnect(socket);
        return;
    }
}

void Server::SendGameMode(int socket)
{
    GameModeInfo info;
    std::shared_ptr<Lobby> lobby_ptr = LobbyManager::getInstance().GetLobby(ClientManager::getInstance().GetClient(socket)->GetCurrentLobby());

    if (lobby_ptr) // Check if the pointer is valid (not nullptr)
    {
        Lobby &lobby = *lobby_ptr; // Get a reference to the Lobby object

        if (!lobby.hasGameStarted())
        {
            info = GameModeInfo(GameMode::WAIT_FOR_PLAYERS);
        }
        else
        {
            if (!lobby.hasRoundStarted())
            {
                if (socket == lobby.GetPlayerDrawing())
                {
                    info = GameModeInfo(GameMode::DRAW);
                    lobby.SetPlayerDrawing(socket);
                }
                else
                {
                    info = GameModeInfo(GameMode::GUESS);
                }

                lobby.GetPlayer(socket)->SetIsReady(true);

                if (lobby.isEveryoneReady())
                {
                    lobby.StartRound();
                }
            }
            else
            {
                info = GameModeInfo(GameMode::STANDBY);
            }
        }
    }
    else
    {
        // Handle the case where the lobby is not found. This is important!
        std::cerr << "ERROR: Lobby not found for client " << socket << std::endl;
        Disconnect(socket);
        return;
    }

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_GAMEMODE), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_GAMEMODE))
    {
        Disconnect(socket);
        return;
    }

    // Send the LobbyManager::getInstance() list to client
    if (!WriteWithRetry(socket, &info, sizeof(info), "GameModeInfo"))
    {
        Disconnect(socket);
        return;
    }
}

void Server::SendChat(int socket)
{
    // Get chat from lobby
    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();
    ChatInfo chat = LobbyManager::getInstance().GetLobby(lobby)->GetChat();

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_CHAT), sizeof(chat));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_CHAT))
    {
        Disconnect(socket);
        return;
    }

    // Send the chat to client
    if (!WriteWithRetry(socket, &chat, sizeof(chat), "ChatInfo"))
    {
        Disconnect(socket);
        return;
    }
}

void Server::SendCanvasChanges(int socket)
{
    // Get canvas from lobby
    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();
    CanvasChangeInfoList list = LobbyManager::getInstance().GetLobby(lobby)->GetCanvasChanges(socket);

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_CANVAS), sizeof(list));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_CANVAS))
    {
        Disconnect(socket);
        return;
    }

    // Send the canvas to client
    if (!WriteWithRetry(socket, &list, sizeof(list), "CanvasChangeInfoList"))
    {
        Disconnect(socket);
        return;
    }

    // TODO: Clean up canvas changes after sending
}

void Server::SendTime(int socket)
{
    // Get time from lobby
    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();
    TimeInfo time = TimeInfo(LobbyManager::getInstance().GetLobby(lobby)->GetTime());

    Message message = Message(static_cast<int>(MessageToClient::UPLOAD_TIME), sizeof(time));

    // Send the message type
    if (!WriteWithRetry(socket, &message, sizeof(Message), MessageToClient::UPLOAD_TIME))
    {
        Disconnect(socket);
        return;
    }

    // Send the time to client
    if (!WriteWithRetry(socket, &time, sizeof(time), "TimeInfo"))
    {
        Disconnect(socket);
        return;
    }
}

void Server::UpdateChat(int socket, int message_size)
{
    TextInfo info;

    // Read the chat info
    if (!ReadWithRetry(socket, &info, message_size, MessageToServer::UPLOAD_TEXT))
    {
        return;
    }

    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();
    std::string name = info.GetPlayerName();
    std::string text = info.GetText();

    // Add message to chat
    LobbyManager::getInstance().GetLobby(lobby)->AddMessage(TextInfo(name, text));

    // Remove from other structers to handle
    ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
}

void Server::UpdateCanvas(int socket, int message_size)
{
    CanvasChangeInfo info;

    // Read the canvas info
    if (!ReadWithRetry(socket, &info, message_size, MessageToServer::UPLOAD_CANVAS))
    {
        return;
    }

    std::string lobby = ClientManager::getInstance().GetClient(socket)->GetCurrentLobby();

    // Add canvas change to lobby
    LobbyManager::getInstance().GetLobby(lobby)->AddCanvasChange(info);

    // Remove from other structers to handle
    ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
}

void Server::CreateLobby(int socket, int message_size)
{
    ConnectInfo info;

    // Read the lobby info
    if (!ReadWithRetry(socket, &info, message_size, MessageToServer::CREATE_LOBBY))
    {
        return;
    }

    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();
    std::string password = info.GetPassword();

    // Check if the lobby name is unique/correct
    if (LobbyManager::getInstance().hasLobby(lobby) || lobby.empty() || lobby.size() > ConnectInfo::MAX_LOBBY_NAME_SIZE)
    {
        SendIncorrectLobbyName(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Make sure that player name is correct
    if (name.empty() || name.size() > ConnectInfo::MAX_CLIENT_NAME_SIZE)
    {
        SendIncorrectPlayerName(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Make sure that password is correct
    if (password.size() > ConnectInfo::MAX_LOBBY_PASSWORD_SIZE)
    {
        SendIncorrectPassword(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Create lobby
    LobbyManager::getInstance().AddLobby(lobby, password);
    std::cout << "Created lobby with name: " << lobby << std::endl;

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully creating a lobby
    SendConfirmConnect(socket);

    // Remove from other structers to handle
    ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
}

void Server::ConnectToLobby(int socket, int message_size)
{
    ConnectInfo info;

    // Read the client info
    if (!ReadWithRetry(socket, &info, message_size, MessageToServer::CONNECT_TO_LOBBY))
    {
        return;
    }

    std::string password = info.GetPassword();
    std::string lobby = info.GetLobbyName();
    std::string name = info.GetPlayerName();

    // Make sure that lobby name is correct
    if (!LobbyManager::getInstance().hasLobby(lobby) || lobby.size() > ConnectInfo::MAX_LOBBY_NAME_SIZE)
    {
        SendIncorrectLobbyName(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Check if password is correct
    if (LobbyManager::getInstance().GetLobby(lobby)->GetPassword() != password || password.size() > ConnectInfo::MAX_LOBBY_PASSWORD_SIZE)
    {
        SendIncorrectPassword(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Check if player name is unique/correct
    if (LobbyManager::getInstance().GetLobby(lobby)->hasPlayerName(name) || name.empty() || name.size() > ConnectInfo::MAX_CLIENT_NAME_SIZE)
    {
        SendIncorrectPlayerName(socket);
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
        return;
    }

    // Add client do lobby
    EnterLobby(socket, lobby, name);

    // Send information about successfully adding to lobby
    SendConfirmConnect(socket);

    // Remove from other structers to handle
    ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(Message());
}

template <typename T>
bool Server::ReadWithRetry(int socket, void *buffer, size_t size, const T& messageType)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        int rv = recv(socket, buffer, size, MSG_DONTWAIT);
        if (rv == -1)
        {
            if (errno == EWOULDBLOCK)
            {
                return false;
            }
            std::cerr << "ERROR: Failed to read data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
        }
        else if (rv == 0)
        {
            std::cout << "Client closed connection (received shutdown)" << std::endl;
            Disconnect(socket);
            return false;
        }
        else
        {
            return true;
        }
    }
    std::cerr << "ERROR: Failed to recv message of type: " << messageType << std::endl;
    Disconnect(socket);
    return false;
}

template <typename T>
bool Server::WriteWithRetry(int socket, const void *buffer, size_t size, const T& messageType)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        if (send(socket, buffer, size, MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
        {
            if (errno == EWOULDBLOCK)
            {
                std::cerr << "ERROR: Send buffor is full" << std::endl;
            }
            else
            {
                std::cerr << "ERROR: Failed to write data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
            }
        }
        else
        {
            return true;
        }
    }
    std::cerr << "ERROR: Failed to send message of type: " << messageType << std::endl;
    return false;
}

void Server::Read(int socket)
{
    // Check if there is a need to read a structure other than Message
    Message altMessage = ClientManager::getInstance().GetClient(socket)->GetMessageToHandle();

    switch (static_cast<MessageToServer>(altMessage.GetMessageType()))
    {
    case MessageToServer::CREATE_LOBBY:
        CreateLobby(socket, altMessage.GetSize());
        return;
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        ConnectToLobby(socket, altMessage.GetSize());
        return;
        break;
    case MessageToServer::UPLOAD_TEXT:
        UpdateChat(socket, altMessage.GetSize());
        return;
        break;
    case MessageToServer::UPLOAD_CANVAS:
        UpdateCanvas(socket, altMessage.GetSize());
        return;
        break;

    case MessageToServer::INVALID: // Nothing to handle, continue
        break;
    default:
        std::cerr << "ERROR: Added unexpected message to handle" << std::endl;
        Exit();
        break;
    }

    Message message;

    // Read the message type
    if (!ReadWithRetry(socket, &message, sizeof(Message), MessageToServer::INVALID))
    {
        return;
    }

    // Handle based on message type
    switch (static_cast<MessageToServer>(message.GetMessageType()))
    {
    case MessageToServer::REQUEST_LOBBIES:
        SendLobbyList(socket);
        break;
    case MessageToServer::CREATE_LOBBY:
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(message);
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(message);
        break;
    case MessageToServer::REQUEST_PLAYERS:
        SendPlayerList(socket);
        break;
    case MessageToServer::REQUEST_GAMEMODE:
        SendGameMode(socket);
        break;
    case MessageToServer::UPLOAD_CANVAS:
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(message);
        break;
    case MessageToServer::REQUEST_CANVAS:
        SendCanvasChanges(socket);
        break;
    case MessageToServer::UPLOAD_TEXT:
        ClientManager::getInstance().GetClient(socket)->SetMessageToHandle(message);
        break;
    case MessageToServer::REQUEST_CHAT:
        SendChat(socket);
        break;
    case MessageToServer::START_GAME:
        LobbyManager::getInstance().GetLobby(ClientManager::getInstance().GetClient(socket)->GetCurrentLobby())->StartGame();
        break;
    case MessageToServer::REQUEST_PROMPTS:
        break;
    case MessageToServer::PICK_PROMPT:
        break;
    case MessageToServer::REQUEST_TIME:
        SendTime(socket);
        break;

    // Remove from descriptors and lobby, close socket
    case MessageToServer::INVALID:
        std::cerr << "ERROR: Received invalid message type from: " << ClientManager::getInstance().GetClient(socket)->GetAddress() << ":"
                  << ClientManager::getInstance().GetClient(socket)->GetPort() << std::endl;
        Disconnect(socket);
        break;
    // Unexpected behaviour
    default:
        std::cerr << "ERROR: Received unexpected message type " << std::endl;
        Disconnect(socket);
        break;
    }
}