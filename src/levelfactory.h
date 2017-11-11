/*
 * levelfactory.h
 */

#pragma once

#include <memory>

#include "world.h"
#include "datafiles.h"

class LevelFactory {
    public:
        LevelFactory(std::shared_ptr<Level> l, Config& conf);
        void setLevel(std::shared_ptr<Level> l) { level = l; }

        void createCell(u32 x, u32 y, std::string def);
        void defineCell(u32 x, u32 y, std::string def);

        // "Painting" on our "canvas"
        void paintCell(u32 x, u32 y, std::string def) { canvas[x][y] = defToCanvas[def]; }
        void paintLine(int x0, int y0, int x1, int y1, std::string def);
        void paintRectangle(int x1, int y1, int x2, int y2, std::string def);
        void fill(std::string def);
        void paintPrefab(int sx, int sy, std::string id);

        void build();

        // TODO: child class for various generators!
        void generateDrunkenWalk();

    private:
        void canvasToEntities();
        std::shared_ptr<Level> level;
        Config& c;
        std::unordered_map<std::string, int> defToCanvas;
        std::unordered_map<int, std::string> canvasToDef;
        int canvas[256][256] = {};  // TODO: use std::array instead?
};

