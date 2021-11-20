#include "Document.hpp"
#include "FontSheet.hpp"
#include "SpriteSheet.hpp"
#include "Common.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string>

void drawTexture(SDL_Texture *texture, int x, int y, int width, int height, SDL_Renderer *renderer)
{
    SDL_Rect source = {0, 0, width, height};
    SDL_Rect destination = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, &source, &destination);
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        crash("SDL failed to start.");
    }

    atexit(SDL_Quit);

    SDL_Window *window = SDL_CreateWindow("snrub-editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 328, SDL_WINDOW_SHOWN);

    if (!window)
    {
        crash("Failed to create a window.");
    }

    SDL_SetWindowResizable(window, SDL_FALSE);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        crash("Failed to create a renderer.");
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Texture *texture = loadBmpTexture("res/font.bmp", renderer);
    SpriteSheet sheet(texture, 8, 12);
    FontSheet font(&sheet, "0123456789"
        "ABCDEFGHIJ"
        "KLMNOPQRST"
        "UVWXYZ    "
        "abcdefghij"
        "klmnopqrst"
        "uvwxyz    "
        "!@#$%^&*()"
        "[]{}/\\|?.,"
        ":;\"'`-+=<>"
        "~_         ");
    SDL_Texture *placeholder16 = loadBmpTexture("res/16.bmp", renderer);
    SDL_Texture *placeholder24 = loadBmpTexture("res/24.bmp", renderer);
    SDL_Texture *placeholder32 = loadBmpTexture("res/32.bmp", renderer);
    Document document = argc > 1 ? Document(argv[1]) : Document();
    bool quit = false;
    bool redraw = true;

    while (!quit)
    {
        SDL_Event event;

        if (redraw)
        {
            SDL_RenderClear(renderer);

            for (int width = 0; width < 640; width += 24)
            {
                drawTexture(placeholder24, width, 0, 24, 24, renderer);
            }

            for (int width = 0; width < 640; width += 16)
            {
                drawTexture(placeholder16, width, 312, 16, 16, renderer);
            }

            SDL_Point to = {0, 24};
            SDL_Point bounds = {80, 24};
            document.draw(to, bounds, &font, renderer);

            SDL_RenderPresent(renderer);

            redraw = false;
        }

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    std::cerr << "Quit detected." << std::endl;
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            std::cerr << "Quit detected." << std::endl;
                            quit = true;
                            break;
                        case SDLK_RETURN:
                            document.addNewline();
                            redraw = true;
                            break;
                        case SDLK_TAB:
                            document.addTab();
                            redraw = true;
                            break;
                        case SDLK_DELETE:
                            document.removeForward();
                            redraw = true;
                            break;
                        case SDLK_BACKSPACE:
                            document.removeBackward();
                            redraw = true;
                            break;
                        case SDLK_UP:
                            document.moveCursorUp();
                            redraw = true;
                            break;
                        case SDLK_DOWN:
                            document.moveCursorDown();
                            redraw = true;
                            break;
                        case SDLK_LEFT:
                            document.moveCursorLeft();
                            redraw = true;
                            break;
                        case SDLK_RIGHT:
                            document.moveCursorRight();
                            redraw = true;
                            break;
                    }

                    break;
                }
                case SDL_TEXTINPUT:
                    document.addText(event.text.text);
                    redraw = true;
                    break;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cerr << "Resources freed." << std::endl;
    SDL_Quit();

    return 0;
}
