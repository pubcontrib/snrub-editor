#include "Document.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>

Document::Document()
{
    lines = std::vector<std::string>();
    lines.push_back("");
    cursor = {0, 0};
    memory = 0;
}

Document::Document(std::string filePath)
{
    lines = std::vector<std::string>();
    cursor = {0, 0};

    std::ifstream fileStream(filePath);

    if (fileStream.is_open())
    {
        std::string line;

        while (getline(fileStream, line))
        {
            lines.push_back(line);
        }
    }
    else
    {
        lines.push_back("`Unable to load file.`");
    }

    fileStream.close();

    if (lines.size() == 0)
    {
        lines.push_back("");
    }
    else
    {
        cursor.x = lines[lines.size() - 1].length();
        cursor.y = lines.size() - 1;
    }

    memory = cursor.x;
}

void Document::addNewline()
{
    std::string line = lines[cursor.y];
    std::vector<std::string>::iterator iterator = lines.begin() + cursor.y + 1;

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
    int width = 2;
    int fill = width - (cursor.x % width);
    std::string space = "";

    for (int index = 0; index < fill; index++)
    {
        space += " ";
    }

    this->addText(space);
}

void Document::addText(std::string text)
{
    std::string line = lines[cursor.y];

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
    std::string line = lines[cursor.y];

    if (cursor.x == line.length() && cursor.y < lines.size() - 1)
    {
        std::vector<std::string>::iterator iterator = lines.begin() + cursor.y + 1;
        lines[cursor.y] = line + lines[cursor.y + 1];
        lines.erase(iterator);
    }
    else if (cursor.x < line.length())
    {
        std::string::iterator iterator = line.begin() + cursor.x;
        line.erase(iterator);
        lines[cursor.y] = line;
    }
}

void Document::removeBackward()
{
    std::string line = lines[cursor.y];

    if (line.length() == 0 && cursor.y > 0)
    {
        std::vector<std::string>::iterator iterator = lines.begin() + cursor.y;
        lines.erase(iterator);
        cursor.y -= 1;
        cursor.x = lines[cursor.y].length();
    }
    else if (cursor.x <= line.length())
    {
        if (cursor.x == 0 && cursor.y > 0)
        {
            std::vector<std::string>::iterator iterator = lines.begin() + cursor.y;
            lines.erase(iterator);
            cursor.x = lines[cursor.y - 1].length();
            lines[cursor.y - 1] += line;
            cursor.y -= 1;
        }
        else if (cursor.x > 0)
        {
            std::string::iterator iterator = line.begin() + cursor.x - 1;
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

void Document::draw(SDL_Point to, SDL_Point bounds, FontSheet *font, SDL_Renderer *renderer)
{
    SDL_Color color = {255, 255, 255};
    font->setColor(&color);

    SDL_Point origin = to;
    SDL_Point offset = {(cursor.x / bounds.x) * bounds.x, (cursor.y / bounds.y) * bounds.y};
    std::size_t size = lines.size();

    for (std::size_t index = 0; index < bounds.y; index++)
    {
        std::size_t adjustedIndex = index + offset.y;

        if (adjustedIndex >= size)
        {
            break;
        }

        std::string line = lines[adjustedIndex];

        if (line.length() >= offset.x)
        {
            std::string text = line.substr(offset.x, bounds.x);
            font->draw(to, text, renderer);
        }

        to.y += font->getFrameHeight();
    }

    color = {0, 255, 0};
    font->setColor(&color);
    to = {origin.x + ((cursor.x - offset.x) * font->getFrameWidth()), origin.y + ((cursor.y - offset.y) * font->getFrameHeight())};
    font->draw(to, "_", renderer);
}
