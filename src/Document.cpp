#include "Document.hpp"
#include <vector>
#include <string>

Document::Document(std::string text)
{
    lines = std::vector<std::string>();
    cursorX = 0;
    cursorY = 0;
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

size_t Document::getCursorX()
{
    return cursorX;
}

size_t Document::getCursorY()
{
    return cursorY;
}

size_t Document::getColumnsCapacity()
{
    return lines[cursorY].length();
}

size_t Document::getLinesCapacity()
{
    return lines.size();
}

std::vector<std::string> Document::getLines()
{
    return lines;
}

void Document::addNewline()
{
    auto line = lines[cursorY];
    auto iterator = lines.begin() + cursorY + 1;

    if (cursorX < line.length())
    {
        lines.insert(iterator, line.substr(cursorX));
        lines[cursorY] = line.substr(0, cursorX);
    }
    else
    {
        lines.insert(iterator, "");
    }

    cursorX = 0;
    cursorY += 1;
    memory = cursorX;
}

void Document::addIndent(size_t width)
{
    auto fill = width - (cursorX % width);
    std::string space = "";

    for (size_t index = 0; index < fill; index++)
    {
        space += " ";
    }

    this->addText(space);
}

void Document::addText(std::string text)
{
    auto line = lines[cursorY];

    if (cursorX < line.length())
    {
        line.insert(cursorX, text);
    }
    else
    {
        line.append(text);
    }

    lines[cursorY] = line;
    cursorX += text.length();
    memory = cursorX;
}

void Document::removeForward()
{
    auto line = lines[cursorY];

    if (cursorX == line.length() && cursorY < lines.size() - 1)
    {
        auto iterator = lines.begin() + cursorY + 1;
        lines[cursorY] = line + lines[cursorY + 1];
        lines.erase(iterator);
    }
    else if (cursorX < line.length())
    {
        auto iterator = line.begin() + cursorX;
        line.erase(iterator);
        lines[cursorY] = line;
    }
}

void Document::removeBackward()
{
    auto line = lines[cursorY];

    if (line.length() == 0 && cursorY > 0)
    {
        auto iterator = lines.begin() + cursorY;
        lines.erase(iterator);
        cursorY -= 1;
        cursorX = lines[cursorY].length();
    }
    else if (cursorX <= line.length())
    {
        if (cursorX == 0 && cursorY > 0)
        {
            auto iterator = lines.begin() + cursorY;
            lines.erase(iterator);
            cursorX = lines[cursorY - 1].length();
            lines[cursorY - 1] += line;
            cursorY -= 1;
        }
        else if (cursorX > 0)
        {
            auto iterator = line.begin() + cursorX - 1;
            line.erase(iterator);
            lines[cursorY] = line;
            cursorX -= 1;
        }
    }

    memory = cursorX;
}

void Document::moveCursorUp()
{
    if (cursorY > 0)
    {
        cursorY -= 1;
        cursorX = std::min(memory, lines[cursorY].length());
    }
}

void Document::moveCursorDown()
{
    if (cursorY < lines.size() - 1)
    {
        cursorY += 1;
        cursorX = std::min(memory, lines[cursorY].length());
    }
}

void Document::moveCursorLeft()
{
    if (cursorX > 0)
    {
        cursorX -= 1;
    }
    else if (cursorY > 0)
    {
        cursorY -= 1;
        cursorX = lines[cursorY].length();
    }

    memory = cursorX;
}

void Document::moveCursorRight()
{
    if (cursorX < lines[cursorY].length())
    {
        cursorX += 1;
    }
    else if (cursorY < lines.size() - 1)
    {
        cursorX = 0;
        cursorY += 1;
    }

    memory = cursorX;
}

void Document::movePageUp(SDL_Point bounds)
{
    cursorY -= bounds.y;

    if (cursorY < 0)
    {
        cursorY = 0;
    }

    cursorX = std::min(memory, lines[cursorY].length());
}

void Document::movePageDown(SDL_Point bounds)
{
    cursorY += bounds.y;

    if (cursorY >= lines.size())
    {
        cursorY = lines.size() - 1;
    }

    cursorX = std::min(memory, lines[cursorY].length());
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
