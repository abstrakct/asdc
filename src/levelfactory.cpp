/*
 * levelfactory.cpp
 *
 * Level Factory basically works like this:
 * We define terrain features (e.g. floor, wall) in terrain.json - that gets read into a Config struct.
 * Then we assign a canvasID to each feature. canvasID is an int.
 * Then we "paint" on a "canvas" (2d array of ints, index into array equals x,y position) with these IDs.
 * So we're just moving numbers back and forth into this array. This is very fast!
 * Then, when we are satisfied with our "painting" (level) on the canvas, we loop through the canvas
 * and create entities for each cell. That way we also avoid checking the entire EntityStore for existing
 * cells/entities whenever we want to add/define a cell.
 *
 */

#include <SFML/Graphics.hpp>
#include "fov.h"
#include "ecs.h"
#include "levelfactory.h"
#include "utils.h"

LevelFactory::LevelFactory(std::shared_ptr<Level> l, Config& conf) : level(l), c(conf)
{
    int canvasID = 1;

    // Assign an ID to each terrain definition, used for painting our canvas.
    for (auto it : c.terrain) {
        defToCanvas[it.first] = canvasID;
        canvasToDef[canvasID] = it.first;
        canvasID++;
    }
}

/*
 * Create a cell at x,y defined by def.
 * This function does NOT check if there's already a cell at that position!
 * Therefore you should only use this function when building an empty level
 * or when, for other reasons, you know there's no other cell at that spot.
 */
void LevelFactory::createCell(u32 x, u32 y, std::string def)
{
    TerrainDefinition d = c.terrain[def];
    level->cells.push_back(ecs::createEntity()
        ->assign(Position(x, y))
        ->assign(Renderable(d.glyph, d.fgColor, d.bgColor))
        ->assign(Physicality(d.blocksLight, d.blocksMovement, d.visible))
        ->assign(MapCell()));
}

// Define the cell at x,y
// Can become very slow if there are many entitites.
// Will create a new entity if there is no mapcell at x,y
// If there is a mapcell at x,y already we delete it and create a new one.
void LevelFactory::defineCell(u32 x, u32 y, std::string def)
{
    // TODO: is Position and Mapcell enough, or do we also need Renderable and Physicality? Seems to work fine for now...
    // TODO: becomes very slow when we have many entities!
    ecs::each_if<Position, MapCell>( [&x, &y] (ecs::Entity &e, Position &pos, MapCell &m) {
            if (pos.x == x && pos.y == y)
                return true;
            else
                return false;
            },
            [] (ecs::Entity &e, Position &pos, MapCell &m) {  // executes if above lambda returns true
                // delete old entity
                deleteEntity(e);
            });

    // create entity from definition
    TerrainDefinition t = c.terrain[def];
    level->cells.push_back(ecs::createEntity()
        ->assign(Position(x, y))
        ->assign(Renderable(t.glyph, t.fgColor, t.bgColor))
        ->assign(Physicality(t.blocksLight, t.blocksMovement, t.visible))
        ->assign(MapCell()));
}

/*
 * Fill the level with cells of definition def.
 */
void LevelFactory::fill(std::string def)
{
    for (int x = 0; x < level->width; x++) {
        for (int y = 0; y < level->height; y++) {
            paintCell(x, y, def);
        }
    }
}

/*
 * "Paint" a line of cells defined by argument def.
 */
void LevelFactory::paintLine(int x0, int y0, int x1, int y1, std::string def)
{
    PointVector points;

    points = getLineCoordinatesBresenham(x0, y0, x1, y1);
    for (auto it : points) {
        paintCell(it.first, it.second, def);
    }
}

void LevelFactory::paintRectangle(int x1, int y1, int x2, int y2, std::string def)
{
    paintLine(x1, y1, x2, y1, def);
    paintLine(x1, y1, x1, y2, def);
    paintLine(x2, y1, x2, y2, def);
    paintLine(x1, y2, x2, y2, def);
}

void LevelFactory::build()
{
    fill("wall");
    generateDrunkenWalk();
    paintRectangle(0, 0, level->lastx, level->lasty, "wall");
    canvasToEntities();
}

/*
 * The "drunken walk algorithm".
 * Makes kinda cool large open caverns.
 */
void LevelFactory::generateDrunkenWalk()
{
    u32 i,j,x,y,q,r,num;

    //q = ri(70, level->lastx);
    //r = ri(50, level->lasty);
    q = level->lastx*3;
    r = level->lasty*2;

    for(i = 2; i < q; ++i) {
        x = level->lastx / 2;
        y = level->lasty / 2;
        for(j = 2; j < r; ++j) {
            num = ri(1,4);
            switch(num) {
                case 1: x++; break;
                case 2: x--; break;
                case 3: y++; break;
                case 4: y--; break;
            }

            if(x < level->lastx && y < level->lasty)
                paintCell(x, y, "floor");
        }
    }
}

void LevelFactory::canvasToEntities()
{
    for (int x = 0; x < level->width; x++) {
        for (int y = 0; y < level->height; y++) {
            createCell(x, y, canvasToDef[canvas[x][y]]);
        }
    }
}
