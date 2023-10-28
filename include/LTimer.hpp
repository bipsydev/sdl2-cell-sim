#pragma once
#ifndef LCODE_LTIMER_HPP
#define LCODE_LTIMER_HPP

#include <SDL2/SDL_types.h>

namespace LCode
{

class LTimer
{
    Uint32 start_ticks,
           paused_ticks;
    bool paused,
         started;
public:
    LTimer();

    void start();
    void stop();
    void pause();
    void unpause();

    Uint32 get_ticks();
    
    bool is_started();
    bool is_paused();
};

} // namespace LCode

#endif // LCODE_LTIMER_HPP