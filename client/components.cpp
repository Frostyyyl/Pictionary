#include "components.hpp"
#include "game_manager.hpp"
#include "texture_manager.hpp"

void SpriteRenderer::Update()
{
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(GameManager::getInstance().renderer, &rect);
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 255, 255, 255);
}

void Button::HandleEvent(Uint32 eventType)
{
    if (eventType == SDL_SCANCODE_W)
    {
        std::cout << "skibidi" << std::endl;
    }
}

void Button::Update()
{
    SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 0, 255);
    SDL_RenderDrawRect(GameManager::renderer, &rect);
    SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
}

Canvas::Canvas()
{
    rect = {20, 20, 400, 400};
    tex = TextureManager::CreateCanvas(400, 400);
}

void Canvas::HandleEvent(Uint32 eventType)
{
    if (eventType == SDL_MOUSEBUTTONDOWN)
    {
        prevPos = {GameManager::getInstance().event.motion.x - rect.x, GameManager::getInstance().event.motion.y - rect.y};
        return;
    }
    int x = GameManager::getInstance().event.motion.x;
    int y = GameManager::getInstance().event.motion.y;
    if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
    {
        std::cout << "Drawing at: " << x << " : " << y << std::endl;

        SDL_SetRenderTarget(GameManager::renderer, tex);

        int canvasX = x - rect.x;
        int canvasY = y - rect.y;

        SDL_SetRenderDrawColor(GameManager::renderer, 255, 0, 0, 255);
        // SDL_RenderDrawPoint(GameManager::renderer, canvasX, canvasY);
        SDL_RenderDrawLine(GameManager::renderer, prevPos.x, prevPos.y, canvasX, canvasY);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;
    }
}

void Canvas::Update()
{
    SDL_RenderCopy(GameManager::renderer, tex, nullptr, &rect);
}