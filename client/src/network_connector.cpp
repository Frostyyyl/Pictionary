#include "network_connector.hpp"

NetworkConnector::NetworkConnector() {}

NetworkConnector &NetworkConnector::getInstance()
{
    static NetworkConnector INSTANCE;
    return INSTANCE;
}

NetworkConnector::~NetworkConnector() {}

void NetworkConnector::Init(int port, std::string address)
{
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(address.c_str());

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == -1)
    {
        std::cerr << "ERROR: Could not create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof serverAddr) == -1)
    {
        std::cerr << "ERROR: Could not connect to server" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    
    isInit = true;
}

void NetworkConnector::Exit()
{
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
    std::cout << "Closing connection due to server error" << std::endl;
    exit(EXIT_FAILURE);
}

LobbyInfoList NetworkConnector::RequestLobbies()
{
    Message message = Message(static_cast<int>(MessageToServer::REQUEST_LOBBIES));
    LobbyInfoList list;

    // Send the message type
    int rv = write(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: REQUEST_LOBBIES" << std::endl;
        return list;
    }

    // Read the message type alongside the size of lobbies
    rv = read(serverSocket, &message, sizeof(Message));
    
    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive message of type: UPLOAD_LOBBIES" << std::endl;
        return list;
    }

    // Read the list of lobbies
    rv = read(serverSocket, &list, message.getSize());

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: LobbyInfoList" << std::endl;
        return list;
    }

    return list;
}

bool NetworkConnector::ValidateData(const std::string& lobby, const std::string& name, const std::string& password)
{
    // TODO: Implement displaying the information about incorrect data size to the user 
    // Alternatively fix the maximum name size, which user can enter, to match the restrictions
    if (lobby.size() > LobbyConnectInfo::MAX_LOBBY_NAME_SIZE){
        std::cout << "Lobby name is too long" << std::endl;
        return false;
    } else if (lobby.empty()){
        std::cout << "Lobby name cannot be empty" << std::endl;
        return false;
    } else if (name.size() > LobbyConnectInfo::MAX_CLIENT_NAME_SIZE){
        std::cout << "Player name is too long" << std::endl;
        return false;
    } else if (name.empty()){
        std::cout << "Player name cannot be empty" << std::endl;
        return false;
    } else if (password.size() > LobbyConnectInfo::MAX_LOBBY_PASSWORD_SIZE){
        std::cout << "Lobby password is too long" << std::endl;
        return false;
    } 
    return true;
}

bool NetworkConnector::CreateLobby(const std::string& lobby, const std::string& name, const std::string& password)
{
    // TODO: Implement handling write/read errors

    if (!ValidateData(lobby, name, password))
    {
        return false;
    }

    LobbyConnectInfo info = LobbyConnectInfo(lobby, name, password);
    Message message = Message(static_cast<int>(MessageToServer::CREATE_LOBBY), sizeof(info));

    // Send the message type
    int rv = write(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: CREATE_LOBBY" << std::endl;
        return false;
    }

    // Send the lobby information
    rv = write(serverSocket, &info, sizeof(info));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: While creating lobby failed to send: LobbyConnectInfo" << std::endl;
        return false;
    }

    // Receive the response
    rv = read(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to receive message in method: CreateLobby" << std::endl;
        return false;
    }
    
    // Handle based on response
    switch (static_cast<MessageToClient>(message.getType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        std::cout << "IMPLEMENT HANDLING PASSING AN ALREADY EXISTING/INCORRECT LOBBY NAME" << std::endl; // TODO:
        return false;
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        std::cout << "IMPLEMENT HANDLING PASSING AN INCORRECT PLAYER NAME" << std::endl; // TODO:
        return false;
        break;
    case MessageToClient::INCORRECT_PASSWORD:
        std::cout << "IMPLEMENT HANDLING PASSING AN INCORRECT PASSWORD" << std::endl; // TODO:
        return false;
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
        std::cerr << "ERROR: While creating lobby received unexpected message type: " << message.getType() << std::endl;
        Exit();
        break;
    }

    return false;
}

bool NetworkConnector::ConnectToLobby(const std::string& lobby, const std::string& name, const std::string& password)
{
    // TODO: Implement handling write/read errors

    if (!ValidateData(lobby, name, password))
    {
        return false;
    }

    LobbyConnectInfo info = LobbyConnectInfo(lobby, name, password);
    Message message = Message(static_cast<int>(MessageToServer::CONNECT_TO_LOBBY), sizeof(info));

    // Send the message type
    int rv = write(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: CONNECT_TO_LOBBY" << std::endl;
        return false;
    }

    // Send the lobby information
    rv = write(serverSocket, &info, sizeof(info));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: While connecting to lobby failed to send: LobbyConnectInfo" << std::endl;
        return false;
    }
    // Receive the response
    rv = read(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to receive message in method: ConnectToLobby" << std::endl;
        return false;
    }

    // Handle based on response
    switch (static_cast<MessageToClient>(message.getType()))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        std::cout << "IMPLEMENT HANDLING PASSING AN ALREADY EXISTING/INCORRECT LOBBY NAME" << std::endl; // TODO:
        return false;
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        std::cout << "IMPLEMENT HANDLING PASSING AN INCORRECT PLAYER NAME" << std::endl; // TODO:
        return false;
        break;
    case MessageToClient::INCORRECT_PASSWORD:
        std::cout << "IMPLEMENT HANDLING PASSING AN INCORRECT PASSWORD" << std::endl; // TODO:
        return false;
        break;
    case MessageToClient::CONNECT:
        std::cout << "Connected with lobby: \"" << lobby << "\", as: \"" << name << "\"" << std::endl;
        return true;
        break;

    case MessageToClient::INVALID:
        Exit();
        break;
    default:
        std::cerr << "ERROR: While connecting to lobby received unexpected message type: " << message.getType() << std::endl;
        Exit();
        break;
    }

    return false;
} 