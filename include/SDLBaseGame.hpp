/**
 * @file    SDLBaseGame.hpp
 * @author  Lily-Heather Crawford @bipsydev
 * 
 * @brief   SDLBaseGame class - An abstract base class used to represent a
 *          generic SDL2 Game. Simply define a subclass that inherits from
 *          SDLBaseGame and override the functions handle_events, update,
 *          and draw, then let SDLBaseGame handle the logic!
 * 
 * @version 0.1
 * @date    2023-11-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

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
/******************************************************************************
 *                         STATIC CLASS VARIABLES                             *
 ******************************************************************************/
private:

    // true when SDL systems have been initialized to prevent a second initialization
    static inline bool systems_initialized = false;

    // Pointer to the current instance (only one should be running at any time)
    static inline SDLBaseGame * current_instance = nullptr;

/******************************************************************************
 *                          STATIC CLASS METHODS                              *
 ******************************************************************************/
public:

    /**
     * @brief Retrieve a pointer to the currently running instance
     *        of `SDLBaseGame`. Only one should be running at any time!
     * 
     * @return `SDLBaseGame *`
     */
    static SDLBaseGame * get_instance();

    /**
     * @brief Retrieves a random floating-point position within the
     *        screen area bounds.
     * 
     * @return a random `SDL_FPoint` in the bounds of
     *         ([0, window_rect.w], [0, window_rect.h])
     */
    static SDL_FPoint get_random_screen_point();

    /**
     * @brief Retrieves a random floating-point x-coordinate within the
     *        screen area bounds.
     */
    static float get_random_screen_x();

    /**
     * @brief Retrieves a random floating-point y-coordinate within the
     *        screen area bounds.
     */
    static float get_random_screen_y();

    /**
     * @brief Flags the current instance to stop the run loop.
     */
    static void exit_game();


/******************************************************************************
 *                           INSTANCE VARIABLES                               *
 ******************************************************************************/
private:
    // list of active game entities
    std::vector<LEntity *> entities;

protected:
    // -------- SDL dynamically allocated objects --------
    // SDL Window object, keeps track of native window on system.
    SDL_Window * window;
    // The main SDL rendering target object created from window.
    GPU_Target * gpu;
    // The default font to use as a fallback in `LTexture` text rendering.
    TTF_Font * font;

    // -------- Game objects --------
    // Times how long the initialization loading takes.
    // Derived class should stop the timer when their loading is finished.
    LTimer load_timer;
    // Used to calculate the current average FPS over time.
    LTimer fps_timer;
    // The position and size of the current window.
    SDL_Rect window_rect;

    // The number of frames that have been rendered thus far.
    int frames;
    // This flag controls the main run loop, set to false to exit.
    bool running;
    // The time of the last frame since initialization, used to calculate delta.
    double last_frame_time;
    // The time since the last frame.
    double delta;
    // The average FPS since initialization.
    double avg_fps;
    // The current FPS calculated by delta.
    double cur_fps;

/******************************************************************************
 *                        PUBLIC INSTANCE METHODS                             *
 ******************************************************************************/
public:
    /**
     * @brief Construct a new SDLBaseGame object, initializating SDL subsystems
     *        and common dynamic objects for every game.
     * 
     * @param screen_width  The desired screen width for the window.
     * @param screen_height The desired screen height for the window.
     * @param font_size     The desired font size for the default font.
     */
    SDLBaseGame(int screen_width = 640, int screen_height = 480, int font_size = 16);

    // Don't allow copying
    SDLBaseGame(const SDLBaseGame & other) = delete;
    SDLBaseGame & operator = (const SDLBaseGame & other) = delete;

    /**
     * @brief Destructor, calls `free()`.
     */
    virtual ~SDLBaseGame();

    /**
     * @brief Deallocates SDL dynamic objects and deinitializes SDL subsystems.
     *        Also deallocates game objects within `entities`
     *        and empties the vector. Resets `current_instance` to nullptr.
     */
    void free();

    /**
     * @brief The main run loop! This will handle events, updating, and drawing
     *        every frame in the base system and the subclass virtual methods
     *        until `running` is false.
     * 
     * @return `int` Program return code, return this in `main()`.
     */
    virtual int run();

    /**
     * @brief Sets `running` to false to exit the main game `run()` loop.
     */
    void exit();

    /**
     * @return `const SDL_Rect &` that represents the position
     *         and size of the game window.
     */
    const SDL_Rect & get_window_rect();

    /**
     * @brief Add a pointer to a newly allocated `LEntity` object,
     *        transferring ownership of the pointer to `SDLBaseGame`
     *        which will eventually deallocate them when `delete_entity`
     *        is called or during the destructor/`free()`.
     * 
     * @param new_entity Pointer to a newly allocated `LEntity` object.
     * @return `LEntity *` The pointer passed in. 
     */
    LEntity * add_entity(LEntity * new_entity);

    /**
     * @brief Searches the `entities` vector for elements that match the
     *        given pointer, then deallocates and removes them from the vector.
     * 
     * @param entity_to_remove The `LEntity` pointer to delete and remove.
     * @return `LEntity *` The pointer passed in.
     */
    LEntity * delete_entity(LEntity * entity_to_remove);

    /**
     * @brief Deallocates the `LEntity` at the given index and then removes
     *        its pointer from the `entities` vector.
     * 
     * @param index The index to remove from `entities`.
     * @return `LEntity *` The pointer removed from that index. 
     */
    LEntity * delete_entity(size_t index);


/******************************************************************************
 *                       PROTECTED INSTANCE METHODS                           *
 ******************************************************************************/
protected:

    // -------- VIRTUAL SUBCLASS RUN LOOP METHODS --------

    /**
     * @brief The subclass's event handler.
     * 
     * @param event The SDL_Event to process.
     */
    virtual void handle_event(SDL_Event & event) = 0;

    /**
     * @brief The subclass's logic updater.
     */
    virtual void update() = 0;

    /**
     * @brief The subclass's drawing/rendering method.
     */
    virtual void draw() = 0;


    // -------- ENTITY CONTROL METHODS --------

    /**
     * @brief Calls `update()` on every `LEntity` within the `entities` vector.
     */
    void update_entities();

    /**
     * @brief Calls `draw()` on every `LEntity` within the `entities` vector.
     */
    void draw_entities();

/******************************************************************************
 *          PRIVATE INSTANCE METHODS (Internal implementation use)            *
 ******************************************************************************/
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