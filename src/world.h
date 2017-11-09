/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"
#include "components.h"

std::vector<ecs::Entity*> findAllEntitiesAtPosition(u32 x, u32 y);
bool positionBlocksMovement(u32 x, u32 y);
ecs::Entity* makeWall(u32 x, u32 y);
ecs::Entity* makeFloor(u32 x, u32 y);

struct MapCacheCell {
    unsigned char glyph;
    sf::Color fgColor;
    bool blocksLight; 
};

class Level {
    private:
        void initCache();
    public:
        Level() {};
        Level(u32 w, u32 h);
        ~Level();

        //void generateFrame();
        void generateFrame(u32 x1, u32 y1, u32 x2, u32 y2);
        void generateHouse(u32 x1, u32 y1, u32 x2, u32 y2);
        void generateVillage();
        void fill(ecs::Entity* (*makerFunction)(u32 x, u32 y));
        void changeIntoWall(u32 x, u32 y);

        u32 width, height;
        std::vector<ecs::Entity *> cells;
        std::array<std::array<MapCacheCell, 256>, 256> cache; // TODO: remove hard coded values!
};

class World {
    private:
    public:
        World() {};

        void generate();
        void addLevel(std::string levelName, u32 w, u32 h);
        void setCurrentLevel(std::string name) {
            if(level.find(name) != level.end())
                currentLevel = level[name];
        }

        std::unordered_map<std::string, std::shared_ptr<Level>> level;
        std::shared_ptr<Level> currentLevel;
};
