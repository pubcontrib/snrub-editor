#include "SpriteSheet.hpp"
#include "Common.hpp"
#include <SDL2/SDL.h>

SpriteSheet::SpriteSheet(Texture *texture, int frameWidth, int frameHeight)
{
    this->texture = texture;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
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
    texture->setColor(color);
}

void SpriteSheet::draw(SDL_Point *to, SDL_Point *cell, SDL_Renderer *renderer)
{
    SDL_Rect source = {cell->x * frameWidth, cell->y * frameHeight, frameWidth, frameHeight};
    SDL_Rect destination = {to->x, to->y, frameWidth, frameHeight};
    texture->draw(source, destination, renderer);
}
