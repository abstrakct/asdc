/*
 * main.cpp
 */

// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <stdint.h>
#include <iostream>
#include <memory>
#include <random>

#include "common.h"
#include "ecs.h"
#include "console.h"
#include "components.h"
#include "systems.h"
#include "messages.h"
#include "world.h"
#include "input.h"
#include "utils.h"

#define FPS_LIMIT      60

sf::RenderWindow window;
sf::RenderTexture tex;

std::unique_ptr<GUI> gui;
std::unique_ptr<World> world;
std::shared_ptr<Console> mapConsole;         // pass as parameter instead of global variable? TODO

u64 seed;
std::mt19937 rng;

// NEXT TODO:
// Build a GUI class that can handle multiple consoles/layers of various sizes.

// IDEA: shared components!?! For components that can be the same for many entities, like the Physical component of walls. 
// But will it be unnecessarily complicated? Maybe/probably... 

// IDEA: sub-consoles! useful? necessary? cool? check cogmind dev.
//
// TODO: look into regex! also, string formatting.
/*
 * Build a cache for a level map, so that we don't need to iterate through a million entities each render loop.
 * Cache is a 2D array inside the Level class that gets allocated in the constructor.
 * Takes a little while to get done, but only needs to be done once for each level.
 * Might need a more clever way of doing it if we get into more advanced levels or something...
 *
 * Also, (TODO) moving around on a large map is really slow, even with this cache and only drawing within FOV. Why????
 * - OK, it now is a bit faster, it seems, with reasonably large maps (e.g. 60x40)
 *   Bumping height to 45 seems to have a big impact though... conclusion: it's probably good enough for now! But larger maps will need optimization / different handling of things.
 *   FINAL NOTE: building with optimization -O3 pretty much eliminates the problem even on large maps! What did we learn? Let the compiler do its magic! 
 */
void buildMapCache(std::shared_ptr<Level> level)
{
    MapCacheCell tmp;
    for (auto it : level->cells) {
        Position *pos = it->component<Position>();
        Renderable *r = it->component<Renderable>();
        MapCell *cell = it->component<MapCell>();
        Physicality *p= it->component<Physicality>();
        if(pos && r && cell) {
            tmp.type = cell->type;
            tmp.glyph = r->glyph;
            tmp.fgColor = r->fgColor;
            tmp.blocksLight = p->blocksLight;
            level->cache[pos->x][pos->y] = tmp;
        }
    }
}

// Tick is called every frame. The parameter specifies how many ms have elapsed
// since the last time it was called.
void tick(double duration_ms) {
    ecs::tick(duration_ms);
}

void run(std::function<void(double)> on_tick)
{
    double durationMS = 0.0;
    bool done = false;


    while(window.isOpen() && !done) {
        clock_t startTime = clock();
        sf::Event event;

        while(window.pollEvent(event)) {    // waitEvent bedre??
            if(event.type == sf::Event::Closed)
                done = true;
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                done = true;
            if(event.type == sf::Event::KeyPressed)
                ecs::emit(KeyPressed{event});
            if(event.type == sf::Event::MouseMoved) {
                setMousePosition(event.mouseMove.x, event.mouseMove.y);
                // quick and dirty hack for now:
                layer(mapLayer)->console->dirty = true;
            }
        }

        //window.clear();
        
        on_tick(durationMS);

        window.display();

        durationMS = ((clock() - startTime) * 1000.0) / CLOCKS_PER_SEC;
    }
}

void initSFML()
{
    window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "asdc");
    sf::Vector2i windowPosition;
    // TODO: remove hard coded values, set position to center of screen
    windowPosition.x = 300;
    windowPosition.y = 200;
    window.setPosition(windowPosition);
    window.setVerticalSyncEnabled(true);

    //tex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
}

int main(int argc, char *argv[])
{
    initSFML();

    // Initialize console
    gui = std::make_unique<GUI>(SCREEN_WIDTH, SCREEN_HEIGHT);
    gui->addLayer(rootLayer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, "res/fonts/Cheepicus_16x16.png", 16, 16);
    gui->addLayer(mapLayer,  16, 16, 800, 480, "res/fonts/terminal16x16.png", 16, 16);

    mapConsole = layer(mapLayer)->console;
    //layer(rootLayer)->console->put(65, 20, '*', 0xFF0000FF);
    layer(rootLayer)->addStaticText(0, 63, 21, "A S D C !", 0xff0000ff);
    layer(rootLayer)->addStaticText(1, 63, 22, "greentext", 0x00ff00ff);
    // TODO: Define handles for gui components

    // Initialize random number generator.
    // Seed is the current time.
    seed = time(0);
    rng.seed(seed);

    // create player and the world (only one level for now)
    ecs::createEntity(playerID)
        ->assign(Position(5, 5))
        ->assign(Renderable('@', 0x0055AAFF))
        ->assign(Vision(10)); // TODO: not hard-code this and other things...
    
    world = std::make_unique<World>();
    world->addLevel("Dungeon Level 1", 50, 30);
    world->setCurrentLevel("Dungeon Level 1");
    world->generate();
    buildMapCache(world->currentLevel);

    // add and configure systems
    ecs::addSystem<PlayerSystem>();
    ecs::addSystem<VisibilitySystem>();
    ecs::addSystem<CameraSystem>();
    ecs::addSystem<ActorMovementSystem>();
    ecs::configureAllSystems();

    // RUN!
    run(tick);

    // Destroy everything and exit
    window.close();

    return 0;
}
