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
    int timeCount = 0;

    std::shared_ptr<Component> GetObject(const std::string &name);
    void AddObject(std::shared_ptr<Component> component);

    void UpdatePlayers();
    void UpdateGameMode();
    void UpdateGameMode(GameMode mode);
    void UpdateChat();
    void UpdateCanvas();
    bool UpdateTime();
    void ReadChanges();

public:
    SceneType sceneType;

    Scene();
    ~Scene();

    void Update();
    void DeleteScene();

    void DeleteObjects(const std::string &name);
    void HideObjects(const std::string &name);
    void ShowObject();

    std::shared_ptr<TextButton> CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                 const std::string &filename, std::function<void()> onClick, const std::string &name);
    std::shared_ptr<TextButton> CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                 Uint32 color, std::function<void()> onClick, const std::string &name);
    std::shared_ptr<Button> CreateButton(int x, int y, int w, int h, const std::string &filename,
                                         std::function<void()> onClick, const std::string &name);
    std::shared_ptr<Button> CreateButton(int x, int y, int w, int h, Uint32 color,
                                         std::function<void()> onClick, const std::string &name);
    std::shared_ptr<TextInput> CreateTextInput(int x, int y, int w, int h,
                                               MessageWindow *msgWindow, const std::string &name);
    std::shared_ptr<FixedTextInput> CreateFixedTextInput(int x, int y, int w, int h, int maxSize, const std::string &name, const std::string &z_index = "2");
    std::shared_ptr<TextObject> CreateTextObject(int x, int y, const std::string &text, const std::string &name, int wrapLength);
    std::shared_ptr<MessageWindow> CreateMessageWindow(int x, int y, int w, int h, const std::string &name);
    std::shared_ptr<Canvas> CreateCanvas();
    std::shared_ptr<Background> CreateBackground(int x, int y, int w, int h, Uint32 color, const std::string &name);

    void CreatePlayerNames();
    void CreateLobbyButtons();

    void CreateForDrawMode();
    void CreateForGuessMode();
    void CreateForWaitMode();
    void CreateForStandByMode();
};