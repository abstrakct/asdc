/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"
#include "components.h"

std::vector<ecs::Entity*> findAllEntitiesAtPosition(int x, int y);
bool cellBlocksMovement(int x, int y);
ecs::Entity* cellIsOpenable(int x, int y);
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
    public:
        Level() {};
        Level(int w, int h);
        ~Level();

        std::string name;
        int width, height;
        int lastx, lasty;
        std::vector<ecs::Entity *> cells;
        std::array<std::array<MapCacheCell, 256>, 256> cache; // TODO: remove hard coded values!
};

class World {
    private:
    public:
        World() {};

        void addLevel(std::string levelName, int w, int h);
        void setCurrentLevel(std::string name) {
            if(level.find(name) != level.end())
                currentLevel = level[name];
        }
        std::pair<int, int> currentLevelGetOpenPosition();                // return coordinates to an open/walkable position on current level

        std::unordered_map<std::string, std::shared_ptr<Level>> level;
        std::shared_ptr<Level> currentLevel;
};
