#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "Texture.hpp"
#include <SDL2/SDL.h>
#include <string>

class SpriteSheet
{
    Texture *texture;
    int frameWidth;
    int frameHeight;

    public:
        SpriteSheet(Texture *texture, int frameWidth, int frameHeight);
        int getFrameWidth();
        int getFrameHeight();
        void setColor(SDL_Color *color);
        void draw(SDL_Point *to, SDL_Point *cell, SDL_Renderer *renderer);
};

#endif
