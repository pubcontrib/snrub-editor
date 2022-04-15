#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <string>

SDL_Texture *loadBmpTexture(std::string path, SDL_Renderer *renderer);

class Texture
{
    SDL_Texture *texture;

    public:
        Texture(SDL_Texture *texture);
        ~Texture();
        void setColor(SDL_Color *color);
        void draw(int x, int y, int width, int height, SDL_Renderer *renderer);
        void draw(SDL_Rect source, SDL_Rect destination, SDL_Renderer *renderer);
};

#endif
