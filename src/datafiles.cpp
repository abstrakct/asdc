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

// TODO: error checking/handling in JSON parsing

void readMainConfigFile(Config& c)
{
    std::ifstream ifs(MAIN_CONFIG_FILE, std::ifstream::binary);
    Json::Value root;
    std::string error;

    ifs >> root;

    /*if (!reader->parse(ifs, root)) {
        std::cout << "Error: " << reader->getFormattedErrorMessages() << std::endl;
        return; // TODO: return something useful when failing!
    }*/

    Json::Value& interfaceOptions = root["interface_options"];

    c.interface.screenHeight = interfaceOptions[0].get("screen_height", 1280).asInt();
    c.interface.screenWidth  = interfaceOptions[0].get("screen_width", 720).asInt();
    c.interface.fullScreen   = interfaceOptions[0].get("fullscreen", false).asBool();
}

void readTerrainConfigFile(Config& c)
{
    std::ifstream ifs(TERRAIN_CONFIG_FILE);
    Json::Value root;

    ifs >> root;

    /*if (!reader.parse(ifs, root)) {
        std::cout << "Error: " << reader.getFormattedErrorMessages() << std::endl;
        return; // TODO: return something useful when failing!
    }
    */

    Json::Value& terrainDefinitions = root["terrain_definitions"];

    for (auto it : terrainDefinitions) {
        std::string id = it.get("id", "ERROR").asString();
        c.terrain[id].id = id;
        c.terrain[id].glyph = *(it.get("glyph", "?").asCString());
        c.terrain[id].fgColor = sf::Color(std::stoul(it.get("fg_color", "0xFFFFFFFF").asString(), nullptr, 16));   // convert string with hexadecimal color codes to sf::Color in one swoop!
        c.terrain[id].bgColor = sf::Color(std::stoul(it.get("bg_color", "0x00000000").asString(), nullptr, 16));
        c.terrain[id].fadedColor = sf::Color(std::stoul(it.get("faded_color", "0x00000000").asString(), nullptr, 16));
        c.terrain[id].blocksLight = it.get("blocks_light", false).asBool();
        c.terrain[id].blocksMovement = it.get("blocks_movement", false).asBool();
        c.terrain[id].visible = it.get("visible", false).asBool();
        c.terrain[id].openable = it.get("openable", false).asBool();
        c.terrain[id].isOpen = it.get("is_open", false).asBool();
        c.terrain[id].openID = it.get("open_id", "ERROR").asString();
        c.terrain[id].closedID = it.get("closed_id", "ERROR").asString();
    }
}

// TODO/IDEA: allow prefabs to define their own terrain types?!
void readPrefabConfigFile(Config& c)
{
    std::ifstream ifs(PREFAB_CONFIG_FILE);
    Json::Value root;

    ifs >> root;

    //if (!reader.parse(ifs, root)) {
        //std::cout << "Error: " << reader.getFormattedErrorMessages() << std::endl;
        //return; // TODO: return something useful when failing!
    //}

    Json::Value& prefab = root["prefab"];
    
    for (auto it : prefab) {
        // TODO: Check that there's an "id" field, it is required!
        std::string id = it.get("id", "ERROR").asString();
        c.prefab[id].id = id;
        c.prefab[id].width  = it.get("width",  0).asInt();
        c.prefab[id].height = it.get("height", 0).asInt();

        Json::Value thelegend = it.get("legend", "ERROR");
        for (u32 i = 0; i < thelegend.size(); i+=2) {
            c.prefab[id].legend[*(thelegend[i].asCString())] = thelegend[i+1].asString();
        }
        Json::Value themap = it.get("map", "ERROR");
        for (u32 i = 0; i < themap.size(); i++) {
            c.prefab[id].map.push_back(themap[i].asString());
        }
    }
}

Config readConfigFiles()
{
    Config c;

    readMainConfigFile(c);
    readTerrainConfigFile(c);
    readPrefabConfigFile(c);

    return c;
}
