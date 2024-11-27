// Probably needs to merge with GameManager or fix entire functionality

#include <SDL2/SDL.h>
#include <iostream>
#include "scene.hpp"
#include "components.hpp"

class Game
{
private:
    int width, height;
    std::string title;

    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;

    Scene *mainMenu;

    Scene *createMainMenu()
    {
        Scene *newScene = new Scene();
        auto startButton = std::make_shared<Entity>();
        startButton->AddComponent<SpriteRenderer>();
        newScene->AddObject(startButton);
        return newScene;
    }

public:
    Game(int width, int height, std::string title)
    {
        this->width = width;
        this->height = height;
        this->title = title;
    }

    ~Game() {}

    int init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            return -2;
        }
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            return -1;
        }

        GameManager::getInstance().renderer = SDL_CreateRenderer(window, -1, 0);

        mainMenu = createMainMenu();

        screenSurface = SDL_GetWindowSurface(window);

        // Fill the surface white
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
        // Update the surface
        SDL_UpdateWindowSurface(window);

        mainMenu->Update();
        SDL_RenderPresent(GameManager::getInstance().renderer);

        return 0;
    }

    void run()
    {
        SDL_Event e;
        bool quit = false;
        while (quit == false)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    quit = true;
            }
            SDL_SetRenderDrawColor(GameManager::getInstance().renderer, 0, 0, 0, 255);
            SDL_RenderClear(GameManager::getInstance().renderer);
        }
    }

    void quit()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};