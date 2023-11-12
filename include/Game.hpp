#pragma once
#ifndef LCODE_GAME_HPP
#define LCODE_GAME_HPP

#include <SDLBaseGame.hpp>
#include "LTexture.hpp"
#include "LEntity.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <sstream>
#include <vector>


namespace LCode
{

class Game : public SDLBaseGame
{
    // Textures
    LTexture fps_avg_texture,
             fps_cur_texture,
             load_time_texture,
             press_spacebar_texture,
             press_a_texture;

    // Game Variables
    bool paused;
    bool space_pressed;
    std::stringstream time_text_avg;
    std::stringstream time_text_cur;
    // list of active game entities
    std::vector<LEntity *> entities;

public:
    // inline initialization of static variables
    static inline const int SCREEN_WIDTH = 1280;
    static inline const int SCREEN_HEIGHT = 720;
    static inline const int SCREEN_FPS = 60;
    static inline const double SCREEN_TICKS_PER_FRAME = 1000.0f / SCREEN_FPS;
    static inline const int FONT_SIZE = 22;
    static inline const SDL_Color TEXT_COLOR{0, 0, 0, 255};
    static inline const int TEXT_PADDING = 6;

    static SDL_FPoint get_random_screen_point();
    static float get_random_screen_x();
    static float get_random_screen_y();

    Game();

    ~Game();

private:
    void game_objects_init();

    void handle_event(SDL_Event & e) override;
    void update() override;
    void draw() override;

    void free_game_objects();
};

} // namespace LCode


#endif // LCODE_GAME_HPP