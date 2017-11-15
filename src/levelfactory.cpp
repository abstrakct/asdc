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
#include <iostream>
#include <exception>

#include "fov.h"
#include "ecs.h"
#include "levelfactory.h"
#include "utils.h"

extern Config c;

class CouldntPaintIt: public std::exception
{
  virtual const char* what() const throw()
  {
    return "LevelFactory: Couldn't paint that thing there!";
  }
} couldNotPlaceDungeonFeature;

/*
 * Create a cell at x,y defined by def.
 * This function does NOT check if there's already a cell at that position!
 * Therefore you should only use this function when building an empty level
 * or when, for other reasons, you know there's no other cell at that spot.
 */
void createCell(std::shared_ptr<Level> level, u32 x, u32 y, std::string def)
{
    TerrainDefinition d = c.terrain[def];
    ecs::Entity *newCell = ecs::createEntity()
        ->assign(Position(x, y))
        ->assign(Renderable(d.glyph, d.fgColor, d.bgColor, d.fadedColor))
        ->assign(Physicality(d.blocksLight, d.blocksMovement, d.visible))
        ->assign(MapCell());

    if(d.openable)
        newCell->assign(Openable(d.isOpen, d.openID, d.closedID));

    level->cells.push_back(newCell);
}


LevelFactory::LevelFactory(std::shared_ptr<Level> l) : level(l)
{
    int canvasID = 1;

    defToCanvas["unpainted"] = 9999;
    canvasToDef[9999] = "unpainted";
    // Assign an ID to each terrain definition, used for painting our canvas.
    for (auto it : c.terrain) {
        defToCanvas[it.first] = canvasID;
        canvasToDef[canvasID] = it.first;
        canvasID++;
    }
}

// Define the cell at x,y
// Can become very slow if there are many entitites.
// Will create a new entity if there is no mapcell at x,y
// If there is a mapcell at x,y already we delete it and create a new one.
void LevelFactory::defineCell(u32 x, u32 y, std::string def)
{
    // TODO: is Position and Mapcell enough, or do we also need Renderable and Physicality? Seems to work fine for now...
    // TODO: can become very slow when we have many entities!
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
        ->assign(Renderable(t.glyph, t.fgColor, t.bgColor, t.fadedColor))
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
 * Fill unpainted cells on the level with cells of definition def.
 */
void LevelFactory::fillUnpainted(std::string def)
{
    for (int x = 0; x < level->width; x++) {
        for (int y = 0; y < level->height; y++) {
            if (canvas[x][y] == defToCanvas["unpainted"])
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

void LevelFactory::paintRectangleFilledFramed(int x1, int y1, int x2, int y2, std::string frame, std::string fill)
{
    for (int y = y1; y <= y2; y++)
        paintLine(x1, y, x2, y, fill);

    paintLine(x1, y1, x2, y1, frame);
    paintLine(x1, y1, x1, y2, frame);
    paintLine(x2, y1, x2, y2, frame);
    paintLine(x1, y2, x2, y2, frame);
}

// TODO: Rotation of prefabs! 
void LevelFactory::paintPrefab(int sx, int sy, std::string id)
{
    int x = 0;
    int y = 0;
    Prefab p = c.prefab[id];
    // if placement wasn't rejected, go ahead and paint it
    if(canPlacePrefab(sx, sy, id)) {
        for (auto it : p.map) {
            for (auto str : it) {
                paintCell(sx + x, sy + y, p.legend[str]);
                x++;
            }
            y++;
            x = 0;
        }
    }
}

bool LevelFactory::canPlacePrefab(int sx, int sy, std::string id)
{
    bool accepted = true;

    for (int cx  = 0; cx < c.prefab[id].width; cx++) {
        for (int cy  = 0; cy < c.prefab[id].height; cy++) {
            // try every cell before painting, and reject/accept placement
            // TODO: Alternatively, add a flag to say when to reject or not
            if (canvas[cx+sx][cy+sy] != defToCanvas["unpainted"])
                accepted = false;
        }
    }

    return accepted;
}

// TODO: support variable number of arguments, to add several acceptable terrain types? or a vector?
bool LevelFactory::canPlacePrefab(int sx, int sy, std::string id, std::string accept)
{
    bool accepted = true;

    for (int cx  = 0; cx < c.prefab[id].width; cx++) {
        for (int cy  = 0; cy < c.prefab[id].height; cy++) {
            // try every cell before painting, and reject/accept placement
            // TODO: Alternatively, add a flag to say when to reject or not
            if (canvas[cx+sx][cy+sy] != defToCanvas[accept])
                accepted = false;
        }
    }

    return accepted;
}

void LevelFactory::build()
{
    generateClassicDungeonAttemptOne();
}

// Generate a village. Very simple semi-working test code for now.
void LevelFactory::generateVillage()
{
    fill("unpainted");
    for (int i = 0; i < 20; i++) {
        paintPrefab(ri(1, 40), ri(1, 20), "test_room");
    }

    fillUnpainted("floor");
    //generateDrunkenWalk();

    paintRectangle(0, 0, level->lastx, level->lasty, "wall");

    canvasToEntities();
}

// Generate the classic dungeon with rooms and corridors.
void LevelFactory::generateClassicDungeonAttemptOne()
{
    fill("wall");
    //int x1, y1, x2, y2;
    //int minx, maxx, miny, maxy;

    // proof of concept
    paintRectangleFilledFramed(5, 5, 10, 10,   "wall", "floor");
    paintRectangleFilledFramed(15, 15, 20, 20, "wall", "floor");
    paintLine(7, 7, 17, 17, "floor");

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

            if(x < level->lastx && y < level->lasty) {
                if(canvas[x][y] == defToCanvas["wall"])
                    paintCell(x, y, "floor");
            }
        }
    }
}

void LevelFactory::canvasToEntities()
{
    for (int x = 0; x < level->width; x++) {
        for (int y = 0; y < level->height; y++) {
            createCell(level, x, y, canvasToDef[canvas[x][y]]);
        }
    }
}
