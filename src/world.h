/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"
#include "components.h"

bool positionBlocksMovement(u32 x, u32 y);
ecs::Entity* makeWall(u32 x, u32 y);
ecs::Entity* makeFloor(u32 x, u32 y);

struct MapCacheCell {
    MapCellType type;
    unsigned char glyph;
    u32 fgColor;
};

class Level {
    private:
        void initCache();
    public:
        Level() {};
        Level(u32 w, u32 h);
        ~Level();
        void generateFrame();
        void fill(ecs::Entity* (*makerFunction)(u32 x, u32 y));

        u32 width, height;
        std::vector<ecs::Entity *> cells;
        MapCacheCell **cache; // TODO: is vector faster? probably not, but... maybe?
};

class World {
    private:
    public:
        World() {};
        World(u32 w, u32 h) {
            level = std::make_shared<Level>(w, h);
        };

        void generate();
        std::shared_ptr<Level> level;
        std::shared_ptr<Level> currentLevel;
};
