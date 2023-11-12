#include "SDLBaseGame.hpp"

#include "random.hpp"
#include "LException.hpp"
#include "LTexture.hpp"
#include "sdl_io.hpp"

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
  load_timer{}, fps_timer{},
  window_rect{},
  frames{0}, running{false}, last_frame_time{0}, delta{0},
  avg_fps{0}, cur_fps{0}
{
    load_timer.start();
    seed_rand();
    SDL_systems_init();
    SDL_objects_init(screen_width, screen_height, font_size);
}


int SDLBaseGame::run()
{
    SDL_Event e;         // captures current event from event queue
    frames = 0;          // reset the frame count
    running = true;      // flag to exit from run loop
    last_frame_time = 0; // (ms) var to save previous frame time to calculate delta
    delta = 0;           // the milliseconds since the last frame
    fps_timer.start();   // start the FPS timer
    // ---- MAIN LOOP ----
    while (running)
    {
        while(SDL_PollEvent(&e) != 0
            && running)
        {
            system_handle_event(e);
            handle_event(e);
        }
        if (running)
        {
            system_update();
        }
        if (running)
        {
            update();
        }
        system_draw_begin();
        draw();
        system_draw_end();
        ++frames;
    }
    return EXIT_SUCCESS;
}


void SDLBaseGame::system_handle_event(SDL_Event & e)
{
    if (e.type == SDL_QUIT)
    {
        running = false;
    }
    if (e.type == SDL_WINDOWEVENT)
    {
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
            || e.window.event == SDL_WINDOWEVENT_MOVED)
        {
            update_window_rect();
            std::cout << "window_rect = " << window_rect << "\n";
        }
    }
    
}

void SDLBaseGame::system_update()
{
    // calculate and correct fps with cap
    avg_fps = static_cast<double>(frames)
                    / fps_timer.get_seconds();
    if (avg_fps > 2'000'000)
    {
        avg_fps = 0;
    }
    // update delta time (in ms)
    delta = fps_timer.get_ms() - last_frame_time;
    last_frame_time = fps_timer.get_ms();

    cur_fps = 1000.0 / delta;

}

void SDLBaseGame::system_draw_begin()
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

void SDLBaseGame::system_draw_end()
{
    // Update screen
    // this function waits for the monitor refresh rate
    // when the renderer is given the option SDL_RENDERER_PRESENTVSYNC
    SDL_RenderPresent(renderer);
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
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        throw LException{"Could not create SDL2 Window: "
                         + std::string{SDL_GetError()} + '\n'};
    }
    update_window_rect();

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

void SDLBaseGame::update_window_rect()
{
    SDL_GetWindowPosition(window, &window_rect.x, &window_rect.y);
    SDL_GetWindowSizeInPixels(window, &window_rect.w, &window_rect.h);
}

} // namespace LCode
