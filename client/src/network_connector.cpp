#include "network_connector.hpp"

NetworkConnector::NetworkConnector() {}

NetworkConnector &NetworkConnector::getInstance()
{
    static NetworkConnector INSTANCE;
    return INSTANCE;
}

NetworkConnector::~NetworkConnector() {}

std::string NetworkConnector::GetError()
{
    return errorMessage;
}

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

    std::cout << "Connected with server: " << ADDRESS << ":" << PORT << std::endl;
    isInit = true;
}

void NetworkConnector::Exit()
{
    shutdown(mySocket, SHUT_RDWR);
    close(mySocket);
    std::cout << "Closing due to server error" << std::endl;
    exit(EXIT_FAILURE);
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
        std::cerr << "ERROR: Could not send message of type: REQUEST_LOBBIES" << std::endl;
        return list;
    }

    // Read the message type alongside the size of lobbies
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: UPLOAD_LOBBIES" << std::endl;
        return list;
    }

    // Read the list of lobbies
    rv = read(mySocket, &list, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: LobbyInfoList" << std::endl;
        return list;
    }

    std::cout << "Received lobbies" << std::endl;
    return list;
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
        std::cerr << "ERROR: Failed to send message of type: CREATE_LOBBY" << std::endl;
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
    case MessageToClient::CONNECT:
        std::cout << "Succesfully created lobby: \"" << lobby << "\"" << std::endl;
        std::cout << "Connected with lobby: \"" << lobby << "\", as: \"" << name << "\"" << std::endl;
        return true;
        break;

    case MessageToClient::INVALID:
        Exit();
        break;
    default:
        std::cerr << "ERROR: While creating lobby received unexpected message type: " << message.GetMessageType() << std::endl;
        Exit();
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
        std::cerr << "ERROR: Failed to send message of type: CONNECT_TO_LOBBY" << std::endl;
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
    case MessageToClient::CONNECT:
        std::cout << "Connected with lobby: \"" << lobby << "\", as: \"" << name << "\"" << std::endl;
        return true;
        break;

    case MessageToClient::INVALID:
        Exit();
        break;
    default:
        std::cerr << "ERROR: While connecting to lobby received unexpected message type: " << message.GetMessageType() << std::endl;
        Exit();
        break;
    }

    return false;
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
        std::cerr << "ERROR: Could not send message of type: REQUEST_PLAYERS" << std::endl;
        return list;
    }

    // Read the message type alongside the size of lobbies
    rv = read(mySocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: UPLOAD_PLAYERS" << std::endl;
        return list;
    }

    // Read the list of lobbies
    rv = read(mySocket, &list, message.GetSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: PlayerInfoList" << std::endl;
        return list;
    }

    std::cout << "Received players" << std::endl;
    return list;
}