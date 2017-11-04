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
    //cell = std::make_shared<std::vector<u8>>();
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

/*
void Console::fill(Rectangle r, u32 color)
{
    for(u32 x = r.x; x <= r.w; x++) {
        for(u32 y = r.y; y <= r.h; y++) {
            pixels[y * width + x] = RED(color);
            pixels[y * width + x + 1] = GREEN(color);
            pixels[y * width + x + 2] = BLUE(color);
            pixels[y * width + x + 3] = ALPHA(color);
        }
    }
}
*/

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

    //u32 x = cellX * fontCharWidth;
    //u32 y = cellY * fontCharHeight;
    //Rectangle srcRect = rectForGlyph(c);
    //Rectangle destRect = { x, y, fontCharWidth, fontCharHeight };

    //copyFontToPixels(destRect, width, srcRect, fontImageWidth, 0xFFFFFFFF);
}

void Console::put(u32 cellX, u32 cellY, unsigned char c, u32 color)
{
    cell[cellX][cellY].c = c;
    sf::Color newColor;
    newColor.r = RED(color);
    newColor.g = GREEN(color);
    newColor.b = BLUE(color);
    newColor.a = ALPHA(color);
    cell[cellX][cellY].fgColor = newColor;
    /*u32 x = cellX * fontCharWidth;
    u32 y = cellY * fontCharHeight;
    Rectangle srcRect = rectForGlyph(c);
    Rectangle destRect = { x, y, (u32)fontCharWidth*4, (u32)fontCharHeight*4 };

    copyFontToPixels(destRect, widthPixels, srcRect, fontImageWidth*4, color);*/
}

void Console::generateRandomPixels()
{
    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            //pixels->at(y * width + x) = ri(0, 2147483647);
        }
    }
}

// Again, credits to PT_Console - https://github.com/pdetagyos/RoguelikeTutorial/blob/tutorial-step-01/pt_console.c
// But modified to work with my implementation
void Console::copyBlend(Rectangle *destRect, u32 destPixelsPerRow,
                        Rectangle *srcRect, u32 srcPixelsPerRow,
                        u32 *newColor)
{
    /*
    // If src and dest rects are not the same size ==> bad things
    assert(destRect->w == srcRect->w && destRect->h == srcRect->h);

    // For each pixel in the destination rect, alpha blend to it the 
    // corresponding pixel in the source rect.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    u32 stopX = destRect->x + destRect->w;
    u32 stopY = destRect->y + destRect->h;

    for (u32 dstY = destRect->y, srcY = srcRect->y; 
         dstY < stopY; 
         dstY++, srcY++) {
        for (u32 dstX = destRect->x, srcX = srcRect->x; 
             dstX < stopX; 
             dstX++, srcX++) {

            u32 srcColor = fontImage[(srcY * srcPixelsPerRow) + srcX];
            u32 *destPixel = &pixels->at((dstY * destPixelsPerRow) + dstX);
            u32 destColor = *destPixel;

            // Colorize our source pixel before we blend it
            srcColor = PT_ColorizePixel(srcColor, *newColor);

            if (ALPHA(srcColor) == 0) {
                // Source is transparent - so do nothing
                continue;
            } else if (ALPHA(srcColor) == 255) {
                // Just copy the color, no blending necessary
                *destPixel = srcColor;
            } else {
                // Do alpha blending
                float srcA = ALPHA(srcColor) / 255.0;
                float invSrcA = (1.0 - srcA);
                float destA = ALPHA(destColor) / 255.0;

                float outAlpha = srcA + (destA * invSrcA);
                u8 fRed = ((RED(srcColor) * srcA) + (RED(destColor) * destA * invSrcA)) / outAlpha;
                u8 fGreen = ((GREEN(srcColor) * srcA) + (GREEN(destColor) * destA * invSrcA)) / outAlpha;
                u8 fBlue = ((BLUE(srcColor) * srcA) + (BLUE(destColor) * destA * invSrcA)) / outAlpha;
                u8 fAlpha = outAlpha * 255;

                *destPixel = COLOR_FROM_RGBA(fRed, fGreen, fBlue, fAlpha);
            }
        }
    }
    */
}

// Again, credits to PT_Console - https://github.com/pdetagyos/RoguelikeTutorial/blob/tutorial-step-01/pt_console.c
// But modified to work with my implementation
void Console::simpleCopyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
                               Rectangle srcRect, u32 srcPixelsPerRow,
                               u32 newColor)
{
/*
    // If src and dest rects are not the same size ==> bad things
    assert(destRect.w == srcRect.w && destRect.h == srcRect.h);

    // For each pixel in the destination rect, alpha blend to it the 
    // corresponding pixel in the source rect.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    u32 stopX = destRect.x + destRect.w;
    u32 stopY = destRect.y + destRect.h;
    u32 *firstPixel = pixels->data();


    

    for (u32 dstY = destRect.y, srcY = srcRect.y; dstY < stopY; dstY++, srcY++) {
        for (u32 dstX = destRect.x, srcX = srcRect.x; dstX < stopX; dstX++, srcX++) {

            //u32 srcColor = fontImage[(srcY * srcPixelsPerRow) + srcX];
            u32 *destPixel = &(firstPixel[(dstY * destPixelsPerRow) + dstX]);
            //u32 destColor = *destPixel;
            *destPixel = newColor;
*/
            // Colorize our source pixel before we blend it
            /*srcColor = PT_ColorizePixel(srcColor, newColor);

            if (ALPHA(srcColor) == 0) {
                // Source is transparent - so do nothing
                continue;
            } else if (ALPHA(srcColor) == 255) {
                // Just copy the color, no blending necessary
                *destPixel = srcColor;
            } else {
                // Do alpha blending
                float srcA = ALPHA(srcColor) / 255.0;
                float invSrcA = (1.0 - srcA);
                float destA = ALPHA(destColor) / 255.0;

                float outAlpha = srcA + (destA * invSrcA);
                u8 fRed = ((RED(srcColor) * srcA) + (RED(destColor) * destA * invSrcA)) / outAlpha;
                u8 fGreen = ((GREEN(srcColor) * srcA) + (GREEN(destColor) * destA * invSrcA)) / outAlpha;
                u8 fBlue = ((BLUE(srcColor) * srcA) + (BLUE(destColor) * destA * invSrcA)) / outAlpha;
                u8 fAlpha = outAlpha * 255;

                *destPixel = COLOR_FROM_RGBA(fRed, fGreen, fBlue, fAlpha);
            }*/
        //}
    //}
}

void Console::copyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
                               Rectangle srcRect, u32 srcPixelsPerRow,
                               u32 newColor)
{
    // If src and dest rects are not the same size ==> bad things
    //assert(destRect.w == srcRect.w && destRect.h == srcRect.h);

    // For each pixel in the destination rect, alpha blend to it the 
    // corresponding pixel in the source rect.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    //u32 stopX = destRect.x + destRect.w;
    //u32 stopY = destRect.y + destRect.h;
    //u8 *firstPixel = pixels;

    //for (u32 dstY = destRect.y, srcY = srcRect.y; dstY < stopY; dstY+=4, srcY++) {
    //    for (u32 dstX = destRect.x, srcX = srcRect.x; dstX < stopX; dstX+=4, srcX++) {

    //        u32 srcColor = fontImage[(srcY * srcPixelsPerRow) + srcX];
    //        u8 *destPixel = &(firstPixel[(dstY * destPixelsPerRow) + dstX]);
    //        u32 destColor = *destPixel;

    //        // Colorize our source pixel before we blend it
    //        srcColor = PT_ColorizePixel(srcColor, newColor);

    //        if (ALPHA(srcColor) == 0) {
    //            // Source is transparent - so do nothing
    //            continue;
    //        } else if (ALPHA(srcColor) == 255) {
    //            // Just copy the color, no blending necessary
    //            *destPixel = RED(srcColor); destPixel++;
    //            *destPixel = GREEN(srcColor); destPixel++;
    //            *destPixel = BLUE(srcColor); destPixel++;
    //            *destPixel = ALPHA(srcColor); destPixel++;
    //        } else {
    //            // Do alpha blending
    //            float srcA = ALPHA(srcColor) / 255.0;
    //            float invSrcA = (1.0 - srcA);
    //            float destA = ALPHA(destColor) / 255.0;

    //            float outAlpha = srcA + (destA * invSrcA);
    //            u8 fRed = ((RED(srcColor) * srcA) + (RED(destColor) * destA * invSrcA)) / outAlpha;
    //            u8 fGreen = ((GREEN(srcColor) * srcA) + (GREEN(destColor) * destA * invSrcA)) / outAlpha;
    //            u8 fBlue = ((BLUE(srcColor) * srcA) + (BLUE(destColor) * destA * invSrcA)) / outAlpha;
    //            u8 fAlpha = outAlpha * 255;

    //            *destPixel = fRed; destPixel++;
    //            *destPixel = fGreen; destPixel++;
    //            *destPixel = fBlue; destPixel++;
    //            *destPixel = fAlpha; destPixel++;
    //            // *destPixel = COLOR_FROM_RGBA(fRed, fGreen, fBlue, fAlpha);
    //        }
    //    }
    //}
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
