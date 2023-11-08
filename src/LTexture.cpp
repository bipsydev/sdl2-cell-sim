#include "LTexture.hpp"
#include "LException.hpp"

#include <SDL2/SDL_image.h>

#include <string>
#include <iostream>

namespace LCode
{
// ---- Static initializers ----
SDL_Renderer * LTexture::fallback_renderer = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
TTF_Font * LTexture::fallback_font = nullptr;
#endif


// constructors
LTexture::LTexture()
: LTexture(nullptr)
{ }

LTexture::LTexture(SDL_Renderer * renderer_ref)
: LTexture(renderer_ref, nullptr)
{ }

LTexture::LTexture(SDL_Renderer * renderer_ref, TTF_Font * font_ref)
: texture{nullptr},
  width{0}, height{0},
  file_path{""},
  renderer{renderer_ref},
  font{font_ref}
{ }


LTexture::LTexture(const LTexture & other)
: LTexture()
{
    if (other.texture != nullptr)
    {
        copy(other);
    }
}

LTexture & LTexture::operator = (const LTexture & other)
{
    if (this != &other && other.texture != nullptr)
    {
        copy(other);
    }
    return *this;
}

void LTexture::copy(const LTexture & other)
{
    free();
    load(other.file_path); // sets width, height, file_path as well as load texture
    renderer = other.renderer;
    font = other.font;
}

LTexture::~LTexture()
{
    free();
}


void LTexture::set_renderer(SDL_Renderer * renderer_ref)
{
    renderer = renderer_ref;
}

void LTexture::set_fallback_renderer(SDL_Renderer * renderer_ref)
{
    fallback_renderer = renderer_ref;
}


#ifdef SDL_TTF_MAJOR_VERSION
void LTexture::set_font(TTF_Font * font_ref)
{
    font = font_ref;
}

void LTexture::set_fallback_font(TTF_Font * font_ref)
{
    fallback_font = font_ref;
}
#endif

bool LTexture::load(std::string path)
{
    // get rid of preexisting texture
    free();

    // final texture
    SDL_Texture * new_texture = nullptr;

    SDL_Surface * loaded_surface = IMG_Load(path.c_str());
    if (loaded_surface == nullptr)
    {
        throw LException{"Unable to load image file into surface at \"" + path
                + "\"! SDL_image Error: " + std::string{IMG_GetError()} + '\n'};
    }
    else
    {
        // Color key image
        SDL_SetColorKey(loaded_surface, SDL_TRUE,
                        SDL_MapRGB(loaded_surface->format, 0x00, 0xFF, 0xFF));
        
        // Create texture from surface pixels
        new_texture = SDL_CreateTextureFromSurface(get_renderer(), loaded_surface);
        if (new_texture == nullptr)
        {
            throw LException{"Unable to create texture from loaded surface from file \""
                + path + "\"! SDL Error: " + std::string{SDL_GetError()} + '\n'};
        }
        else
        {
            width = loaded_surface->w;
            height = loaded_surface->h;
            file_path = path;
        }

        SDL_FreeSurface(loaded_surface);
    }

    texture = new_texture;
    return texture != nullptr;
}

#ifdef SDL_TTF_MAJOR_VERSION
bool LTexture::load_text(std::string text, SDL_Color color, TTF_Font * font_override)
{
    // Get rid of preexisting texture
    free();

    // render text to a surface
    SDL_Surface * text_surface = TTF_RenderText_Solid(get_font(font_override), text.c_str(), color);
    if (text_surface == nullptr)
    {
        throw LException{"Unable to render text surface! SDL_TTF Error: "
                         + std::string{TTF_GetError()} + '\n'};
    }
    else
    {
        // create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(get_renderer(), text_surface);
        if (texture == nullptr)
        {
            throw LException{"Unable to create texture from rendered text surface! SDL Error: "
                             + std::string{SDL_GetError()} + '\n'};
        }
        else
        {
            width = text_surface->w;
            height = text_surface->h;
            file_path = "";
        }

        SDL_FreeSurface(text_surface);
    }

    return texture != nullptr;

}
#endif

void LTexture::free()
{
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        width = 0;
        height = 0;
    }
}

void LTexture::set_color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
    set_color(red, blue, green);
    set_alpha(alpha);
}
void LTexture::set_color(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(texture, red, green, blue);
}
void LTexture::set_color(SDL_Color color)
{
    set_color(color.r, color.g, color.b, color.a);
}
void LTexture::set_alpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(texture, alpha);
}

void LTexture::set_blend_mode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(texture, blending);
}

// Renders texture at specified point, and other optional parameters
void LTexture::render(int x, int y, SDL_Rect * clip, double angle,
            SDL_Point * center, SDL_RendererFlip flip)
{
    return render(renderer, x, y, clip, angle, center, flip);
}

void LTexture::render(SDL_Renderer * renderer_override, int x, int y, SDL_Rect * clip, double angle,
            SDL_Point * center, SDL_RendererFlip flip)
{
    // screen space to render texture to
    SDL_Rect render_quad{x, y, width, height};
    // alter to clip dimensions
    if (clip != nullptr)
    {
        render_quad.w = clip->w;
        render_quad.h = clip->h;
    }

    // Render to screen!
    SDL_RenderCopyEx(get_renderer(renderer_override), texture, clip, &render_quad, angle, center, flip);
}

int LTexture::get_width()
{
    return width;
}
int LTexture::get_height()
{
    return height;
}


// ---- PRIVATE METHODS ----

SDL_Renderer * LTexture::get_renderer(SDL_Renderer * renderer_override)
{
    if (renderer_override != nullptr)
    {
        return renderer_override;
    }
    else if (renderer != nullptr)
    {
        return renderer;
    }
    else if (fallback_renderer != nullptr)
    {
        return fallback_renderer;
    }
    else
    {
        throw LException{"Operation failed: No SDL_Renderer* to retrieve!"};
    }
}

#ifdef SDL_TTF_MAJOR_VERSION
TTF_Font * LTexture::get_font(TTF_Font * font_override)
{
    if (font_override != nullptr)
    {
        return font_override;
    }
    else if (font != nullptr)
    {
        return font;
    }
    else if (fallback_font != nullptr)
    {
        return fallback_font;
    }
    else
    {
        throw LException{"Operation failed: No TTF_Font* to retrieve!"};
    }
}
#endif

} // namespace LCode
