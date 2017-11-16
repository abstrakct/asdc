/*
 * levelfactory.h
 */

#pragma once

#include <memory>

#include "world.h"
#include "datafiles.h"

class LevelFactory {
    public:
        LevelFactory(std::shared_ptr<Level> l);
        void setLevel(std::shared_ptr<Level> l) { level = l; } // TODO: clean/reset stuff

        //void createCell(int x, int y, std::string def);
        void defineCell(int x, int y, std::string def);

        // "Painting" on our "canvas"
        void paintCell(int x, int y, std::string def) { canvas[x][y] = defToCanvas[def]; }
        void paintLine(int x0, int y0, int x1, int y1, std::string def);
        //void paintRectangle(int x1, int y1, int x2, int y2, std::string def);
        void paintRectangle(int x1, int y1, int x2, int y2, std::string frame, std::string fill = "", bool doFrame = true, bool doFill = false);
        void paintRectangleFilled(int x1, int y1, int x2, int y2, std::string def);
        void paintRectangleFilledFramed(int x1, int y1, int x2, int y2, std::string frame, std::string fill);
        void fill(std::string def);
        void fillUnpainted(std::string def);
        void paintPrefab(int sx, int sy, std::string id);

        void build();

        // TODO: child classes for various generators!?
        void generateVillage();
        void generateClassicDungeonAttemptOne();
        void generateDrunkenWalk();

        // Utility functions
        bool canPlacePrefab(int sx, int sy, std::string id);
        bool canPlacePrefab(int sx, int sy, std::string id, std::string accept);

    private:
        void canvasToEntities();
        std::shared_ptr<Level> level;
        std::unordered_map<std::string, int> defToCanvas;
        std::unordered_map<int, std::string> canvasToDef;
        int canvas[256][256] = {};  // TODO: use std::array instead?
};

void createCell(std::shared_ptr<Level> level, int x, int y, std::string def);
