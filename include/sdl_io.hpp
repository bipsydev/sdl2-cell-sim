#pragma once
#ifndef LCODE_SDL_IO_HPP
#define LCODE_SDL_IO_HPP

#include <SDL2/SDL.h>

#include <iostream>

inline std::ostream & operator << (std::ostream & os, const SDL_Rect & rect)
{
    return os << "SDL_Rect{ (" << rect.x << ", " << rect.y << "), " << rect.w << "x" << rect.h << " }";
}

#endif // LCODE_SDL_IO_HPP