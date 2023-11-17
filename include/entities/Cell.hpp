#pragma once
#ifndef LCODE_CELL_HPP
#define LCODE_CELL_HPP

#include "LEntity.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>

namespace LCode
{

class Cell : public LEntity
{
    SDL_FPoint velocity;

    SDL_Color color;
    bool draw_box;
    Uint8 width;
    Sint16 radius;
    float speed;


public:
    Cell();
    Cell(SDL_FPoint new_pos);
    Cell(float x, float y);

    void update(double delta_ms) override;
    void draw(GPU_Target * gpu) override;
};

} // namespace LCode


#endif // LCODE_CELL_HPP