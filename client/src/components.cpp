#include <SDL2/SDL2_gfxPrimitives.h>
#include <algorithm>

#include "components.hpp"
#include "game_manager.hpp"
#include "texture_manager.hpp"
#include "text_manager.hpp"
#include "network_connector.hpp"

SpriteRenderer::SpriteRenderer(int x, int y, const std::string &filename, const std::string &name) : Component(name)
{
    tex = TextureManager::LoadTexture(filename.c_str());
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    rect = {x, y, w, h};
}

void SpriteRenderer::Update()
{
    TextureManager::Draw(tex, rect);
}

TextObject::TextObject(int x, int y, const std::string &content, const std::string &name, int wrapLength)
    : Component(name), wrapLength(wrapLength)
{
    text.text = content;
    rect.x = x;
    rect.y = y;
    rect.h = 20;
    LoadText();
}

TextObject::TextObject(int x, int y, int wrapLength)
    : Component("TextComponent"), wrapLength(wrapLength)
{
    text.text = "";
    rect.x = x;
    rect.y = y;
    rect.h = 20;
    LoadText();
}

void TextObject::LoadText()
{
    tex = TextManager::getInstance().LoadText(TextManager::getInstance().getFont(), text.text, rect, wrapLength);
}

void TextObject::Update()
{
    TextureManager::Draw(tex, rect);
    // SDL_RenderCopy(GameManager::renderer, tex, NULL, &rect);
}

void TextInput::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_TEXTINPUT)
    {
        if ((int)text.text.text.size() < TextInfo::MAX_TEXT_SIZE)
        {
            text.text.text += event.text.text;
            text.LoadText();
        }
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
            SendMessage();
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        GameManager::getInstance().SetCurrentTextInput(this->GetName());
    }
}

void TextInput::Update()
{
    text.Update();
}

void TextInput::SendMessage()
{
    std::string msg = text.text.text;

    if (msg.empty())
        return;

    if (!NetworkConnector::getInstance().UploadText(GameManager::getInstance().GetPlayerName(), msg))
    {
        return;
    }

    msgWindow->AddMessage(GameManager::getInstance().GetPlayerName()+ ": " + msg);

    text.text.text = "";
    text.LoadText();
}

void FixedTextInput::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_TEXTINPUT)
    {
        if ((int)text.text.text.size() < maxSize)
        {
            text.text.text += event.text.text;
            text.LoadText();
        }
    }
    else if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_BACKSPACE && !text.text.text.empty())
        {
            text.text.text.pop_back();
            text.LoadText();
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
        GameManager::getInstance().SetCurrentTextInput(this->GetName());
}

void FixedTextInput::Update()
{
    text.Update();
}

Button::Button(int x, int y, int w, int h, const std::string &filename, std::function<void()> func, const std::string &name) : Interactable(name)
{
    rect = {x, y, w, h};
    tex = TextureManager::LoadTexture(filename.c_str());
    onClick = func;
}

Button::Button(int x, int y, int w, int h, Uint32 color, std::function<void()> func, const std::string &name) : Interactable(name)
{
    rect = {x, y, w, h};
    tex = TextureManager::LoadSolidColor(w, h, color);
    onClick = func;
}

void Button::HandleEvent(SDL_Event event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
        onClick();
}

void Button::Update()
{
    TextureManager::Draw(tex, rect);
}

TextButton::TextButton(int x, int y, int w, int h, Padding padding, const std::string &text,
                       const std::string &filename, std::function<void()> func, const std::string &name)
    : Button(x, y, w, h, filename, func, name), text(x + padding.x, (y + h / 2) + padding.y, text, name, w - padding.x) {}

TextButton::TextButton(int x, int y, int w, int h, Padding padding, const std::string &text,
                       Uint32 color, std::function<void()> func, const std::string &name)
    : Button(x, y, w, h, color, func, name), text(x + padding.x, (y + h / 2) + padding.y, text, name, w - padding.x) {}

void TextButton::Update()
{
    Button::Update();
    text.Update();
}

void MessageWindow::Update()
{
    for (const auto &obj : messages)
    {
        obj.get()->Update();
    }
}

void MessageWindow::ClearMessages()
{
    messages.clear();
    height = 0;
}

void MessageWindow::AddMessage(std::string message)
{
    if (message.empty())
        return;

    auto newMessage = std::make_shared<TextObject>(rect.x, 0, message, "Message", rect.w);
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

    int yOffset = rect.y + rect.h;

    for (auto it = messages.rbegin(); it != messages.rend(); ++it)
    {
        auto &message = *it;
        yOffset -= (message->GetHeight());
        message->SetPosition(rect.x, yOffset);
    }
}

Canvas::Canvas(const std::string &name) : Interactable(name)
{
    rect = {20, 20, 400, 400};
    tex = TextureManager::CreateCanvas(400, 400);
    currentColor = Color::ABGR_BLACK;
}

void Canvas::HandleEvent(SDL_Event event)
{
    if (!isClicked(event))
        return;
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        prevPos = {event.motion.x - rect.x, event.motion.y - rect.y};

        SDL_SetRenderTarget(GameManager::renderer, tex);
        filledCircleColor(GameManager::renderer, prevPos.x, prevPos.y, 3, currentColor);
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
        filledCircleColor(GameManager::renderer, canvasX, canvasY, 3, currentColor);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;

        NetworkConnector::getInstance().HandleCanvasChange(tex);

        return;
    }
    if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
    {
        SDL_SetRenderTarget(GameManager::renderer, tex);

        int canvasX = x - rect.x;
        int canvasY = y - rect.y;

        SDL_SetRenderDrawColor(GameManager::renderer, 255, 0, 0, 255);
        thickLineColor(GameManager::renderer, prevPos.x, prevPos.y, canvasX, canvasY, 6, currentColor);

        SDL_SetRenderTarget(GameManager::renderer, nullptr);
        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        prevPos.x = canvasX;
        prevPos.y = canvasY;

        NetworkConnector::getInstance().HandleCanvasChange(tex);
    }
}

void Canvas::Update()
{
    TextureManager::Draw(tex, rect);
}