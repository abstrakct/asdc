/*
 * console.h
 */

#include <iostream>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "common.h"

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
        u32 width, height;

        // Separate font to its own class/struct?
        std::string fontFileName;
        u32* fontImage;
        u32 fontImageWidth, fontImageHeight;
        sf::Texture fontTexture;
        sf::Sprite fontSprite[256];
        u8* pixels;
        Cell cell[256][256];            // TODO: remove hard coded max values

        sf::IntRect rectForGlyph(unsigned char c);
        void createSprites();
    public:
        Console(u32 w, u32 h);
        ~Console();

        void setFont(std::string font, u8 cw, u8 ch, u32 iw, u32 ih);
        u8* getPixels() { return pixels; };
        void fillColor(u32 color);
        void fillChar(unsigned char c);
        //void fill(Rectangle r, u32 color);
        void clear();
        void render();

        void put(u32 cellX, u32 cellY, unsigned char c);
        void put(u32 cellX, u32 cellY, unsigned char c, u32 color);

        void copyBlend(Rectangle *destRect, u32 destPixelsPerRow,
             Rectangle *srcRect, u32 srcPixelsPerRow,
             u32 *newColor);
        void copyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
             Rectangle srcRect, u32 srcPixelsPerRow,
             u32 newColor);
        void simpleCopyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
             Rectangle srcRect, u32 srcPixelsPerRow,
             u32 newColor);
        

        void generateRandomPixels();

        bool dirty = true;
        u32 rows, cols;
        u32 widthPixels, heightPixels;
        u8 fontCharWidth, fontCharHeight;
        Rectangle cellToRectangle(u32 x, u32 y);
};
