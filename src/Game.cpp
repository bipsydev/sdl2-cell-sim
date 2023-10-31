#include "Game.hpp"

#include "LException.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace LCode
{

// constructor / initialization
Game::Game()
: window{nullptr},
  renderer{nullptr},
  font{nullptr},
  fps_timer{}, cap_timer{}, load_timer{},
  fps_texture{}, load_time_texture{}, press_spacebar_texture{},
  frames{0},
  time_text{},
  x{0}, y{SCREEN_HEIGHT / 2.0},
  paused{true}, space_pressed{false},
  direction{RIGHT}
{
    load_timer.start();

    SDL_systems_init();
    SDL_objects_init();
    game_objects_init();

    load_timer.pause();
    Uint32 load_time = load_timer.get_ticks();

    std::stringstream load_time_text;
    load_time_text << "time to load: " << load_time << "ms";
    load_time_texture.load_text(load_time_text.str(), TEXT_COLOR);
}

Game::Game(const Game & other)
: Game()
{
    copy(other);
}

Game & Game::operator = (const Game & other)
{
    if (this != &other)
    {
        free_game_objects();
        game_objects_init();
        copy(other);
    }
    return *this;
}

void Game::copy(const Game & other)
{
    frames = other.frames;
    x = other.x;
    y = other.y;
    paused = other.paused;
    space_pressed = other.space_pressed;
    direction = other.direction;
}


void Game::SDL_systems_init()
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
}


void Game::SDL_objects_init()
{
    window = SDL_CreateWindow( "Hello SDL2!",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        throw LException{"Could not create SDL2 Window: "
                         + std::string{SDL_GetError()} + '\n'};
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        throw LException{"Could not create SDL2 Renderer: "
                         + std::string{SDL_GetError()} + '\n'};
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    font = TTF_OpenFont("assets/Sol Schori.ttf", FONT_SIZE);
    if (font == nullptr)
    {
        throw LException{"Failed to load font! SDL_ttf Error: "
                         + std::string{TTF_GetError()} + '\n'};
    }
}


void Game::game_objects_init()
{
    // Game's dynamic resources
    fps_texture.set_renderer(renderer);
    fps_texture.set_font(font);

    load_time_texture.set_renderer(renderer);
    load_time_texture.set_font(font);

    press_spacebar_texture.set_renderer(renderer);
    press_spacebar_texture.set_font(font);
    press_spacebar_texture.load_text("Press Spacebar!", TEXT_COLOR);

}


int Game::run()
{
    SDL_Event e;        // captures current event from event queue
    frames = 0;         // reset the frame count
    bool running = true;        // flag to exit from run loop
    Uint32 last_frame_time = 0; // var to save previous frame time to calculate delta
    Uint32 delta = 0;           // the milliseconds since the last frame
    fps_timer.start();  // start the FPS timer
    // ---- MAIN LOOP ----
    while (running)
    {
        cap_timer.start();

        // handle event queue
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
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
        float avg_fps = static_cast<float>(frames)
                      / (static_cast<float>(fps_timer.get_ticks()) / 1000.0f);
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
            double step = 120.0f * static_cast<float>(delta) / 1000.0f;
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
        Sint16 radius = 16;
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

    return EXIT_SUCCESS;
}

Game::~Game()
{
    free_game_objects();
    free_SDL_objects();
    quit_SDL_systems();
}

void Game::free_game_objects()
{
    fps_texture.free();
    load_time_texture.free();
    press_spacebar_texture.free();
}

void Game::free_SDL_objects()
{
    TTF_CloseFont(font);
    font = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
}

void Game::quit_SDL_systems()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

} // namespace LCode
