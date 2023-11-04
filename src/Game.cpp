#include "Game.hpp"

#include "LException.hpp"
#include "entities/Cell.hpp"
#include "random.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace LCode
{

// ---- STATIC functions ----

SDL_FPoint Game::get_random_screen_point()
{
    return SDL_FPoint{get_random_screen_x(),
                      get_random_screen_y()};
}

float Game::get_random_screen_x()
{ return rand_float<float>(0, SCREEN_WIDTH); }

float Game::get_random_screen_y()
{ return rand_float<float>(0, SCREEN_HEIGHT); }



// constructor / initialization
Game::Game()
: window{nullptr},
  renderer{nullptr},
  font{nullptr},
  fps_timer{}, load_timer{},
  fps_avg_texture{}, fps_cur_texture{}, load_time_texture{},
  press_spacebar_texture{}, press_a_texture{},
  frames{0}, running{false}, last_frame_time{0}, delta{0},
  paused{true},
  space_pressed{false},
  time_text_avg{}, time_text_cur{},
  entities{}
{
    load_timer.start();

    seed_rand();

    SDL_systems_init();
    SDL_objects_init();
    game_objects_init();

    load_timer.pause();
    double load_time_ms = load_timer.get_ms();

    std::stringstream load_time_text;
    load_time_text << "time to load: " << load_time_ms << " ms";
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
    entities = other.entities; // copy over LEntity elements
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
                                            | SDL_RENDERER_PRESENTVSYNC);
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
    fps_avg_texture.set_renderer(renderer);
    fps_avg_texture.set_font(font);
    fps_cur_texture.set_renderer(renderer);
    fps_cur_texture.set_font(font);

    load_time_texture.set_renderer(renderer);
    load_time_texture.set_font(font);

    press_spacebar_texture.set_renderer(renderer);
    press_spacebar_texture.set_font(font);
    press_spacebar_texture.load_text("Spacebar: pause/unpause", TEXT_COLOR);

    press_a_texture.set_renderer(renderer);
    press_a_texture.set_font(font);
    press_a_texture.load_text("A: Add a cell", TEXT_COLOR);

    // game objects
    entities.push_back(new Cell{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
}


int Game::run()
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
        handle_events(e);
        update();
        draw();
        ++frames;
    }
    return EXIT_SUCCESS;
}

void Game::handle_events(SDL_Event & e)
{
    // handle event queue
    while(SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }
        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.scancode)
            {
            case SDL_SCANCODE_SPACE:
            {
                if (!e.key.repeat)
                {
                    std::cout << "SPACE PRESSED \n";
                    paused = !paused;
                    space_pressed = true;
                }
                break;
            }
            case SDL_SCANCODE_A:
            {
                const Uint8 * keystate = SDL_GetKeyboardState(nullptr);
                if (keystate[SDL_SCANCODE_LCTRL] &&
                    !e.key.repeat || keystate[SDL_SCANCODE_LSHIFT])
                {
                    std::cout << "Adding 10 cells...\n";
                    for (Uint8 i = 0; i < 10; ++i)
                    {
                        entities.push_back(new Cell);
                    }
                }
                else if (!e.key.repeat || keystate[SDL_SCANCODE_LSHIFT])
                {
                    std::cout << "Adding a cell...\n";
                    entities.push_back(new Cell);
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

void Game::update()
{
    // calculate and correct fps with cap
    double avg_fps = static_cast<double>(frames)
                    / fps_timer.get_seconds();
    if (avg_fps > 2'000'000)
    {
        avg_fps = 0;
    }
    // update delta time (in ms)
    delta = fps_timer.get_ms() - last_frame_time;
    last_frame_time = fps_timer.get_ms();

    double cur_fps = 1000.0 / delta;

    // Update text
    time_text_avg.str("");
    time_text_avg << "Average FPS: " << avg_fps;

    time_text_cur.str("");
    time_text_cur << "Current FPS: " << cur_fps;

    // update game objects
    if (!paused)
    {
        for (LEntity * entity : entities)
        {
            entity->update(delta);
        }
    }
}

void Game::draw()
{
    // Render string text to texture
    if ( ! fps_avg_texture.load_text(time_text_avg.str(), TEXT_COLOR) )
    {
        std::cerr << "Unable to render FPS Texture!\n";
    }

    if ( ! fps_cur_texture.load_text(time_text_cur.str(), TEXT_COLOR) )
    {
        std::cerr << "Unable to render FPS Texture!\n";
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // draw all entities
    for (LEntity * entity : entities)
    {
        entity->draw(renderer);
    }

    // Draw text textures
    load_time_texture.render(TEXT_PADDING, TEXT_PADDING);
    fps_avg_texture.render(TEXT_PADDING, TEXT_PADDING * 2 + FONT_SIZE);
    fps_cur_texture.render(TEXT_PADDING, TEXT_PADDING * 3 + FONT_SIZE * 2);
    press_a_texture.render(TEXT_PADDING,
                            TEXT_PADDING * 4 + FONT_SIZE * 3);
    if (!space_pressed)
    {
        press_spacebar_texture.render(TEXT_PADDING,
                                        TEXT_PADDING * 5 + FONT_SIZE * 4);
    }
    
    // Update screen
    // this function waits for the monitor refresh rate
    // when the renderer is given the option SDL_RENDERER_PRESENTVSYNC
    SDL_RenderPresent(renderer);
}

Game::~Game()
{
    free_game_objects();
    free_SDL_objects();
    quit_SDL_systems();
}

void Game::free_game_objects()
{
    // free game textures
    fps_avg_texture.free();
    fps_cur_texture.free();
    load_time_texture.free();
    press_spacebar_texture.free();

    // free game objects
    for (size_t i = 0; i < entities.size(); ++i)
    {
        delete entities[i];
    }
    entities.clear();
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
