#include "Texture.hpp"
#include "Common.hpp"
#include <SDL2/SDL.h>

SDL_Texture *loadBmpTexture(std::string path, SDL_Renderer *renderer)
{
    auto surface = SDL_LoadBMP(path.c_str());

    if (!surface)
    {
        crash("Failed to load sprite sheet surface.");
    }

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));

    auto texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
    {
        crash("Failed to create sprite sheet texture.");
    }

    SDL_FreeSurface(surface);

    return texture;
}

Texture::Texture(SDL_Texture *texture)
{
    this->texture = texture;
}

Texture::~Texture()
{
    SDL_DestroyTexture(texture);
}

void Texture::setColor(SDL_Color *color)
{
    SDL_SetTextureColorMod(texture, color->r, color->g, color->b);
}

void Texture::draw(int x, int y, int width, int height, SDL_Renderer *renderer)
{
    SDL_Rect source = {0, 0, width, height};
    SDL_Rect destination = {x, y, width, height};
    draw(source, destination, renderer);
}

void Texture::draw(SDL_Rect source, SDL_Rect destination, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, texture, &source, &destination);
}
