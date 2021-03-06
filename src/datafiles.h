/*
 * datafiles.h
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "common.h"

#define MAIN_CONFIG_FILE "data/config.json"
#define TERRAIN_CONFIG_FILE "data/terrain.json"
#define PREFAB_CONFIG_FILE "data/prefab.json"

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
    sf::Color fgColor;
    sf::Color bgColor;
    sf::Color fadedColor;
    bool blocksLight;
    bool blocksMovement;
    bool visible;
    bool openable;          // can be opened/closed
    bool isOpen;
    std::string openID, closedID;
};

struct Prefab {
    std::string id;
    int width, height;
    std::unordered_map<char, std::string> legend;
    std::vector<std::string> map;
};

struct Config {
    InterfaceConfig interface;
    GameConfig game;
    std::unordered_map<std::string, TerrainDefinition> terrain;
    std::unordered_map<std::string, Prefab> prefab;
};

Config readConfigFiles();
