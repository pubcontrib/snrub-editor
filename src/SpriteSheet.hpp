#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <string>

SDL_Texture *loadBmpTexture(std::string path, SDL_Renderer *renderer);

class SpriteSheet
{
    SDL_Texture *texture;
    int frameWidth;
    int frameHeight;

    public:
        SpriteSheet(SDL_Texture *texture, int frameWidth, int frameHeight);
        ~SpriteSheet();
        int getFrameWidth();
        int getFrameHeight();
        void setColor(SDL_Color *color);
        void draw(SDL_Point *to, SDL_Point *cell, SDL_Renderer *renderer);
};

#endif
