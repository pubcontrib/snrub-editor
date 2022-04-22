#include "FontSheet.hpp"
#include "SpriteSheet.hpp"
#include <SDL2/SDL.h>
#include <string>

FontSheet::FontSheet(SpriteSheet *sheet)
{
    this->sheet = sheet;
    this->style = FontStyle::BASIC;
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

void FontSheet::setStyle(FontStyle style)
{
    this->style = style;
}

void FontSheet::draw(SDL_Point to, std::string text, SDL_Renderer *renderer)
{
    SDL_Point from = {0, static_cast<int>(style)};
    auto length = text.length();

    for (size_t index = 0; index < length; index++)
    {
        auto symbol = text[index];

        if (symbol >= 32 && symbol <= 126)
        {
            // Use the ASCII code as the position in the sprite sheet
            from.x = symbol - 32;
        }
        else
        {
            // Otherwise use the 0th index which is reserved for undefined
            from.x = 127 - 32;
        }

        sheet->draw(&to, &from, renderer);

        to.x += this->sheet->getFrameWidth();
    }
}
