#include "LEntity.hpp"

#include "SDLBaseGame.hpp"

namespace LCode
{

LEntity::LEntity()
: pos{static_cast<float>(SDLBaseGame::get_instance()->get_window_rect().w) / 2.0f,
      static_cast<float>(SDLBaseGame::get_instance()->get_window_rect().h) / 2.0f}
{ }

LEntity::LEntity(SDL_FPoint new_pos)
: pos{new_pos}
{ }

LEntity::LEntity(float x, float y)
: pos{x, y}
{ }

void LEntity::delete_self()
{
    SDLBaseGame::get_instance()->delete_entity(this);
}

} // namespace LCode
