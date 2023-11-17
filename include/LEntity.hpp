#pragma once
#ifndef LCODE_LENTITY_HPP
#define LCODE_LENTITY_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>

namespace LCode
{
    
class LEntity
{
protected:
    // 2D Point using floats
    SDL_FPoint pos;

public:
    LEntity();
    LEntity(SDL_FPoint new_pos);
    LEntity(float x, float y);

    virtual ~LEntity() = default;
    virtual void update(double delta_ms) = 0;
    virtual void draw(GPU_Target * gpu) = 0;

};

} // namespace LCode


#endif // LCODE_LENTITY_HPP