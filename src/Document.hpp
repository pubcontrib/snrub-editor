#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Document
{
    std::vector<std::string> lines;
    size_t cursorX;
    size_t cursorY;
    size_t memory;

    public:
        Document(std::string text);
        size_t getCursorX();
        size_t getCursorY();
        size_t getColumnsCapacity();
        size_t getLinesCapacity();
        std::vector<std::string> getLines();
        void addNewline();
        void addIndent(size_t width);
        void addText(std::string text);
        void removeForward();
        void removeBackward();
        void moveCursorUp();
        void moveCursorDown();
        void moveCursorLeft();
        void moveCursorRight();
        void movePageUp(SDL_Point bounds);
        void movePageDown(SDL_Point bounds);
        std::string copyToString();
};

#endif
