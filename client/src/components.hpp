#pragma once

#include <iostream>
#include <functional>
#include <deque>
#include <memory>

#include "interactable.hpp"
#include "text.hpp"

// Used in canvas
struct Position
{
    int x = {}, y = {};
};

class Padding
{
public:
    int x = {}, y = {};

    Padding(int x, int y = -14) : x(x), y(y) {}
    Padding() {}
};

class SpriteRenderer : public Component
{
private:
    SDL_Texture *tex;

public:
    SpriteRenderer(const std::string& name = "") : Component(name)
    {
        rect = {100, 100, 20, 20};
    }
    SpriteRenderer(int x, int y, const char *filename, const std::string& name = "");
    ~SpriteRenderer() {}

    void Update() override;
};

class TextObject : public Component
{
private:
    SDL_Texture *tex;
    int wrapLength;

public:
    // here because text input wants to access it
    Text text;

    TextObject(int x, int y, const std::string& content = "", const std::string& name = "", int wrapLength = 500);
    TextObject(int x, int y, int wrapLength = 500);
    ~TextObject() 
    {
        SDL_DestroyTexture(tex);
    }

    void LoadText();

    int GetHeight() const { return rect.h; }
    void SetPosition(int x, int y)
    {
        rect.x = x;
        rect.y = y;
    }

    void Update() override;
};

class Button : public Interactable
{
protected:
    SDL_Texture *tex;
    std::function<void()> onClick;

public:
    Button(int x, int y, int w, int h, const char *filename, std::function<void()> func, const std::string& name = "");
    Button(int x, int y, int w, int h, Uint32 color, std::function<void()> func, const std::string& name = "");
    virtual ~Button() {}

    virtual void HandleEvent(SDL_Event event) override;
    virtual void Update() override;
};

class TextButton : public Button
{
private:
    Padding padding;
    TextObject text;

public:
    TextButton(int x, int y, int w, int h, Padding padding, const std::string& text, const char *filename, 
        std::function<void()> func, const std::string& name = "");
    TextButton(int x, int y, int w, int h, Padding padding, const std::string& text, Uint32 color, 
        std::function<void()> func, const std::string& name = "");
    ~TextButton() {}

    void Update() override;
};

// if we want scroll we need Interactable though
class MessageWindow : public Component
{
private:
    std::deque<std::shared_ptr<TextObject>> messages;
    int height;
    const int msgOffset = 6;

public:
    MessageWindow(int x, int y, int w, int h, const std::string& name = "") : Component(name)
    {
        rect = {x, y, w, h};
    }
    ~MessageWindow() {}

    void Update() override;

    void AddMessage(std::string message);
};

class TextInput : public Interactable
{
private:
    TextObject text;
    MessageWindow *msgWindow;

    bool isClicked(SDL_Event event);

public:
    TextInput(int x, int y, int w, int h, MessageWindow *window, const std::string& name = "") 
        : Interactable(name), text(x, y, 100)
    {
        rect = {x, y, w, h};
        msgWindow = window;
    }
    ~TextInput() {};

    std::string GetText() { return text.text.text; }
    void SendMessage();

    void HandleEvent(SDL_Event event) override;
    void Update() override;
};

class FixedTextInput : public Interactable
{
private:
    TextObject text;
    int maxSize;

    bool isClicked(SDL_Event event);

public:
    FixedTextInput(int x, int y, int w, int h, int maxSize, const std::string& name = "") 
        : Interactable(name), text(x, y, 400), maxSize(maxSize)
    {
        rect = {x, y, w, h};
    }
    ~FixedTextInput() {};

    std::string GetText() { return text.text.text; }

    void HandleEvent(SDL_Event event) override;
    void Update() override;
};

class Canvas : public Interactable
{
private:
    SDL_Texture *tex;
    Uint32 currentColor;

    Position prevPos;

public:
    Canvas(const std::string& name = "");
    ~Canvas() 
    {
        SDL_DestroyTexture(tex);
    }

    void HandleEvent(SDL_Event event) override;
    void Update() override;

    void ChangeColor(Uint32 newColor)
    {
        currentColor = newColor;
    }
};