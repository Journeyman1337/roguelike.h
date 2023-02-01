// SPDX-FileCopyrightText: 2021-2023 Daniel Aimé Valcour <fosssweeper@gmail.com>
//
// SPDX-License-Identifier: MIT

/*
    Copyright (c) 2021-2023  Daniel Aimé Valcour
    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
    roguelike.h version v2.0.0
    Header only roguelike rendering library.
    The source for this library can be found on GitHub:
    https://github.com/Journeyman-dev/roguelike.h

    CHANGELOG
    - Version 2.0
        Features
            - Depreciated rlhAtlas_s, and all atlas manipulation is done directly with rlhTerminal_s.
            - Extended maximum glyph count for atlases from 65535 to as many that memory can contain.
            - Added support for G, GA, RGBA, and BGRA atlas bitmaps.
            - Switched from using glDebug.h to the newly made gl_debug.h library.
            - Modified the OpenGL renderer to use a vertex buffer object for storing tile data.
            - Added support for OpenGL ES 2 so roguelike.h now works in emscripten and mobile platforms.
    - Version 1.4
        Features
            - Renderer now gets and stores shader uniforms on terminal initialization instead of getting it every
              frame.
        Bugfixes
            - Fixed issue with glDebug.h macros when glDebug.h is not used.
            - Removed extra const qualifier to RLH_RESULT_DESCRIPTIONS
    - Version 1.3
        Features
            - Added option macro RLH_RETAINED_MODE to to disable automatic tile buffer clearing on each new frame.
            - Changed usage of glDebug.h to be consistent with version 1.0.
    - Version 1.2
        Features
            - Added automatic terminal clear after each draw so users don't have to remember
              clear the data themselves to prevent memory leaks.
            - Update the header comment in roguelike.h with a detailed explanation of usage.
            - Result codes have been reordered, and RLH_RESULT_LAST_NON_ERROR was added to make it
            easier to determine if a result code is an error or not.
    - Version 1.1
        Bugfixes
            - Added missing include to stddef.h
    - Version 1.0
        Initial Release
*/

#ifndef ROGUELIKE_H
#define ROGUELIKE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>

// If gl_debug.h is not included, define the macros manually so the code still compiles.
#ifndef GLD_H
#define GLD_START()
#define GLD_SET_CALLBACK(callback)
#define GLD_CALL(glFunc) glFunc;
#define GLD_COMPILE(shaderHandleVar, identifier) glCompileShader(shaderHandleVar);
#define GLD_LINK(programHandleVar, identifier) glLinkProgram(programHandleVar);
#endif

  typedef struct rlhColor_s
  {
    float r;
    float g;
    float b;
    float a;
  } rlhColor_s;

// custom color macros
#define RLH_COLOR(red, green, blue, alpha) ((rlhColor_s){(red), (green), (blue), (alpha)})

// standard color macros
#define RLH_RED ((rlhColor_s){(1.0f), (0.0f), (0.0f), (1.0f)})
#define RLH_LIME ((rlhColor_s){(0.0f), (1.0f), (0.0f), (1.0f)})
#define RLH_BLUE ((rlhColor_s){(0.0f), (0.0f), (1.0f), (1.0f)})
#define RLH_WHITE ((rlhColor_s){(1.0f), (1.0f), (1.0f), (1.0f)})
#define RLH_BLACK ((rlhColor_s){(0.0f), (0.0f), (0.0f), (1.0f)})
#define RLH_SILVER ((rlhColor_s){(0.752941f), (0.752941f), (0.752941f), (1.0f)})
#define RLH_GRAY ((rlhColor_s){(0.5f), (0.5f), (0.5f), (1.0f)})
#define RLH_MAROON ((rlhColor_s){(0.5f), (0.0f), (0.0f), (1.0f)})
#define RLH_YELLOW ((rlhColor_s){(1.0f), (1.0f), (0.0f), (1.0f)})
#define RLH_OLIVE ((rlhColor_s){(0.5f), (0.5f), (0.0f), (1.0f)})
#define RLH_GREEN ((rlhColor_s){(0.0f), (0.5f), (0.0f), (1.0f)})
#define RLH_AQUA ((rlhColor_s){(0.0f), (1.0f), (1.0f), (1.0f)})
#define RLH_TEAL ((rlhColor_s){(0.0f), (0.5f), (0.5f), (1.0f)})
#define RLH_NAVY ((rlhColor_s){(0.0f), (0.0f), (0.5f), (1.0f)})
#define RLH_FUCHSIA ((rlhColor_s){(1.0f), (0.0f), (1.0f), (1.0f)})
#define RLH_PURPLE ((rlhColor_s){(0.5f), (0.0f), (0.5f), (1.0f)})
#define RLH_TRANSPARENT ((rlhColor_s){(0.0f), (0.0f), (0.0f), (0.0f)})

  typedef enum rlhbool_t
  {
    RLH_FALSE = 0,
    RLH_TRUE = 1
  } rlhbool_t;

  typedef uint16_t rlhglyph_t;

  typedef struct rlhTerm_s *rlhTerm_h;

  typedef enum rlhresult_t
  {
    RLH_RESULT_OK = 0,
    RLH_RESULT_ERROR_NULL_ARGUMENT = 1,
    RLH_RESULT_ERROR_INVALID_VALUE = 2,
    RLH_RESULT_ERROR_OUT_OF_MEMORY = 3,
    RLH_RESULT_COUNT
  } rlhresult_t;

  extern const char *const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT];

  typedef enum rlhcolortype_t
  {
    RLH_COLOR_G,
    RLH_COLOR_GA,
    RLH_COLOR_RGBA,
    RLH_COLOR_BGRA,
    RLH_COLOR_TYPE_COUNT
  } rlhcolortype_t;

  extern const char *const RLH_COLOR_TYPE_NAMES[RLH_COLOR_TYPE_COUNT];

  typedef enum rlhsizemode_t
  {
    RLH_SIZE_TILES,
    RLH_SIZE_PIXELS,
    RLH_SIZE_MODE_COUNT
  } rlhsizemode_t;

  typedef struct rlhAtlasCreateInfo_t
  {
    int width;
    int height;
    int pages;
    int channel_size;
    rlhcolortype_t color;
    uint8_t *pixel_data;
    int glyph_count;
    float *glyph_stpqp;
    int tile_width;
    int tile_height;
  } rlhAtlasCreateInfo_t;

  typedef struct rlhTermSizeInfo_t
  {
    int width;
    int height;
    rlhsizemode_t size_mode;
    rlhbool_t floor_pixels_to_tiles;
    float pixel_scale;
  } rlhTermSizeInfo_t;

  typedef struct rlhTermCreateInfo_t
  {
    rlhTermSizeInfo_t *size_info;
    rlhAtlasCreateInfo_t *atlas_info;
  } rlhTermCreateInfo_t;

  // Clear the color of the console area with a solid color.
  void rlhClearColor(const rlhColor_s color);
  // Set viewport area to draw to.
  void rlhViewport(int x, int y, int width, int height);
  // Create a terminal.
  rlhresult_t rlhTermCreate(rlhTermCreateInfo_t *term_info, rlhTerm_h *term);
  // Destroy a term object and free all of its resources.
  void rlhTermDestroy(rlhTerm_h const term);
  // Set the atlas of a terminal.
  rlhresult_t rlhTermSetAtlas(rlhTerm_h const term, rlhAtlasCreateInfo_t *atlas_info);
  // Get the amount of glyphs in a terminal's atlas.
  int rlhTermGetGlyphCount(rlhTerm_h const term);
  // Get the size ratio of a terminal pixel per screen pixel.
  float rlhTermGetPixelScale(rlhTerm_h const term);
  // Get the tile dimensions of a console.
  void rlhTermGetTerminalSize(rlhTerm_h const term, int *const tiles_wide, int *const tiles_tall);
  // Get the pixel dimensions of a console.
  void rlhTermGetPixelSize(rlhTerm_h const term, int *const pixel_width, int *const pixel_height);
  // Get the pixel dimensions of a single tile.
  void rlhTermGetTileSize(rlhTerm_h const term, int *const tile_width, int *const tile_height);
  // Resize a terminal object.
  rlhresult_t rlhTermSetSize(rlhTerm_h const term, rlhTermSizeInfo_t *const size_info);
  // Clear a terminal's data buffer.
  rlhresult_t rlhTermClearTileData(rlhTerm_h const term);
  // Get how many tiles have been set since the last clear.
  int rlhTermGetTileDataCount(rlhTerm_h const term);
  // Push a tile to the terminal that is stretched over the entire terminal area.
  rlhresult_t rlhTermPushFill(rlhTerm_h const term, const uint16_t glyph, const rlhColor_s fg, const rlhColor_s bg);
  // Push a tile to a terminal in a grid cell position with default pixel width and pixel height.
  rlhresult_t rlhTermPushGrid(rlhTerm_h const term, const int grid_x, const int grid_y, const uint16_t glyph, const rlhColor_s fg, const rlhColor_s bg);
  // Push a tile to a terminal in a grid cell position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushGridSized(rlhTerm_h const term, const int grid_x, const int grid_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor_s fg, const rlhColor_s bg);
  // Push a tile to a terminal in a pixel position with a default pixel width and pixel height.
  rlhresult_t rlhTermPushFree(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const rlhglyph_t glyph, const rlhColor_s fg, const rlhColor_s bg);
  // Push a tile to a terminal in a pixel position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushFreeSized(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const int tile_pixel_width, const int tile_pixel_height, const rlhglyph_t glyph, const rlhColor_s fg, const rlhColor_s bg);
  // Draw a terminal to the current bound framebuffer of the current graphics context. Draws it to fit the viewport, which might distort pixels.
  rlhresult_t rlhTermDraw(rlhTerm_h const term);
  // Draw a terminal pixel perfect, centered in the viewport.
  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term, const int viewport_width, const int viewport_height);
  // Draw a terminal pixel perfect, translated to by a 2d pixel vector.
  rlhresult_t rlhTermDrawTranslated(rlhTerm_h const term, const int translate_x, const int translate_y, const int viewport_width, const int viewport_height);
  // Draw a terminal translated to by a 2d pixel vector and scaled by a 2d float vector.
  rlhresult_t rlhTermDrawTransformed(rlhTerm_h const term, const int translate_x, const int translate_y, const float scale_x, const float scale_y, const int viewport_width, const int viewport_height);
  // Draw a terminal transformed by a matrix 4x4 (with 16 floats)
  rlhresult_t rlhTermDrawMatrix(rlhTerm_h const term, const float *const matrix_4x4);

#ifdef RLH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define MAX(x, y) ((x) > (y)) ? (x) : y
#endif

  const char *const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT] = {
      "no errors occured", "unexpected null argument",
      "unexpected argument value", "out of memory"};

  const float RLH_OPENGL_SCREEN_MATRIX[4 * 4] = {2.0f, 0.0f, 0.0f, -1.0f, 0.0f, -2.0f, 0.0f, 1.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

  const char *RLH_VERTEX_SOURCE =
      "#version 330 core\n"
      "in vec2 a_pos;\n"
      "in vec3 a_uvp;\n"
      "in vec4 a_fg;\n"
      "in vec4 a_bg;\n"
      "out vec3 v_uvp;\n"
      "out vec4 v_fg;\n"
      "out vec4 v_bg;"
      "uniform mat4 u_matrix;\n"
      "void main()\n"
      "{\n"
      "  gl_Position = u_matrix * vec4(a_pos, 0.0, 1.0);\n"
      "  v_uvp = a_uvp;\n"
      "  v_fg = a_fg;\n"
      "  v_bg = a_bg;"
      "}";

  const char *RLH_FRAGMENT_ALPHA_BG_SOURCE =
      "#version 330 core\n"
      "in vec3 v_uvp;\n"
      "in vec4 v_fg;\n"
      "in vec4 v_bg;\n"
      "out vec4 f_color;\n"
      "uniform sampler2DArray u_atlas;\n"
      "void main()\n"
      "{\n"
      "  vec4 tex_color = texture(u_atlas, v_uvp);\n"
      "  vec4 actual_tex_color = vec4(tex_color.rgb, 1.0);\n"
      "  f_color = mix(v_bg, v_fg * actual_tex_color, tex_color.a);\n"
      "}";

  const char *RLH_FRAGMENT_GREEN_BG_SOURCE =
      "#version 330 core\n"
      "in vec3 v_uvp;\n"
      "in vec4 v_fg;\n"
      "in vec4 v_bg;\n"
      "out vec4 f_color;\n"
      "uniform sampler2DArray u_atlas;\n"
      "void main()\n"
      "{\n"
      "  vec4 tex_color = texture(u_atlas, v_uvp);\n"
      "  vec4 actual_tex_color = vec4(tex_color.r, tex_color.r, tex_color.r, 1.0);\n"
      "  f_color = mix(v_bg, v_fg * actual_tex_color, tex_color.g);\n"
      "}";

  const char *RLH_FRAGMENT_STENCIL_SOURCE =
      "#version 330 core\n"
      "in vec3 v_uvp;\n"
      "in vec4 v_color;\n"
      "out vec4 f_color;\n"
      "uniform sampler2DArray u_atlas;\n"
      "void main()\n"
      "{\n"
      "  vec4 tex_color = texture(u_atlas, v_uvp);\n"
      "  f_color = mix(v_bg, v_fg, tex_color.r);\n"
      "}";

  typedef enum rlhfragmenttype_t
  {
    RLH_FRAGMENT_NONE,
    RLH_FRAGMENT_STENCIL,
    RLH_FRAGMENT_GREEN_BG,
    RLH_FRAGMENT_ALPHA_BG,
    RLH_FRAGMENT_COUNT
  } rlhfragmenttype_t;

  const size_t RLH_VERTEX_DATA_ATTRIBUTES_PER_VERTEX = 13;
  const size_t RLH_FONTMAP_COORDINATES_PER_GLYPH = 5;
  const size_t RLH_VERTICES_PER_TILE = 4;
  const size_t RLH_ELEMENTS_PER_TILE = 6;
  const size_t RLH_MATRIX_FLOAT_COUNT = 16;
  GLint RLH_ATLAS_TEXTURE_SLOT = 0;

  typedef struct rlhTerm_s
  {
    size_t pixel_width;
    size_t pixel_height;
    size_t tiles_wide;
    size_t tiles_tall;
    float pixel_scale;
    size_t tile_width;
    size_t tile_height;
    size_t vertex_data_tile_capacity;
    size_t vertex_data_tile_count;
    float *vertex_data;
    rlhbool_t vertex_data_changed;
    size_t atlas_width;
    size_t atlas_height;
    size_t atlas_pages;
    size_t glyph_count;
    float *glyph_stpqp;
    size_t element_glyph_count;
    GLuint *element_data;
    rlhcolortype_t atlas_color;
    rlhfragmenttype_t fragment_type;

    // OpenGL
    GLuint gl_program;
    GLuint gl_vertex_array;
    GLuint gl_vertex_buffer;
    GLuint gl_element_buffer;
    GLuint gl_matrix_uniform_location;
    GLuint gl_atlas_texture_2d_array;
  } rlhTerm_s;

  static inline GLuint _rlhColorTypeToGlFormat(const rlhcolortype_t color)
  {
    switch (color)
    {
    case RLH_COLOR_G:
      return GL_R;
    case RLH_COLOR_GA:
      return GL_RG;
    case RLH_COLOR_RGBA:
      return GL_RGBA;
    case RLH_COLOR_BGRA:
      return GL_BGRA;
    default:
      return GL_NONE;
    }
  }

  static inline GLuint _rlhColorTypeToGlInternalFormat(const rlhcolortype_t color)
  {
    switch (color)
    {
    case RLH_COLOR_G:
      return GL_R8;
    case RLH_COLOR_GA:
      return GL_RG;
    case RLH_COLOR_RGBA:
    case RLH_COLOR_BGRA:
      return GL_RGBA;
    default:
      return GL_NONE;
    }
  }

  static inline rlhresult_t _rlhCreateGlTextureArray(const rlhAtlasCreateInfo_t *const atlas_info, GLint *const gl_texture_2d_array)
  {
    *gl_texture_2d_array = GL_NONE;
    const GLenum format = _rlhColorTypeToGlFormat(atlas_info->color);
    const GLenum internal_format = _rlhColorTypeToGlInternalFormat(atlas_info->color);
    if (internal_format == GL_NONE || format == GL_NONE)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }
    GLD_START();
    GLD_CALL(glGenTextures(1, gl_texture_2d_array));
    GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, *gl_texture_2d_array));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0));
    GLD_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal_format, atlas_info->width, atlas_info->height, atlas_info->pages, 0, format, GL_UNSIGNED_BYTE, atlas_info->pixel_data));
    return RLH_RESULT_OK;
  }

  static inline GLuint _rlhCreateGlProgram(const char *vertex_source, const char *fragment_source)
  {
    GLint gl_program, gl_vertex_shader, gl_fragment_shader;
    GLD_CALL(gl_vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    GLD_CALL(glShaderSource(gl_vertex_shader, 1, &vertex_source, NULL));
    GLD_COMPILE(gl_vertex_shader, "rlh vertex shader");
    GLD_CALL(gl_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    GLD_CALL(glShaderSource(gl_fragment_shader, 1, &fragment_source, NULL));
    GLD_COMPILE(gl_fragment_shader, "rlh fragment shader");
    GLD_CALL(gl_program = glCreateProgram());
    GLD_CALL(glAttachShader(gl_program, gl_vertex_shader));
    GLD_CALL(glAttachShader(gl_program, gl_fragment_shader));
    GLD_LINK(gl_program, "rlh shader program");
    GLD_CALL(glDetachShader(gl_program, gl_vertex_shader));
    GLD_CALL(glDetachShader(gl_program, gl_fragment_shader));
    GLD_CALL(glDeleteShader(gl_vertex_shader));
    GLD_CALL(glDeleteShader(gl_fragment_shader));
    return gl_program;
  }

  static inline size_t _rlhGetVertexDataSize(size_t tile_count)
  {
    return tile_count *
           RLH_VERTEX_DATA_ATTRIBUTES_PER_VERTEX *
           RLH_VERTICES_PER_TILE *
           sizeof(float);
  }

  static inline size_t _rlhGetVertexAttributeCount(size_t tile_count)
  {
    return tile_count *
           RLH_VERTEX_DATA_ATTRIBUTES_PER_VERTEX *
           RLH_VERTICES_PER_TILE;
  }

  static inline size_t _rlhGetElementDataSize(size_t tile_count)
  {
    return tile_count *
           RLH_ELEMENTS_PER_TILE *
           sizeof(GLuint);
  }

  static inline size_t _rlhGetElementCount(size_t tile_count)
  {
    return tile_count *
           RLH_ELEMENTS_PER_TILE;
  }

  static inline rlhresult_t _rlhTermTryAppendElements(rlhTerm_h const term)
  {
    if (term->element_glyph_count >= term->vertex_data_tile_count)
      return RLH_RESULT_OK;
    unsigned int *new_element_data = realloc(term->element_data, _rlhGetElementDataSize(term->vertex_data_tile_count));
    if (new_element_data == NULL)
    {
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }
    term->element_data = new_element_data;
    for (; term->element_glyph_count < term->vertex_data_tile_count; term->element_glyph_count++)
    {
      const GLuint vert0 = term->element_glyph_count * RLH_VERTICES_PER_TILE;
      size_t element_data_i = term->element_glyph_count * RLH_ELEMENTS_PER_TILE;
      term->element_data[element_data_i++] = vert0;
      term->element_data[element_data_i++] = vert0 + 1;
      term->element_data[element_data_i++] = vert0 + 2;
      term->element_data[element_data_i++] = vert0 + 1;
      term->element_data[element_data_i++] = vert0 + 2;
      term->element_data[element_data_i] = vert0 + 3;
    }
    return RLH_RESULT_OK;
  }

  static inline rlhfragmenttype_t _rlhColorTypeToFragmentType(rlhcolortype_t color)
  {
    switch (color)
    {
    case RLH_COLOR_G:
      return RLH_FRAGMENT_STENCIL;
    case RLH_COLOR_GA:
      return RLH_FRAGMENT_GREEN_BG;
    }
    return RLH_FRAGMENT_ALPHA_BG;
  }

  static inline const char *_rlhFragmentSourceFromFragmentType(rlhfragmenttype_t fragment)
  {
    switch (fragment)
    {
    case RLH_FRAGMENT_STENCIL:
      return RLH_FRAGMENT_STENCIL_SOURCE;
    case RLH_FRAGMENT_GREEN_BG:
      return RLH_FRAGMENT_GREEN_BG_SOURCE;
    }
    return RLH_FRAGMENT_ALPHA_BG_SOURCE;
  }

  static inline rlhresult_t _rlhSizeInfoCheck(rlhTermSizeInfo_t *size_info)
  {
    if (size_info == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (
        size_info->width <= 0 ||
        size_info->height <= 0 ||
        size_info->size_mode < RLH_SIZE_TILES ||
        size_info->size_mode >= RLH_SIZE_MODE_COUNT)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }
    return RLH_RESULT_OK;
  }

  static inline rlhresult_t _rlhAtlasInfoCheck(rlhAtlasCreateInfo_t *atlas_info)
  {
    if (
        atlas_info == NULL ||
        atlas_info->pixel_data == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (
        atlas_info->width <= 0 ||
        atlas_info->height <= 0 ||
        atlas_info->pages <= 0 ||
        atlas_info->color < RLH_COLOR_G ||
        atlas_info->color >= RLH_COLOR_TYPE_COUNT ||
        atlas_info->glyph_count <= 0 ||
        atlas_info->tile_width <= 0 ||
        atlas_info->tile_height <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }
    return RLH_RESULT_OK;
  }

  static inline rlhresult_t _rlhTermInfoCheck(rlhTermCreateInfo_t *term_info)
  {
    if (
        term_info == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    rlhresult_t result = _rlhSizeInfoCheck(term_info->size_info);
    if (result != RLH_RESULT_OK)
    {
      return result;
    }
    return _rlhAtlasInfoCheck(term_info->atlas_info);
  }

  static inline void _rlhTermSetPixelSize(rlhTerm_h term, rlhTermSizeInfo_t *size_info, int tile_width, int tile_height)
  {
    term->pixel_width = size_info->width;
    term->pixel_height = size_info->height;
    if (size_info->size_mode == RLH_SIZE_TILES)
    {
      term->pixel_width *= tile_width;
      term->pixel_height *= tile_height;
    }
    else if (size_info->floor_pixels_to_tiles)
    {
      term->pixel_width -= term->pixel_width % tile_width;
      term->pixel_height -= term->pixel_height % tile_height;
    }
    term->pixel_width *= size_info->pixel_scale;
    term->pixel_height *= size_info->pixel_scale;
    term->pixel_scale = size_info->pixel_scale;
    term->tile_width = tile_width;
    term->tile_height = tile_height;
    term->tiles_wide = term->pixel_width / (tile_height * term->pixel_scale);
    term->tiles_tall = term->pixel_height / (tile_height * term->pixel_scale);
  }

  static inline rlhresult_t _rlhTermSetAtlas(rlhTerm_h term, rlhAtlasCreateInfo_t *atlas_info)
  {
    const size_t stpqp_size = atlas_info->glyph_count * RLH_FONTMAP_COORDINATES_PER_GLYPH * sizeof(float);
    float *glyph_stpqp = realloc(term->glyph_stpqp, stpqp_size);
    if (glyph_stpqp == NULL)
    {
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }
    term->glyph_stpqp = glyph_stpqp;
    GLD_START();
    GLint gl_atlas_texture_2d_array = GL_NONE;
    rlhresult_t result = _rlhCreateGlTextureArray(atlas_info, &gl_atlas_texture_2d_array);
    if (result != RLH_RESULT_OK)
    {
      return result;
    }
    rlhfragmenttype_t fragment_type = _rlhColorTypeToFragmentType(atlas_info->color);
    if (fragment_type != term->fragment_type)
    {
      if (term->gl_program != GL_NONE)
      {
        GLD_CALL(glDeleteProgram(term->gl_program));
        term->gl_program = GL_NONE;
      }
      const char *fragment_source = _rlhFragmentSourceFromFragmentType(fragment_type);
      term->gl_program = _rlhCreateGlProgram(RLH_VERTEX_SOURCE, fragment_source);
      term->gl_matrix_uniform_location = GLD_CALL(glGetUniformLocation(term->gl_program, "u_matrix"));
    }
    term->fragment_type = fragment_type;
    if (term->gl_atlas_texture_2d_array != GL_NONE)
    {
      GLD_CALL(glDeleteTextures(1, &term->gl_atlas_texture_2d_array));
      term->gl_atlas_texture_2d_array = GL_NONE;
    }
    term->gl_atlas_texture_2d_array = gl_atlas_texture_2d_array;
    memcpy(term->glyph_stpqp, atlas_info->glyph_stpqp, stpqp_size);
    term->glyph_count = atlas_info->glyph_count;
    return RLH_RESULT_OK;
  }

  void rlhClearColor(const rlhColor_s color)
  {
    GLD_START();
    GLD_CALL(glClearColor(color.r, color.g, color.b, color.a));
    GLD_CALL(glClear(GL_COLOR_BUFFER_BIT));
  }

  void rlhViewport(const int x, const int y, const int width, const int height)
  {
    GLD_START();
    GLD_CALL(glViewport(x, y, width, height));
  }

  rlhresult_t rlhTermCreate(rlhTermCreateInfo_t *term_info, rlhTerm_h *term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    rlhresult_t result = _rlhTermInfoCheck(term_info);
    if (result != RLH_RESULT_OK)
    {
      return result;
    }
    rlhTerm_h term_h = (rlhTerm_h)malloc(sizeof(rlhTerm_s));
    if (term_h == NULL)
    {
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }

    memset(term_h, 0, sizeof(rlhTerm_s));
    _rlhTermSetPixelSize(
        term_h,
        term_info->size_info,
        term_info->atlas_info->tile_width,
        term_info->atlas_info->tile_height);
    term_h->vertex_data_tile_capacity = term_h->tiles_wide * term_h->tiles_tall;
    const size_t vertex_data_size = _rlhGetVertexDataSize(term_h->vertex_data_tile_capacity);
    term_h->vertex_data = malloc(vertex_data_size);
    if (term_h->vertex_data == NULL)
    {
      free(term_h);
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }
    result = _rlhTermSetAtlas(term_h, term_info->atlas_info);
    if (result != RLH_RESULT_OK)
    {
      free(term_h);
      return result;
    }
    *term = term_h;
    return RLH_RESULT_OK;
  }

  void rlhTermDestroy(rlhTerm_h const term)
  {
    if (term == NULL)
      return;
    free(term->vertex_data);
    term->vertex_data = NULL;
    free(term->glyph_stpqp);
    term->glyph_stpqp = NULL;
    free(term->element_data);
    term->element_data = NULL;
    GLD_START();
    if (term->gl_vertex_array != GL_NONE)
    {
      GLD_CALL(glDeleteVertexArrays(1, &term->gl_vertex_array));
      term->gl_vertex_array = GL_NONE;
    }
    if (term->gl_vertex_buffer != GL_NONE)
    {
      GLD_CALL(glDeleteBuffers(1, &term->gl_vertex_buffer));
      term->gl_vertex_buffer = GL_NONE;
    }
    if (term->gl_element_buffer != GL_NONE)
    {
      GLD_CALL(glDeleteBuffers(1, &term->gl_element_buffer));
      term->gl_element_buffer = GL_NONE;
    }
    if (term->gl_program != GL_NONE)
    {
      GLD_CALL(glDeleteProgram(term->gl_program));
    }
    if (term->gl_atlas_texture_2d_array != GL_NONE)
    {
      GLD_CALL(glDeleteTextures(1, &term->gl_atlas_texture_2d_array));
      term->gl_atlas_texture_2d_array = GL_NONE;
    }
    free(term);
  }

  rlhresult_t rlhTermSetAtlas(rlhTerm_h const term, rlhAtlasCreateInfo_t *atlas_info)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    rlhresult_t result = _rlhAtlasInfoCheck(atlas_info);
    if (result != RLH_RESULT_OK)
    {
      return result;
    }
    return _rlhTermSetAtlas(term, atlas_info);
  }

  int rlhTermGetGlyphCount(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0;
    }
    return (int)term->glyph_count;
  }

  float rlhTermGetPixelScale(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0.0f;
    }
    return term->pixel_scale;
  }

  void rlhTermGetTerminalSize(rlhTerm_h const term, int *const tiles_wide, int *const tiles_tall)
  {
    if (term == NULL)
    {
      return;
    }
    if (tiles_wide != NULL)
    {
      *tiles_wide = (int)term->tiles_wide;
    }
    if (tiles_tall != NULL)
    {
      *tiles_tall = (int)term->tiles_tall;
    }
  }

  void rlhTermGetPixelSize(rlhTerm_h const term, int *const pixel_width, int *const pixel_height)
  {
    if (term == NULL)
    {
      return;
    }
    if (pixel_width != NULL)
    {
      *pixel_width = (int)term->pixel_width;
    }
    if (pixel_height != NULL)
    {
      *pixel_height = (int)term->pixel_height;
    }
  }

  void rlhTermGetTileSize(rlhTerm_h const term, int *const tile_width, int *const tile_height)
  {
    if (term == NULL)
    {
      return;
    }
    if (tile_width != NULL)
    {
      *tile_width = (int)term->tile_width;
    }
    if (tile_height != NULL)
    {
      *tile_height = (int)term->tile_height;
    }
  }

  rlhresult_t rlhTermClearTileData(rlhTerm_h term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    term->vertex_data_tile_count = 0;
    return RLH_RESULT_OK;
  }

  int rlhTermGetTileDataCount(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0;
    }
    return (int)term->vertex_data_tile_count;
  }

  static inline rlhbool_t _rlhTermTryReserveVertexData(rlhTerm_h const term)
  {
    // If we hit the reserved tile count, double the amount of reserved space.
    if (term->vertex_data_tile_count < term->vertex_data_tile_capacity)
      return RLH_TRUE;
    size_t new_capacity = (term->vertex_data_tile_capacity == 0) ? 8 : term->vertex_data_tile_capacity * 2;
    float *new_vertex_data = (float *)realloc(
        term->vertex_data,
        _rlhGetVertexDataSize(new_capacity));
    if (new_vertex_data == NULL)
    {
      return RLH_FALSE; // out of memory
    }
    term->vertex_data = new_vertex_data;
    term->vertex_data_tile_capacity = new_capacity;
    return RLH_TRUE;
  }

  static inline void _rlhTermPushTile(rlhTerm_h const term, const int pixel_x, const int pixel_y,
                                      const int pixel_w, const int pixel_h, const uint16_t glyph,
                                      const rlhColor_s fg, const rlhColor_s bg)
  {
    if (glyph > term->glyph_count)
      return;
    float position_s = (float)pixel_x / (float)term->pixel_width;
    float position_t = position_s + ((float)pixel_w / (float)term->pixel_width);
    float position_p = (float)pixel_y / (float)term->pixel_height;
    float position_q = position_p + ((float)pixel_h / (float)term->pixel_height);
    position_s *= term->pixel_scale;
    position_t *= term->pixel_scale;
    position_p *= term->pixel_scale;
    position_q *= term->pixel_scale;
    size_t glyph_stpqp_i = glyph * RLH_FONTMAP_COORDINATES_PER_GLYPH;
    const float atlas_s = term->glyph_stpqp[glyph_stpqp_i++];
    const float atlas_t = term->glyph_stpqp[glyph_stpqp_i++];
    const float atlas_p = term->glyph_stpqp[glyph_stpqp_i++];
    const float atlas_q = term->glyph_stpqp[glyph_stpqp_i++];
    const float atlas_page = term->glyph_stpqp[glyph_stpqp_i];
    size_t index = _rlhGetVertexAttributeCount(term->vertex_data_tile_count);
    // v0
    term->vertex_data[index++] = position_s;
    term->vertex_data[index++] = position_p;
    term->vertex_data[index++] = atlas_s;
    term->vertex_data[index++] = atlas_p;
    term->vertex_data[index++] = atlas_page;
    term->vertex_data[index++] = fg.r;
    term->vertex_data[index++] = fg.g;
    term->vertex_data[index++] = fg.b;
    term->vertex_data[index++] = fg.a;
    term->vertex_data[index++] = bg.r;
    term->vertex_data[index++] = bg.g;
    term->vertex_data[index++] = bg.b;
    term->vertex_data[index++] = bg.a;
    // v1
    term->vertex_data[index++] = position_t;
    term->vertex_data[index++] = position_p;
    term->vertex_data[index++] = atlas_t;
    term->vertex_data[index++] = atlas_p;
    term->vertex_data[index++] = atlas_page;
    term->vertex_data[index++] = fg.r;
    term->vertex_data[index++] = fg.g;
    term->vertex_data[index++] = fg.b;
    term->vertex_data[index++] = fg.a;
    term->vertex_data[index++] = bg.r;
    term->vertex_data[index++] = bg.g;
    term->vertex_data[index++] = bg.b;
    term->vertex_data[index++] = bg.a;
    // v2
    term->vertex_data[index++] = position_s;
    term->vertex_data[index++] = position_q;
    term->vertex_data[index++] = atlas_s;
    term->vertex_data[index++] = atlas_q;
    term->vertex_data[index++] = atlas_page;
    term->vertex_data[index++] = fg.r;
    term->vertex_data[index++] = fg.g;
    term->vertex_data[index++] = fg.b;
    term->vertex_data[index++] = fg.a;
    term->vertex_data[index++] = bg.r;
    term->vertex_data[index++] = bg.g;
    term->vertex_data[index++] = bg.b;
    term->vertex_data[index++] = bg.a;
    // v3
    term->vertex_data[index++] = position_t;
    term->vertex_data[index++] = position_q;
    term->vertex_data[index++] = atlas_t;
    term->vertex_data[index++] = atlas_q;
    term->vertex_data[index++] = atlas_page;
    term->vertex_data[index++] = fg.r;
    term->vertex_data[index++] = fg.g;
    term->vertex_data[index++] = fg.b;
    term->vertex_data[index++] = fg.a;
    term->vertex_data[index++] = bg.r;
    term->vertex_data[index++] = bg.g;
    term->vertex_data[index++] = bg.b;
    term->vertex_data[index] = bg.a;
    term->vertex_data_tile_count++;
    term->vertex_data_changed = RLH_TRUE;
  }

  rlhresult_t rlhTermPushFill(rlhTerm_h const term, const uint16_t glyph, const rlhColor_s fg,
                              const rlhColor_s bg)
  {
    if (!_rlhTermTryReserveVertexData(term))
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, 0, 0, term->pixel_width, term->pixel_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushGrid(rlhTerm_h const term, const int grid_x, const int grid_y,
                              const uint16_t glyph, const rlhColor_s fg,
                              const rlhColor_s bg)
  {
    const int pixel_x = grid_x * term->tile_width;
    const int pixel_y = grid_y * term->tile_height;
    if (!_rlhTermTryReserveVertexData(term))
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, pixel_x, pixel_y, term->tile_width, term->tile_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushGridSized(rlhTerm_h const term, const int grid_x, const int grid_y,
                                   const int tile_pixel_width, const int tile_pixel_height,
                                   const uint16_t glyph, const rlhColor_s fg,
                                   const rlhColor_s bg)
  {
    if (!_rlhTermTryReserveVertexData(term))
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    const unsigned int pixel_x = (unsigned int)grid_x * term->tile_width;
    const unsigned int pixel_y = (unsigned int)grid_y * term->tile_height;
    const unsigned int pixel_w_u = (unsigned int)tile_pixel_width;
    const unsigned int pixel_h_u = (unsigned int)tile_pixel_height;
    _rlhTermPushTile(term, pixel_x, pixel_y, pixel_w_u, pixel_h_u, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushFree(rlhTerm_h const term, const int screen_pixel_x,
                              const int screen_pixel_y, const uint16_t glyph,
                              const rlhColor_s fg, const rlhColor_s bg)
  {
    if (!_rlhTermTryReserveVertexData(term))
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, (unsigned int)screen_pixel_x, (unsigned int)screen_pixel_y,
                     term->tile_width, term->tile_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushFreeSized(rlhTerm_h const term, const int screen_pixel_x,
                                   const int screen_pixel_y, const int tile_pixel_width,
                                   const int tile_pixel_height, const uint16_t glyph,
                                   const rlhColor_s fg, const rlhColor_s bg)
  {
    if (!_rlhTermTryReserveVertexData(term))
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, screen_pixel_x, screen_pixel_y, tile_pixel_width, tile_pixel_height,
                     glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDraw(rlhTerm_h term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    rlhTermDrawMatrix(term, RLH_OPENGL_SCREEN_MATRIX);

    return RLH_RESULT_OK;
  }

  // Translate an opengl screen matrix so that a rectangle using it is flat facing the screen and
  // its 2d edges match the screen space pixel coordinates given.
  static inline void _rlhTransformMatrix(float *matrix, int screen_width, int screen_height,
                                         int translate_x, int translate_y, int console_width,
                                         int console_height)
  {
    const float screenspace_width_scalar = (console_width / ((float)screen_width));
    const float screenspace_height_scalar = (console_height / ((float)screen_height));
    const float screenspace_translate_x = (translate_x / (float)screen_width) * 2.0f;
    const float screenspace_translate_y = (translate_y / (float)screen_height) * 2.0f;
    matrix[0] *= screenspace_width_scalar;
    matrix[5] *= screenspace_height_scalar;
    matrix[3] += screenspace_translate_x;
    matrix[7] -= screenspace_translate_y;
  }

  static void _rlhSetTermScissor(const int translate_x, const int translate_y, const int width,
                                 const int height)
  {
    GLD_START();
    // crop the scissor area so the position is not less than 0 (this causes opengl error)
    const int cropped_x = MAX(translate_x, 0);
    const int cropped_y = MAX(translate_y, 0);
    const int translate_x_difference = cropped_x - translate_x;
    const int translate_y_difference = cropped_y - translate_y;
    const int cropped_width = width - translate_x_difference;
    const int cropped_height = height - translate_y_difference;
    // set scissor
    GLD_CALL(glScissor(cropped_x, cropped_y, cropped_width, cropped_height));
    GLD_CALL(glEnable(GL_SCISSOR_TEST));
  }

  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term,
                                  const int viewport_width, const int viewport_height)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    const int width_difference = viewport_width - term->pixel_width;
    const int height_difference = viewport_height - term->pixel_height;
    const int translate_x = width_difference / 2;
    const int translate_y = height_difference / 2;

    rlhTermDrawTranslated(term, translate_x, translate_y, viewport_width, viewport_height);

    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawTranslated(rlhTerm_h const term,
                                    const int translate_x, const int translate_y,
                                    const int viewport_width, const int viewport_height)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    float matrix[RLH_MATRIX_FLOAT_COUNT];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * RLH_MATRIX_FLOAT_COUNT);
    _rlhTransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                        term->pixel_width, term->pixel_height);
    _rlhSetTermScissor(translate_x, translate_y, term->pixel_width, term->pixel_height);
    // draw
    rlhTermDrawMatrix(term, matrix);
    GLD_START();
    // unset the scissor
    GLD_CALL(glDisable(GL_SCISSOR_TEST));
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawTransformed(rlhTerm_h const term,
                                     const int translate_x, const int translate_y,
                                     const float scale_x, const float scale_y,
                                     const int viewport_width, const int viewport_height)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    float matrix[RLH_MATRIX_FLOAT_COUNT];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * RLH_MATRIX_FLOAT_COUNT);
    _rlhTransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                        term->pixel_width * scale_x, term->pixel_height * scale_y);
    _rlhSetTermScissor(translate_x, translate_y, term->pixel_width * scale_x,
                       term->pixel_height * scale_y);
    // draw
    rlhTermDrawMatrix(term, matrix);
    GLD_START();
    // unset the scissor
    GLD_CALL(glDisable(GL_SCISSOR_TEST));
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawMatrix(rlhTerm_h const term,
                                const float *const matrix_4x4)
  {
    if (term == NULL || matrix_4x4 == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (term->vertex_data_tile_count == 0)
    {
      return RLH_RESULT_OK;
    }
    GLD_START();
    // Update vertex buffer and element buffer data. Create objects if they don't exist yet.
    const size_t before_element_glyph_count = term->element_glyph_count;
    rlhresult_t result = _rlhTermTryAppendElements(term);
    if (result != RLH_RESULT_OK)
    {
      return result;
    }
    rlhbool_t create_vertex_array = term->gl_vertex_array == GL_NONE;
    if (create_vertex_array)
    {
      GLD_CALL(glGenVertexArrays(1, &term->gl_vertex_array));
      GLD_CALL(glGenBuffers(1, &term->gl_vertex_buffer));
      GLD_CALL(glGenBuffers(1, &term->gl_element_buffer));
    }
    if (term->vertex_data_changed)
    {
      GLD_CALL(glBindVertexArray(term->gl_vertex_array));
      GLD_CALL(glBindBuffer(GL_ARRAY_BUFFER, term->gl_vertex_buffer));
      GLD_CALL(glBufferData(
          GL_ARRAY_BUFFER,
          _rlhGetVertexDataSize(term->vertex_data_tile_count),
          term->vertex_data,
          GL_STREAM_DRAW));
      term->vertex_data_changed = RLH_FALSE;
    }
    if (before_element_glyph_count != term->element_glyph_count)
    {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, term->gl_element_buffer);
      glBufferData(
          GL_ELEMENT_ARRAY_BUFFER,
          _rlhGetElementDataSize(term->element_glyph_count),
          term->element_data,
          GL_DYNAMIC_DRAW);
    }
    if (create_vertex_array)
    {
      const size_t stride = RLH_VERTEX_DATA_ATTRIBUTES_PER_VERTEX * sizeof(float);
      // position
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
      glEnableVertexAttribArray(0);
      // uvp
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)2);
      glEnableVertexAttribArray(1);
      // forground color
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void *)5);
      glEnableVertexAttribArray(2);
      // background color
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void *)8);
      glEnableVertexAttribArray(3);
    }
    // Bind objects
    GLD_CALL(glBindVertexArray(term->gl_vertex_array));
    GLD_CALL(glUseProgram(term->gl_program));
    // bind the atlas texture
    GLD_CALL(glActiveTexture(GL_TEXTURE0 + RLH_ATLAS_TEXTURE_SLOT));
    GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, term->gl_atlas_texture_2d_array));
    // set the matrix uniform
    GLD_CALL(glUniformMatrix4fv(term->gl_matrix_uniform_location, 1, GL_TRUE, matrix_4x4));
    // set blend mode
    GLD_CALL(glEnable(GL_BLEND));
    GLD_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    // DRAW!!!
    GLD_CALL(glDrawElements(GL_TRIANGLES, _rlhGetElementCount(term->vertex_data_tile_count), GL_UNSIGNED_INT, NULL));
    return RLH_RESULT_OK;
  }
#endif
#ifdef __cplusplus
}
#endif
#endif
