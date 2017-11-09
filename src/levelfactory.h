/*
 * levelfactory.h
 */

#pragma once

#include <memory>

#include "world.h"
#include "datafiles.h"
// IDEA: build the level/map in an array, then when complete make entities from the array ?!

class LevelFactory {
    public:
        LevelFactory(std::shared_ptr<Level> l, Config& conf) : level(l), c(conf) {}
        void defineCell(u32 x, u32 y, std::string def);
    private:
        std::shared_ptr<Level> level;
        Config& c;
};

