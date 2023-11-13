#pragma once
#ifndef LCODE_SDL_MATH_HPP
#define LCODE_SDL_MATH_HPP

#include <SDL2/SDL_rect.h>

#define _USE_MATH_DEFINES
#include <cmath>

inline const SDL_FPoint EAST = {std::cos(0.0f), std::sin(0.0f)},
                        NORTH = {std::cos(M_PI / 2.0f), std::sin(M_PI / 2.0f)},
                        WEST = {std::cos(M_PI), std::sin(M_PI)},
                        SOUTH = {std::cos(3.0f * M_PI / 2.0f), std::sin(3.0f * M_PI / 2.0f)};

inline SDL_FPoint operator * (float a, SDL_FPoint vec)
{
    return SDL_FPoint{vec.x * a, vec.y * a};
}

inline SDL_FPoint operator - (SDL_FPoint vec1, SDL_FPoint vec2)
{
    return SDL_FPoint{vec1.x - vec2.x, vec1.y - vec2.y};
}

inline float dot_prod(SDL_FPoint vec1, SDL_FPoint vec2)
{
    return vec1.x*vec2.x + vec1.y*vec2.y;
}

inline SDL_FPoint reflect(SDL_FPoint vec, SDL_FPoint surf_norm)
{
    return vec - 2.0f * dot_prod(vec, surf_norm) * surf_norm;
}


#endif // LCODE_SDL_MATH_HPP