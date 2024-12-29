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
    std::cerr << "ERROR: Closing due to server error" << std::endl;
    exit(EXIT_FAILURE);
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

    // Prepare error message in case of failure
    errorMessage = "Failed to create lobby, please try again";

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::CREATE_LOBBY << std::endl;
        return false;
    }

    // Send the lobby information
    rv = write(mySocket, &info, sizeof(info));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: While creating lobby failed to send: ConnectInfo" << std::endl;
        return false;
    }

    // Receive the response
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to receive message in method: CreateLobby" << std::endl;
        return false;
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

    errorMessage = "Failed to connect with lobby, please try again";

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::CONNECT_TO_LOBBY << std::endl;
        return false;
    }

    // Send the lobby information
    rv = write(mySocket, &info, sizeof(info));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: While connecting to lobby failed to send: ConnectInfo" << std::endl;
        return false;
    }
    // Receive the response
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to receive message in method: ConnectToLobby" << std::endl;
        return false;
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
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: " << MessageToServer::REQUEST_LOBBIES << std::endl;
        return list;
    }

    // Read the message type alongside the size of lobbies
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: " << MessageToClient::UPLOAD_LOBBIES << std::endl;
        return list;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_LOBBIES:
        break;

    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting lobbies received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the list of lobbies
    rv = read(mySocket, &list, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: LobbyInfoList" << std::endl;
        return list;
    }

    return list;
}

PlayerInfoList NetworkConnector::RequestPlayers()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_PLAYERS));
    PlayerInfoList list;

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: " << MessageToServer::REQUEST_PLAYERS << std::endl;
        return list;
    }

    // Read the message type alongside the size of players list
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: " << MessageToClient::UPLOAD_PLAYERS << std::endl;
        return list;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_PLAYERS:
        break;

    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting players received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the list of players
    rv = read(mySocket, &list, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: PlayerInfoList" << std::endl;
        return list;
    }

    return list;
}

GameMode NetworkConnector::RequestGameMode()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_GAMEMODE));
    GameMode mode;

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: " << MessageToServer::REQUEST_GAMEMODE << std::endl;
        return mode;
    }

    // Read the message type alongside the size of game mode
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: " << MessageToClient::UPLOAD_GAMEMODE << std::endl;
        return mode;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_GAMEMODE:
        break;

    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting game mode received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the game mode
    rv = read(mySocket, &mode, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: GameModeInfo" << std::endl;
        return mode;
    }

    return mode;
}

ChatInfo NetworkConnector::RequestChat()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_CHAT));
    ChatInfo chat;

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: " << MessageToServer::REQUEST_CHAT << std::endl;
        return chat;
    }

    // Read the message type alongside the size of chat
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: " << MessageToClient::UPLOAD_CHAT << std::endl;
        return chat;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::UPLOAD_CHAT:
        break;

    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While requesting chat received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    // Read the chat
    rv = read(mySocket, &chat, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: GameModeInfo" << std::endl;
        return chat;
    }

    return chat;
}

bool NetworkConnector::StartGame()
{
    Message message = Message(static_cast<int>(MessageToServer::START_GAME));

    // Send the message type
    int rv = write(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: " << MessageToServer::START_GAME << std::endl;
        return false;
    }

    // Read the message type
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: " << MessageToClient::CONFIRM_GAME_START << std::endl;
        return false;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.GetMessageType()))
    {
    case MessageToClient::CONFIRM_GAME_START:
        return true;
        break;

    case MessageToClient::INVALID:
        ExitError();
        break;
    default:
        std::cerr << "ERROR: While starting game received unexpected message type" << std::endl;
        ExitError();
        break;
    }

    return false;
}
