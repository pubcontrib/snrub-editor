#ifndef FONTSHEET_H
#define FONTSHEET_H

#include "SpriteSheet.hpp"
#include "Texture.hpp"
#include <SDL2/SDL.h>
#include <string>

enum class FontStyle : int
{
    BASIC = 0,
    INVERTED = 1
};

class FontSheet
{
    SpriteSheet *sheet;
    FontStyle style;

    public:
        FontSheet(SpriteSheet *sheet);
        int getFrameWidth();
        int getFrameHeight();
        void setColor(SDL_Color *color);
        void setStyle(FontStyle style);
        void draw(SDL_Point to, std::string text, SDL_Renderer *renderer);
};

#endif
