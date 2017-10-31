/*
 * main.cpp
 */

#include <SDL2/SDL.h>
#include <stdint.h>

#include <iostream>
#include <memory>

#include "common.h"
#include "ecs.h"
#include "console.h"
#include "components.h"
#include "world.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// global variables for SDL
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screen;
std::shared_ptr<Console> console;
std::shared_ptr<World> world;

long seed;
boost::random::mt19937 rng;
const u64 playerID = 1;     // Entity with ID 1 is always the player.

/*
 * Look at all entities in a certain position,
 * return true if any blocks movement.
 */
bool positionBlocksMovement(u32 x, u32 y)
{
    for (auto it : ecs::findAllEntitiesWithComponent<Position>()) {
        Position *c = it->component<Position>();
        if(c && c->x == x && c->y == y) {
            Physicality *p = it->component<Physicality>();
            if(p->blocksMovement)
                return true;
        }
    }
    return false;
}

struct ActorMovedMessage : ecs::BaseMessage {
    ActorMovedMessage() {}
    ActorMovedMessage(ecs::Entity* a, i32 x, i32 y) : actor(a), dx(x), dy(y) {}
    
    ecs::Entity *actor;
    i32 dx, dy;
};

struct ActorMovementSystem : public ecs::BaseSystem {
    virtual void configure() override {
        systemName = "Actor Movement System";
        subscribe<ActorMovedMessage>([](ActorMovedMessage &msg) {
                u32 newx = msg.actor->component<Position>()->x + msg.dx;
                u32 newy = msg.actor->component<Position>()->y + msg.dy;
                if (!positionBlocksMovement(newx, newy)) {
                    msg.actor->component<Position>()->x = newx;
                    msg.actor->component<Position>()->y = newy;
                }
                });
    }
    virtual void update(const double durationMS) override {
    }
};

void renderScreen(SDL_Renderer *renderer, SDL_Texture *screen, std::shared_ptr<Console> c)
{
    // this entire thing is very slow with many renderable entities! optimize!
    if(c->dirty) {
        c->clear();
        // Find all renderable entities
        std::vector<ecs::Entity *> r = ecs::findAllEntitiesWithComponent<Renderable>();
        for(auto it : r) {
            // Get position and renderable components
            Position *pos = it->component<Position>();
            Renderable *r = it->component<Renderable>();
            if(pos && r) {
                c->put(pos->x, pos->y, r->glyph, r->fgColor);
            } else {
                std::runtime_error("Renderable component has no position (or vice versa)!");
            }
        }

        SDL_UpdateTexture(screen, NULL, c->getPixels(), SCREEN_WIDTH * sizeof(u32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
        c->dirty = false;
    }
}

void sdl_init()
{
    // TODO: SDL error checking
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("asdc",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            0);

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

int main(int argc, char *argv[])
{
    sdl_init();

    // Initialize console
    console = std::make_shared<Console>(SCREEN_WIDTH, SCREEN_HEIGHT);
    console->setFont("res/fonts/terminal16x16.png", 16, 16);
    console->clear();

    // Initialize random number generator
    seed = time(0);
    rng.seed(seed);

    // create some entitites for now
    ecs::Entity *player      = ecs::createEntity(playerID)->assign(Position(25, 20))->assign(Renderable('@', 0x0055AAFF));
    //ecs::Entity *testWall    = makeWall(27, 20);
    //ecs::Entity *testWalltwo = makeWall(37, 25);
    world = std::make_shared<World>(console->rows, console->cols);
    world->generate();


    // add and configure systems
    ecs::addSystem<ActorMovementSystem>();
    ecs::configureAllSystems();


    // loop
    bool done = false;
    while(!done) {
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                switch(key) {
                    case SDLK_ESCAPE:
                        done = true;
                        break;
                    case SDLK_m:
                        //ecs::emit(TestMessage {17, 42});
                        break;
                    case SDLK_DOWN:
                    case SDLK_j:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),  0,  1 });
                        console->dirty = true;
                        break;
                    case SDLK_UP:
                    case SDLK_k:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 0, -1 });
                        console->dirty = true;
                        break;
                    case SDLK_LEFT:
                    case SDLK_h:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1,  0 });
                        console->dirty = true;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_l:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1,  0 });
                        console->dirty = true;
                        break;
                    case SDLK_y:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1, -1 });
                        console->dirty = true;
                        break;
                    case SDLK_u:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1, -1 });
                        console->dirty = true;
                        break;
                    case SDLK_b:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID),-1,  1 });
                        console->dirty = true;
                        break;
                    case SDLK_n:
                        ecs::emit(ActorMovedMessage { ecs::entity(playerID), 1,  1 });
                        console->dirty = true;
                        break;
                    default:
                        break;
                }

                if(event.type == SDL_QUIT) {
                    done = true;
                    break;
                }
            }
            renderScreen(renderer, screen, console);
        }
    }

    // Destroy everything
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
