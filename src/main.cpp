/*
 * main.cpp
 */

// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <stdint.h>
#include <iostream>
#include <memory>

#include "common.h"
#include "ecs.h"
#include "console.h"
#include "components.h"
#include "systems.h"
#include "messages.h"
#include "world.h"

#define FPS_LIMIT      60


std::unique_ptr<GUI> gui;
sf::RenderWindow window;
sf::RenderTexture tex;

std::shared_ptr<World> world;

long seed;
boost::random::mt19937 rng;

// NEXT TODO:
// Build a GUI class that can handle multiple consoles/layers of various sizes.


/*
 * Build a cache for a level map, so that we don't need to iterate through a million entities each render loop.
 * Cache is a 2D array inside the Level class that gets allocated in the constructor.
 * Takes a little while to get done, but only needs to be done once for each level.
 * Might need a more clever way of doing it if we get into more advanced levels or something...
 */
void buildMapCache(std::shared_ptr<Level> level)
{
    for (auto it : level->cells) {
        Position *pos = it->component<Position>();
        Renderable *r = it->component<Renderable>();
        MapCell *cell = it->component<MapCell>();
        if(pos && r) {
            level->cache[pos->x][pos->y].type = cell->type;
            level->cache[pos->x][pos->y].glyph = r->glyph;
            level->cache[pos->x][pos->y].fgColor = r->fgColor;
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

    tex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
}

int main(int argc, char *argv[])
{
    initSFML();

    // Initialize console
    gui = std::make_unique<GUI>(SCREEN_WIDTH, SCREEN_HEIGHT);
    gui->addLayer(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, "res/fonts/terminal16x16.png");
    gui->addLayer(1, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, "res/fonts/terminal16x16.png");
    //console->setFont("res/fonts/terminal16x16.png", 16, 16, 256, 256);


    // Initialize random number generator
    seed = time(0);
    rng.seed(seed);


    // create player and the world (only one level for now)
    ecs::Entity *player = ecs::createEntity(playerID)->assign(Position(10, 10))->assign(Renderable('@', 0x0055AAFF));
    //world = std::make_shared<World>(gui->getLayer(0)->console->widthInChars, gui->getLayer(0)->console->heightInChars);
    world = std::make_shared<World>(40, 40);
    world->generate();
    buildMapCache(world->level);


    // add and configure systems
    ecs::addSystem<PlayerSystem>();
    ecs::addSystem<CameraSystem>();
    ecs::addSystem<ActorMovementSystem>();
    ecs::configureAllSystems();

    // RUN!
    run(tick);

    // Destroy everything and exit
    window.close();

    return 0;
}
