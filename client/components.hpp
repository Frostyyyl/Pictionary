#pragma once

#include "interactable.hpp"
#include "text.hpp"
#include <iostream>
#include <functional>

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
    Text text;
    SDL_Texture *tex;

public:
    TextObject();
    ~TextObject() {}
    void Update() override;
    void Delete() override
    {
        SDL_DestroyTexture(tex);
        this->~TextObject();
    };
};

class TextInput : public Interactable
{
private:
    TextObject text;

    bool isClicked(SDL_Event event);

public:
    TextInput(int x, int y, int w, int h)
    {
        rect = {x, y, w, h};
    }
    ~TextInput() {};

    void HandleEvent(SDL_Event event) override;
    void Update() override;
    void Delete() override {};
};

class Button : public Interactable
{
private:
    std::function<void()> onClick;

public:
    Position pos;
    Button(int x, int y, int w, int h, std::function<void()> func)
    {
        pos = {x, y};
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