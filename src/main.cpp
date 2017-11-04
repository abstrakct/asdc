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


sf::RenderWindow window;

std::shared_ptr<Console> console;
std::shared_ptr<World> world;

long seed;
boost::random::mt19937 rng;

/*
 * Build a cache for a level map, so that we don't need to iterate through a million entities each render loop.
 * Cache is a 2D array inside the Level class that gets allocated in the constructor.
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

    // loop
    bool done = false;
    
    while(!done) {
        sf::Event event;
        //u32 startTime = SDL_GetTicks();
        //i32 timePerFrame = 1000 / FPS_LIMIT;
        //i32 frameStart = 0;

        while(window.pollEvent(event)) {
            //frameStart = SDL_GetTicks();
            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        done = true;
                        break;
                    case sf::Keyboard::Down:
                    case sf::Keyboard::J:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),  0,  1 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::Up:
                    case sf::Keyboard::K:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 0, -1 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::Left:
                    case sf::Keyboard::H:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1,  0 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::Right:
                    case sf::Keyboard::L:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1,  0 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::Y:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1, -1 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::U:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1, -1 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::B:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1,  1 });
                        console->dirty = true;
                        break;
                    case sf::Keyboard::N:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1,  1 });
                        console->dirty = true;
                        break;
                    default:
                        break;
                }

                if(event.type == sf::Event::Closed) {
                    done = true;
                    break;
                }
            }

            on_tick(durationMS);

            // Limit FPS
            //i32 sleepTime = timePerFrame - (SDL_GetTicks() - frameStart);
            //if(sleepTime > 0)
            //    SDL_Delay(sleepTime);

            //durationMS = ((SDL_GetTicks() - frameStart) * 1000) / FPS_LIMIT;

            //window.display();
        }
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
}

int main(int argc, char *argv[])
{
    initSFML();

    // Initialize console
    console = std::make_shared<Console>(SCREEN_WIDTH, SCREEN_HEIGHT);
    console->setFont("res/fonts/terminal16x16.png", 16, 16, 256, 256);
    console->clear();


    // Initialize random number generator
    seed = time(0);
    rng.seed(seed);


    // create player and the world (only one level for now)
    ecs::Entity *player = ecs::createEntity(playerID)->assign(Position(40, 25))->assign(Renderable('@', 0x0055AAFF));
    world = std::make_shared<World>(console->cols, console->rows);
    world->generate();
    buildMapCache(world->level);


    // add and configure systems
    ecs::addSystem<ActorMovementSystem>();
    ecs::addSystem<CameraSystem>();
    ecs::configureAllSystems();

    // RUN!
    run(tick);

    // Destroy everything and exit
    window.close();

    return 0;
}
