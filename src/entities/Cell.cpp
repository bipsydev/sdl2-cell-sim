#include "entities/Cell.hpp"
#include "Game.hpp"
#include "random.hpp"
#include "sdl_math.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define _USE_MATH_DEFINES
#include <cmath>

namespace LCode
{

Cell::Cell()
: Cell(Game::get_random_screen_point())
{ }

Cell::Cell(SDL_FPoint new_pos)
: Cell(new_pos.x, new_pos.y)
{ }

Cell::Cell(float x, float y)
: LEntity(x, y),
  velocity{0.0f, 0.0f},
  color{rand_int<Uint8>(0x00, 0xFF), rand_int<Uint8>(0x00, 0xFF),
        rand_int<Uint8>(0x00, 0xFF), rand_int<Uint8>(0x88, 0xFF)},
  draw_box{false}, width{8},
  radius{rand_int<Sint16>(16, 128)},
  speed{rand_float(60.0f, 240.0f)}
{
    float angle = rand_float(0.0f, 2.0f * M_PI_F);
    velocity.x = std::cos(angle);
    velocity.y = std::sin(angle);
}

void Cell::update(double delta_ms)
{
    float step = speed * static_cast<float>(delta_ms) / 1000.0f;
    const Uint8 * keystate = SDL_GetKeyboardState(nullptr);
    if (keystate[SDL_SCANCODE_LSHIFT])
    {
        step *= 2;
    }
    pos.x += velocity.x * step;
    pos.y += velocity.y * step;

    SDL_Rect window_rect = Game::get_instance()->get_window_rect();
    // check X position
    if (pos.x + radius > static_cast<float>(window_rect.w))
    {
        velocity = reflect(velocity, WEST);
        pos.x = static_cast<float>(window_rect.w) - radius;
    }
    else if (pos.x - radius < 0)
    {
        velocity = reflect(velocity, EAST);
        pos.x = radius;
    }
    // check Y position
    if (pos.y + radius > static_cast<float>(window_rect.h))
    {
        velocity = reflect(velocity, NORTH);
        pos.y = static_cast<float>(window_rect.h) - radius;
    }
    else if (pos.y - radius < 0)
    {
        velocity = reflect(velocity, SOUTH);
        pos.y = radius;
    }
}

void Cell::draw(SDL_Renderer * renderer)
{
    Sint16 x_i = static_cast<Sint16>(std::round(pos.x));
    Sint16 y_i = static_cast<Sint16>(std::round(pos.y));
    if (draw_box)
    {
        boxRGBA(renderer, x_i - radius, y_i - radius,
                          x_i + radius, y_i + radius,
                          // use opposite color
                          0xFF - color.r, 0xFF - color.g,
                          0xFF - color.b, color.a / 4);
    }
    filledCircleRGBA(renderer,
                     x_i, y_i,
                     radius,
                     color.r, color.g, color.b, color.a);
    for (Uint8 ring = 0; ring < width; ++ring)
    {
        circleRGBA(renderer,
                   x_i, y_i,
                   radius - ring,
                   0x00, 0x00, 0x00, 0xFF);
    }
}

} // namespace LCode
