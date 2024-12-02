#include "components.hpp"
#include "game_manager.hpp"
#include "texture_manager.hpp"
#include "text_manager.hpp"

#include "SDL2/SDL2_gfxPrimitives.h"

void SpriteRenderer::Update()
{
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(GameManager::getInstance().renderer, &rect);
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 255, 255, 255);
}

TextObject::TextObject()
{
    text.text = {"hellooo"};
    src = {400, 200, 200, 200};
    TTF_SizeText(TextManager::getInstance().getFont(), text.text.c_str(), &text.dest.w, &text.dest.h);
    tex = TextManager::getInstance().loadText(TextManager::getInstance().getFont(), text, src);
}

void TextObject::Update()
{
    SDL_RenderCopy(GameManager::renderer, tex, NULL, &src);
}

bool Button::isClicked()
{
    return (GameManager::getInstance().event.button.x >= rect.x && GameManager::getInstance().event.button.x <= rect.x + rect.w && GameManager::getInstance().event.button.y >= rect.y && GameManager::getInstance().event.button.y <= rect.y + rect.h);
}

void Button::HandleEvent(Uint32 eventType)
{
    if (isClicked())
    {
        onClick();
    }
}

void Button::Update()
{
    SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 0, 255);
    // SDL_RenderDrawRect(GameManager::renderer, &rect);
    roundedBoxColor(GameManager::renderer, rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h), 4, 0xff8f7b4a);
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

        SDL_SetRenderTarget(GameManager::renderer, tex);
        filledCircleColor(GameManager::renderer, prevPos.x, prevPos.y, 3, 0xff00cc00);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        SDL_SetRenderTarget(GameManager::renderer, nullptr);

        return;
    }

    int x = GameManager::getInstance().event.motion.x;
    int y = GameManager::getInstance().event.motion.y;
    if (eventType == SDL_MOUSEBUTTONUP)
    {

        SDL_SetRenderTarget(GameManager::renderer, tex);

        int canvasX = x - rect.x;
        int canvasY = y - rect.y;
        filledCircleColor(GameManager::renderer, canvasX, canvasY, 3, 0xff00cc00);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;
        return;
    }
    if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
    {
        std::cout << "Drawing at: " << x << " : " << y << std::endl;

        SDL_SetRenderTarget(GameManager::renderer, tex);

        int canvasX = x - rect.x;
        int canvasY = y - rect.y;

        SDL_SetRenderDrawColor(GameManager::renderer, 255, 0, 0, 255);
        thickLineColor(GameManager::renderer, prevPos.x, prevPos.y, canvasX, canvasY, 6, 0xff00cc00);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;
    }
    GameManager::getInstance().player->HandleCanvasChange(tex);
}

void Canvas::Update()
{
    SDL_RenderCopy(GameManager::renderer, tex, nullptr, &rect);
}