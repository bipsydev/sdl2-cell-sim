#include "entities/Cell.hpp"
#include "Game.hpp"
#include "random.hpp"
#include "sdl_math.hpp"
#include "lilyutils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>

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
  radius{rand_int<Sint16>(16, 128)},
  width{static_cast<Uint8>(sqrt(radius))},
  speed{rand_float(60.0f, 240.0f)}, draw_box{false},
  life{rand_float(5.0, 20.0)}, life_total{life},
  text_label{}
{
    float angle = rand_float(0.0f, 2.0f * M_PI_F);
    velocity.x = std::cos(angle);
    velocity.y = std::sin(angle);
}

void Cell::update(double delta_ms)
{
    double delta_sec = delta_ms / 1000.0;

    life -= delta_sec;
    if (life <= 1.0 /*TODO: && color != BLACK*/)
    {
        color = BLACK;
    }
    if (life <= 0.0)
    {
        delete_self();
        return;
    }

    float step = speed * static_cast<float>(delta_sec);
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

void Cell::draw(GPU_Target * gpu)
{
    // draw a box!
    if (draw_box)
    {
        GPU_RectangleFilled(gpu, pos.x - radius, pos.y - radius,
                                 pos.x + radius, pos.y + radius,
                            SDL_Color{
                                // use opposite color
                                static_cast<Uint8>(0xFF - color.r),
                                static_cast<Uint8>(0xFF - color.g),
                                static_cast<Uint8>(0xFF - color.b),
                                static_cast<Uint8>(color.a / 4)});
    }
    // draw a circle!
    GPU_CircleFilled(gpu, pos.x, pos.y, radius, color);
    // draw the black outline, `width` pixels wide
    for (float ring = 0; ring < static_cast<float>(width); ring += 0.5f)
    {
        GPU_Circle(gpu, pos.x, pos.y, radius - ring, BLACK);
    }

    // load and render the text label!
    text_label.load_text("HP: " + round_to(life, 1) + " / " + round_to(life_total, 1),
                         life < 1.0? WHITE : BLACK);
    text_label.render(pos.x - static_cast<float>(text_label.get_width())/2.0f, pos.y - static_cast<float>(text_label.get_height())/2.0f);
}

} // namespace LCode
