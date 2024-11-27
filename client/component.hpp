#pragma once

class Component
{
public:
    Component() {}
    virtual ~Component() {}
    virtual void Update() = 0;
    virtual void HandleEvent() = 0;
    virtual void Delete() = 0;
};