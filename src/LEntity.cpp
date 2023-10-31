#include "LEntity.hpp"

#include "Game.hpp"

namespace LCode
{

LEntity::LEntity()
: pos{Game::SCREEN_WIDTH / 2, Game::SCREEN_HEIGHT / 2}
{ }

LEntity::LEntity(float x, float y)
: pos{x, y}
{ }

} // namespace LCode
