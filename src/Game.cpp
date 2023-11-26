#include "Game.hpp"

#include "LException.hpp"
#include "entities/Cell.hpp"
#include "random.hpp"
#include "lilyutils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace LCode
{

// constructor / initialization
Game::Game()
: SDLBaseGame(SCREEN_WIDTH, SCREEN_HEIGHT, FONT_SIZE),
  fps_avg_texture{}, fps_cur_texture{}, load_time_texture{},
  press_spacebar_texture{}, press_a_texture{},
  paused{true},
  space_pressed{false},
  time_text_avg{}, time_text_cur{}
{
    game_objects_init();

    load_timer.pause();
    double load_time_ms = load_timer.get_ms();

    std::stringstream load_time_text;
    load_time_text << "time to load: " << round_to(load_time_ms, 0) << " ms";
    load_time_texture.load_text(load_time_text.str(), TEXT_COLOR);
}

void Game::game_objects_init()
{
    press_spacebar_texture.load_text("Spacebar: pause/unpause", TEXT_COLOR);
    press_a_texture.load_text("A: Add a cell", TEXT_COLOR);

    // add game entities to SDLBaseGame entity handler
    add_entity(new Cell{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
}

void Game::handle_event(SDL_Event & e)
{
    // handle event from event queue
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
                (!e.key.repeat || keystate[SDL_SCANCODE_LSHIFT]))
            {
                std::cout << "Adding 10 cells...\n";
                for (Uint8 i = 0; i < 10; ++i)
                {
                    add_entity(new Cell);
                }
            }
            else if (!e.key.repeat || keystate[SDL_SCANCODE_LSHIFT])
            {
                std::cout << "Adding a cell...\n";
                add_entity(new Cell);
            }
            break;
        }
        default:
            break;
        }
    }
}

void Game::update()
{
    // Update text
    time_text_avg.str("");
    time_text_avg << "Average FPS: " << round_to(avg_fps, 2);

    time_text_cur.str("");
    time_text_cur << "Current FPS: " << round_to(cur_fps, 1);

    // update game entities only if unpaused
    if (!paused)
    {
        update_entities();
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

    // draw all game entities
    draw_entities();

    // Draw text textures
    load_time_texture.render(TEXT_PADDING, TEXT_PADDING);
    fps_avg_texture.render(TEXT_PADDING, TEXT_PADDING * 2 + FONT_SIZE);
    fps_cur_texture.render(TEXT_PADDING, TEXT_PADDING * 3 + FONT_SIZE * 2);
    press_a_texture.render(TEXT_PADDING, TEXT_PADDING * 4 + FONT_SIZE * 3);
    if (!space_pressed)
    {
        float screen_width = static_cast<float>(get_window_rect().w);
        float screen_height = static_cast<float>(get_window_rect().h);
        float width = static_cast<float>(press_spacebar_texture.get_width());
        float height = static_cast<float>(press_spacebar_texture.get_height());
        press_spacebar_texture.render(screen_width / 2.0f - width / 2.0f,
                                      screen_height / 3.0f - height / 2.0f);
    }
    
}

} // namespace LCode
