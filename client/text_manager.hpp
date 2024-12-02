#pragma once
#include "SDL2/SDL_TTF.h"
#include "game_manager.hpp"
#include "text.hpp"

class TextManager
{
private:
    TextManager() {};
    const char *fontFilename = "fonts/placeholder.ttf";
    TTF_Font *font;

public:
    static TextManager &getInstance()
    {
        static TextManager INSTANCE;
        return INSTANCE;
    }
    TTF_Font *loadFont(const char *fontFilename, int fontSize)
    {
        TTF_Font *font = TTF_OpenFont(fontFilename, fontSize);
        if (!font)
        {
            std::cerr << "ERROR: Failed to load font: " << TTF_GetError() << std::endl;
        }
        return font;
    }
    SDL_Texture *loadText(TTF_Font *font, Text &contents, SDL_Rect &dest)
    {
        SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, dest.w, dest.h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, contents.text.c_str(), {12, 15, 10}, contents.dest.w);
        if (!tempSurface)
        {
            std::cerr << "ERROR: Failed to render text surface: " << TTF_GetError() << std::endl;
        }
        if (!textSurface)
        {
            std::cerr << "ERROR: Failed to render text surface: " << TTF_GetError() << std::endl;
        }
        if (SDL_BlitSurface(textSurface, NULL, tempSurface, &contents.dest) == -1)
        {
            std::cerr << "ERROR: Failed to merge surfaces";
        }
        SDL_Texture *tex = SDL_CreateTextureFromSurface(GameManager::renderer, tempSurface);
        if (!tex)
        {
            std::cerr << "ERROR: Failed to create text texture: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(tempSurface);
        SDL_FreeSurface(textSurface);

        return tex;
    }
    void init()
    {
        font = loadFont(fontFilename, 16);
    }
    TTF_Font *getFont() { return font; }
};