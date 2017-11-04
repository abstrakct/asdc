/*
 * console.cpp
 */

#include <iostream>
#include "console.h"
#include "common.h"
#include "utils.h"

extern sf::Texture tex;
extern sf::RenderWindow window;

Console::Console(u32 w, u32 h)
{
    width = w;
    height = h;

    widthPixels = width * 4;
    heightPixels = height * 4;
    
    pixels = (u8*) malloc(sizeof(u8) * widthPixels * heightPixels);
}

Console::~Console()
{
    free(pixels);
}

// Taken from / inspired by PT_Console at https://github.com/pdetagyos/RoguelikeTutorial/
sf::IntRect Console::rectForGlyph(unsigned char c)
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
    rows = width / fontCharWidth;
    cols = height / fontCharHeight;

    // initialize cells
    for(u32 x = 0; x < cols; x++) {
        for(u32 y = 0; y < rows; y++) {
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
    for (u32 y = 0; y < rows; y++) {
        for (u32 x = 0; x < cols; x++) {
            cell[x][y].fgColor = newColor;
        }
    }
}


void Console::fillChar(unsigned char c)
{
    for (u32 y = 0; y < rows; y++) {
        for (u32 x = 0; x < cols; x++) {
            cell[x][y].c = c;
        }
    }
}

void Console::clear()
{
    fillChar(0);
}

void Console::render()
{
    // Go through cells, render all to pixels
    for (u32 y = 0; y < rows; y++) {
        for (u32 x = 0; x < cols; x++) {
            unsigned char c = cell[x][y].c;
            if(c) {
                fontSprite[c].setPosition(x*fontCharWidth, y*fontCharHeight);
                fontSprite[c].setColor(cell[x][y].fgColor);
                window.draw(fontSprite[c]);
            }
        }
    }
}

// Also inspired by PT_Console
// x,y = cell coordinates, not pixels!
void Console::put(u32 cellX, u32 cellY, unsigned char c)
{
    cell[cellX][cellY].c = c;
}

void Console::put(u32 cellX, u32 cellY, unsigned char c, u32 color)
{
    sf::Color newColor;
    newColor.r = RED(color);
    newColor.g = GREEN(color);
    newColor.b = BLUE(color);
    newColor.a = ALPHA(color);

    cell[cellX][cellY].c = c;
    cell[cellX][cellY].fgColor = newColor;
}

void Console::generateRandomPixels()
{
    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            //pixels->at(y * width + x) = ri(0, 2147483647);
        }
    }
}

Rectangle Console::cellToRectangle(u32 x, u32 y)
{
    Rectangle result;

    result.x = x * fontCharWidth;
    result.y = y * fontCharWidth;
    result.w = fontCharWidth;
    result.h = fontCharHeight;

    return result;
}
