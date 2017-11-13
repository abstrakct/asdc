/*
 * components.h
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "ecs.h"

/*
 * Health component.
 * Holds data related to an entity's health.
 * For now, current and max HP.
 */
struct Health {
    Health() {}
    Health(u32 h, u32 m) : hitPoints(h), maxHitPoints(m) {}

    u32 hitPoints = 50;
    u32 maxHitPoints = 100;
};

/*
 * Position component.
 * Hold position data. For now: x, y and level.
 */
struct Position {
    Position() {}
    Position(u32 _x, u32 _y) : x(_x), y(_y) {}

    u32 x, y;
    //std::string level;
};

/*
 * Renderable component.
 * Holds data for glyph and color. An entity needs this component to be renderable.
 */
struct Renderable {
    Renderable() {}
    Renderable(unsigned char g, sf::Color fg, sf::Color bg, sf::Color faded) : glyph(g), fgColor(fg), bgColor(bg), fadedColor(faded) {}

    unsigned char glyph;
    sf::Color fgColor;
    sf::Color bgColor;
    sf::Color fadedColor;
};

/*
 * Mapcell component. Indicates that this entity is a cell on the map, and
 * what type of cell it is.
 */
enum MapCellType {
    cellUnused = 0,
    cellFloor,
    cellWall
};

struct MapCell {
//    MapCell() {}
//    MapCell(MapCellType t) : type(t) {}
//
//    MapCellType type = cellUnused;
};

/*
 * Physicality component. Handles the physical properties of an entity.
 * For now, whether it blocks light or not, and whether it blocks movement or not.
 * Also whether or not the entity is actually visible. E.g. a floor doesn't block
 * light or movement, but it's still visible.
 * A trap or something else hidden can be not visible and not blocking light/movement.
 */
struct Physicality {
    Physicality() {}
    Physicality(bool l, bool m, bool v) : blocksLight(l), blocksMovement(m), visible(v) {}

    bool blocksLight;
    bool blocksMovement;
    bool visible;        // TODO: use this for FOV? probably not very efficient?
};

/*
 * Vision component. Handles FOV data, and in the future other data related
 * to vision, e.g. the ability to see invisible things, blindness, etc.
 */
#define MAXFOV 10
struct Vision {
    Vision() {}
    Vision(int fov) : fovRadius(fov) {}

    int fovRadius = 0;

    // 2D array for what's in FOV. For each cell, if true then it's visible.
    std::array<std::array<bool, 256>, 256> fovMap = {};                     // For now, make it a big array so we don't to translate coordinates and crap. Also, zero out the array. TODO: move FOV to mapcache?
};

/*
 * Openable component. For things that can be opened and closed
 */

struct Openable {
    Openable() {}
    Openable(bool b, std::string o, std::string c) : isOpen(b), openID(o), closedID(c) {}

    bool isOpen;
    std::string openID, closedID;
};























