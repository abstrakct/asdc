/*
 * world.h
 */

#pragma once

#include <memory>
#include "ecs.h"

ecs::Entity* makeWall(u32 x, u32 y);

class Level {
    private:
    public:
        Level() {};
        Level(u32 w, u32 h) : width(w), height(h) {};
        void generateFrame();

        u32 width, height;
        std::vector<ecs::Entity *> cells;
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
