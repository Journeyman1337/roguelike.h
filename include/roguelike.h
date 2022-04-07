/*
    Copyright (c) 2021-2022 Daniel Valcour
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

#ifndef ROGUELIKE_H
#define ROGUELIKE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

// If glDebug.h is not included, make the debug macros do nothing.
#ifndef GL_DEBUG_H
#define GLD_START()
#define GLD_SET_CALLBACK(callback)
#define GLD_CALL(glFunc) glFunc;
#define GLD_COMPILE(shaderHandleVar) glCompileShader(shaderHandleVar);
#define GLD_LINK(programHandleVar) glLinkProgram(programHandleVar);
#define GLD_END()
#endif


typedef struct rlhColor32_s
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rlhColor32_s;

#define RLH_C32_NON_COMPOUND(red, green, blue, alpha) { red, green, blue, alpha }
#define RLH_C32(red, green, blue, alpha) ((rlhColor32_s){ (red), (green), (blue), (alpha) })

#define RLH_RED() ((rlhColor32_s){ (255), (0), (0), (255) })
#define RLH_LIME() ((rlhColor32_s){ (0), (255), (0), (255) })
#define RLH_BLUE() ((rlhColor32_s){ (0), (0), (255), (255) })
#define RLH_WHITE() ((rlhColor32_s){ (255), (255), (255), (255) })
#define RLH_BLACK() ((rlhColor32_s){ (0), (0), (0), (255) })
#define RLH_SILVER() ((rlhColor32_s){ (192), (192), (192), (255) })
#define RLH_GRAY() ((rlhColor32_s){ (128), (128), (128), (255) })
#define RLH_MAROON() ((rlhColor32_s){ (128), (0), (0), (255) })
#define RLH_YELLOW() ((rlhColor32_s){ (255), (255), (0), (255) })
#define RLH_OLIVE() ((rlhColor32_s){ (128), (128), (0), (255) })
#define RLH_GREEN() ((rlhColor32_s){ (0), (128), (0), (255) })
#define RLH_AQUA() ((rlhColor32_s){ (0), (255), (255), (255) })
#define RLH_TEAL() ((rlhColor32_s){ (0), (128), (128), (255) })
#define RLH_NAVY() ((rlhColor32_s){ (0), (0), (128), (255) })
#define RLH_FUCHSIA() ((rlhColor32_s){ (255), (0), (255), (255) })
#define RLH_PURPLE() ((rlhColor32_s){ (128), (0), (128), (255) })
#define RLH_TRANSPARENT() ((rlhColor32_s){ (0), (0), (0), (0) })

typedef struct rlhTerm_s* rlhTerm_h;

// Clear the color of the console area with a solid color.
void rlhClearColor(const rlhColor32_s color);
// Create a term object
rlhTerm_h rlhTermCreate(const size_t console_tiles_wide, const size_t console_tiles_tall, const float console_pixel_scale, const size_t default_tile_pixel_width, const size_t default_tile_pixel_height, const size_t atlas_pixel_width, const size_t atlas_pixel_height, const size_t atlas_page_count, const uint8_t* atlas_pixel_rgba, const size_t atlas_glyph_count, const float* atlas_glyph_stpqp);
// Destroy a term object and free all of its resources.
void rlhTermDestroy(rlhTerm_h const term);
// Resize the tile dimensions of a console.
void rlhTermResizeConsole(rlhTerm_h const term, const size_t console_tiles_wide, const size_t console_tiles_tall);
// Change the size ratio of a terminal pixel per screen pixel.
void rlhTermResizePixelScale(rlhTerm_h const term, const float console_pixel_scale);
// Change the default width and height of console tiles.
void rlhTermResizeTiles(rlhTerm_h const term, const size_t default_tile_width, const size_t default_tile_height);
// Get the tile dimensions of a console.
void rlhTermGetTileDimensions(rlhTerm_h const term, int* const console_tiles_wide, int* const console_tiles_tall);
// Get the size ratio of a terminal pixel per screen pixel.
float rlhTermGetPixelScale(rlhTerm_h const term);
// Get the pixel dimensions of a console.
void rlhTermGetPixelDimensions(rlhTerm_h const term, size_t* const console_pixel_width, size_t* const console_pixel_height);
// Change the atlas texture and glyph coordinates
void rlhTermChangeAtlas(rlhTerm_h const term, const size_t atlas_pixel_width, const size_t atlas_pixel_height, const size_t atlas_page_count, const uint8_t* atlas_pixel_rgba, const size_t atlas_glyph_count, const float* atlas_glyph_stpqp);
// Clear a terminal's data buffer.
void rlhTermClear(rlhTerm_h const term);
// Get how many tiles have been set since the last clear.
size_t rlhTermGetTileCount(rlhTerm_h const term);
// Push a tile to a terminal in a grid cell position with default pixel width and pixel height.
void rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
// Push a tile to a terminal in a grid cell position with a custom pixel width and pixel height.
void rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
// Push a tile to a terminal in a pixel position with a default pixel width and pixel height.
void rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
// Push a tile to a terminal in a pixel position with a custom pixel width and pixel height.
void rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
// Set viewport area to draw to.
void rlhViewport(const int x, const int y, const int width, const int height);
// Draw a terminal to the current bound framebuffer of the current graphics context. Draws it to fit the viewport.
void rlhTermDraw(rlhTerm_h const term);
// Draw a terminal translated to by a 2d pixel vector.
void rlhTermDrawTranslated(rlhTerm_h const term, const int translate_x, const int translate_y, const int viewport_width, const int viewport_height);
// Draw a terminal translated to by a 2d pixel vector and scaled by a 2d float vector.
void rlhTermDrawTransformed(rlhTerm_h const term, const int translate_x, const int translate_y, const float scale_x, const float scale_y, const int viewport_width, const int viewport_height);
// Draw a terminal transformed by a matrix 4x4 (with 16 floats)
void rlhTermDrawMatrix(rlhTerm_h const term, const float* const matrix_4x4);

#ifdef RLH_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

const float kOpengl33ScreenMatrix[4 * 4] = {2.0f, 0.0f, 0.0f, -1.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

const char* kVertexSource =
"#version 330 core\n\
\n\
uniform vec2 ConsolePixelUnitSize;\n\
uniform usamplerBuffer Data; //batch data buffer.\n\
uniform samplerBuffer Fontmap; //the coordinate uv buffer if not uv grid\n\
uniform mat4 Matrix; //transform matrix for entire batch\n\
out vec3 UV; //uv texture position\n\
out vec4 FG; //foreground color\n\
out vec4 BG; //background color\n\
vec3 getVertexUV(int ch, int tile_vertex)\n\
{\n\
    int index = ch * 5;\n\
    float s = texelFetch(Fontmap, index).r;\n\
    index += 1;\n\
    float t = texelFetch(Fontmap, index).r;\n\
    index += 1;\n\
    float p = texelFetch(Fontmap, index).r;\n\
    index += 1;\n\
    float q = texelFetch(Fontmap, index).r;\n\
    index += 1;\n\
    float page = texelFetch(Fontmap, index).r;\n\
    vec4 uv_square = vec4(s, t, p, q);\n\
    vec2 vert_uvs[6] = vec2[](uv_square.sp, uv_square.sq, uv_square.tq, uv_square.sp, uv_square.tq, uv_square.tp);\n\
    vec3 uv = vec3(vert_uvs[tile_vertex], page);\n\
    return uv;\n\
}\n\
vec4 getVertexPosition(int tile, int tile_vertex, inout int buffer_offset)\n\
{\n\
    uint x_pixel = texelFetch(Data, buffer_offset).r;\n\
    buffer_offset += 1;\n\
    x_pixel += texelFetch(Data, buffer_offset).r * 256u;\n\
    buffer_offset += 1;\n\
    uint y_pixel = texelFetch(Data, buffer_offset).r;\n\
    buffer_offset += 1;\n\
    y_pixel += texelFetch(Data, buffer_offset).r * 256u;\n\
    buffer_offset += 1;\n\
    uint tile_pixel_width = texelFetch(Data, buffer_offset).r;\n\
    buffer_offset += 1;\n\
    tile_pixel_width += texelFetch(Data, buffer_offset).r * 256u;\n\
    buffer_offset += 1;\n\
    uint tile_pixel_height = texelFetch(Data, buffer_offset).r;\n\
    buffer_offset += 1;\n\
    tile_pixel_height += texelFetch(Data, buffer_offset).r * 256u;\n\
    buffer_offset += 1;\n\
    float actual_x_pixel = float(int(x_pixel) - 16384);\n\
    float actual_y_pixel = float(int(y_pixel) - 16384);\n\
    float tile_screen_lx = actual_x_pixel * ConsolePixelUnitSize.x;\n\
    float tile_screen_by = actual_y_pixel * ConsolePixelUnitSize.y;\n\
    float tile_screen_width = float(tile_pixel_width) * ConsolePixelUnitSize.x;\n\
    float tile_screen_height = float(tile_pixel_height) * ConsolePixelUnitSize.y;\n\
    float tile_screen_rx = tile_screen_lx + tile_screen_width;\n\
    float tile_screen_ty = tile_screen_by + tile_screen_height;\n\
    vec4 position_square = vec4(tile_screen_lx, tile_screen_rx, tile_screen_ty, tile_screen_by);\n\
    vec2 vert_positions[6] = vec2[](position_square.sp, position_square.sq, position_square.tq, position_square.sp, position_square.tq, position_square.tp);\n\
    vec2 position = vert_positions[tile_vertex];\n\
    return vec4(position, 0.0, 1.0) * Matrix;\n\
}\n\
vec4 getColor32(inout int buffer_offset)\n\
{\n\
    float r = float(texelFetch(Data, buffer_offset).r) / 255.0;\n\
    buffer_offset += 1;\n\
    float g = float(texelFetch(Data, buffer_offset).r) / 255.0;\n\
    buffer_offset += 1;\n\
    float b = float(texelFetch(Data, buffer_offset).r) / 255.0;\n\
    buffer_offset += 1;\n\
    float a = float(texelFetch(Data, buffer_offset).r) / 255.0;\n\
    buffer_offset += 1;\n\
    return vec4(r, g, b, a);\n\
}\n\
int getGlyph16(inout int buffer_offset)\n\
{\n\
    int glyph = int(texelFetch(Data, buffer_offset).r);\n\
    buffer_offset += 1;\n\
    glyph += int(texelFetch(Data, buffer_offset).r * 256u);\n\
    buffer_offset += 1;\n\
    return glyph;\n\
}\n\
void main()\n\
{\n\
    int tile = gl_VertexID / 6;\n\
    int tile_vertex = gl_VertexID % 6;\n\
    int buffer_offset = 18 * tile;\n\
    gl_Position = getVertexPosition(tile, tile_vertex, buffer_offset);\n\
    int glyph = getGlyph16(buffer_offset);\n\
    UV = getVertexUV(glyph, tile_vertex);\n\
    FG = getColor32(buffer_offset);\n\
    BG = getColor32(buffer_offset);\n\
}";

const char* kFragmentSource =
"#version 330 core\n\
\n\
uniform sampler2DArray Atlas;\n\
in vec3 UV;\n\
in vec4 FG;\n\
in vec4 BG;\n\
out vec4 FragColor;\n\
\n\
void main()\n\
{\n\
    vec4 c = texture(Atlas, UV);\n\
    c = mix(BG, FG * vec4(c.rgb, 1.0), c.a);\n\
    FragColor = c;\n\
}";

const size_t kBytesPerTileData = 18;
const size_t kChannelsPerAtlasPixel = 4;
const size_t kElementsPerFontmapGlyph = 5;
const size_t kVerticesPerTile = 6;
const int kTilePositionOffset = 16384;
const int kMaxTilePixelDimensions = 65565;

typedef struct rlhTerm_s
{
    size_t AtlasPixelWidth;
    size_t AtlasPixelHeight;
    size_t AtlasPageCount;
    size_t AtlasGlyphCount;
    size_t ConsolePixelWidth;
    size_t ConsolePixelHeight;
    size_t ConsoleTilesWide;
    size_t ConsoleTilesTall;
    float ConsolePixelScale;
    size_t DefaultTilePixelWidth;
    size_t DefaultTilePixelHeight;
    size_t TileDataSize;
    size_t DataTileCount;
    size_t DataReservedTileCount;
    uint8_t* Data;

    // Opengl33 handles
    GLuint Program;
    GLuint VAO;
    GLuint AtlasTEX;
    GLuint DataBUF;
    GLuint DataTEX;
    GLuint FontmapBUF;
    GLuint FontmapTEX;
} rlhTerm_s;

static inline GLuint _rlhCreateGlTextureArray(const size_t pixel_width, const size_t pixel_height, const size_t page_count, const uint8_t* const pixel_rgba)
{
    GLuint gl_texture_array = 0;

    GLD_START();

    // load texture array
    GLD_CALL(glGenTextures(1, &gl_texture_array));
    GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, gl_texture_array));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0));
    GLD_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, pixel_width, pixel_height, page_count, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_rgba));

    GLD_END();

    return gl_texture_array;
}

void rlhClearColor(const rlhColor32_s color)
{
    GLD_CALL(glClearColor((float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f));
    GLD_CALL(glClear(GL_COLOR_BUFFER_BIT));
}
rlhTerm_h rlhTermCreate(const size_t console_tiles_wide, const size_t console_tiles_tall, const float console_pixel_scale, const size_t default_tile_pixel_width, const size_t default_tile_pixel_height, const size_t atlas_pixel_width, const size_t atlas_pixel_height, const size_t atlas_page_count, const uint8_t* atlas_pixel_rgba, const size_t atlas_glyph_count, const float* atlas_glyph_stpqp)
{
    rlhTerm_h term = (rlhTerm_h)malloc(sizeof(rlhTerm_s));
    if (term == NULL) return NULL;
    memset(term, 0, sizeof(rlhTerm_s)); // set all struct properties to 0 by default.
    term->ConsolePixelScale = console_pixel_scale;
    term->ConsoleTilesWide = console_tiles_wide;
    term->ConsoleTilesTall = console_tiles_tall;
    term->DefaultTilePixelWidth = default_tile_pixel_width;
    term->DefaultTilePixelHeight = default_tile_pixel_height;
    term->ConsolePixelWidth = console_tiles_wide * default_tile_pixel_width * console_pixel_scale;
    term->ConsolePixelHeight = console_tiles_tall * default_tile_pixel_height * console_pixel_scale;
    term->DataReservedTileCount = term->ConsoleTilesWide * term->ConsoleTilesTall;
    term->AtlasPixelWidth = atlas_pixel_width;
    term->AtlasPixelHeight = atlas_pixel_height;
    term->AtlasPageCount = atlas_page_count;
    term->AtlasGlyphCount = atlas_glyph_count;

    const size_t atlas_size = sizeof(uint8_t) * atlas_pixel_width * atlas_pixel_height * atlas_page_count * kChannelsPerAtlasPixel;
    const size_t fontmap_size = sizeof(float) * atlas_glyph_count * kElementsPerFontmapGlyph;
    const size_t reserved_data_size = sizeof(uint8_t) * term->DataReservedTileCount * kBytesPerTileData;

    term->Data = (uint8_t*)malloc(reserved_data_size);
    if (term->Data == NULL)
    {
        memset(term, 0, sizeof(rlhTerm_s));
        return term;
    }

    GLD_START();

    { // load shader program
        int gl_vertex_shader, gl_fragment_shader;
        GLD_CALL(gl_vertex_shader = glCreateShader(GL_VERTEX_SHADER));
        GLD_CALL(glShaderSource(gl_vertex_shader, 1, &kVertexSource, NULL));
        GLD_COMPILE(gl_vertex_shader);
        GLD_CALL(gl_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
        GLD_CALL(glShaderSource(gl_fragment_shader, 1, &kFragmentSource, NULL));
        GLD_COMPILE(gl_fragment_shader);
        GLD_CALL(term->Program = glCreateProgram());
        GLD_CALL(glAttachShader(term->Program, gl_vertex_shader));
        GLD_CALL(glAttachShader(term->Program, gl_fragment_shader));
        GLD_LINK(term->Program);
        GLD_CALL(glDetachShader(term->Program, gl_vertex_shader));
        GLD_CALL(glDetachShader(term->Program, gl_fragment_shader));
        GLD_CALL(glDeleteShader(gl_vertex_shader));
        GLD_CALL(glDeleteShader(gl_fragment_shader));
    }

    // generate and bind vertex array object
    GLD_CALL(glGenVertexArrays(1, &term->VAO));
    GLD_CALL(glBindVertexArray(term->VAO));

    // load atlas texture
    term->AtlasTEX = _rlhCreateGlTextureArray(atlas_pixel_width, atlas_pixel_height, atlas_page_count, atlas_pixel_rgba);

    // load fontmap texture buffer object
    GLD_CALL(glGenBuffers(1, &term->FontmapBUF));
    GLD_CALL(glBindBuffer(GL_TEXTURE_BUFFER, term->FontmapBUF));
    GLD_CALL(glBufferData(GL_TEXTURE_BUFFER, (size_t)term->AtlasGlyphCount * kElementsPerFontmapGlyph * sizeof(float), atlas_glyph_stpqp, GL_DYNAMIC_DRAW));
    GLD_CALL(glGenTextures(1, &term->FontmapTEX));
    GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->FontmapTEX));
    GLD_CALL(glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, term->FontmapBUF));

    // set shader uniforms
    GLD_CALL(glUseProgram(term->Program));
    // texture slots
    GLD_CALL(glUniform1i(glGetUniformLocation(term->Program, "Atlas"), 2));
    GLD_CALL(glUniform1i(glGetUniformLocation(term->Program, "Fontmap"), 3));
    GLD_CALL(glUniform1i(glGetUniformLocation(term->Program, "Data"), 4));

    GLD_END();
    return term;
}

void rlhTermDestroy(rlhTerm_h const term)
{
    // free allocated memory
    free(term->Data);

    GLD_START();

    // destroy vertex array object
    if (term->VAO != 0)
    {
        GLD_CALL(glDeleteVertexArrays(1, &term->VAO));
        term->VAO = 0;
    }

    // free data texture buffer object
    if (term->DataBUF != 0)
    {
        GLD_CALL(glDeleteBuffers(1, &term->DataBUF));
        term->DataBUF = 0;
    }
    if (term->DataTEX != 0)
    {
        GLD_CALL(glDeleteTextures(1, &term->DataTEX));
        term->DataTEX = 0;
    }

    GLD_CALL(glDeleteProgram(term->Program));

    // destroy fontmap texture buffer object
    if (term->FontmapBUF != 0)
    {
        GLD_CALL(glDeleteBuffers(1, &term->FontmapBUF));
        term->FontmapBUF = 0;
    }
    if (term->FontmapTEX != 0)
    {
        GLD_CALL(glDeleteTextures(1, &term->FontmapTEX));
        term->FontmapTEX = 0;
    }

    // destroy atlas texture
    if (term->AtlasTEX != 0)
    {
        GLD_CALL(glDeleteTextures(1, &term->AtlasTEX));
        term->AtlasTEX = 0;
    }

    GLD_END();

    // wipe all value in struct to 0
    memset(term, 0, sizeof(rlhTerm_s));
}

void rlhTermResizeConsole(rlhTerm_h const term, const size_t console_tiles_wide, const size_t console_tiles_tall)
{
    term->ConsoleTilesWide = console_tiles_wide;
    term->ConsoleTilesTall = console_tiles_tall;
    term->ConsolePixelWidth = term->ConsoleTilesWide * term->DefaultTilePixelWidth * term->ConsolePixelScale;
    term->ConsolePixelHeight = term->ConsoleTilesTall * term->DefaultTilePixelHeight * term->ConsolePixelScale;
    term->DataTileCount = 0;
}

void rlhTermResizePixelScale(rlhTerm_h const term, const float console_pixel_scale)
{
	term->ConsolePixelScale = console_pixel_scale;
	term->ConsolePixelWidth = term->ConsoleTilesWide * term->DefaultTilePixelWidth * term->ConsolePixelScale;
    term->ConsolePixelHeight = term->ConsoleTilesTall * term->DefaultTilePixelHeight * term->ConsolePixelScale;
}

void rlhTermResizeTiles(rlhTerm_h const term, const size_t default_tile_width, const size_t default_tile_height)
{
    term->DefaultTilePixelWidth = default_tile_width;
    term->DefaultTilePixelHeight = default_tile_height;
    term->ConsolePixelWidth = term->ConsoleTilesWide * term->DefaultTilePixelWidth * term->ConsolePixelScale;
    term->ConsolePixelHeight = term->ConsoleTilesTall * term->DefaultTilePixelHeight * term->ConsolePixelScale;
    term->DataTileCount = 0;
}

void rlhTermGetTileDimensions(rlhTerm_h const term, int* const console_tiles_wide, int* const console_tiles_tall)
{
    if (console_tiles_wide != NULL)
    {
        *console_tiles_wide = term->ConsoleTilesWide;
    }
    if (console_tiles_tall != NULL)
    {
        *console_tiles_tall = term->ConsoleTilesTall;
    }
}

float rlhTermGetPixelScale(rlhTerm_h const term)
{
	return term->ConsolePixelScale;
}

void rlhTermGetPixelDimensions(rlhTerm_h const term, size_t* const console_pixel_width, size_t* const console_pixel_height)
{
    if (console_pixel_width != NULL)
    {
        *console_pixel_width = term->ConsoleTilesWide;
    }
    if (console_pixel_height != NULL)
    {
        *console_pixel_height = term->ConsolePixelHeight;
    }
}

void rlhTermChangeAtlas(rlhTerm_h const term, const size_t atlas_pixel_width, const size_t atlas_pixel_height, const size_t atlas_page_count, const uint8_t* atlas_pixel_rgba, const size_t atlas_glyph_count, const float* atlas_glyph_stpqp)
{
    GLD_START();

    if (term->AtlasTEX != 0)
    {
        GLD_CALL(glDeleteTextures(1, &term->AtlasTEX));
        term->AtlasTEX = 0;
    }

    term->AtlasTEX = _rlhCreateGlTextureArray(atlas_pixel_width, atlas_pixel_height, atlas_page_count, atlas_pixel_rgba);

    GLD_CALL(glBindBuffer(GL_TEXTURE_BUFFER, term->FontmapBUF));
    GLD_CALL(glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * atlas_glyph_count* kElementsPerFontmapGlyph, atlas_glyph_stpqp, GL_DYNAMIC_DRAW));

    GLD_END();
}

void rlhTermClear(rlhTerm_h term)
{
    term->DataTileCount = 0;
}

size_t rlhTermGetTileCount(rlhTerm_h const term)
{
    return term->DataTileCount;
}

static inline uint8_t _rlhTermTryReserve(rlhTerm_h const term)
{
    if (term->DataTileCount == term->DataReservedTileCount) // If we hit the reserved tile count, double the amount of reserved space.
    {
        size_t new_tile_count = term->DataReservedTileCount * 2;
        uint8_t* new_data_ptr = (uint8_t*)realloc(term->Data, new_tile_count * kBytesPerTileData);
        if (new_data_ptr == NULL)
        {
            return 0; //out of memory
        }
        term->Data = new_data_ptr;
        term->DataReservedTileCount = new_tile_count;
    }
    return 1;
}

static inline void _rlhTermPushTile(rlhTerm_h const term, const int pixel_x, const int pixel_y, const int pixel_w, const int pixel_h, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg)
{
    const unsigned int offset_pixel_x = (unsigned int)(pixel_x + kTilePositionOffset);
    const unsigned int offset_pixel_y = (unsigned int)(pixel_y + kTilePositionOffset);
    size_t index = term->DataTileCount * kBytesPerTileData;
    term->Data[index++] = offset_pixel_x & 0xff;
    term->Data[index++] = (offset_pixel_x >> 8) & 0xff;
    term->Data[index++] = offset_pixel_y & 0xff;
    term->Data[index++] = (offset_pixel_y >> 8) & 0xff;
    term->Data[index++] = pixel_w & 0xff;
    term->Data[index++] = (pixel_w >> 8) & 0xff;
    term->Data[index++] = pixel_h & 0xff;
    term->Data[index++] = (pixel_h >> 8) & 0xff;
    term->Data[index++] = (glyph) & 0xff;
    term->Data[index++] = (glyph >> 8) & 0xff;
    term->Data[index++] = fg.r;
    term->Data[index++] = fg.g;
    term->Data[index++] = fg.b;
    term->Data[index++] = fg.a;
    term->Data[index++] = bg.r;
    term->Data[index++] = bg.g;
    term->Data[index++] = bg.b;
    term->Data[index++] = bg.a;
    term->DataTileCount++;
}

void rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg)
{
    if (grid_x < 0 || grid_y < 0 || grid_x > term->ConsoleTilesWide || grid_y > term->ConsoleTilesTall) return;
    const int pixel_x = grid_x * term->DefaultTilePixelWidth;
    const int pixel_y = grid_y * term->DefaultTilePixelHeight;
    if (!_rlhTermTryReserve(term)) return;
    _rlhTermPushTile(term, pixel_x, pixel_y, term->DefaultTilePixelWidth, term->DefaultTilePixelHeight, glyph, fg, bg);
}

void rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg)
{
    if (grid_x < 0 || grid_y < 0 || grid_x > term->ConsoleTilesWide || grid_y > term->ConsoleTilesTall) return;
    if (tile_pixel_width <= 0 || tile_pixel_height <= 0 || tile_pixel_width > kMaxTilePixelDimensions || tile_pixel_height > kMaxTilePixelDimensions) return;
    if (!_rlhTermTryReserve(term)) return;
    const unsigned int pixel_x = (unsigned int)grid_x * term->DefaultTilePixelWidth;
    const unsigned int pixel_y = (unsigned int)grid_y * term->DefaultTilePixelHeight;
    const unsigned int pixel_w_u = (unsigned int)tile_pixel_width;
    const unsigned int pixel_h_u = (unsigned int)tile_pixel_height;
    _rlhTermPushTile(term, pixel_x, pixel_y, pixel_w_u, pixel_h_u, glyph, fg, bg);
}

void rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg)
{
    const int negative_default_width = -(int)term->DefaultTilePixelWidth;
    const int negative_default_height = -(int)term->DefaultTilePixelHeight;
    if (screen_pixel_x < negative_default_width || screen_pixel_y < negative_default_height || screen_pixel_x > (int)term->ConsolePixelWidth || screen_pixel_y > (int)term->ConsolePixelHeight) return;
    if (!_rlhTermTryReserve(term)) return;
    _rlhTermPushTile(term, (unsigned int)screen_pixel_x, (unsigned int)screen_pixel_y, term->DefaultTilePixelWidth, term->DefaultTilePixelHeight, glyph, fg, bg);
}

void rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg)
{
    if (screen_pixel_x < -tile_pixel_width || screen_pixel_y < -tile_pixel_height || screen_pixel_x > (int)term->ConsolePixelWidth || screen_pixel_y > (int)term->ConsolePixelHeight) return;
    if (tile_pixel_width <= 0 || tile_pixel_height <= 0 || tile_pixel_width > kMaxTilePixelDimensions || tile_pixel_height > kMaxTilePixelDimensions) return;
    if (!_rlhTermTryReserve(term)) return;
    _rlhTermPushTile(term, screen_pixel_x, screen_pixel_y, tile_pixel_width, tile_pixel_height, glyph, fg, bg);
}

void rlhViewport(const int x, const int y, const int width, const int height)
{
    GLD_START();

    // set viewport
    GLD_CALL(glViewport(x, y, width, height));

    GLD_END();
}

void rlhTermDraw(rlhTerm_h term)
{
    rlhTermDrawMatrix(term, kOpengl33ScreenMatrix);
}

//Translate an opengl screen matrix so that a rectangle using it is flat facing the screen and its 2d edges match the screen space pixel coordinates given.
static inline void _TransformMatrix(float* matrix, int screen_width, int screen_height, int translate_x, int translate_y, int console_width, int console_height)
{
    float width_scalar = (console_width / ((float)screen_width));
    float height_scalar = (console_height / ((float)screen_height));
    float x_translate = (translate_x / (float)screen_width) * 2.0f;
    float y_translate = (translate_y / (float)screen_height) * 2.0f;
    matrix[0] *= width_scalar;
    matrix[5] *= height_scalar;
    matrix[3] += x_translate;
    matrix[7] -= y_translate;
}

#define MAX(x, y) ((x) > (y)) ? (x) : y

static void _setTermScissor(const int translate_x, const int translate_y, const int width, const int height)
{
	GLD_START();
	
	//crop the scissor area so the position is not less than 0 (this causes opengl error)
	const int cropped_x = MAX(translate_x, 0);
	const int cropped_y = MAX(translate_y, 0);
	
	const int translate_x_difference = cropped_x - translate_x;
	const int translate_y_difference = cropped_y - translate_y;
	
	const int cropped_width = width - translate_x_difference;
	const int cropped_height = height - translate_y_difference;	
	
	// set scissor
	GLD_CALL(glScissor(cropped_x, cropped_y, cropped_width, cropped_height));
	GLD_CALL(glEnable(GL_SCISSOR_TEST));

	GLD_END();
}

void rlhTermDrawTranslated(rlhTerm_h const term, const int translate_x, const int translate_y, const int viewport_width, const int viewport_height)
{
    float matrix[16];
    memcpy(matrix, kOpengl33ScreenMatrix, sizeof(float) * 16);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y, term->ConsolePixelWidth, term->ConsolePixelHeight);

	_setTermScissor(translate_x, translate_y, term->ConsolePixelWidth, term->ConsolePixelHeight);
	
    // draw
    rlhTermDrawMatrix(term, matrix);

	GLD_START();

	// unset the scissor
	GLD_CALL(glDisable(GL_SCISSOR_TEST));

	GLD_END();
}

void rlhTermDrawTransformed(rlhTerm_h const term, const int translate_x, const int translate_y, const float scale_x, const float scale_y, const int viewport_width, const int viewport_height)
{
    float matrix[16];
    memcpy(matrix, kOpengl33ScreenMatrix, sizeof(float) * 16);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y, term->ConsolePixelWidth * scale_x, term->ConsolePixelHeight * scale_y);
	
	_setTermScissor(translate_x, translate_y, term->ConsolePixelWidth * scale_x, term->ConsolePixelHeight * scale_y);
	
    // draw
    rlhTermDrawMatrix(term, matrix);

	GLD_START();

	// unset the scissor
	GLD_CALL(glDisable(GL_SCISSOR_TEST));

	GLD_END();
}

void rlhTermDrawMatrix(rlhTerm_h const term, const float* const matrix_4x4)
{
    if (term->DataTileCount > 0) // only render if there are tiles to render
    {
        GLD_START();

        // bind objects
        GLD_CALL(glBindVertexArray(term->VAO));
        GLD_CALL(glUseProgram(term->Program));

        // fill data texture buffer with new data, and create if not yet exist
        if (term->DataBUF == 0)
        {
            GLD_CALL(glGenBuffers(1, &term->DataBUF));
        }
        GLD_CALL(glBindBuffer(GL_TEXTURE_BUFFER, term->DataBUF));
        GLD_CALL(glBufferData(GL_TEXTURE_BUFFER, term->DataTileCount * kBytesPerTileData, term->Data, GL_STREAM_DRAW));
        if (term->DataTEX == 0)
        {
            GLD_CALL(glGenTextures(1, &term->DataTEX));
            GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->DataTEX));
            GLD_CALL(glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, term->DataBUF));
        }

        // bind the atlas texture
        GLD_CALL(glActiveTexture(GL_TEXTURE2));
        GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, term->AtlasTEX));

        // bind the fontmap
        GLD_CALL(glActiveTexture(GL_TEXTURE3));
        GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->FontmapTEX));

        // bind data texture buffer
        GLD_CALL(glActiveTexture(GL_TEXTURE4));
        GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->DataTEX));

        // set the matrix uniform
        GLD_CALL(glUniformMatrix4fv(glGetUniformLocation(term->Program, "Matrix"), 1, 0, matrix_4x4));

        // set the screen tile dimensions uniform
        GLD_CALL(glUniform2f(glGetUniformLocation(term->Program, "ConsolePixelUnitSize"), (float)term->ConsolePixelScale / (float)term->ConsolePixelWidth, (float)term->ConsolePixelScale / (float)term->ConsolePixelHeight));

        // set blend mode
        GLD_CALL(glEnable(GL_BLEND));
        GLD_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // DRAW!!!
        GLD_CALL(glDrawArrays(GL_TRIANGLES, 0, term->DataTileCount * (size_t)kVerticesPerTile));

        GLD_END();
    }
}
#endif
#ifdef __cplusplus
}
#endif
#endif