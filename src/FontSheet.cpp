#include "FontSheet.hpp"
#include "SpriteSheet.hpp"
#include <SDL2/SDL.h>
#include <string>

FontSheet::FontSheet(SpriteSheet *sheet)
{
    this->sheet = sheet;
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
    auto length = text.length();

    for (auto index = 0; index < length; index++)
    {
        auto symbol = text[index];

        if (symbol >= 32 && symbol <= 126)
        {
            // Use the ASCII code as the position in the sprite sheet
            symbol -= 31;
        }
        else
        {
            // Otherwise use the 0th index which is reserved for undefined
            symbol = 0;
        }

        SDL_Point from = {symbol, 0};
        sheet->draw(&to, &from, renderer);

        to.x += this->sheet->getFrameWidth();
    }
}
