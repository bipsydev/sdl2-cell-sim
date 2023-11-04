#include "entities/Cell.hpp"
#include "Game.hpp"
#include "random.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

namespace LCode
{

Cell::Cell(float x, float y)
: LEntity(x, y),
  direction{rand_int(0, 1)? LEFT : RIGHT},
  color{rand_int<Uint8>(0x00, 0xFF), rand_int<Uint8>(0x00, 0xFF),
        rand_int<Uint8>(0x00, 0xFF), rand_int<Uint8>(0x88, 0xFF)},
  radius{rand_int<Sint16>(16, 128)},
  speed{rand_float(60.0f, 240.0f)}
{ }

void Cell::update(double delta_ms)
{
    float step = speed * static_cast<float>(delta_ms) / 1000.0f;
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
    boxRGBA(renderer, x_i - radius, y_i - radius,
                      x_i + radius, y_i + radius,
                      // use opposite color
                      0xFF - color.r, 0xFF - color.g,
                      0xFF - color.b, color.a / 2);
    filledCircleRGBA(renderer,
                     x_i, y_i,
                     radius,
                     color.r, color.g, color.b, color.a);
}

} // namespace LCode
