/*
 * datafiles.h
 */

#pragma once

#include <unordered_map>
#include "common.h"

#define MAIN_CONFIG_FILE "data/config.json"
#define TERRAIN_CONFIG_FILE "data/terrain.json"

struct InterfaceConfig {
    u32 screenWidth, screenHeight;
    bool fullScreen;
};

struct GameConfig {
    // Add various game-related config options here
};

struct TerrainDefinition {
    std::string id;
    unsigned char glyph;
    // TODO: how to translate a color/string to sf::Color?
    bool blocksLight;
    bool blocksMovement;
    bool visible;
};

struct Config {
    InterfaceConfig interface;
    GameConfig game;
    std::unordered_map<std::string, TerrainDefinition> terrain;
};

Config readConfigFiles();
