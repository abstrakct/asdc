/*
 * world.cpp
 */

#include "world.h"
#include "components.h"
#include "utils.h"
#include "ecs.h"

// TODO: move these definitions to a data file
#define WALL_GLYPH  '#'
#define WALL_COLOR  0xAAAAAAFF
#define FLOOR_GLYPH '.'
#define FLOOR_COLOR 0x444444FF

std::vector<ecs::Entity*> findAllEntitiesAtPosition(u32 x, u32 y)
{
    std::vector<ecs::Entity*> ret;

    auto positions = ecs::findAllEntitiesWithComponent<Position>();
    for (auto it : positions) {
        if (it->component<Position>()->x == x && it->component<Position>()->y == y)
            ret.push_back(it);
    }

    return ret;
}

    // we will need a good function for finding entities at a certain position (that is, with a Position component matching what we're looking for, or a specific subset of components, e.g. mapcell & position)
    // TODO: implement variadic each from RLTK!!!!!!!!!!!!!
    // Also TODO: a better way to change e.g. mapcells based on definitions or templates or something.
    // TODO: check if garbage collection works.
    // also TODO: findallentititiesatposition is very slow! see above and ecs.h/cpp
    
void Level::makeWall(u32 x, u32 y)
{
    // TODO: only problem now is that this is very slow!
    // UPDATE: it's faster now that we set done=true - but still pretty slow (about 2 seconds for a 10x10 room.
    // If we are doing this many times on each level it will be very slow. TODO: OPTIMIZE  (variadic each might be the thing)
    bool done = false;
    if (cells.size() > 0) {
        for (auto it : cells) {
            if(done) break;
            if (it) {
                Position *pos = it->component<Position>();
                if(pos) {
                    if(pos->x == x && pos->y == y) {
                        // update entity
                        MapCell *m = it->component<MapCell>();
                        Renderable *r = it->component<Renderable>();
                        Physicality *p = it->component<Physicality>();

                        if (m && r && p) {
                            ecs::deleteComponent<MapCell>(it->id);
                            ecs::deleteComponent<Renderable>(it->id);
                            ecs::deleteComponent<Physicality>(it->id);
                            // make sure components are removed so we can add new ones
                            ecs::collectGarbage();
                            it->assign(Renderable(WALL_GLYPH, WALL_COLOR))->assign(MapCell(cellWall))->assign(Physicality(true, true));
                            done = true;
                        }
                    }
                }
            }
        }
    }
}

ecs::Entity* makeFloor(u32 x, u32 y)
{
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(FLOOR_GLYPH, FLOOR_COLOR))->assign(MapCell(cellFloor))->assign(Physicality(false, false));
}

Level::Level(u32 w, u32 h)
{
    width = w;
    height = h;

    fill(makeFloor);
}

Level::~Level()
{
    //for(u32 i = 0; i < height; i++)
    //    delete cache[i];

    //delete *cache;
}

void Level::fill(ecs::Entity* (*makerFunction)(u32 x, u32 y))
{
    for (u32 x = 0; x < width; x++) {
        for (u32 y = 0; y < height; y++) {
            cells.push_back(makerFunction(x, y));
        }
    }
}

// TODO: probably not needed?!
void Level::initCache()
{
    //cache.resize(width*height);
    
    //cache = new MapCacheCell*[width];
    //for (u32 i = 0; i < width; i++)
    //    cache[i] = new MapCacheCell[height];

    //for (u32 x = 0; x < width; x++) {
    //    for (u32 y = 0; y < height; y++) {
    //        cache[x][y].type = cellUnused;
    //        cache[x][y].glyph = ' ';
    //        cache[x][y].fgColor = 0;
    //    }
    //}
}

void Level::generateFrame(u32 x1, u32 y1, u32 x2, u32 y2)
{
    for (u32 x = x1; x <= x2; x++) {
        makeWall(x, y1);
        makeWall(x, y2);
    }
    for (u32 y = y1; y <= y2; y++) {
        makeWall(x1, y);
        makeWall(x2, y);
    }
}

void Level::generateVillage()
{
    generateFrame(10, 10, 20, 20);
}

void World::generate()
{
    currentLevel->generateFrame(0, 0, currentLevel->width-1, currentLevel->height-1);
    currentLevel->generateVillage();
//    for (u32 x = 0; x < currentLevel->width; x++) {
//        for (u32 y = 0; y < currentLevel->height; y++) {
//            if(one_in(4)) {
//                currentLevel->cells.push_back(makeWall(x, y));
//            }
//        }
//    }
}

void World::addLevel(std::string levelName, u32 w, u32 h)
{
    level[levelName] = std::make_shared<Level>(w, h);
}

/*
 * Look at all entities in a certain position,
 * return true if any blocks movement.
 * TODO: move to cache!???!??!
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

