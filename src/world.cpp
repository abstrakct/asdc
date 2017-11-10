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
    
//void Level::changeIntoWall(u32 x, u32 y)
//{
    // TODO: only problem now is that this is very slow!
    // UPDATE: it's faster now that we set done=true - but still pretty slow (about 2 seconds for a 10x10 room.
    // If we are doing this many times on each level it will be very slow. TODO: OPTIMIZE  (variadic each might be the thing)
    //
    // BEST way is probably to delete the entity and create a new one, at least during level generation.
    // Probably not good in-game because it requires rebuilding map cache
//}

Level::Level(u32 w, u32 h)
{
    width = w;
    height = h;
    lastx = w - 1;
    lasty = h - 1;
}

Level::~Level()
{
}


// TODO/IDEA: when generating a level, "fill" it with nothing, then populate the void with dungeon features, then fill in remaining cells with floor...?

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

