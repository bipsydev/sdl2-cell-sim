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

    start_ticks = SDL_GetTicks();
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

        paused_ticks = SDL_GetTicks() - start_ticks;
        start_ticks = 0;
    }
}

void LTimer::unpause()
{
    if (started && paused)
    {
        paused = false;

        start_ticks = SDL_GetTicks() - paused_ticks;
        paused_ticks = 0;
    }
}

Uint32 LTimer::get_ticks()
{
    Uint32 time = 0;

    if (started)
    {
        if (paused)
        {
            time = paused_ticks;
        }
        else
        {
            time = SDL_GetTicks() - start_ticks;
        }
    }

    return time;
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
