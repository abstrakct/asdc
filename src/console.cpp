/*
 * console.cpp
 *
 * As of now it can handle drawing characters from a .png bitmap font file to the screen.
 * Should be expanded.
 *
 * Most of this file was originally take from / inspired by PT_Console at https://github.com/pdetagyos/RoguelikeTutorial/
 *
 * I have since migrated from SDL to SFML and rewritten more or less everything in this file.
 */

#include <iostream>
#include "console.h"
#include "common.h"
#include "utils.h"

//extern sf::RenderWindow window;
extern sf::RenderTexture tex;

Console::Console(int xO, int yO, u32 w, u32 h, sf::BlendMode b)
{
    widthInPixels = w;
    heightInPixels = h;
    xOffset = xO;
    yOffset = yO;
    blend = b;
    tex.create(w, h);
}

Console::~Console()
{
}

sf::IntRect Console::rectForGlyph(unsigned char c) const
{
    int index = c;
    int charsPerRow = (fontImageWidth / fontCharWidth);
    int xOffset = (index % charsPerRow) * fontCharWidth;
    int yOffset = (index / charsPerRow) * fontCharHeight;

    sf::IntRect r(xOffset, yOffset, fontCharWidth, fontCharHeight);
    return r;
}

void Console::setFont(std::string font, u8 cw, u8 ch, u32 iw, u32 ih)
{
    fontFileName = font;
    fontCharWidth = cw;
    fontCharHeight = ch;
    fontImageWidth = iw;
    fontImageHeight = ih;
    heightInChars = heightInPixels / fontCharHeight;
    widthInChars = widthInPixels / fontCharWidth;

    // initialize cells
    for(u32 x = 0; x < widthInChars; x++) {
        for(u32 y = 0; y < heightInChars; y++) {
            cell[x][y].c = 0;
            cell[x][y].fgColor = sf::Color::White;
            cell[x][y].bgColor = sf::Color::Black;
        }
    }
    
    if(!fontTexture.loadFromFile(fontFileName)) {
        std::runtime_error("Couldn't load font file!");
    }

    createSprites();
}

// Create a sprite for each character in font.
// Hard-coded for now to only work with a font with 256 glyphs.
void Console::createSprites()
{
    for(u32 i = 0; i < 256; i++) {
        fontSprite[i].setTexture(fontTexture);
        fontSprite[i].setTextureRect(rectForGlyph(i));
    }
}

void Console::fillColor(u32 color)
{
    sf::Color newColor;
    newColor.r = RED(color);
    newColor.g = GREEN(color);
    newColor.b = BLUE(color);
    newColor.a = ALPHA(color);
    for (u32 y = 0; y < heightInChars; y++) {
        for (u32 x = 0; x < widthInChars; x++) {
            cell[x][y].fgColor = newColor;
        }
    }
}

void Console::fillColor(sf::Color color)
{
    for (u32 y = 0; y < heightInChars; y++) {
        for (u32 x = 0; x < widthInChars; x++) {
            cell[x][y].fgColor = color;
        }
    }
}

void Console::fillChar(unsigned char c)
{
    for (u32 y = 0; y < heightInChars; y++) {
        for (u32 x = 0; x < widthInChars; x++) {
            cell[x][y].c = c;
        }
    }
}

void Console::clear()
{
    fillChar(0);
}

void Console::render(sf::RenderWindow &window)
{
    tex.clear(sf::Color::Black);
    // Go through cells, render all to pixels
    for (u32 x = 0; x < widthInChars; x++) {
        for (u32 y = 0; y < heightInChars; y++) {
            unsigned char c = cell[x][y].c;
            if(c) {
                fontSprite[c].setPosition(x*fontCharWidth, y*fontCharHeight);
                fontSprite[c].setColor(cell[x][y].fgColor);
                tex.draw(fontSprite[c]);
            }
        }
    }

    tex.display();
    sf::Sprite compositor(tex.getTexture());
    compositor.move(xOffset, yOffset);
    window.draw(compositor, blend);
}

void Console::render(sf::RenderWindow &window, u32 startx, u32 starty, u32 endx, u32 endy)
{
    // Go through subset of cells, render all to pixels
    for (u32 x = startx; x <= endx; x++) {
        for (u32 y = starty; y <= endy; y++) {
            unsigned char c = cell[x][y].c;
            if(c) {
                fontSprite[c].setPosition(x*fontCharWidth, y*fontCharHeight);
                fontSprite[c].setColor(cell[x][y].fgColor);
                window.draw(fontSprite[c]);
            }
        }
    }
}

// x,y = cell coordinates, not pixels!
void Console::put(u32 cellX, u32 cellY, unsigned char c)
{
    cell[cellX][cellY].c = c;
}

void Console::put(u32 x, u32 y, unsigned char c, u32 color)
{
    sf::Color newColor;
    newColor.r = RED(color);
    newColor.g = GREEN(color);
    newColor.b = BLUE(color);
    newColor.a = ALPHA(color);

    cell[x][y].c = c;
    cell[x][y].fgColor = newColor;
}

void Console::put(u32 x, u32 y, unsigned char c, sf::Color color)
{
    cell[x][y].c = c;
    cell[x][y].fgColor = color;
}

void Console::print(int x, int y, std::string text)
{
    // TODO: ERROR checking etc!
    for (auto c : text) {
        put(x, y, c);
        x++;
    }
}

void Console::print(int x, int y, std::string text, u32 fgColor)
{
    // TODO: ERROR checking etc!
    for (auto c : text) {
        put(x, y, c, fgColor);
        x++;
    }
}

void Console::print(int x, int y, std::string text, sf::Color fgColor)
{
    // TODO: ERROR checking etc!
    for (auto c : text) {
        put(x, y, c, fgColor);
        x++;
    }
}

namespace boxchar {
const int topleft    = 0xda;
const int topright   = 0xbf;
const int botleft    = 0xc0;
const int botright   = 0xd9;
const int vertical   = 0xc4;
const int horizontal = 0xb3;
}
void Console::printBox(int x, int y, int w, int h)
{
    put(x, y, boxchar::topleft, 0xffffffff);
    put(x+w, y, boxchar::topright, 0xffffffff);
    put(x, y+h, boxchar::botleft, 0xffffffff);
    put(x+w, y+h, boxchar::botright, 0xffffffff);

    for (int i = x+1; i < (x+w); i++) {
        put(i, y, boxchar::vertical, 0xffffffff);
        put(i, y+h, boxchar::vertical, 0xffffffff);
    }
    for (int i = y+1; i < (y+h); i++) {
        put(x, i, boxchar::horizontal, 0xffffffff);
        put(x+w, i, boxchar::horizontal, 0xffffffff);
    }
}

void Console::textBox(std::string title, std::string text)
{
    fillChar(' ');
    // TODO: support word-wrapping / multiline text
    const int boxWidth = text.size() + 4;
    const int boxHeight = 5;
    const int x = (widthInChars / 2) - (boxWidth / 2);
    const int y = (heightInChars / 2) - (boxHeight / 2);
    const int titlex = x + (boxWidth / 2) - (title.size() / 2);

    printBox(x, y, boxWidth, boxHeight);
    print(titlex, y, title, sf::Color::White);
    print(x+3, y+2, text, sf::Color::White);
}

Rectangle Console::cellToRectangle(u32 x, u32 y) const
{
    Rectangle result;

    result.x = x * fontCharWidth;
    result.y = y * fontCharWidth;
    result.w = fontCharWidth;
    result.h = fontCharHeight;

    return result;
}
