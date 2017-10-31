/*
 * console.h
 */

#include <iostream>
#include <vector>
#include <memory>

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

// TODO: support cells with different size than the font?? e.g. 8x8 fonts in 16x16 cells?!
class Console {
    private:
        u32 width, height;

        // Separate font to its own class/struct?
        std::string fontFileName;
        u32* fontImage;
        u8 fontCharWidth, fontCharHeight;
        u32 fontImageWidth, fontImageHeight;

        std::shared_ptr<std::vector<u32>> pixels;

        Rectangle rectForGlyph(unsigned char c);
    public:
        Console(u32 w, u32 h);
        ~Console() = default;

        void setFont(std::string font, u8 cw, u8 ch);
        u32* getPixels() { return pixels->data(); };
        void fill(u32 color);
        void fill(Rectangle r, u32 color);
        void clear();

        void put(u32 cellX, u32 cellY, unsigned char c);
        void put(u32 cellX, u32 cellY, unsigned char c, u32 color);

        void copyBlend(Rectangle *destRect, u32 destPixelsPerRow,
             Rectangle *srcRect, u32 srcPixelsPerRow,
             u32 *newColor);
        void copyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
             Rectangle srcRect, u32 srcPixelsPerRow,
             u32 newColor);
        

        void generateRandomPixels();

        bool dirty = true;
        u32 rows, cols;
};
