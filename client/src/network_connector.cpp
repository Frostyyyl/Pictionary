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
    exit(EXIT_FAILURE);
}

template <typename T>
bool NetworkConnector::WriteWithRetry(int socket, const void *buffer, size_t size, const T &messageType)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        // NOTE: Attempting to write data twice in a row without server
        // reading causes the server to crash
        if (send(socket, buffer, size, 0) != -1)
        {
            return true;
        }
        std::cerr << "ERROR: Failed to write data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
        sleep(RETRY_DELAY_SEC);
    }
    std::cerr << "ERROR: Failed to write message of type: " << messageType << std::endl;
    return false;
}

template <typename T>
bool NetworkConnector::ReadWithRetry(int socket, void *buffer, size_t size, const T &messageType)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        int rv = recv(socket, buffer, size, 0);
        if (rv > -1)
        {
            return true;
        }
        else if (rv == 0)
        {
            std::cerr << "ERROR: Server closed connection (received shutdown)" << std::endl;
            return false;
        }
        std::cerr << "ERROR: Failed to read data, attempt " << (attempt + 1) << " of " << MAX_RETRIES << std::endl;
        sleep(RETRY_DELAY_SEC);
    }
    std::cerr << "ERROR: Failed to read message of type: " << messageType << std::endl;
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
        errorMessage = "Nickname cannot be empty, please try again";
    }
    else if (name == "SERVER")
    {
        errorMessage = "This name is taken, please try again";
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
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::CREATE_LOBBY))
    {
        ExitError();
    }

    // Send the create information
    if (!WriteWithRetry(mySocket, &info, sizeof(info), "ConnectInfo"))
    {
        ExitError();
    }

    // Receive the response
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::CONFIRM_CONNECT))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        errorMessage = "Unfortunately this lobby name is taken, please pick another one";
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        errorMessage = "Incorrect nickname, please try again";
        break;
    case MessageToClient::MAX_LOBBIES_REACHED:
        errorMessage = "Max numbers of lobbies reached, join to one already available";
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
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::CONNECT_TO_LOBBY))
    {
        ExitError();
    }

    // Send the connect information
    if (!WriteWithRetry(mySocket, &info, sizeof(info), "ConnectInfo"))
    {
        ExitError();
    }

    // Receive the response
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::CONFIRM_CONNECT))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        errorMessage = "This lobby no longer exists, please try refreshing";
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        errorMessage = "Unfortunately someone already took this name";
        break;
    case MessageToClient::INCORRECT_PASSWORD:
        errorMessage = "Given password is incorrect, try again";
        break;
    case MessageToClient::CONFIRM_CONNECT:
        hasCreated = false;
        return true;
        break;

    case MessageToClient::INVALID:
        std::cerr << "Server closed connection" << std::endl;
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
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_LOBBIES))
    {
        ExitError();
    }

    // Read the message type alongside the size of lobbies
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_LOBBIES))
    {
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
    if (!ReadWithRetry(mySocket, &list, message.GetSize(), "LobbyInfoList"))
    {
        ExitError();
    }

    return list;
}

PlayerInfoList NetworkConnector::RequestPlayers()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_PLAYERS));
    PlayerInfoList list;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_PLAYERS))
    {
        ExitError();
    }

    // Read the message type alongside the size of players list
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_PLAYERS))
    {
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
    if (!ReadWithRetry(mySocket, &list, message.GetSize(), "PlayerInfoList"))
    {
        ExitError();
    }

    return list;
}

GameModeInfo NetworkConnector::RequestGameMode()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_GAMEMODE));
    GameModeInfo mode;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_GAMEMODE))
    {
        ExitError();
    }

    // Read the message type alongside the size of game mode
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_GAMEMODE))
    {
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
    if (!ReadWithRetry(mySocket, &mode, message.GetSize(), "GameModeInfo"))
    {
        ExitError();
    }

    return mode;
}

ChatInfo NetworkConnector::RequestChat()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_CHAT));
    ChatInfo chat;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_CHAT))
    {
        ExitError();
    }

    // Read the message type alongside the size of chat info
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_CHAT))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_CHAT:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting chat received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the chat info
    if (!ReadWithRetry(mySocket, &chat, message.GetSize(), "ChatInfo"))
    {
        ExitError();
    }

    return chat;
}

CanvasChangeInfoList NetworkConnector::RequestCanvasChange()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_CANVAS));
    CanvasChangeInfoList list;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_CANVAS))
    {
        ExitError();
    }

    // Read the message type alongside the size of canvas info list
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_CANVAS))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_CANVAS:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting canvas received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the canvas info
    if (!ReadWithRetry(mySocket, &list, message.GetSize(), "CanvasInfo"))
    {
        ExitError();
    }

    return list;
}

PromptsInfoList NetworkConnector::RequestPrompts()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_PROMPTS));
    PromptsInfoList list;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_PROMPTS))
    {
        ExitError();
    }

    // Read the message type alongside the size of prompts list
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_PROMPTS))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_PROMPTS:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting prompts received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the prompts list
    if (!ReadWithRetry(mySocket, &list, message.GetSize(), "PromptInfo"))
    {
        ExitError();
    }

    return list;
}

TimeInfo NetworkConnector::RequestTime()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_TIME));
    TimeInfo time;

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::REQUEST_TIME))
    {
        ExitError();
    }

    // Read the message type alongside the size of time
    if (!ReadWithRetry(mySocket, &message, sizeof(Message), MessageToClient::UPLOAD_TIME))
    {
        ExitError();
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_TIME:
        break; // Continue
    case MessageToClient::INVALID:
        std::cerr << "ERROR: Server closed connection" << std::endl;
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting time received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the time
    if (!ReadWithRetry(mySocket, &time, message.GetSize(), "TimeInfo"))
    {
        ExitError();
    }

    return time;
}

void NetworkConnector::StartGame()
{
    Message message = Message(static_cast<int>(MessageToServer::START_GAME));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::START_GAME))
    {
        ExitError();
    }
}

void NetworkConnector::UploadCanvasChange(const CanvasChangeInfo& info)
{
    Message message = Message(static_cast<int>(MessageToServer::UPLOAD_CANVAS), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::UPLOAD_CANVAS))
    {
        ExitError();
    }

    // Send the canvas information
    if (!WriteWithRetry(mySocket, &info, sizeof(info), "CanvasInfo"))
    {
        ExitError();
    }
}

void NetworkConnector::UploadText(const std::string &player, const std::string &text)
{
    TextInfo info = TextInfo(player, text);
    Message message = Message(static_cast<int>(MessageToServer::UPLOAD_TEXT), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::UPLOAD_TEXT))
    {
        ExitError();
    }

    // Send the text information
    if (!WriteWithRetry(mySocket, &info, sizeof(info), "TextInfo"))
    {
        ExitError();
    }
}

void NetworkConnector::UploadPrompt(const std::string &prompt)
{
    PromptInfo info = PromptInfo(prompt);
    Message message = Message(static_cast<int>(MessageToServer::UPLOAD_PROMPT), sizeof(info));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::UPLOAD_PROMPT))
    {
        ExitError();
    }

    // Send the prompt information
    if (!WriteWithRetry(mySocket, &info, sizeof(info), "PromptInfo"))
    {
        ExitError();
    }
}

void NetworkConnector::UploadLobbyExit()
{
    Message message = Message(static_cast<int>(MessageToServer::EXIT_LOBBY));

    // Send the message type
    if (!WriteWithRetry(mySocket, &message, sizeof(Message), MessageToServer::EXIT_LOBBY))
    {
        ExitError();
    }
}