#include "network_connector.hpp"

NetworkConnector::NetworkConnector() {}

NetworkConnector &NetworkConnector::getInstance()
{
    static NetworkConnector INSTANCE;
    return INSTANCE;
}

NetworkConnector::~NetworkConnector() 
{
    Exit();
}

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
}

void NetworkConnector::Exit()
{
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
    std::cout << "Closing connection" << std::endl;
}

LobbiesList NetworkConnector::RequestLobbies()
{
    Message message = Message(static_cast<int>(MessageToServer::ENTER_HUB));
    LobbiesList list;

    // Send the message type
    int rv = write(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not send message of type: ENTER_HUB" << std::endl;
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
    rv = read(serverSocket, &list, message.size);

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Could not receive: LobbiesList" << std::endl;
        return list;
    }

    return list;
}

void NetworkConnector::Connect(std::string lobbyName)
{
    
} 

void NetworkConnector::Disconnect()
{

} 

void NetworkConnector::CreateLobby(std::string name, std::string password)
{
    // TODO: Implement displaying the information about incorrect data size to the user 
    // Alternatively fix the maximum name size, which user can enter, to match the restrictions
    if (name.size() > LobbyInfo::MAX_LOBBY_NAME_SIZE){
        std::cout << "Lobby name is too long" << std::endl;
    } else if (password.size() > LobbyInfo::MAX_LOBBY_PASSWORD_SIZE){
        std::cout << "Lobby password is too long" << std::endl;
    }

    LobbyCreateInfo info = LobbyCreateInfo(name, password);
    Message message = Message(static_cast<int>(MessageToServer::CREATE_LOBBY), sizeof(info));

    // Send the message type
    int rv = write(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: CREATE_LOBBY" << std::endl;
        return;
    }

    // Send the lobby information
    rv = write(serverSocket, &info, sizeof(info));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to send message of type: CREATE_LOBBY" << std::endl;
        return;
    }

    // Receive the response
    rv = read(serverSocket, &message, sizeof(Message));

    // Handle errors
    if (rv == -1)
    {
        std::cerr << "ERROR: Failed to receive message in CREATE_LOBBY" << std::endl;
        return;
    }
    
    // Handle based on response
    switch (static_cast<MessageToClient>(message.type))
    {
    case MessageToClient::INCORRECT_LOBBY_NAME:
        std::cout << "IMPLEMENT HANDLING PASSING AN ALREADY EXISTING LOBBY NAME" << std::endl; // TODO:
        break;
    case MessageToClient::CONNECT_TO_LOBBY:
        std::cout << "HANDLE CREATING LOBBY" << std::endl; // TODO:
        break;
    default:
        std::cerr << "ERROR: While creating lobby received unexpected message type: " << message.type << std::endl;
        break;
    }

}