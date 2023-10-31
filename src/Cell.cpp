#include "entities/Cell.hpp"

#include "Game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

namespace LCode
{

Cell::Cell(float x, float y)
: LEntity(x, y),
  space_pressed{false},
  direction{RIGHT}
{ }

void Cell::press_space()
{
    space_pressed = true;
}

bool Cell::was_space_pressed()
{
    return space_pressed;
}

void Cell::update(Uint32 delta_ms)
{
    float step = 120.0f * static_cast<float>(delta_ms) / 1000.0f;
    switch (direction)
    {
    case RIGHT:
        pos.x += step;
        break;
    case LEFT:
        pos.x -= step;
        break;
    }

    if (pos.x > Game::SCREEN_WIDTH)
    {
        direction = LEFT;
    }
    else if (pos.x < 0)
    {
        direction = RIGHT;
    }
}

void Cell::draw(SDL_Renderer * renderer)
{
    Sint16 x_i = static_cast<Sint16>(std::round(pos.x));
    Sint16 y_i = static_cast<Sint16>(std::round(pos.y));
    Sint16 radius = 16;
    boxColor(renderer, x_i - radius / 2, y_i - radius / 2,
                       x_i + radius / 2, y_i + radius / 2,
                       0x00FFFFFF);
    filledCircleRGBA(renderer,
                     x_i, y_i,
                     radius,
                     0xFF, 0x00, 0x00, 0xFF);
}

} // namespace LCode
