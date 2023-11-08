#include "SDLBaseGame.hpp"

#include "random.hpp"
#include "LException.hpp"
#include "LTexture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <iostream>

namespace LCode
{

bool SDLBaseGame::systems_initialized = false;


SDLBaseGame::SDLBaseGame(int screen_width, int screen_height, int font_size)
: window{nullptr},
  renderer{nullptr},
  font{nullptr},
  load_timer{}
{
    load_timer.start();
    seed_rand();
    SDL_systems_init();
    SDL_objects_init(screen_width, screen_height, font_size);
}


SDLBaseGame::~SDLBaseGame()
{
    free_SDL_objects();
    quit_SDL_systems();
}


void SDLBaseGame::SDL_systems_init()
{
    if (!systems_initialized)
    {
        // SDL Init
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw LException{"Could not initialize SDL2: "
                            + std::string{SDL_GetError()} + '\n'};
        }

        // Set linear texture filtering
        if ( ! SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") )
        {
            std::cerr << "Warning: Linear texture filtering not enabled!\n";
        }

        // SDL_image Init
        if ( ! (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) )
        {
            throw LException{"Could not initialize SDL_image for PNG loading! SDL_image Error: "
                    + std::string{IMG_GetError()} + '\n'};
        }
        // SDL_ttf Init
        if (TTF_Init() < 0)
        {
            throw LException{"Could not initialize SDL_ttf for font loading! SDL_ttf Error: "
                    + std::string{TTF_GetError()} + '\n'};
        }

        systems_initialized = true;
    }
}


void SDLBaseGame::SDL_objects_init(int screen_width, int screen_height,
                                   int font_size)
{
    window = SDL_CreateWindow( "Hello SDL2!",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               screen_width, screen_height,
                               SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        throw LException{"Could not create SDL2 Window: "
                         + std::string{SDL_GetError()} + '\n'};
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
                                            | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        throw LException{"Could not create SDL2 Renderer: "
                         + std::string{SDL_GetError()} + '\n'};
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    font = TTF_OpenFont("assets/Sol Schori.ttf", font_size);
    if (font == nullptr)
    {
        throw LException{"Failed to load font! SDL_ttf Error: "
                         + std::string{TTF_GetError()} + '\n'};
    }

    LTexture::set_fallback_renderer(renderer);
    LTexture::set_fallback_font(font);
}


void SDLBaseGame::free_SDL_objects()
{
    TTF_CloseFont(font);
    font = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
}

void SDLBaseGame::quit_SDL_systems()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


} // namespace LCode
