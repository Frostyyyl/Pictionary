#pragma once

#include <iostream>
#include <set>
#include <memory>
#include "component.hpp"

class Entity
{
private:
    std::set<std::shared_ptr<Component>> Components;

public:
    bool enabled;

    Entity() : enabled(false) {}
    ~Entity() {}

    void HandleEvent()
    {
        for (const auto &comp : Components)
        {
            comp->HandleEvent();
        }
    }

    void Update()
    {
        for (const auto &comp : Components)
        {
            comp->Update();
        }
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        Components.insert(component);

        return component;
    }

    template <typename T>
    std::shared_ptr<T> GetComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        // good enough for us I think
        for (const auto &component : Components)
        {
            if (auto casted = std::dynamic_pointer_cast<T>(component))
            {
                return casted;
            }
        }

        return nullptr;
    }

    void Delete()
    {
        for (const auto &comp : Components)
        {
            comp->Delete();
        }
    }
};