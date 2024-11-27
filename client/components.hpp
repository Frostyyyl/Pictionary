#include "component.hpp"
#include "game_manager.hpp"

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

    void Update() override
    {
        SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(GameManager::getInstance().renderer, &rect);
    }

    virtual void HandleEvent() override {};
    virtual void Delete() override {};
};