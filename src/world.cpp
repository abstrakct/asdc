/*
 * world.cpp
 */

#include "world.h"
#include "components.h"
#include "utils.h"

// TODO: move these definitions to a data file?!
#define WALL_GLYPH '#'
#define WALL_COLOR 0xAAAAAAFF

ecs::Entity* makeWall(u32 x, u32 y)
{
    // TODO: look for already created cell at position?!
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(WALL_GLYPH, WALL_COLOR))->assign(MapCell(cellWall))->assign(Physicality(true, true));

}

void Level::generateFrame()
{
    for (u32 x = 0; x < width; x++) {
        cells.push_back(makeWall(x, 0));
        cells.push_back(makeWall(x, height-1));
    }
    for (u32 y = 0; y < height; y++) {
        cells.push_back(makeWall(0, y));
        cells.push_back(makeWall(width-1, y));
    }
}

void World::generate()
{
    level->generateFrame();
}
