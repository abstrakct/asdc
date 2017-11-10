/*
 * datafiles.cpp
 */

#include <json/json.h>
#include <iostream>
#include <fstream>
#include <string>

#include "datafiles.h"

// TODO: JSON might not be the best choice for doing things like predefined dungeon/map parts - investigate!
// (CDDA uses JSON all over the place, doesn't it?)

void readMainConfigFile(Config& c)
{
    std::ifstream ifs(MAIN_CONFIG_FILE);
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(ifs, root)) {
        std::cout << "Error: " << reader.getFormattedErrorMessages() << std::endl;
        return; // TODO: return something useful when failing!
    }

    Json::Value& interfaceOptions = root["interface_options"];

    c.interface.screenHeight = interfaceOptions[0].get("screen_height", 1280).asInt();
    c.interface.screenWidth  = interfaceOptions[0].get("screen_width", 720).asInt();
    c.interface.fullScreen   = interfaceOptions[0].get("fullscreen", false).asBool();
}

void readTerrainConfigFile(Config& c)
{
    std::ifstream ifs(TERRAIN_CONFIG_FILE);
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(ifs, root)) {
        std::cout << "Error: " << reader.getFormattedErrorMessages() << std::endl;
        return; // TODO: return something useful when failing!
    }

    Json::Value& terrainDefinitions = root["terrain_definitions"];

    for (auto it : terrainDefinitions) {
        std::string id = it.get("id", "ERROR").asString();
        c.terrain[id].id = id;
        c.terrain[id].glyph = *(it.get("glyph", "?").asCString());
        c.terrain[id].fgColor = sf::Color(std::stoul(it.get("fg_color", "0xFFFFFFFF").asString(), nullptr, 16));   // convert string with hexadecimal color codes to sf::Color in one swoop!
        c.terrain[id].bgColor = sf::Color(std::stoul(it.get("bg_color", "0x00000000").asString(), nullptr, 16));
        c.terrain[id].blocksLight = it.get("blocks_light", false).asBool();
        c.terrain[id].blocksMovement = it.get("blocks_movement", false).asBool();
        c.terrain[id].visible = it.get("visible", false).asBool();
    }
}

Config readConfigFiles()
{
    Config c;

    readMainConfigFile(c);
    readTerrainConfigFile(c);

    return c;
}
