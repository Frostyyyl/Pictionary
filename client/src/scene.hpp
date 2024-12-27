#pragma once

#include <memory>
#include <set>
#include <vector>
#include <string>

#include "components.hpp"
#include "network_connector.hpp"
#include "../../global/src/messages.hpp"

enum class SceneType
{
    MAIN_MENU,
    LOBBY,
    GAME,
};

class Scene
{
private:
    std::set<std::shared_ptr<Component>> objects;
    std::vector<std::shared_ptr<Component>> hiddenObjects;
    int frameCount = 0;

public:
    SceneType sceneType;

    Scene();
    ~Scene();

    void Update();
    void AddObject(std::shared_ptr<Component> component);
    void DeleteObject(const std::string &name);
    void DeleteObject(const std::shared_ptr<Component>& obj);
    void HideObject(const std::string &name);
    void ShowObject();
    void UpdatePlayers();
    void DeleteScene();
};