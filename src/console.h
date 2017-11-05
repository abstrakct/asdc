/*
 * console.h
 */

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "common.h"

// TODO: Add printing function! For strings!

// Helper macros
#define RED(c)   ((c & 0xff000000) >> 24)
#define GREEN(c) ((c & 0x00ff0000) >> 16)
#define BLUE(c)  ((c & 0x0000ff00) >>  8)
#define ALPHA(c) (c & 0xff)
#define COLOR_FROM_RGBA(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

struct Rectangle {
    u32 x, y, w, h;
};

struct Cell {
    unsigned char c;
    sf::Color fgColor;
    sf::Color bgColor;
};

// TODO: support cells with different size than the font?? e.g. 8x8 fonts in 16x16 cells?!
class Console {
    private:
        u32 widthInPixels, heightInPixels;

        // Separate font to its own class/struct?
        std::string fontFileName;
        u32* fontImage;
        u32 fontImageWidth, fontImageHeight;
        sf::Texture fontTexture;
        sf::Sprite fontSprite[256];
        Cell cell[256][256];            // TODO: remove hard coded max values
        sf::RenderTexture tex;
        int xOffset, yOffset;

        sf::IntRect rectForGlyph(unsigned char c);
        void createSprites();
    public:
        Console(int xO, int yO, u32 w, u32 h);
        //Console(u32 w, u32 h, std::string font);
        ~Console();

        void setFont(std::string font, u8 cw, u8 ch, u32 iw, u32 ih);

        void fillColor(u32 color);
        void fillChar(unsigned char c);
        void clear();
        void render(sf::RenderWindow &window);
        void render(sf::RenderWindow &window, u32 x, u32 y, u32 w, u32 h);

        void put(u32 cellX, u32 cellY, unsigned char c);
        void put(u32 cellX, u32 cellY, unsigned char c, u32 color);
        void print(int x, int y, std::string text);

        bool dirty = true;
        u32 widthInChars;
        u32 heightInChars;
        u8 fontCharWidth, fontCharHeight;
        Rectangle cellToRectangle(u32 x, u32 y);
};
