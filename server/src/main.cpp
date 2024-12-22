#include "server.hpp"

int main()
{
    Server::getInstance().Init(1100);
    Server::getInstance().Run();
}