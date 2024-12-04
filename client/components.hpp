#pragma once

#include "interactable.hpp"
#include "text.hpp"
#include <iostream>
#include <functional>
#include <deque>
#include <memory>

// Do we need this though? (X, Y) is in SDL_Rect
struct Position
{
    int x, y;
};

class SpriteRenderer : public Component
{
private:
public:
    SpriteRenderer()
    {
        rect = {100, 100, 20, 20};
    }
    SpriteRenderer(int x, int y)
    {
        rect = {x, y, 40, 40};
    }
    ~SpriteRenderer() {}

    void Update() override;
    void Delete() override
    {
        this->~SpriteRenderer();
    };
};

class TextObject : public Component
{
private:
    SDL_Texture *tex;

public:
    // here because text input wants to access it
    Text text;

    TextObject(int x, int y, std::string content = "");
    ~TextObject() {}

    void LoadText();

    int GetHeight() const { return rect.h; }
    void SetPosition(int x, int y)
    {
        rect.x = x;
        rect.y = y;
    }

    void Update() override;
    void Delete() override
    {
        SDL_DestroyTexture(tex);
        this->~TextObject();
    };
};

class Button : public Interactable
{
private:
    std::function<void()> onClick;

public:
    Button(int x, int y, int w, int h, std::function<void()> func)
    {
        rect = {x, y, w, h};
        onClick = func;
    }
    ~Button() {}

    void HandleEvent(SDL_Event event) override;
    void Update() override;
    void Delete() override
    {
        this->~Button();
    };
};

// if we want scroll we need Interactable though
class MessageWindow : public Component
{
private:
    std::deque<std::shared_ptr<TextObject>> messages;
    int height;
    const int msgOffset = 6;

public:
    MessageWindow(int x, int y, int w, int h)
    {
        rect = {x, y, w, h};
    }
    ~MessageWindow() {}

    void Update() override;
    void Delete() override {};

    void AddMessage(std::string message);
};

class TextInput : public Interactable
{
private:
    TextObject text;
    MessageWindow *msgWindow;

    bool isClicked(SDL_Event event);

public:
    TextInput(int x, int y, int w, int h, MessageWindow *window) : text(x, y)
    {
        rect = {x, y, w, h};
        msgWindow = window;
    }
    ~TextInput() {};

    std::string GetText() { return text.text.text; }
    void SendMessage();

    void HandleEvent(SDL_Event event) override;
    void Update() override;
    void Delete() override {};
};

class Canvas : public Interactable
{
private:
    SDL_Texture *tex;

    Position prevPos;

public:
    Canvas();
    ~Canvas() {}

    void HandleEvent(SDL_Event event) override;
    void Update() override;
    void Delete() override
    {
        SDL_DestroyTexture(tex);
        this->~Canvas();
    };
};