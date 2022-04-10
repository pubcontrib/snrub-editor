#include "Document.hpp"
#include <vector>
#include <string>
#include <algorithm>

Document::Document(std::string text)
{
    lines = std::vector<std::string>();
    cursor = {0, 0};
    memory = 0;

    std::string buffer = "";

    for (auto iterator = text.begin(); iterator != text.end(); iterator++)
    {
        auto symbol = *iterator;

        if (symbol == '\n')
        {
            lines.push_back(buffer);
            buffer = "";
        }
        else
        {
            buffer.push_back(symbol);
        }
    }

    lines.push_back(buffer);
}

int Document::getColumnCount()
{
    return lines[cursor.y].length();
}

int Document::getCursorColumnIndex()
{
    return cursor.x + 1;
}

int Document::getLineCount()
{
    return lines.size();
}

int Document::getCursorLineIndex()
{
    return cursor.y + 1;
}

void Document::addNewline()
{
    auto line = lines[cursor.y];
    auto iterator = lines.begin() + cursor.y + 1;

    if (cursor.x < line.length())
    {
        lines.insert(iterator, line.substr(cursor.x));
        lines[cursor.y] = line.substr(0, cursor.x);
    }
    else
    {
        lines.insert(iterator, "");
    }

    cursor.x = 0;
    cursor.y++;
    memory = cursor.x;
}

void Document::addTab()
{
    auto width = 2;
    auto fill = width - (cursor.x % width);
    std::string space = "";

    for (auto index = 0; index < fill; index++)
    {
        space += " ";
    }

    this->addText(space);
}

void Document::addText(std::string text)
{
    auto line = lines[cursor.y];

    if (cursor.x < line.length())
    {
        line.insert(cursor.x, text);
    }
    else
    {
        line.append(text);
    }

    lines[cursor.y] = line;
    cursor.x += text.length();
    memory = cursor.x;
}

void Document::removeForward()
{
    auto line = lines[cursor.y];

    if (cursor.x == line.length() && cursor.y < lines.size() - 1)
    {
        auto iterator = lines.begin() + cursor.y + 1;
        lines[cursor.y] = line + lines[cursor.y + 1];
        lines.erase(iterator);
    }
    else if (cursor.x < line.length())
    {
        auto iterator = line.begin() + cursor.x;
        line.erase(iterator);
        lines[cursor.y] = line;
    }
}

void Document::removeBackward()
{
    auto line = lines[cursor.y];

    if (line.length() == 0 && cursor.y > 0)
    {
        auto iterator = lines.begin() + cursor.y;
        lines.erase(iterator);
        cursor.y -= 1;
        cursor.x = lines[cursor.y].length();
    }
    else if (cursor.x <= line.length())
    {
        if (cursor.x == 0 && cursor.y > 0)
        {
            auto iterator = lines.begin() + cursor.y;
            lines.erase(iterator);
            cursor.x = lines[cursor.y - 1].length();
            lines[cursor.y - 1] += line;
            cursor.y -= 1;
        }
        else if (cursor.x > 0)
        {
            auto iterator = line.begin() + cursor.x - 1;
            line.erase(iterator);
            lines[cursor.y] = line;
            cursor.x -= 1;
        }
    }

    memory = cursor.x;
}

void Document::moveCursorUp()
{
    if (cursor.y > 0)
    {
        cursor.y -= 1;
        cursor.x = std::min(memory, (int) lines[cursor.y].length());
    }
}

void Document::moveCursorDown()
{
    if (cursor.y < lines.size() - 1)
    {
        cursor.y += 1;
        cursor.x = std::min(memory, (int) lines[cursor.y].length());
    }
}

void Document::moveCursorLeft()
{
    if (cursor.x > 0)
    {
        cursor.x -= 1;
    }
    else if (cursor.y > 0)
    {
        cursor.y -= 1;
        cursor.x = lines[cursor.y].length();
    }

    memory = cursor.x;
}

void Document::moveCursorRight()
{
    if (cursor.x < lines[cursor.y].length())
    {
        cursor.x += 1;
    }
    else if (cursor.y < lines.size() - 1)
    {
        cursor.x = 0;
        cursor.y += 1;
    }

    memory = cursor.x;
}

void Document::movePageUp(SDL_Point bounds)
{
    cursor.y -= bounds.y;

    if (cursor.y < 0)
    {
        cursor.y = 0;
    }

    cursor.x = std::min(memory, (int) lines[cursor.y].length());
}

void Document::movePageDown(SDL_Point bounds)
{
    cursor.y += bounds.y;

    if (cursor.y >= lines.size())
    {
        cursor.y = lines.size() - 1;
    }

    cursor.x = std::min(memory, (int) lines[cursor.y].length());
}

std::string Document::copyToString()
{
    std::string text = "";
    auto first = true;

    for (auto line : lines)
    {
        if (!first)
        {
            text.push_back('\n');
        }
        else
        {
            first = false;
        }

        text.append(line);
    }

    return text;
}

void Document::draw(SDL_Point to, SDL_Point bounds, color_theme_t *theme, FontSheet *font, SDL_Renderer *renderer)
{
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
