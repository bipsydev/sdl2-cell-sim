#pragma once
#ifndef LCODE_SDLBASEGAME_HPP
#define LCODE_SDLBASEGAME_HPP

#include "LTimer.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace LCode
{

class SDLBaseGame
{
    static bool systems_initialized;

protected:
    // SDL dynamic objects
    SDL_Window * window;
    SDL_Renderer * renderer;
    TTF_Font * font;

    LTimer load_timer;

public:
    SDLBaseGame(int screen_width = 640, int screen_height = 480, int font_size = 16);

    // Don't allow copying
    SDLBaseGame(const SDLBaseGame & other) = delete;
    SDLBaseGame & operator = (const SDLBaseGame & other) = delete;

    virtual ~SDLBaseGame();

    virtual int run() = 0;

private:
    void SDL_systems_init();
    void SDL_objects_init(int screen_width = 640, int screen_height = 480, int font_size = 16);

    void quit_SDL_systems();
    void free_SDL_objects();
};

} // namespace LCode


#endif // LCODE_SDLBASEGAME_HPP