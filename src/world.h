/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"
#include "components.h"

std::vector<ecs::Entity*> findAllEntitiesAtPosition(u32 x, u32 y);
bool cellBlocksMovement(int x, int y);
ecs::Entity* cellIsOpenable(u32 x, u32 y);
bool cellOpen(ecs::Entity *e);


struct MapCacheCell {
    unsigned char glyph;
    sf::Color fgColor;
    sf::Color bgColor;
    sf::Color fadedColor;
    bool blocksLight; 
    bool blocksMovement;
    bool seen = false;
};

class Level {
    private:
        void initCache();
    public:
        Level() {};
        Level(u32 w, u32 h);
        ~Level();

        int width, height;
        u32 lastx, lasty;
        std::vector<ecs::Entity *> cells;
        std::array<std::array<MapCacheCell, 256>, 256> cache; // TODO: remove hard coded values!
};

class World {
    private:
    public:
        World() {};

        void addLevel(std::string levelName, u32 w, u32 h);
        void setCurrentLevel(std::string name) {
            if(level.find(name) != level.end())
                currentLevel = level[name];
        }
        std::pair<int, int> currentLevelGetOpenPosition();                // return coordinates to an open/walkable position on current level

        std::unordered_map<std::string, std::shared_ptr<Level>> level;
        std::shared_ptr<Level> currentLevel;
};
