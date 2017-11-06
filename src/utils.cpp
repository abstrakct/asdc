/* 
 * Various utility functions
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
    boost::random::uniform_int_distribution<> dist(a, b);
    return dist(rng);
}

bool fiftyfifty()
{
    u64 i;

    boost::random::uniform_int_distribution<> dist(1, 100);
    i = dist(rng);

    if(i <= 50)
        return true;
    else
        return false;
}

bool x_in_y(u64 x, u64 y)
{
    u64 i;

    boost::random::uniform_int_distribution<> dist(1, y);
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
    boost::random::uniform_int_distribution<> dist(1, 6);
    return dist(rng);
}

int dice(int num, int sides, signed int modifier) {
    int min, max;

    min = num;
    max = num * sides;
    boost::random::uniform_int_distribution<> dist(min, max);
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

int myabs(int n)
{
    return ( (n > 0) ? n : (n * (-1)));
}

// Draw a line from x0,y0 to x1,y1
// Return vector of int pairs, each pair contains one coordinate on the line.
// DDA algorithm, based on https://www.tutorialspoint.com/computer_graphics/line_generation_algorithm.htm
// and http://www.geeksforgeeks.org/dda-line-generation-algorithm-computer-graphics/
std::vector<std::pair<int, int>> getLineCoordinates(int x0, int y0, int x1, int y1)
{
    std::vector<std::pair<int, int>> line;

    float x = x0;
    float y = y0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps;
    if(myabs(dx) > myabs(dy))
        steps = myabs(dx);
    else
        steps = myabs(dy);

    float xinc = dx / (float) steps;
    float yinc = dy / (float) steps;

    // Add first point (needed?)
   // line.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
    // Add the rest
    for (int i = 0; i <= steps; i++) {
        x += xinc;
        y += yinc;
        line.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
    }

    return line;
}

// Not working, unfinished implementation based on wikipedia article on Bresenham's line algorithm:
std::vector<std::pair<int, int>> getLineCoordinatesBresenhamNOTWORKINGORFINISHED(int x0, int y0, int x1, int y1)
{
    std::vector<std::pair<int, int>> line;

    float deltax = x1 - x0;
    float deltay = y1 - y0;
    float deltaerr = abs(deltay / deltax);
    float error = 0.0;

    int y =  y0;
    for (int x = x0; x <= x1; x++) {
        line.push_back(std::make_pair(x, y));
        error += deltaerr;
        while (error >= 0.5) {
            if (deltay < 0)
                y -= 1;
            else
                y += 1;
        }
    }

    return line;
}

// vim: fdm=syntax ft=cpp
