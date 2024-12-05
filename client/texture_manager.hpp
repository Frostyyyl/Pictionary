#pragma once
#include <SDL2/SDL_image.h>
#include "game_manager.hpp"
#include <iostream>

class TextureManager
{
public:
    static SDL_Texture *LoadTexture(const char *filename)
    {
        SDL_Surface *tempSurface = IMG_Load(filename);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(GameManager::renderer, tempSurface);
        if (tex == nullptr)
        {
            std::cerr << "ERROR: Couldn't load texture" << std::endl;
            exit(1);
        }
        SDL_FreeSurface(tempSurface);

        return tex;
    }

    static SDL_Texture *CreateCanvas(int w, int h)
    {
        SDL_Texture *canvas = SDL_CreateTexture(GameManager::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (!canvas)
        {
            std::cerr << "Failed to create canvas texture! SDL_Error: " << SDL_GetError() << std::endl;
            exit(1);
        }
        SDL_SetRenderTarget(GameManager::renderer, canvas);

        SDL_SetRenderDrawColor(GameManager::renderer, 255, 255, 255, 255);
        SDL_RenderClear(GameManager::renderer);
        SDL_SetRenderTarget(GameManager::renderer, nullptr);

        return canvas;
    }

    static void Draw(SDL_Texture *tex, SDL_Rect &src, SDL_Rect &dest)
    {
        SDL_RenderCopy(GameManager::renderer, tex, &src, &dest);
    }
    static void Draw(SDL_Texture *tex, SDL_Rect &dest)
    {
        SDL_RenderCopy(GameManager::renderer, tex, NULL, &dest);
    }
};