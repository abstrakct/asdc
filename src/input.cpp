/*
 * input.h
 *
 * Input handling
 *
 * 
 */

#include <algorithm>
#include <iostream>
#include "input.h"

static Mouse mouse;

Mouse& getMouseState()
{
    return mouse;
}

std::pair<int, int> getMousePosition()
{
    return std::make_pair(mouse.x, mouse.y);
}

void setMousePosition(const int x, const int y)
{
    mouse.x = x;
    mouse.y = y;
    //std::cout << "mx = " << mouse.x << "  my = " << mouse.y << std::endl;
}
