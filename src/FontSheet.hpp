#ifndef FONTSHEET_H
#define FONTSHEET_H

#include "SpriteSheet.hpp"
#include "Texture.hpp"
#include <SDL2/SDL.h>
#include <string>

class FontSheet
{
    SpriteSheet *sheet;

    public:
        FontSheet(SpriteSheet *sheet);
        int getFrameWidth();
        int getFrameHeight();
        void setColor(SDL_Color *color);
        void draw(SDL_Point to, std::string text, SDL_Renderer *renderer);
};

#endif
