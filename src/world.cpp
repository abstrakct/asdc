/*
 * world.cpp
 */

#include "world.h"
#include "components.h"
#include "utils.h"

// TODO: move these definitions to a data file
#define WALL_GLYPH '#'
#define WALL_COLOR 0xAAAAAAFF
#define FLOOR_GLYPH '.'
#define FLOOR_COLOR 0x444444FF

ecs::Entity* makeWall(u32 x, u32 y)
{
    // TODO: look for already created cell at position?!
    // yes. we will need a good function for finding entities at a certain position (that is, with a Position component matching what we're looking for)
    
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(WALL_GLYPH, WALL_COLOR))->assign(MapCell(cellWall))->assign(Physicality(true, true));

}

ecs::Entity* makeFloor(u32 x, u32 y)
{
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(FLOOR_GLYPH, FLOOR_COLOR))->assign(MapCell(cellFloor))->assign(Physicality(false, false));
}

Level::Level(u32 w, u32 h)
{
    width = w;
    height = h;
    initCache();
}

Level::~Level()
{
    for(u32 i = 0; i < height; i++)
        delete cache[i];

    delete *cache;
}

void Level::fill(ecs::Entity* (*makerFunction)(u32 x, u32 y))
{
    for (u32 x = 0; x < width; x++) {
        for (u32 y = 0; y < height; y++) {
            cells.push_back(makerFunction(x, y));
        }
    }
}

void Level::initCache()
{
    cache = new MapCacheCell*[width];
    for (u32 i = 0; i < width; i++)
        cache[i] = new MapCacheCell[height];

    for (u32 x = 0; x < width; x++) {
        for (u32 y = 0; y < height; y++) {
            cache[x][y].type = cellUnused;
            cache[x][y].glyph = ' ';
            cache[x][y].fgColor = 0;
        }
    }
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
    level->fill(makeFloor);
    level->generateFrame();
    for (u32 x = 0; x < level->width; x++) {
        for (u32 y = 0; y < level->height; y++) {
            if(fiftyfifty()) {
                level->cells.push_back(makeWall(x, y));
            }
        }
    }
}

/*
 * Look at all entities in a certain position,
 * return true if any blocks movement.
 */
bool positionBlocksMovement(u32 x, u32 y)
{
    for (auto it : ecs::findAllEntitiesWithComponent<Position>()) {
        Position *c = it->component<Position>();
        if(c && c->x == x && c->y == y) {
            Physicality *p = it->component<Physicality>();
            if(p->blocksMovement)
                return true;
        }
    }
    return false;
}

