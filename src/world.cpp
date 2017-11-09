/*
 * world.cpp
 */

#include "world.h"
#include "components.h"
#include "utils.h"
#include "ecs.h"

// TODO: move these definitions to a data file
#define WALL_GLYPH  '#'
#define WALL_COLOR  sf::Color(0xAAAAAAFF)
#define FLOOR_GLYPH '.'
#define FLOOR_COLOR sf::Color(0x444444FF)

// TODO TOP PRIORITY!
// Make a nice and flexible level building / dungeon generating system!
// * Read definitions of dungeon features and such from a config file.
// * Use these definitions when generating or changing a cell.
// * Add a level builder with methods for making a room, making hallways, placing them on the level, etc.
//
// IDEA: use exceptions?
// Like this maybe:
// try {
//   place_room_at(x,y,w,h);
// } catch exception {
//   --- room couldn't be placed there
//   ...
// TODO: read about exceptions and throwing and catching!

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
    
void Level::changeIntoWall(u32 x, u32 y)
{
    // TODO: only problem now is that this is very slow!
    // UPDATE: it's faster now that we set done=true - but still pretty slow (about 2 seconds for a 10x10 room.
    // If we are doing this many times on each level it will be very slow. TODO: OPTIMIZE  (variadic each might be the thing)
    //
    // BEST way is probably to delete the entity and create a new one, at least during level generation.
    // Probably not good in-game because it requires rebuilding map cache
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
                            it->assign(Renderable(WALL_GLYPH, WALL_COLOR))->assign(MapCell())->assign(Physicality(true, true, true));
                            done = true;
                        }
                    }
                }
            }
        }
    }
}

ecs::Entity* makeWall(u32 x, u32 y)
{
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(WALL_GLYPH, WALL_COLOR))->assign(MapCell())->assign(Physicality(true, true, true));
}

ecs::Entity* makeFloor(u32 x, u32 y)
{
    return ecs::createEntity()->assign(Position(x, y))->assign(Renderable(FLOOR_GLYPH, FLOOR_COLOR))->assign(MapCell())->assign(Physicality(false, false, true));
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
        cells.push_back(makeWall(x, y1));
        cells.push_back(makeWall(x, y2));
    }
    for (u32 y = y1; y <= y2; y++) {
        cells.push_back(makeWall(x1, y));
        cells.push_back(makeWall(x2, y));
    }
}

void Level::generateHouse(u32 x1, u32 y1, u32 x2, u32 y2)
{
/*    u32 doorx;
    u32 doory;
    bool horizontal = false;

    // pick door to be on horizontal or vertical walls
    // TODO: delete or fix the shit down below
    // rewrite when we have a nice way of changing walls into floors and such.
    if (fiftyfifty()) {
        horizontal = true;
        doorx = ri(x1+1, x2-1);
        doory = fiftyfifty() ? y1 : y2;
    } else {
        horizontal = false;
        doory = ri(y1+1, y2-1);
        doorx = fiftyfifty() ? x1 : x2;
    }

    for (u32 x = x1; x <= x2; x++) {
        if (x == doorx && horizontal)
            continue;
        if (doory == y1) {
            makeWall(x, y2);
        }
        if (doory == y2) {
            makeWall(x, y1);
        }

    }
    for (u32 y = y1; y <= y2; y++) {
        if (y == doory && !horizontal)
            continue;
        if (doorx == x1) {
            makeWall(x2, y);
        }
        if (doorx == x2) {
            makeWall(x1, y);
        }

    }
*/
}

void Level::generateVillage()
{
    // Generate many houses with entrances/doors
    generateFrame(10+ri(1,5), 10+ri(1,5), ri(15,25), ri(15,25));
}

// TODO/IDEA: when generating a level, "fill" it with nothing, then populate the void with dungeon features, then fill in remaining cells with floor...?

void World::generate()
{
    currentLevel->generateFrame(0, 0, currentLevel->width-1, currentLevel->height-1);
    currentLevel->generateVillage();
    //for (u32 x = 0; x < currentLevel->width; x++) {
    //    for (u32 y = 0; y < currentLevel->height; y++) {
    //        if(one_in(4)) {
    //            currentLevel->cells.push_back(makeWall(x, y));
    //        } else {
    //            currentLevel->cells.push_back(makeFloor(x, y));
    //        }
    //    }
    //}
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

