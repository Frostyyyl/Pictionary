#pragma once

#include <iostream>
#include <functional>
#include <deque>
#include <memory>

#include "interactable.hpp"
#include "text.hpp"
#include "../../global/src/messages.hpp"

// Used in Canvas
struct Position
{
    int x = {}, y = {};
};

// Used in TextButton
struct Padding
{
public:
    int x = {}, y = {};

    Padding(int x, int y = -14) : x(x), y(y) {}
    Padding() {}
};

class Color
{
private:
    Color() {}

public:
    // Colors used for drawing (AABBGGRR)
    static constexpr Uint32 ABGR_WHITE = 0xffffffff;
    static constexpr Uint32 ABGR_BLACK = 0xff000000;
    
    // Colors used for objects (RRGGBBAA)
    static constexpr Uint32 WHITE = 0xf0f0f000;
    static constexpr Uint32 BLACK = 0x000000ff;
    static constexpr Uint32 YINMN_BLUE = 0x3D5A80ff;
    static constexpr Uint32 LIGHT_SKY_BLUE = 0xA2D2FFff;
    static constexpr Uint32 MUSTARD = 0xFFD449ff;
    static constexpr Uint32 ORANGE = 0xF9A620ff; 
    static constexpr Uint32 BURNT_SIENNA = 0xEE6C4Dff;
    static constexpr Uint32 CORAL_PINK = 0xE8998Dff;
};

class SpriteRenderer : public Component
{
private:
    SDL_Texture *tex;

public:
    SpriteRenderer(const std::string &name) : Component(name)
    {
        rect = {100, 100, 20, 20};
    }
    SpriteRenderer(int x, int y, const std::string &filename, const std::string &name);
    ~SpriteRenderer() {}

    void Update() override;
};

class TextObject : public Component
{
private:
    SDL_Texture *tex;
    int wrapLength;

public:
    Text text;

    TextObject(int x, int y, const std::string &content, const std::string &name, int wrapLength);
    TextObject(int x, int y, int wrapLength);
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

class Background : public Component
{
protected:
    SDL_Texture *tex;

public:
    Background(int x, int y, int w, int h, Uint32 color, const std::string &name);
    ~Background() {}

    void Update() override;
};

class Button : public Interactable
{
protected:
    SDL_Texture *tex;
    std::function<void()> onClick;

public:
    Button(int x, int y, int w, int h, const std::string &filename, std::function<void()> func, const std::string &name);
    Button(int x, int y, int w, int h, Uint32 color, std::function<void()> func, const std::string &name);
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
    TextButton(int x, int y, int w, int h, Padding padding, const std::string &text, const std::string &filename,
               std::function<void()> func, const std::string &name);
    TextButton(int x, int y, int w, int h, Padding padding, const std::string &text, Uint32 color,
               std::function<void()> func, const std::string &name);
    ~TextButton() {}

    void Update() override;
};

// NOTE: if we want scroll we need Interactable though
class MessageWindow : public Component
{
private:
    std::deque<std::shared_ptr<TextObject>> messages;
    int height = 0;

public:
    MessageWindow(int x, int y, int w, int h, const std::string &name) : Component(name)
    {
        rect = {x, y, w, h};
    }
    ~MessageWindow() {}

    void Update() override;
    void ClearMessages();
    void AddMessage(std::string message);
};

class TextInput : public Interactable
{
private:
    TextObject text;
    MessageWindow *msgWindow;

public:
    TextInput(int x, int y, int w, int h, MessageWindow *window, const std::string &name)
        : Interactable(name), text(x, y, w)
    {
        rect = {x, y, w, h};
        msgWindow = window;
    }
    ~TextInput() {};

    std::string GetText() { return text.text.text; }
    void FlushText()
    {
        text.text.text = "";
        text.LoadText();
    }
    void SendMessage();

    void HandleEvent(SDL_Event event) override;
    void Update() override;
};

class FixedTextInput : public Interactable
{
private:
    TextObject text;
    int maxSize;

public:
    FixedTextInput(int x, int y, int w, int h, int maxSize, const std::string &name)
        : Interactable(name), text(x, y, w), maxSize(maxSize)
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

    Position prevPos = {0, 0};

public:
    Canvas(const std::string &name);
    ~Canvas()
    {
        SDL_DestroyTexture(tex);
    }

    void HandleEvent(SDL_Event event) override;
    void Update() override;
    void DrawLine(int x1, int y1, int x2, int y2, Uint32 color);
    void DrawCircle(int x, int y, int radius, Uint32 color);
    void ClearCanvas();

    void ChangeColor(Uint32 newColor)
    {
        currentColor = newColor;
    }
};
