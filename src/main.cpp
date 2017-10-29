

#include <SDL2/SDL.h>
#include <stdint.h>

#include <iostream>
#include <memory>

#include "common.h"
#include "entity.h"
#include "console.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

long seed;
boost::random::mt19937 rng;

void render_screen(SDL_Renderer *renderer, SDL_Texture *screen, std::shared_ptr<Console> c)
{
    c->clear();

    SDL_UpdateTexture(screen, NULL, c->getPixels(), SCREEN_WIDTH * sizeof(u32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("asdc",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Texture *screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Initialize console
    std::shared_ptr<Console> console = std::make_shared<Console>(SCREEN_WIDTH, SCREEN_HEIGHT);
    console->setFont("res/fonts/terminal16x16.png", 16, 16);
    console->clear();
    console->put(10, 10, '!');

    // Initialize random number generator
    seed = time(0);
    rng.seed(seed);

    bool done = false;
    while(!done) {
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {
            SDL_Keycode key = event.key.keysym.sym;
            switch(key) {
                case SDLK_ESCAPE:
                    done = true;
                    break;
                default:
                    break;
            }

            if(event.type == SDL_QUIT) {
                done = true;
                break;
            }
            render_screen(renderer, screen, console);
        }

    }

    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
