#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "FontSheet.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

#define SYMBOL_ESCAPE '\\'
#define SYMBOL_COMMENT '`'
#define SYMBOL_NUMBER '#'
#define SYMBOL_STRING '"'
#define SYMBOL_LIST_START '['
#define SYMBOL_LIST_END ']'
#define SYMBOL_MAP_START '{'
#define SYMBOL_MAP_END '}'
#define SYMBOL_CALL_START '('
#define SYMBOL_CALL_END ')'
#define SYMBOL_NULL '?'

typedef struct
{
    SDL_Color comments;
    SDL_Color numbers;
    SDL_Color strings;
    SDL_Color lists;
    SDL_Color maps;
    SDL_Color calls;
    SDL_Color nulls;
    SDL_Color cursor;
    SDL_Color undefined;
} color_theme_t;

class Document
{
    std::vector<std::string> lines;
    SDL_Point cursor;
    int memory;

    public:
        Document(std::string text);
        int getColumnCount();
        int getCursorColumnIndex();
        int getLineCount();
        int getCursorLineIndex();
        void addNewline();
        void addTab();
        void addText(std::string text);
        void removeForward();
        void removeBackward();
        void moveCursorUp();
        void moveCursorDown();
        void moveCursorLeft();
        void moveCursorRight();
        void movePageUp(SDL_Point bounds);
        void movePageDown(SDL_Point bounds);
        void draw(SDL_Point to, SDL_Point bounds, color_theme_t *theme, FontSheet *font, SDL_Renderer *renderer);
        bool save(std::string filePath);
};

#endif
