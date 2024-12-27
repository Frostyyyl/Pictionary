#include <iostream>

#include "scene.hpp"
#include "game_manager.hpp"

Scene::Scene() {}
Scene::~Scene() {}

void Scene::Update()
{
    // FIXME: For now update is split into two parts
    // Firstly update nameless objects (background objects)
    for (const auto &obj : objects)
    {
        if (obj->GetName() == ""){
            obj->Update();  
        }
    }

    // Secondly update the rest
    for (const auto &obj : objects)
    {
        if (obj->GetName() != ""){
            obj->Update();  
        }
    }

    if (sceneType == SceneType::GAME)
    {
        if (frameCount == FRAMES_PER_SECOND)
        {
            UpdatePlayers();

            frameCount = 0;
        }

        frameCount++;
    }
}

void Scene::AddObject(std::shared_ptr<Component> component)
{
    objects.insert(component);
}

void Scene::DeleteObject(const std::string &name)
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == name)
        {
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Scene::DeleteObject(const std::shared_ptr<Component>& obj)
{
    objects.erase(obj);
}

void Scene::HideObject(const std::string &name)
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == name)
        {
            hiddenObjects.push_back(*it);
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Scene::ShowObject()
{
    if (!hiddenObjects.empty())
    {
        objects.insert(hiddenObjects.back());
        hiddenObjects.pop_back();
    }
}

void Scene::UpdatePlayers()
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == "Player")
        {
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    PlayerInfoList list = NetworkConnector::getInstance().RequestPlayers();
    for (int i = 0; i < list.GetSize(); i++)
    {
        auto txt = std::make_shared<TextObject>(440, 20 + (20 * i), list.GetPlayer(i).GetPlayerName(), "Player", 200);
        AddObject(txt);
    }
}

void Scene::DeleteScene()
{
    objects.clear();
}