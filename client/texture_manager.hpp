#pragma once
#include "SDL2/SDL_image.h"
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
    static void Draw(SDL_Texture *tex, SDL_Rect &src, SDL_Rect &dest)
    {
        SDL_RenderCopy(GameManager::renderer, tex, &src, &dest);
    }
};