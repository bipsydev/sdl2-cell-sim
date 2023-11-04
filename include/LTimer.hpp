#pragma once
#ifndef LCODE_LTIMER_HPP
#define LCODE_LTIMER_HPP

#include <SDL2/SDL_types.h>

namespace LCode
{

class LTimer
{
    Uint64 start_ticks,
           paused_ticks;
    bool paused,
         started;
public:
    LTimer();

    void start();
    void stop();
    void pause();
    void unpause();

    Uint64 get_ticks();

    double get_seconds();
    double get_sec() { return get_seconds(); }
    double get_s() { return get_seconds(); }

    double get_milliseconds();
    double get_ms() { return get_milliseconds(); }
    
    bool is_started();
    bool is_paused();
};

} // namespace LCode

#endif // LCODE_LTIMER_HPP