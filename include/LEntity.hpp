#pragma once
#ifndef LCODE_LENTITY_HPP
#define LCODE_LENTITY_HPP

#include <SDL2/SDL.h>

namespace LCode
{
    
class LEntity
{
protected:
    // 2D Point using floats
    SDL_FPoint pos;

public:
    LEntity();
    LEntity(float x, float y);

    virtual ~LEntity() = default;
    virtual void update(Uint32 delta_ms) = 0;
    virtual void draw(SDL_Renderer * renderer) = 0;

};

} // namespace LCode


#endif // LCODE_LENTITY_HPP