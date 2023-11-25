#include "LTimer.hpp"

#include <SDL2/SDL.h>

namespace LCode
{

LTimer::LTimer()
: start_ticks{0}, paused_ticks{0},
  paused{false}, started{false}
{ }

void LTimer::start()
{
    started = true;
    paused = false;

    start_ticks = SDL_GetPerformanceCounter();
    paused_ticks = 0;
}

void LTimer::stop()
{
    started = false;
    paused = false;

    start_ticks = 0;
    paused_ticks = 0;
}

void LTimer::pause()
{
    if (started && ! paused)
    {
        paused = true;

        paused_ticks = SDL_GetPerformanceCounter() - start_ticks;
        start_ticks = 0;
    }
}

void LTimer::unpause()
{
    if (started && paused)
    {
        paused = false;

        start_ticks = SDL_GetPerformanceCounter() - paused_ticks;
        paused_ticks = 0;
    }
}

Uint64 LTimer::get_ticks()
{
    Uint64 time = 0;

    if (started)
    {
        if (paused)
        {
            time = paused_ticks;
        }
        else
        {
            time = SDL_GetPerformanceCounter() - start_ticks;
        }
    }

    return time;
}

double LTimer::get_seconds()
{
    // divide by how many ticks in a second (SDL_GetPerformanceFrequency)
    return static_cast<double>(get_ticks())
           / static_cast<double>(SDL_GetPerformanceFrequency());
}

double LTimer::get_milliseconds()
{
    return get_seconds() * 1000.0;
}

bool LTimer::is_started()
{
    return started;
}

bool LTimer::is_paused()
{
    return paused && started;
}

} // namespace LCode
