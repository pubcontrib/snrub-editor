#include "SpriteSheet.hpp"
#include "Common.hpp"
#include <SDL2/SDL.h>
#include <string>

SDL_Texture *loadBmpTexture(std::string path, SDL_Renderer *renderer)
{
    SDL_Surface *surface = SDL_LoadBMP(path.c_str());

    if (!surface)
    {
        crash("Failed to load sprite sheet surface.");
    }

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
    {
        crash("Failed to create sprite sheet texture.");
    }

    SDL_FreeSurface(surface);

    return texture;
}

SpriteSheet::SpriteSheet(SDL_Texture *texture, int frameWidth, int frameHeight)
{
    this->texture = texture;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
}

SpriteSheet::~SpriteSheet()
{
    SDL_DestroyTexture(texture);
}

int SpriteSheet::getFrameWidth()
{
    return this->frameWidth;
}

int SpriteSheet::getFrameHeight()
{
    return this->frameHeight;
}

void SpriteSheet::setColor(SDL_Color *color)
{
    SDL_SetTextureColorMod(texture, color->r, color->g, color->b);
}

void SpriteSheet::draw(SDL_Point *to, SDL_Point *cell, SDL_Renderer *renderer)
{
    SDL_Rect source = {cell->x * frameWidth, cell->y * frameHeight, frameWidth, frameHeight};
    SDL_Rect destination = {to->x, to->y, frameWidth, frameHeight};
    SDL_RenderCopy(renderer, texture, &source, &destination);
}
