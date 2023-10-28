#pragma once
#ifndef LCODE_LTEXTURE_HPP
#define LCODE_LTEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

namespace LCode
{

class LTexture
{
    // The texture
    SDL_Texture * texture;
    int width, height;
    std::string file_path;

    // the renderer used to create & render the textures
    SDL_Renderer * renderer;
    // the font used for text rendering
    TTF_Font * font;

public:
    // initialize
    LTexture();
    LTexture(SDL_Renderer * renderer_ref);
    LTexture(SDL_Renderer * renderer_ref, TTF_Font * font_ref);

    // for memory safety
    LTexture(const LTexture & other);
    LTexture & operator = (const LTexture & other);
    void copy(const LTexture & other);

    // deallocates memory
    ~LTexture();


    // sets the renderer used by all LTextures during loading (should be the window renderer)
    void set_renderer(SDL_Renderer * renderer_ref);

    #ifdef SDL_TTF_MAJOR_VERSION
    void set_font(TTF_Font * font_ref);
    #endif

    // loads image at specified path
    bool load(std::string path);

    // loads an image with a font given some text
    #ifdef SDL_TTF_MAJOR_VERSION
    bool load_text(std::string text, SDL_Color color = SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, TTF_Font * font_override = nullptr);
    #endif

    // deallocates texture
    void free();

    // set color modulation
    void set_color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
    void set_color(Uint8 red, Uint8 green, Uint8 blue);
    void set_color(SDL_Color color);
    void set_alpha(Uint8 alpha);

    void set_blend_mode(SDL_BlendMode blending);

    // Renders texture at specified point, and other optional parameters
    void render(int x, int y, SDL_Rect * clip = nullptr, double angle = 0.0,
                SDL_Point * center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void render(SDL_Renderer * renderer_override, int x, int y, SDL_Rect * clip = nullptr, double angle = 0.0,
                SDL_Point * center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

    int get_width();
    int get_height();

};

} // namespace LCode


#endif // LCODE_LTEXTURE_HPP