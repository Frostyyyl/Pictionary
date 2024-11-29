#include "window.hpp"
#include "game_manager.hpp"

void Window::HandleEvent(Uint32 eventType)
{
    GameManager::getInstance().Exit();
}