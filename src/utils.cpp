/* 
 * utils.h
 *
 * Various useful things and utility functions, like easy access to random numbers in various ways.
 */

#include <stdlib.h>
#include <string>
#include <cctype>
#include <iostream>
#include <algorithm>

using namespace std;

#include "utils.h"
#include "common.h"
#include "console.h"

u64 ri(u64 a, u64 b) 
{
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

bool fiftyfifty()
{
    u64 i;

    std::uniform_int_distribution<> dist(1, 100);
    i = dist(rng);

    if(i <= 50)
        return true;
    else
        return false;
}

bool x_in_y(u64 x, u64 y)
{
    u64 i;

    std::uniform_int_distribution<> dist(1, y);
    i = dist(rng);

    if(i <= x)
        return true;
    else
        return false;
}

bool one_in(int chance)
{
    return x_in_y(1, chance);
}

int roll_die() {
    return ri(1, 6);
}

int dice(int num, int sides, signed int modifier) {
    int min, max;

    min = num;
    max = num * sides;
    std::uniform_int_distribution<> dist(min, max);
    return (dist(rng) + modifier);
}

// Again, credits to PT_Console - https://github.com/pdetagyos/RoguelikeTutorial/blob/tutorial-step-01/pt_console.c
u32 PT_ColorizePixel(u32 dest, u32 src) 
{
    // Colorize the destination pixel using the source color
    if (ALPHA(dest) == 255) {
        return src;
    } else if (ALPHA(dest) > 0) {
        // Scale the final alpha based on both dest & src alphas
        return COLOR_FROM_RGBA(RED(src), 
                               GREEN(src), 
                               BLUE(src), 
                               (u8)(ALPHA(src) * (ALPHA(dest) / 255.0)));
    } else {
        return dest;
    }
}












// vim: fdm=syntax ft=cpp
