#include <chrono>
#include <thread>

#include "network_connector.hpp"

NetworkConnector::NetworkConnector() {}

NetworkConnector &NetworkConnector::getInstance()
{
    static NetworkConnector INSTANCE;
    return INSTANCE;
}

NetworkConnector::~NetworkConnector() {}

void NetworkConnector::Init()
{
    struct sockaddr_in address;

    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(ADDRESS.c_str());

    mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mySocket == -1)
    {
        std::cerr << "ERROR: Could not create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (connect(mySocket, (struct sockaddr *)&address, sizeof address) == -1)
    {
        std::cerr << "ERROR: Could not connect to server" << std::endl;
        close(mySocket);
        exit(EXIT_FAILURE);
    }

    isInit = true;
}

void NetworkConnector::Exit()
{
    if (isInitialized())
    {
        shutdown(mySocket, SHUT_RDWR);
        close(mySocket);
    }
}

void NetworkConnector::ExitError()
{
    shutdown(mySocket, SHUT_RDWR);
    close(mySocket);
    exit(EXIT_FAILURE);
}

bool NetworkConnector::WriteWithRetry(int socket, const void *buffer, size_t size)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        if (write(socket, buffer, size) != -1)
        {
            return true;
        }
        std::cerr << "ERROR: Failed to write data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }
    return false;
}

bool NetworkConnector::ReadWithRetry(int socket, void *buffer, size_t size)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        int rv = read(socket, buffer, size);
        if (rv > -1)
        {
            return true;
        }
        else if (rv == 0)
        {
            std::cerr << "ERROR: Server closed connection" << std::endl;
            ExitError();
        }
        std::cerr << "ERROR: Failed to read data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }
    return false;
}

bool NetworkConnector::ValidateData(const std::string &lobby, const std::string &name, const std::string &password)
{
    if (lobby.empty())
    {
        errorMessage = "Lobby name must be non-empty, please try again";
    }
    else if (name.empty())
    {
        errorMessage = "Player name must be non-empty, please try again";
    }
    else
    {
        return true;
    }

    return false;
}

bool NetworkConnector::CreateLobby(const std::string &lobby, const std::string &name, const std::string &password)
{
    if (!ValidateData(lobby, name, password))
    {
        return false;
    }

    ConnectInfo info = ConnectInfo(lobby, name, password);
    Message message = Message(static_cast<int>(MessageToServer::CREATE_LOBBY), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::CREATE_LOBBY << std::endl;
        ExitError();
    }

    // Send the create information
    if (!WriteWithRetry(mySocket, &info, sizeof(info)))
    {
        std::cerr << "ERROR: While creating lobby failed to send: ConnectInfo" << std::endl;
    }

    // Receive the response
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message in method: CreateLobby" << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        errorMessage = "Unfortunately this lobby name is taken, please pick another one";
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        errorMessage = "Player name must be non-empty, please try again";
        break;
    case MessageToClient::CONFIRM_CONNECT:
        hasCreated = true;
        return true;
        break;
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While creating lobby received unexpected message type: " << message.GetMessageType() << std::endl;
        ExitError();
        break;
    }

    return false;
}

bool NetworkConnector::ConnectToLobby(const std::string &lobby, const std::string &name, const std::string &password)
{
    if (!ValidateData(lobby, name, password))
    {
        return false;
    }

    ConnectInfo info = ConnectInfo(lobby, name, password);
    Message message = Message(static_cast<int>(MessageToServer::CONNECT_TO_LOBBY), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::CONNECT_TO_LOBBY << std::endl;
        ExitError();
    }

    // Send the connect information
    if (!WriteWithRetry(mySocket, &info, sizeof(info)))
    {
        std::cerr << "ERROR: While connecting to lobby failed to send: ConnectInfo" << std::endl;
        ExitError();
    }

    // Receive the response
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message in method: ConnectToLobby" << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        errorMessage = "This lobby no longer exists, please try refreshing";
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        errorMessage = "Unfortunately someone already took this name, please pick another one";
        break;
    case MessageToClient::INCORRECT_PASSWORD:
        errorMessage = "Given password is incorrect, try again";
        break;
    case MessageToClient::CONFIRM_CONNECT:
        hasCreated = false;
        return true;
        break;

    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While connecting to lobby received unexpected message type: " << message.GetMessageType() << std::endl;
        ExitError();
        break;
    }

    return false;
}

LobbyInfoList NetworkConnector::RequestLobbies()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_LOBBIES));
    LobbyInfoList list;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::REQUEST_LOBBIES << std::endl;
        ExitError();
    }

    // Read the message type alongside the size of lobbies
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message of type: " << MessageToClient::UPLOAD_LOBBIES << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_LOBBIES:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting lobbies received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the list of lobbies
    if (!ReadWithRetry(mySocket, &list, message.GetSize()))
    {
        std::cerr << "ERROR: Failed to receive: LobbyInfoList" << std::endl;
        ExitError();
    }

    return list;
}

PlayerInfoList NetworkConnector::RequestPlayers()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_PLAYERS));
    PlayerInfoList list;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::REQUEST_PLAYERS << std::endl;
        ExitError();
    }

    // Read the message type alongside the size of players list
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message of type: " << MessageToClient::UPLOAD_PLAYERS << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_PLAYERS:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting players received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the list of players
    if (!ReadWithRetry(mySocket, &list, message.GetSize()))
    {
        std::cerr << "ERROR: Failed to receive: PlayerInfoList" << std::endl;
        ExitError();
    }

    return list;
}

GameMode NetworkConnector::RequestGameMode()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_GAMEMODE));
    GameMode mode;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::REQUEST_GAMEMODE << std::endl;
        ExitError();
    }

    // Read the message type alongside the size of game mode
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message of type: " << MessageToClient::UPLOAD_GAMEMODE << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_GAMEMODE:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting game mode received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the game mode
    if (!ReadWithRetry(mySocket, &mode, message.GetSize()))
    {
        std::cerr << "ERROR: Failed to receive: GameModeInfo" << std::endl;
        ExitError();
    }

    return mode;
}

ChatInfo NetworkConnector::RequestChat()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_CHAT));
    ChatInfo chat;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::REQUEST_CHAT << std::endl;
        ExitError();
    }

    // Read the message type alongside the size of chat info
    if (!ReadWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to receive message of type: " << MessageToClient::UPLOAD_CHAT << std::endl;
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_CHAT:
        break; // Continue
    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting chat received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the chat info
    if (!ReadWithRetry(mySocket, &chat, message.GetSize()))
    {
        std::cerr << "ERROR: Failed to receive: ChatInfo" << std::endl;
        ExitError();
    }

    return chat;
}

void NetworkConnector::StartGame()
{
    Message message = Message(static_cast<int>(MessageToServer::START_GAME));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type " << MessageToServer::START_GAME << std::endl;
        ExitError();
    }
}

void NetworkConnector::UploadText(const std::string &player, const std::string &text)
{
    TextInfo info = TextInfo(player, text);
    Message message = Message(static_cast<int>(MessageToServer::UPLOAD_TEXT), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message)))
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::UPLOAD_TEXT << std::endl;
        ExitError();
    }

    // Send the text information
    if (!WriteWithRetry(mySocket, &info, sizeof(info)))
    {
        std::cerr << "ERROR: While uploading text failed to send: TextInfo" << std::endl;
        ExitError();
    }
}