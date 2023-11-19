#pragma once
#ifndef LCODE_SDLBASEGAME_HPP
#define LCODE_SDLBASEGAME_HPP

#include "LTimer.hpp"
#include "LEntity.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <cstddef>

namespace LCode
{

class SDLBaseGame
{
    static bool systems_initialized;
    static SDLBaseGame * current_instance;

protected:
    // SDL dynamic objects
    SDL_Window * window;
    GPU_Target * gpu;
    TTF_Font * font;

    LTimer load_timer,
           fps_timer;
    SDL_Rect window_rect;

    int frames;
    bool running;
    double last_frame_time;
    double delta;
    double avg_fps,
           cur_fps;

private:
    // list of active game entities
    std::vector<LEntity *> entities;

public:
    SDLBaseGame(int screen_width = 640, int screen_height = 480, int font_size = 16);

    // Don't allow copying
    SDLBaseGame(const SDLBaseGame & other) = delete;
    SDLBaseGame & operator = (const SDLBaseGame & other) = delete;

    virtual ~SDLBaseGame();

    virtual int run();

    static SDLBaseGame * get_instance();

    const SDL_Rect & get_window_rect();

    static SDL_FPoint get_random_screen_point();
    static float get_random_screen_x();
    static float get_random_screen_y();

    // Ownership of pointer is transferred to SDLBaseGame
    LEntity * add_entity(LEntity * new_entity);
    LEntity * delete_entity(LEntity * entity_to_remove);
    LEntity * delete_entity(size_t index);

protected:
    virtual void handle_event(SDL_Event & event) = 0;
    virtual void update() = 0;
    virtual void draw() = 0;

    void update_entities();
    void draw_entities();

private:
    void SDL_systems_init();
    void SDL_objects_init(int screen_width = 640, int screen_height = 480, int font_size = 16);

    void system_handle_event(SDL_Event & e);
    void system_update();
    void system_draw_begin();
    void system_draw_end();

    void update_window_rect();

    void quit_SDL_systems();
    void free_SDL_objects();
};

} // namespace LCode


#endif // LCODE_SDLBASEGAME_HPP