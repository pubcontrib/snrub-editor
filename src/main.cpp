#include "FontSheet.hpp"
#include "SpriteSheet.hpp"
#include "Texture.hpp"
#include "Document.hpp"
#include "Common.hpp"
#include <SDL2/SDL.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 326;
const char SYMBOL_ESCAPE = '\\';
const char SYMBOL_COMMENT = '`';
const char SYMBOL_NUMBER = '#';
const char SYMBOL_STRING = '"';
const char SYMBOL_LIST_START = '[';
const char SYMBOL_LIST_END = ']';
const char SYMBOL_MAP_START = '{';
const char SYMBOL_MAP_END = '}';
const char SYMBOL_CALL_START = '(';
const char SYMBOL_CALL_END = ')';
const char SYMBOL_NULL = '?';

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

void applicationLoop(SDL_Window *window, SDL_Renderer *renderer, int argc, char **argv);
void drawDocument(Document document, SDL_Point to, SDL_Point bounds, color_theme_t *theme, FontSheet *font, SDL_Renderer *renderer);
std::string readFile(std::string path);
bool writeFile(std::string path, std::string text);

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        crash("SDL failed to start.");
    }

    atexit(SDL_Quit);

    auto window = SDL_CreateWindow("snrub-editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
    {
        crash("Failed to create a window.");
    }

    SDL_SetWindowResizable(window, SDL_FALSE);

    auto renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        crash("Failed to create a renderer.");
    }

    SDL_SetRenderDrawColor(renderer, 8, 8, 8, 255);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    applicationLoop(window, renderer, argc, argv);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cerr << "Resources freed." << std::endl;
    SDL_Quit();

    return 0;
}

void applicationLoop(SDL_Window *window, SDL_Renderer *renderer, int argc, char **argv)
{
    std::filesystem::path res("res");

    if (!std::filesystem::is_directory(res))
    {
        res = std::filesystem::path("/usr/local/share/snrub-editor/res");
    }

    if (!std::filesystem::is_directory(res))
    {
        crash("Failed to locate resources directory.");
    }

    auto texture = std::make_unique<Texture>(loadBmpTexture(res / "font.bmp", renderer));
    SpriteSheet sheet(texture.get(), 8, 12);
    FontSheet font(&sheet);
    color_theme_t plainTheme;
    plainTheme.comments = {255, 255, 255};
    plainTheme.numbers = {255, 255, 255};
    plainTheme.strings = {255, 255, 255};
    plainTheme.lists = {255, 255, 255};
    plainTheme.maps = {255, 255, 255};
    plainTheme.calls = {255, 255, 255};
    plainTheme.nulls = {255, 255, 255};
    plainTheme.cursor = {255, 255, 255};
    plainTheme.undefined = {255, 255, 255};
    color_theme_t highlightedTheme;
    highlightedTheme.comments = {194, 194, 194};
    highlightedTheme.numbers = {195, 90, 190};
    highlightedTheme.strings = {78, 168, 238};
    highlightedTheme.lists = {136, 22, 131};
    highlightedTheme.maps = {167, 52, 48};
    highlightedTheme.calls = {47, 92, 192};
    highlightedTheme.nulls = {218, 19, 12};
    highlightedTheme.cursor = {255, 255, 255};
    highlightedTheme.undefined = {0, 255, 20};
    auto theme = highlightedTheme;
    size_t indentWidth = 2;
    auto toolbar = std::make_unique<Texture>(loadBmpTexture(res / "toolbar.bmp", renderer));
    auto statusbar = std::make_unique<Texture>(loadBmpTexture(res / "statusbar.bmp", renderer));
    auto check = std::make_unique<Texture>(loadBmpTexture(res / "check.bmp", renderer));
    auto oneX = std::make_unique<Texture>(loadBmpTexture(res / "1x.bmp", renderer));
    auto twoX = std::make_unique<Texture>(loadBmpTexture(res / "2x.bmp", renderer));
    auto toggle = std::make_unique<Texture>(loadBmpTexture(res / "toggle.bmp", renderer));
    SpriteSheet toggleSheet(toggle.get(), 24, 24);
    auto document = argc > 1 ? Document(readFile(argv[1])) : Document("");
    auto quit = false;
    auto redraw = true;
    auto highlighted = true;
    auto lastUpdate = SDL_GetTicks();
    auto lastMeasure = SDL_GetTicks();
    auto lastFps = 0;
    auto currentFps = 0;
    auto targetFps = 60;
    SDL_Point bounds = {80, 24};
    auto scale = 1;

    while (!quit)
    {
        SDL_Event event;

        if (redraw)
        {
            SDL_RenderClear(renderer);

            // Draw tool bar
            for (auto width = 0; width < 640; width += 24)
            {
                toolbar->draw(width, 0, 24, 24, renderer);
            }

            check->draw(0, 0, 24, 24, renderer);

            if (scale == 1)
            {
                twoX->draw(24, 0, 24, 24, renderer);
            }
            else if (scale == 2)
            {
                oneX->draw(24, 0, 24, 24, renderer);
            }

            SDL_Point to = {48, 0};
            SDL_Point cell = {highlighted ? 0 : 1, 0};
            toggleSheet.draw(&to, &cell, renderer);

            // Draw text editor
            to = {0, 24};
            drawDocument(document, to, bounds, &theme, &font, renderer);

            // Draw status bar
            for (auto width = 0; width < 640; width += 16)
            {
                statusbar->draw(width, 312, 16, 16, renderer);
            }

            to = {0, 314};
            auto line = "  L "
                + std::to_string(document.getCursorY() + 1)
                + "/"
                + std::to_string(document.getLinesCapacity())
                + "  |  "
                + "C "
                + std::to_string(document.getCursorX() + 1)
                + "/"
                + std::to_string(document.getColumnsCapacity())
                + "  |";
            auto capacity = WINDOW_WIDTH / font.getFrameWidth();

            for (auto index = line.length(); index < capacity - 11; index++)
            {
                line += " ";
            }

            line += "|  "
                + std::to_string(lastFps)
                + " FPS";

            for (auto index = line.length(); index < capacity; index++)
            {
                line += " ";
            }

            SDL_Color color = {142, 173, 149, 255};
            font.setColor(&color);
            font.setStyle(FontStyle::INVERTED);
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
                            break;
                        case SDLK_TAB:
                            document.addIndent(indentWidth);
                            break;
                        case SDLK_DELETE:
                            document.removeForward();
                            break;
                        case SDLK_BACKSPACE:
                            document.removeBackward();
                            break;
                        case SDLK_UP:
                            document.moveCursorUp();
                            break;
                        case SDLK_DOWN:
                            document.moveCursorDown();
                            break;
                        case SDLK_LEFT:
                            document.moveCursorLeft();
                            break;
                        case SDLK_RIGHT:
                            document.moveCursorRight();
                            break;
                        case SDLK_PAGEUP:
                            document.movePageUp(bounds);
                            break;
                        case SDLK_PAGEDOWN:
                            document.movePageDown(bounds);
                            break;
                    }

                    redraw = true;
                    break;
                case SDL_KEYUP:
                    redraw = true;
                    break;
                case SDL_TEXTINPUT:
                    document.addText(event.text.text);
                    redraw = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        SDL_Point click = {event.button.x, event.button.y};
                        SDL_Rect acceptBox = {0 * scale, 0 * scale, 24 * scale, 24 * scale};

                        if (SDL_PointInRect(&click, &acceptBox))
                        {
                            if (argc > 1)
                            {
                                auto text = document.copyToString();
                                auto saved = writeFile(std::string(argv[1]), text);

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

                        SDL_Rect scaleBox = {24 * scale, 0 * scale, 24 * scale, 24 * scale};

                        if (SDL_PointInRect(&click, &scaleBox))
                        {
                            scale = scale == 1 ? 2 : 1;
                            SDL_RenderSetScale(renderer, scale, scale);
                            SDL_SetWindowSize(window, WINDOW_WIDTH * scale, WINDOW_HEIGHT * scale);
                            break;
                        }

                        SDL_Rect highlightBox = {48 * scale, 0 * scale, 24 * scale, 24 * scale};

                        if (SDL_PointInRect(&click, &highlightBox))
                        {
                            highlighted = !highlighted;
                            theme = highlighted ? highlightedTheme : plainTheme;
                            redraw = true;
                            break;
                        }
                    }

                    break;
            }
        }

        // Measure frame rate
        if (SDL_GetTicks() - lastMeasure >= 1000)
        {
            if (lastFps != currentFps)
            {
                redraw = true;
            }

            lastMeasure = SDL_GetTicks();
            lastFps = currentFps;
            currentFps = 0;
        }

        currentFps += 1;

        // Sleep until the next expected update frame
        auto duration = SDL_GetTicks() - lastUpdate;
        auto target = 1000 / targetFps;

        if (duration < target)
        {
            SDL_Delay(target - duration);
        }

        lastUpdate = SDL_GetTicks();
    }
}

void drawDocument(Document document, SDL_Point to, SDL_Point bounds, color_theme_t *theme, FontSheet *font, SDL_Renderer *renderer)
{
    std::vector<std::string> lines = document.getLines();
    SDL_Point cursor = {document.getCursorX(), document.getCursorY()};
    SDL_Point position = {0, 0};
    SDL_Point min = {(cursor.x / bounds.x) * bounds.x, (cursor.y / bounds.y) * bounds.y};
    SDL_Point max = {min.x + bounds.x, min.y + bounds.y};
    auto escaping = false;
    enum stream_state_t {
        NOTHING_STREAM_STATE,
        COMMENT_STREAM_STATE,
        NUMBER_STREAM_STATE,
        STRING_STREAM_STATE
    };
    auto state = NOTHING_STREAM_STATE;

    font->setStyle(FontStyle::BASIC);

    for (auto line : lines)
    {
        for (auto symbol : line)
        {
            if (escaping)
            {
                escaping = false;
            }
            else
            {
                switch (symbol)
                {
                    case SYMBOL_ESCAPE:
                        escaping = true;
                        break;
                    case SYMBOL_COMMENT:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            state = COMMENT_STREAM_STATE;
                        }
                        else if (state == COMMENT_STREAM_STATE)
                        {
                            state = NOTHING_STREAM_STATE;
                        }

                        break;
                    case SYMBOL_NUMBER:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            state = NUMBER_STREAM_STATE;
                        }
                        else if (state == NUMBER_STREAM_STATE)
                        {
                            state = NOTHING_STREAM_STATE;
                        }

                        break;
                    case SYMBOL_STRING:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            state = STRING_STREAM_STATE;
                        }
                        else if (state == STRING_STREAM_STATE)
                        {
                            state = NOTHING_STREAM_STATE;
                        }

                        break;
                }

                switch (symbol)
                {
                    case SYMBOL_ESCAPE:
                        break;
                    case SYMBOL_COMMENT:
                        if (state == COMMENT_STREAM_STATE)
                        {
                            font->setColor(&theme->comments);
                        }

                        break;
                    case SYMBOL_NUMBER:
                        if (state == NUMBER_STREAM_STATE)
                        {
                            font->setColor(&theme->numbers);
                        }

                        break;
                    case SYMBOL_STRING:
                        if (state == STRING_STREAM_STATE)
                        {
                            font->setColor(&theme->strings);
                        }

                        break;
                    case SYMBOL_LIST_START:
                    case SYMBOL_LIST_END:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            font->setColor(&theme->lists);
                        }

                        break;
                    case SYMBOL_MAP_START:
                    case SYMBOL_MAP_END:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            font->setColor(&theme->maps);
                        }

                        break;
                    case SYMBOL_CALL_START:
                    case SYMBOL_CALL_END:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            font->setColor(&theme->calls);
                        }

                        break;
                    case SYMBOL_NULL:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            font->setColor(&theme->nulls);
                        }

                        break;
                    default:
                        if (state == NOTHING_STREAM_STATE)
                        {
                            font->setColor(&theme->undefined);
                        }

                        break;
                }
            }

            if (position.x >= min.x && position.x < max.x && position.y >= min.y && position.y < max.y)
            {
                auto text = std::string(1, symbol);
                font->draw({to.x + ((position.x - min.x) * font->getFrameWidth()), to.y + ((position.y - min.y) * font->getFrameHeight())}, text, renderer);
            }

            position.x += 1;
        }

        position.x = 0;
        position.y += 1;
    }

    font->setColor(&theme->cursor);
    font->draw({to.x + ((cursor.x - min.x) * font->getFrameWidth()), to.y + ((cursor.y - min.y) * font->getFrameHeight())}, "_", renderer);
}

std::string readFile(std::string path)
{
    std::ifstream stream(path);

    if (stream.is_open())
    {
        return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    }
    else
    {
        return "`Unable to load file.`";
    }
}

bool writeFile(std::string path, std::string text)
{
    std::ofstream stream(path);

    if (stream.is_open())
    {
        stream << text;
        return true;
    }
    else
    {
        return false;
    }
}
