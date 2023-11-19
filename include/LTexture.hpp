#pragma once
#ifndef LCODE_LTEXTURE_HPP
#define LCODE_LTEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>
#include <SDL2/SDL_ttf.h>
#include <string>

namespace LCode
{

class LTexture
{
    // The texture
    //SDL_Texture * texture;
    GPU_Image * image;
    SDL_Color color;
    int width, height;
    std::string file_path;

    // the renderer used to create & render the textures
    GPU_Target * gpu;
    // static fallback is used if renderer is nullptr on instance
    static GPU_Target * fallback_gpu;

    #ifdef SDL_TTF_MAJOR_VERSION
    // the font used for text rendering
    TTF_Font * font;
    static TTF_Font * fallback_font;
    #endif

public:
    // initialize
    LTexture();
    LTexture(GPU_Target * gpu_ref);
    LTexture(GPU_Target * gpu_ref, TTF_Font * font_ref);

    // for memory safety
    LTexture(const LTexture & other);
    LTexture & operator = (const LTexture & other);
    void copy(const LTexture & other);

    // deallocates memory
    ~LTexture();


    // sets the renderer used by all LTextures during loading (should be the window renderer)
    void set_gpu(GPU_Target * renderer_ref);
    static void set_fallback_gpu(GPU_Target * renderer_ref);

    #ifdef SDL_TTF_MAJOR_VERSION
    void set_font(TTF_Font * font_ref);
    static void set_fallback_font(TTF_Font * font_ref);
    #endif

    // loads image at specified path
    bool load(std::string path);

    // loads an image with a font given some text
    #ifdef SDL_TTF_MAJOR_VERSION
    bool load_text(std::string text, SDL_Color text_color = SDL_Color{0x00, 0x00, 0x00, 0xFF}, TTF_Font * font_override = nullptr);
    #endif

    // deallocates texture
    void free();

    // set color modulation
    void set_color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
    void set_color(Uint8 red, Uint8 green, Uint8 blue);
    void set_color(SDL_Color color);
    void set_alpha(Uint8 alpha);

    void set_blend_mode(GPU_BlendPresetEnum blend_mode);

    // Renders texture at specified point, and other optional parameters
    void render(float x, float y, GPU_Rect * clip = nullptr, float angle = 0.0,
                SDL_Point * center = nullptr, GPU_FlipEnum flip = GPU_FLIP_NONE);

    void render(GPU_Target * gpu_override, float x, float y, GPU_Rect * clip = nullptr, float angle = 0.0,
                SDL_Point * center = nullptr, GPU_FlipEnum flip = GPU_FLIP_NONE);

    int get_width();
    int get_height();

private:

    GPU_Target * get_gpu(GPU_Target * gpu_override = nullptr);

    #ifdef SDL_TTF_MAJOR_VERSION
    TTF_Font * get_font(TTF_Font * font_override = nullptr);
    #endif

};

} // namespace LCode


#endif // LCODE_LTEXTURE_HPP