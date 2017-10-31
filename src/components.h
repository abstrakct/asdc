/*
 * components.h
 */

#pragma once

#include "ecs.h"

struct Health {
    Health() {}
    Health(u32 h, u32 m) : hitPoints(h), maxHitPoints(m) {}

    u32 hitPoints = 50;
    u32 maxHitPoints = 100;
};

struct Position {
    Position() {}
    Position(u32 _x, u32 _y) : x(_x), y(_y) {}

    u32 x, y;
};

struct Renderable {
    Renderable() {}
    Renderable(unsigned char g, u32 c) : glyph(g), fgColor(c) {}

    unsigned char glyph;
    u32 fgColor;
};

enum MapCellType {
    cellUnknown = 0,
    cellFloor,
    cellWall
};

struct MapCell {
    MapCell() {}
    MapCell(MapCellType t) : type(t) {}

    MapCellType type = cellUnknown;
};

struct Physicality {
    Physicality() {}
    Physicality(bool l, bool m) : blocksLight(l), blocksMovement(m) {}

    bool blocksLight;
    bool blocksMovement;
};
