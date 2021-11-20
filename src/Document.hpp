#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "FontSheet.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Document
{
    std::vector<std::string> lines;
    SDL_Point cursor;
    int memory;

    public:
        Document();
        Document(std::string filePath);
        void addNewline();
        void addTab();
        void addText(std::string text);
        void removeForward();
        void removeBackward();
        void moveCursorUp();
        void moveCursorDown();
        void moveCursorLeft();
        void moveCursorRight();
        void draw(SDL_Point to, SDL_Point bounds, FontSheet *font, SDL_Renderer *renderer);
};

#endif
