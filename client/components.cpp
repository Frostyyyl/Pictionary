#include "components.hpp"
#include "game_manager.hpp"
#include "texture_manager.hpp"
#include "text_manager.hpp"
#include "network_connector.hpp"

#include "SDL2/SDL2_gfxPrimitives.h"

// Lots of functions (update mostly) are just placeholders

void SpriteRenderer::Update()
{
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(GameManager::getInstance().renderer, &rect);
    SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 255, 255, 255);
}

TextObject::TextObject(int x, int y, std::string content)
{
    text.text = content;
    rect.x = x;
    rect.y = y;
    LoadText();
}

void TextObject::LoadText()
{
    tex = TextManager::getInstance().loadText(TextManager::getInstance().getFont(), text.text, rect);
}

void TextObject::Update()
{
    SDL_RenderCopy(GameManager::renderer, tex, NULL, &rect);
}

void TextInput::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_TEXTINPUT)
    {
        text.text.text += event.text.text;
        text.LoadText();
    }
    else if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_BACKSPACE && !text.text.text.empty())
        {
            text.text.text.pop_back();
            text.LoadText();
        }
        else if (event.key.keysym.sym == SDLK_RETURN)
        {
            std::cout << "skkibidi" << std::endl;
            if (!text.text.text.empty())
                SendMessage();
        }
    }
}

void TextInput::Update()
{
    text.Update();
}

void TextInput::SendMessage()
{
    std::string msg = text.text.text;
    msgWindow->AddMessage(msg);
    NetworkConnector::getInstance().HandleNewMessage(GameManager::getInstance().currentPlayer->GetNickname(), msg);

    text.text.text = "";
    text.LoadText();
}

void Button::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
        onClick();
}

void Button::Update()
{
    SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 0, 255);
    // SDL_RenderDrawRect(GameManager::renderer, &rect);
    roundedBoxColor(GameManager::renderer, rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h), 4, 0xff8f7b4a);
    SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
}

void MessageWindow::Update()
{
    for (const auto &obj : messages)
    {
        obj.get()->Update();
    }
}

void MessageWindow::AddMessage(std::string message)
{
    auto newMessage = std::make_shared<TextObject>(rect.x, 0, message);
    int messageHeight = newMessage->GetHeight();

    while (height + messageHeight > rect.h)
    {
        if (!messages.empty())
        {
            height -= messages.front()->GetHeight();
            messages.pop_front();
        }
        else
        {
            break;
        }
    }

    messages.push_back(newMessage);
    height += messageHeight;

    int yOffset = rect.y + rect.h + msgOffset;

    for (auto it = messages.rbegin(); it != messages.rend(); ++it)
    {
        auto &message = *it;
        yOffset -= (message->GetHeight() + msgOffset);
        message->SetPosition(rect.x, yOffset);
    }
}

Canvas::Canvas()
{
    rect = {20, 20, 400, 400};
    tex = TextureManager::CreateCanvas(400, 400);
}

void Canvas::HandleEvent(SDL_Event event)
{
    if (!isClicked(event))
        return;
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        prevPos = {event.motion.x - rect.x, event.motion.y - rect.y};

        SDL_SetRenderTarget(GameManager::renderer, tex);
        filledCircleColor(GameManager::renderer, prevPos.x, prevPos.y, 3, 0xff00cc00);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        SDL_SetRenderTarget(GameManager::renderer, nullptr);

        NetworkConnector::getInstance().HandleCanvasChange(tex);

        return;
    }

    int x = event.motion.x;
    int y = event.motion.y;
    if (event.type == SDL_MOUSEBUTTONUP)
    {

        SDL_SetRenderTarget(GameManager::renderer, tex);

        int canvasX = x - rect.x;
        int canvasY = y - rect.y;
        filledCircleColor(GameManager::renderer, canvasX, canvasY, 3, 0xff00cc00);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;

        NetworkConnector::getInstance().HandleCanvasChange(tex);

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

        NetworkConnector::getInstance().HandleCanvasChange(tex);
    }
}

void Canvas::Update()
{
    SDL_RenderCopy(GameManager::renderer, tex, nullptr, &rect);
}