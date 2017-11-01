/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"
#include "components.h"

ecs::Entity* makeWall(u32 x, u32 y);

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

        u32 width, height;
        std::vector<ecs::Entity *> cells;
        MapCacheCell **cache;
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
};
