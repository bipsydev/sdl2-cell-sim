#include "LTexture.hpp"
#include "LException.hpp"


#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>

#include <string>
#include <iostream>

namespace LCode
{
// ---- Static initializers ----
GPU_Target * LTexture::fallback_gpu = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
TTF_Font * LTexture::fallback_font = nullptr;
#endif


// constructors
LTexture::LTexture()
: LTexture(nullptr)
{ }

LTexture::LTexture(GPU_Target * gpu_ref)
: LTexture(gpu_ref, nullptr)
{ }

LTexture::LTexture(GPU_Target * gpu_ref, TTF_Font * font_ref)
: image{nullptr},
  color{0xFF, 0xFF, 0xFF, 0xFF},
  width{0}, height{0},
  file_path{""},
  gpu{gpu_ref},
  font{font_ref}
{
    set_color(color);
}


LTexture::LTexture(const LTexture & other)
: LTexture()
{
    if (other.image != nullptr)
    {
        copy(other);
    }
}

LTexture & LTexture::operator = (const LTexture & other)
{
    if (this != &other && other.image != nullptr)
    {
        copy(other);
    }
    return *this;
}

void LTexture::copy(const LTexture & other)
{
    free();
    load(other.file_path); // sets width, height, file_path as well as load texture
    gpu = other.gpu;
    font = other.font;
}

LTexture::~LTexture()
{
    free();
}


void LTexture::set_gpu(GPU_Target * gpu_ref)
{
    gpu = gpu_ref;
}

void LTexture::set_fallback_gpu(GPU_Target * gpu_ref)
{
    fallback_gpu = gpu_ref;
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

    image = GPU_LoadImage(path.c_str());
    if (image == nullptr)
    {
        throw LException{"Unable to load image file into surface at \"" + path
                + "\"! SDL Error: " + std::string{SDL_GetError()} + '\n'};
    }
    else
    {
        width = image->w;
        height = image->h;
        file_path = path;
    }

    return image != nullptr;
}

#ifdef SDL_TTF_MAJOR_VERSION
bool LTexture::load_text(std::string text, SDL_Color text_color, TTF_Font * font_override)
{
    // Get rid of preexisting texture
    free();

    // render text to a surface
    SDL_Surface * text_surface = TTF_RenderText_Solid(get_font(font_override), text.c_str(), text_color);
    if (text_surface == nullptr)
    {
        throw LException{"Unable to render text surface! SDL_TTF Error: "
                         + std::string{TTF_GetError()} + '\n'};
    }
    else
    {
        // create texture from surface pixels
        image = GPU_CopyImageFromSurface(text_surface);
        if (image == nullptr)
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

    return image != nullptr;

}
#endif

void LTexture::free()
{
    if (image != nullptr)
    {
        GPU_FreeImage(image);
        image = nullptr;
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
    set_color(SDL_Color{red, green, blue, color.a});
}
void LTexture::set_color(SDL_Color new_color)
{
    GPU_SetColor(image, new_color);
    color = new_color;
}
void LTexture::set_alpha(Uint8 alpha)
{
    set_color(color.r, color.g, color.b, alpha);
}

void LTexture::set_blend_mode(GPU_BlendPresetEnum blend_mode)
{
    GPU_SetBlendMode(image, blend_mode);
}

// Renders texture at specified point, and other optional parameters
void LTexture::render(float x, float y, GPU_Rect * clip, float angle,
            SDL_Point * center, GPU_FlipEnum flip)
{
    return render(gpu, x, y, clip, angle, center, flip);
}

void LTexture::render(GPU_Target * gpu_override, float x, float y, GPU_Rect * clip, float angle,
            SDL_Point * center, GPU_FlipEnum flip)
{
    // screen space to render texture to
    GPU_Rect render_rect{x, y, static_cast<float>(width), static_cast<float>(height)};
    // alter to clip dimensions
    if (clip != nullptr)
    {
        render_rect.w = clip->w;
        render_rect.h = clip->h;
    }

    // Render to screen!
    //SDL_RenderCopyEx(get_renderer(renderer_override), texture, clip, &render_quad, angle, center, flip);
    GPU_BlitRectX(image, clip, get_gpu(gpu_override), &render_rect, angle,
                    static_cast<float>(center != nullptr? center->x : 0),
                    static_cast<float>(center != nullptr? center->y : 0), flip);
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

GPU_Target * LTexture::get_gpu(GPU_Target * gpu_override)
{
    if (gpu_override != nullptr)
    {
        return gpu_override;
    }
    else if (gpu != nullptr)
    {
        return gpu;
    }
    else if (fallback_gpu != nullptr)
    {
        return fallback_gpu;
    }
    else
    {
        throw LException{"Operation failed: No GPU_Target* to retrieve!"};
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
