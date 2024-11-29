#pragma once
#include <SDL2/SDL.h>
#include "interactable.hpp"
#include <iostream>

class Window : Interactable
{
private:
    SDL_Window *window;

public:
    Window() {}
    ~Window() {}

    void CreateWindow(int width, int height, std::string title)
    {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    }

    SDL_Renderer *CreateRenderer()
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
        return renderer;
    }
    void HandleEvent(Uint32 eventType);
};