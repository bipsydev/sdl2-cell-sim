#pragma once
#ifndef LCODE_CELL_HPP
#define LCODE_CELL_HPP

#include "LEntity.hpp"

#include <SDL2/SDL.h>

namespace LCode
{

class Cell : public LEntity
{
    bool space_pressed;
    enum Directions { LEFT, RIGHT } direction;

public:
    Cell(float x, float y);

    void press_space();
    bool was_space_pressed();

    void update(Uint32 delta_ms) override;
    void draw(SDL_Renderer * renderer) override;
};

} // namespace LCode


#endif // LCODE_CELL_HPP