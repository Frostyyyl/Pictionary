#include "component.hpp"

struct Position
{
    int x, y;
};

class Transform : public Component
{
private:
    Position position;

public:
    Transform(int x, int y) : position{x, y} {}
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
    /* data */
public:
    SpriteRenderer(/* args */) {}
    ~SpriteRenderer() {}

    void Update() override
    {
    }
};