#pragma once
#ifndef LCODE_GAME_HPP
#define LCODE_GAME_HPP

#include "LTimer.hpp"
#include "LTexture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <iostream>
#include <sstream>
#include <cstdlib>


namespace LCode
{

class Game // : public SDLGame //TODO common SDL2 Game functionality in common base class
{
    // inline initialization of static variables
    static inline const int SCREEN_WIDTH = 1280;
    static inline const int SCREEN_HEIGHT = 720;
    static inline const int SCREEN_FPS = 60;
    static inline const double SCREEN_TICKS_PER_FRAME = 1000.0f / SCREEN_FPS;
    static inline const int FONT_SIZE = 22;
    static inline const SDL_Color TEXT_COLOR{0, 0, 0, 255};
    static inline const int TEXT_PADDING = 6;

    // SDL dynamic objects
    SDL_Window * window;
    SDL_Renderer * renderer;
    TTF_Font * font;

    // Timers
    LTimer fps_timer, cap_timer, load_timer;

    // Textures
    LTexture fps_texture; // use window renderer, previously loaded font
    LTexture load_time_texture;
    LTexture press_spacebar_texture;

    // Game Variables
    int frames;
    std::stringstream time_text;
    // circle position
    double x, y;
    bool paused;
    bool space_pressed;

    enum Directions { LEFT, RIGHT } direction;

public:
    Game();

    // for memory safety
    Game(const Game & other);
    Game & operator = (const Game & other);
    void copy(const Game & other);

    ~Game();
    int run();

private:
    void SDL_systems_init();
    void SDL_objects_init();
    void game_objects_init();

    void quit_SDL_systems();
    void free_SDL_objects();
    void free_game_objects();
};

} // namespace LCode


#endif // LCODE_GAME_HPP