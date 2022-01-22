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

    SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Texture *texture = loadBmpTexture("res/font.bmp", renderer);
    SpriteSheet sheet(texture, 8, 12);
    FontSheet font(&sheet);
    color_theme_t theme;
    theme.comments = {194, 194, 194};
    theme.numbers = {197, 15, 31};
    theme.strings = {58, 150, 221};
    theme.lists = {47, 92, 192};
    theme.maps = {47, 92, 192};
    theme.calls = {47, 92, 192};
    theme.nulls = {249, 241, 165};
    theme.cursor = {255, 255, 255};
    theme.undefined = {255, 0, 0};
    SDL_Texture *toolbar = loadBmpTexture("res/toolbar.bmp", renderer);
    SDL_Texture *statusbar = loadBmpTexture("res/statusbar.bmp", renderer);
    SDL_Texture *check = loadBmpTexture("res/check.bmp", renderer);
    Document document = argc > 1 ? Document(argv[1]) : Document();
    bool quit = false;
    bool redraw = true;
    Uint32 lastUpdate = SDL_GetTicks();
    Uint32 lastMeasure = SDL_GetTicks();
    int lastFps = 0;
    int currentFps = 0;
    int targetFps = 60;
    SDL_Point bounds = {80, 24};

    while (!quit)
    {
        SDL_Event event;

        if (redraw)
        {
            SDL_RenderClear(renderer);

            // Draw tool bar
            for (int width = 0; width < 640; width += 24)
            {
                drawTexture(toolbar, width, 0, 24, 24, renderer);
            }

            drawTexture(check, 0, 0, 24, 24, renderer);

            // Draw text editor
            SDL_Point to = {0, 24};
            document.draw(to, bounds, &theme, &font, renderer);

            // Draw status bar
            for (int width = 0; width < 640; width += 16)
            {
                drawTexture(statusbar, width, 312, 16, 16, renderer);
            }

            to = {0, 316};
            std::string line = " L "
                + std::to_string(document.getCursorLineIndex())
                + "/"
                + std::to_string(document.getLineCount())
                + " | "
                + "C "
                + std::to_string(document.getCursorColumnIndex())
                + "/"
                + std::to_string(document.getColumnCount())
                + " |";
            SDL_Color color = {125, 125, 125, 255};
            font.setColor(&color);
            font.draw(to, line, renderer);

            to = {568, 316};
            line = "| "
                + std::to_string(lastFps)
                + " FPS";
            font.setColor(&color);
            font.draw(to, line, renderer);

            // Swap render buffer
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
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                    {
                        redraw = true;
                    }

                    break;
                case SDL_KEYDOWN:
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
                        case SDLK_PAGEUP:
                            document.movePageUp(bounds);
                            redraw = true;
                            break;
                        case SDLK_PAGEDOWN:
                            document.movePageDown(bounds);
                            redraw = true;
                            break;
                    }

                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                        case SDLK_TAB:
                        case SDLK_DELETE:
                        case SDLK_BACKSPACE:
                        case SDLK_UP:
                        case SDLK_DOWN:
                        case SDLK_LEFT:
                        case SDLK_RIGHT:
                        case SDLK_PAGEUP:
                        case SDLK_PAGEDOWN:
                            redraw = true;
                            break;
                    }

                    break;
                case SDL_TEXTINPUT:
                    document.addText(event.text.text);
                    redraw = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        SDL_Point click = {event.button.x, event.button.y};
                        SDL_Rect acceptBox = {0, 0, 24, 24};

                        if (SDL_PointInRect(&click, &acceptBox))
                        {
                            if (argc > 1)
                            {
                                auto saved = document.save(std::string(argv[1]));

                                if (saved)
                                {
                                    quit = 1;
                                    break;
                                }
                                else
                                {
                                    crash("Failed to save to file.");
                                }
                            }
                            else
                            {
                                crash("No file to save to.");
                            }
                        }
                    }

                    break;
            }
        }

        // Measure frame rate
        if (SDL_GetTicks() - lastMeasure >= 1000)
        {
            redraw = lastFps != currentFps;
            lastMeasure = SDL_GetTicks();
            lastFps = currentFps;
            currentFps = 0;
        }

        currentFps += 1;

        // Sleep until the next expected update frame
        Uint32 duration = SDL_GetTicks() - lastUpdate;
        int target = 1000 / targetFps;

        if (duration < target)
        {
            SDL_Delay(target - duration);
        }

        lastUpdate = SDL_GetTicks();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cerr << "Resources freed." << std::endl;
    SDL_Quit();

    return 0;
}
