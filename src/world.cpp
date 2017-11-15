/*
 * world.cpp
 */

#include "world.h"
#include "components.h"
#include "utils.h"
#include "ecs.h"
#include "datafiles.h"
#include "levelfactory.h"
#include "game.h"
#include "messages.h"

extern GameState gs;
extern std::shared_ptr<World> world;

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


void World::addLevel(std::string levelName, u32 w, u32 h)
{
    level[levelName] = std::make_shared<Level>(w, h);
}

// Return coordinates to a position on currentLevel that can be occupied by a being.
// Assumes currentLevel points to an initialized level.
// TODO: use cache?!
std::pair<int, int> World::currentLevelGetOpenPosition()
{
    bool done = false;
    int x = 0;
    int y = 0;

    while(!done) {
        x = ri(1, currentLevel->lastx);
        y = ri(1, currentLevel->lasty);
        // TODO: check for other beings!
        if(!cellBlocksMovement(x, y))
            done = true;
    }

    return std::make_pair(x, y);
}

/*
 * Look at all entities in a certain position,
 * return true if any blocks movement.
 * TODO: move to cache!???!??!
 * TODO: return pointer to Entity if successful, nullptr if not?
 */
bool cellBlocksMovement(u32 x, u32 y)
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

// TODO: make a more generic "isInteractable/interact" function?
ecs::Entity* cellIsOpenable(u32 x, u32 y)
{
    for (auto it : ecs::findAllEntitiesWithComponent<Position>()) {
        Position *c = it->component<Position>();
        if(c && c->x == x && c->y == y) {
            Openable *o = it->component<Openable>();
            if(o)
                return it;
        }
    }
    return nullptr;
}

// TODO: only works on current level for now!
// Change if e.g. monsters/npcs on other levels can walk around and open doors!
// Alternatively, when Position component has level info, just use that.
// 
// Open an Openable entity.
// Returns true if successful.
bool cellOpen(ecs::Entity *e)
{
    if(e == nullptr)
        return false;

    Position *p = e->component<Position>();
    Openable *o = e->component<Openable>();

    if(p && o) {
        if(!o->isOpen) {
            createCell(world->currentLevel, p->x, p->y, o->openID);
            ecs::deleteEntity(e->id);
            emit(RebuildMapCacheMessage(world->currentLevel, gs.isWizardMode));
            return true;
        }
    }

    return false;
}
