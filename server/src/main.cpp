#include "server.hpp"
#include <csignal>

int main()
{
    signal(SIGINT, Server::Exit);

    Server::getInstance().Init();
    Server::getInstance().Run();
}