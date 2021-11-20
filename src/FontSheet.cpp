#include "FontSheet.hpp"
#include "SpriteSheet.hpp"
#include <SDL2/SDL.h>
#include <string>

FontSheet::FontSheet(SpriteSheet *sheet, std::string symbols)
{
    this->sheet = sheet;
    this->symbols = symbols;
}

int FontSheet::getFrameWidth()
{
    return this->sheet->getFrameWidth();
}

int FontSheet::getFrameHeight()
{
    return this->sheet->getFrameHeight();
}

void FontSheet::setColor(SDL_Color *color)
{
    this->sheet->setColor(color);
}

void FontSheet::draw(SDL_Point to, std::string text, SDL_Renderer *renderer)
{
    size_t length;

    length = text.length();

    for (size_t index = 0; index < length; index++)
    {
        char symbol = text[index];

        if (symbol != ' ')
        {
            size_t match = symbols.find(symbol);

            if (match != std::string::npos)
            {
                SDL_Point from = {(int) (match % 10), (int) (match / 10)};
                sheet->draw(&to, &from, renderer);
            }
            else
            {
                SDL_Point from = {0, 11};
                sheet->draw(&to, &from, renderer);
            }
        }

        to.x += this->sheet->getFrameWidth();
    }
}
