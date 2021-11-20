#ifndef FONTSHEET_H
#define FONTSHEET_H

#include "SpriteSheet.hpp"
#include <SDL2/SDL.h>
#include <string>

class FontSheet
{
    SpriteSheet *sheet;
    std::string symbols;

    public:
        FontSheet(SpriteSheet *sheet, std::string symbols);
        int getFrameWidth();
        int getFrameHeight();
        void setColor(SDL_Color *color);
        void draw(SDL_Point to, std::string text, SDL_Renderer *renderer);
};

#endif
