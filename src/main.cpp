#include "LTimer.hpp"
#include "LTexture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <iostream>
#include <sstream>
#include <cstdlib>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_FPS = 60;
const double SCREEN_TICKS_PER_FRAME = 1000.0f / SCREEN_FPS;
const int FONT_SIZE = 22;
const SDL_Color TEXT_COLOR{0, 0, 0, 255};
const int TEXT_PADDING = 6;

int main(int argc, char * argv[])
{
    std::cout << "Hello!\n";

    // ---- INITIALIZATION ----

    // Timers
    LCode::LTimer fps_timer, cap_timer, load_timer;
    // Start loading timer
    load_timer.start();

    // SDL dynamic objects
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    TTF_Font * font = nullptr;

    // SDL Init
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Could not initialize SDL2: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }
    // Set linear texture filtering
    if ( ! SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") )
    {
        std::cerr << "Warning: Linear texture filtering not enabled!\n";
    }

    // SDL_image Init
    if ( ! (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) )
    {
        std::cerr << "Could not initialize SDL_image for PNG loading! SDL_image Error: "
                  << IMG_GetError() << '\n';
        return EXIT_FAILURE;
    }
    if (TTF_Init() < 0)
    {
        std::cerr << "Could not initialize SDL_ttf for font loading! SDL_ttf Error: "
                  << TTF_GetError() << '\n';
        return EXIT_FAILURE;
    }


    window = SDL_CreateWindow( "Hello SDL2!",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "Could not create SDL2 Window: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Could not create SDL2 Renderer: " << SDL_GetError() <<  '\n';
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    font = TTF_OpenFont("assets/Sol Schori.ttf", FONT_SIZE);
    if (font == nullptr)
    {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << '\n';
    }

    // Game's dynamic resources
    LCode::LTexture fps_texture{renderer, font}; // use window renderer, previously loaded font
    LCode::LTexture load_time_texture{renderer, font};
    LCode::LTexture press_spacebar_texture{renderer, font};
    press_spacebar_texture.load_text("Press Spacebar!", TEXT_COLOR);

    // game variables
    std::stringstream time_text;
    int frames = 0;
    // circle position
    double x = 0,
           y = SCREEN_HEIGHT / 2.0;
        
    // Circle movement direction
    enum Directions { LEFT, RIGHT } direction = RIGHT;


    load_timer.pause();
    auto load_time = load_timer.get_ticks();

    std::stringstream load_time_text;
    load_time_text << "time to load: " << load_time << "ms";
    load_time_texture.load_text(load_time_text.str(), TEXT_COLOR);

    // ---- SETUP MAIN LOOP ----

    bool quit = false;
    SDL_Event e;

    frames = 0;
    fps_timer.start();
    Uint32 last_frame_time = 0;
    Uint32 delta = 0;
    bool paused = true;
    bool space_pressed = false;
    // ---- MAIN LOOP ----
    while (!quit)
    {
        cap_timer.start();

        // handle event queue
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN && !e.key.repeat)
            {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE)
                {
                    std::cout << "SPACE PRESSED \n";
                    paused = !paused;
                    space_pressed = true;
                }
            }
        }

        // calculate and correct fps with cap
        float avg_fps = frames / (fps_timer.get_ticks() / 1000.0f);
        if (avg_fps > 2'000'000)
        {
            avg_fps = 0;
        }
        // update delta time
        delta = fps_timer.get_ticks() - last_frame_time;
        last_frame_time = fps_timer.get_ticks();

        // update game objects
        if (!paused)
        {
            double step = 120.0f * delta / 1000.0f;
            switch (direction)
            {
            case RIGHT:
                x += step;
                break;
            case LEFT:
                x -= step;
                break;
            }

            if (x > SCREEN_WIDTH)
            {
                direction = LEFT;
            }
            else if (x < 0)
            {
                direction = RIGHT;
            }
        }

        // Update and render text
        time_text.str("");
        time_text << "Average FPS (with cap): " << avg_fps;
        if ( ! fps_texture.load_text(time_text.str(), TEXT_COLOR) )
        {
            std::cerr << "Unable to render FPS Texture!\n";
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        // Render textures
        load_time_texture.render(TEXT_PADDING, TEXT_PADDING);
        fps_texture.render(TEXT_PADDING, TEXT_PADDING * 2 + FONT_SIZE);
        if (!space_pressed)
        {
            press_spacebar_texture.render(TEXT_PADDING,
                                          TEXT_PADDING * 3 + FONT_SIZE * 2);
        }

        // draw a circle
        Sint16 x_i = static_cast<Sint16>(std::round(x));
        Sint16 y_i = static_cast<Sint16>(std::round(y));
        int radius = 16;
        boxColor(renderer, x_i - radius / 2, y_i - radius / 2,
                        x_i + radius / 2, y_i + radius / 2,
                        0x00FFFFFF);
        filledCircleRGBA(renderer,
                         x_i, y_i,
                         radius,
                         0xFF, 0x00, 0x00, 0xFF);
        
        // Update screen
        SDL_RenderPresent(renderer);
        ++frames;

        // if frame finished early...
        int frame_ticks = cap_timer.get_ticks();
        if (frame_ticks < SCREEN_TICKS_PER_FRAME)
        {
            Uint32 delay = static_cast<Uint32>(SCREEN_TICKS_PER_FRAME
                                        - static_cast<double>(frame_ticks));
            //std::cout << "waiting " << delay << "...\n";
            // ... wait the remaining time
            SDL_Delay(delay);
        }

    }

    // ---- DE-INITIALIZATION ----

    fps_texture.free();
    load_time_texture.free();
    press_spacebar_texture.free();

    TTF_CloseFont(font);
    font = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}