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
    static const int SCREEN_WIDTH;
    static const int SCREEN_HEIGHT;
    static const int SCREEN_FPS;
    static const double SCREEN_TICKS_PER_FRAME;
    static const int FONT_SIZE;
    static const SDL_Color TEXT_COLOR;
    static const int TEXT_PADDING;

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