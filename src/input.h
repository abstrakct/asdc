/*
 * input.h
 *
 * Input handling
 *
 * 
 */

#pragma once

#include <algorithm>

// TODO: Improve this (overall Game State struct) std::pair for mx,my?
struct Mouse {
    int x, y;  // mouse x,y
    bool b[5];  // for mouse buttons, m[1] is true if mouse button 1 is pressed, etc.
};

Mouse& getMouseState();
std::pair<int, int> getMousePosition();
void setMousePosition(const int x, const int y);

