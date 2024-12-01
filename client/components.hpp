#pragma once

#include "interactable.hpp"
#include "text.hpp"
#include <iostream>
#include <functional>

struct Position
{
    int x, y;
};

class SpriteRenderer : public Component
{
private:
    SDL_Rect rect;

public:
    Position pos;
    SpriteRenderer()
    {
        rect = {100, 100, 20, 20};
    }
    SpriteRenderer(int x, int y)
    {
        pos = {x, y};
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
    SDL_Rect src;

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

class Button : public Interactable
{
private:
    SDL_Rect rect;
    std::function<void()> onClick;

    bool isClicked(SDL_Event event);

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
    SDL_Rect rect;
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