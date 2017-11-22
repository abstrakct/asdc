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
#include "datafiles.h"
#include "levelfactory.h"
#include "game.h"

#define FPS_LIMIT      60

sf::RenderWindow window;
sf::RenderTexture tex;

std::unique_ptr<GUI> gui;
std::unique_ptr<World> world;
std::shared_ptr<Console> mapConsole;         // pass as parameter instead of global variable? TODO

std::unordered_map<GameStateType, ecs::BaseMessage> stateMessageType;

u64 seed;
std::mt19937 rng;

GameState gs;
Config c;
u64 playerID;

//////////// VARIOUS NOTES AND IDEAS AND THOUGHTS
// NEXT TODO:
// Build a GUI class that can handle multiple consoles/layers of various sizes.
// TODO mostly done!
// TODO: UI screens/layers must be able to receive input! For handling various things, like an inventory screen.
// or maybe we will have multiple game states, and input will be redirected based on what state we are in!
// So, if state is "show inventory" then all input will be delivered as a "inventoryInputEvent" message!!
// That seems like a good idea!

// IDEA: shared components!?! For components that can be the same for many entities, like the Physical component of walls. 
// But will it be unnecessarily complicated? Maybe/probably... 

// IDEA: sub-consoles! useful? necessary? cool? check cogmind dev.
//
// TODO: look into regex! also, string formatting.
/*
 * BELOW COMMENT IS MORE OR LESS DEPRECATED/DONE:
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
//////////// END VARIOUS NOTES AND IDEAS AND THOUGHTS

// TODO 2017-11-16
// Look for C++17 features to use! like initializers in if/switch


// For debugging only:
void wizardMode()
{
    ecs::entity(playerID)->component<Vision>()->fovRadius = 50;
    layer(LHandle::root)->addStaticText(1, 62, 22, "Wizard Mode", 0x00ff00ff);
    ecs::emit(MapRerenderMessage{});
    ecs::emit(BuildMapCacheMessage(world->currentLevel, true));
    ecs::emit(PlayerMovedMessage{});
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

    while (window.isOpen() && !done) {
        clock_t startTime = clock();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                done = true;

            // game state / input redirection is fairly hardcoded for now. TODO: rewrite in a more dynamic way!
            if (gs.state == gsInGame) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                    done = true;
                else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                    if (!gs.isWizardMode) {
                        wizardMode();
                        gs.isWizardMode = true;
                    }
                } else if (event.type == sf::Event::KeyPressed)
                    ecs::emit(KeyPressed{event});

                if (event.type == sf::Event::MouseMoved) {
                    setMousePosition(event.mouseMove.x, event.mouseMove.y);
                    ecs::emit(MapRerenderMessage{});
                }
            }
        }

        //window.clear();
        
        on_tick(durationMS);  // EDIT: runs ecs::tick  ------- runs tick(double) - see above

        window.display();

        durationMS = ((clock() - startTime) * 1000.0) / CLOCKS_PER_SEC;
    }
}

void initSFML()
{
    // Create the main window
    window.create(sf::VideoMode(c.interface.screenWidth, c.interface.screenHeight), "asdc");
    
    // TODO: remove hard coded values, set position to center of screen
    sf::Vector2i windowPosition;
    windowPosition.x = 300;
    windowPosition.y = 200;
    window.setPosition(windowPosition);
    window.setVerticalSyncEnabled(true);
}

void initGUI()
{
    // Initialize console
    // TODO: make dynamic based on config/datafiles!
    gui = std::make_unique<GUI>(c.interface.screenWidth, c.interface.screenHeight);
    gui->addLayer(LHandle::root, 0, 0, c.interface.screenWidth, c.interface.screenHeight, "res/fonts/Cheepicus_16x16.png", 16, 16);

    int mapLayerWidth = 800;
    int mapLayerHeight = 480;
    gui->addLayer(LHandle::map,  16, 16, mapLayerWidth, mapLayerHeight, "res/fonts/terminal16x16.png", 16, 16);
    gui->addLayer(LHandle::msg,  16, mapLayerHeight+16, c.interface.screenWidth, c.interface.screenHeight - mapLayerHeight, "res/fonts/terminal16x16.png", 16, 16);
    gui->addLayer(LHandle::info, mapLayerWidth+16,  16, c.interface.screenWidth - mapLayerWidth, c.interface.screenHeight, "res/fonts/terminal16x16.png", 16, 16);
    gui->addLayer(LHandle::dialog, 0, 0, c.interface.screenWidth, c.interface.screenHeight, "res/fonts/terminal16x16.png", 16, 16);

    mapConsole = layer(LHandle::map)->console;
    layer(LHandle::root)->addStaticText(0, 63, 21, "A S D C !", 0xff0000ff);
    layer(LHandle::msg)->addStaticText(1, 1, 1, "messages", 0x00ff00ff);
    layer(LHandle::info)->addStaticText(2, 1, 1, "info", 0x00ff00ff);
    
    // TODO: Define handles for gui components
}

void initPlayer()
{ 
    // TODO: don't hardcode these values

    ecs::Entity *e = ecs::createEntity()
        ->assign(Being())
        ->assign(Controllable())
        ->assign(Position(world->currentLevelGetOpenPosition()))
        ->assign(Renderable('@', sf::Color(0x36425EFF), sf::Color(0x00000000), sf::Color::Black))
        ->assign(Vision(9));

    playerID = e->id;
}

int main(int argc, char *argv[])
{
    c = readConfigFiles();

    initSFML();

    initGUI();

    // Initialize random number generator. Seed is the current time.
    seed = time(0);
    rng.seed(seed);

    // create world (only one level for now)
    world = std::make_unique<World>();
    world->addLevel("Dungeon Level 1", 100, 50);
    world->setCurrentLevel("Dungeon Level 1");

    LevelFactory lf(world->currentLevel);
    lf.build();

    // add and configure systems
    ecs::addSystem<MapCacheSystem>();           // for (re)building the map cache
    ecs::addSystem<PlayerSystem>();             // for handling player movement. expand in the future.
    ecs::addSystem<FOVSystem>();                // for keeping the FOV map up to date
    ecs::addSystem<RenderSystem>();             // for drawing the map on screen
    //ecs::addSystem<ActorMovementSystem>();    // for general movement of actors/beings. not really used yet.
    ecs::configureAllSystems();

    ecs::emit(BuildMapCacheMessage(world->currentLevel, false));

    initPlayer();

    // RUN!
    gs.isRunning = true;
    gs.state = gsInGame;
    run(ecs::tick);

    // Destroy everything and exit
    window.close();

    return 0;
}

// vim: fdm=syntax
