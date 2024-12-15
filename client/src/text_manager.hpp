#pragma once
#include <SDL2/SDL_ttf.h>
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

    SDL_Texture *loadText(TTF_Font *font, const std::string &text, SDL_Rect &dest)
    {
        if (text.empty())
            return nullptr;
        // In {} is the color in RGB, later maybe we will loadText with color?
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), {0, 0, 0}, 100);
        if (!textSurface)
        {
            std::cerr << "ERROR: Failed to render text surface: " << TTF_GetError() << std::endl;
            return nullptr;
        }
        dest.w = textSurface->w;
        dest.h = textSurface->h;
        SDL_Texture *texture = SDL_CreateTextureFromSurface(GameManager::renderer, textSurface);
        if (!texture)
        {
            std::cerr << "ERROR: Failed to create text texture: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(textSurface);

        return texture;
    }

    void init()
    {
        font = loadFont(fontFilename, 16);
    }
    TTF_Font *getFont() { return font; }
};