#pragma once

#include "component.hpp"
#include "interactable.hpp"
#include <iostream>

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
    void Delete() override {};
};

class Button : public Component, public Interactable
{
private:
    SDL_Rect rect;

public:
    Position pos;
    Button(int x, int y, int w, int h)
    {
        pos = {x, y};
        rect = {x, y, w, h};
    }
    ~Button() {}

    void HandleEvent(Uint32 eventType) override;
    void Update() override;
    void Delete() override {};
};

class Canvas : public Component, public Interactable
{
private:
    SDL_Rect rect;
    SDL_Texture *tex;

    Position prevPos;

public:
    Canvas();
    ~Canvas() {}

    void HandleEvent(Uint32 eventType) override;
    void Update() override;
    void Delete() override {};
};