/*
 * console.cpp
 */

#include <iostream>
#include "console.h"
#include "common.h"
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

Console::Console(u32 w, u32 h)
{
    width = w;
    height = h;
    pixels = std::make_shared<std::vector<u32>>();

    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            pixels->push_back(0);
        }
    }
}

// Taken from / inspired by PT_Console at https://github.com/pdetagyos/RoguelikeTutorial/
Rectangle Console::rectForGlyph(unsigned char c)
{
    u32 index = c;
    u32 charsPerRow = (fontImageWidth / fontCharWidth);
    u32 xOffset = (index % charsPerRow) * fontCharWidth;
    u32 yOffset = (index / charsPerRow) * fontCharHeight;

    Rectangle r = {xOffset, yOffset, fontCharWidth, fontCharHeight};
    return r;
}

void Console::setFont(std::string font, u8 cw, u8 ch)
{
    fontFileName = font;
    fontCharWidth = cw;
    fontCharHeight = ch;
    rows = width / fontCharWidth;
    cols = height / fontCharHeight;
    
    int imgWidth, imgHeight, numComponents;
    unsigned char *imgData = stbi_load(fontFileName.c_str(), &imgWidth, &imgHeight, &numComponents, STBI_rgb_alpha);
    fontImageWidth = imgWidth;
    fontImageHeight = imgHeight;
    fontImage = (u32*) malloc(imgWidth*imgHeight*sizeof(u32));
    memcpy(fontImage, imgData, (imgWidth*imgHeight*sizeof(u32)));
    stbi_image_free(imgData);
}

void Console::fill(u32 color)
{
    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            pixels->at(y * width + x) = color;
        }
    }
}

void Console::fill(Rectangle r, u32 color)
{
    for(u32 x = r.x; x <= r.w; x++) {
        for(u32 y = r.y; y <= r.h; y++) {
            pixels->at(y * width + x) = color;
        }
    }
}

void Console::clear()
{
    fill(0x000000FF);
}

// Also inspired by PT_Console
// x,y = cell coordinates, not pixels!
void Console::put(u32 cellX, u32 cellY, unsigned char c)
{
    u32 x = cellX * fontCharWidth;
    u32 y = cellY * fontCharHeight;
    Rectangle srcRect = rectForGlyph(c);
    Rectangle destRect = { x, y, fontCharWidth, fontCharHeight };

    copyFontToPixels(destRect, width, srcRect, fontImageWidth, 0xFFFFFFFF);
}

void Console::generateRandomPixels()
{
    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            pixels->at(y * width + x) = ri(0, 2147483647);
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
void Console::copyFontToPixels(Rectangle destRect, u32 destPixelsPerRow,
                        Rectangle srcRect, u32 srcPixelsPerRow,
                        u32 newColor)
{
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

            u32 srcColor = fontImage[(srcY * srcPixelsPerRow) + srcX];
            u32 *destPixel = &(firstPixel[(dstY * destPixelsPerRow) + dstX]);
            u32 destColor = *destPixel;

            // Colorize our source pixel before we blend it
            srcColor = PT_ColorizePixel(srcColor, newColor);

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
}
