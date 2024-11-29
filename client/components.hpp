#pragma once

#include "component.hpp"
#include "interactable.hpp"
#include <iostream>

struct Position
{
    int x, y;
};

class Transform : public Component
{
private:
    Position position;

public:
    Transform(int x = 0, int y = 0) : position{x, y} {}
    ~Transform() {}

    void Update() override {}
    void HandleEvent() override {}
    void Delete() override {}

    Position GetPosition()
    {
        return position;
    }

    void SetPosition(Position newPosition)
    {
        position = newPosition;
    }
};

class SpriteRenderer : public Component
{
private:
    SDL_Rect rect;

public:
    SpriteRenderer()
    {
        rect = {100, 100, 20, 20};
    }
    ~SpriteRenderer() {}

    void Update() override;

    virtual void HandleEvent() override {};
    virtual void Delete() override {};
};

class Button : public Component, public Interactable
{
private:
    SDL_Rect rect;

public:
    Button(int x, int y, int w, int h)
    {
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }
    ~Button() {}

    void HandleEvent(Uint32 eventType)
    {
        std::cout << "aaaaaaaaaa" << std::endl;
    }
};