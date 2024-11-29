#include "components.hpp"
#include "game_manager.hpp"

void SpriteRenderer::Update()
{
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(GameManager::getInstance().renderer, &rect);
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 255, 255, 255);
}