/*
 * levelfactory.cpp
 */

#include <SFML/Graphics.hpp>
#include "ecs.h"
#include "levelfactory.h"

void LevelFactory::defineCell(u32 x, u32 y, std::string def)
{
    // TODO: is Position and Mapcell enough, or do we also need Renderable and Physicality?
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
        ->assign(Renderable(t.glyph, sf::Color::White))
        ->assign(Physicality(t.blocksLight, t.blocksMovement, t.visible))
        ->assign(MapCell()));
}
