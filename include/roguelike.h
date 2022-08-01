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

/*
    roguelike.h version 1.2.0
    Header only roguelike rendering library.
    The source for this library can be found on GitHub:
    https://github.com/Journeyman-dev/roguelike.h

    FEATURES
    - A performant batched terminal rendering system that is similliar to the renderer employed by
      the video game Dwarf Fortress:
    - The entire terminal is rendered in a single draw call.
    - Draw data is passed to the GPU in a giant buffer, which contains elements per tile rather than
      per vertex.
    - The data buffer is compacted as much as possible to improve latency. It contains only 18 bytes
      per tile.
    - On the GPU, most calculation is done per vertex rather than per fragment (pixel), which is
      more efficient.
    - Support for custom glyph atlasses with up to 65535 tiles of custom sizes across multiple
      texture pages.
    - 32 bit fullcolor background and foreground colors per tile.
    - Ability to render tiles on top of each other, with tiles rendered FIFO in the order that they
      are pushed into the terminal.
    - Ability to render tiles offset from gridspace positions.
    - Ability to render tiles with custom width and height per tile.

    HOW TO SETUP
    The roguelike.h library can be included in your project in one of two different ways:
        - Copy and paste the roguelike.h file directly into your source tree.
        - Clone the GitHub as a git submodule to your project's repository, and use the roguelike.h.
          - In bash console from the root directory of your project's repository:

                git submodule add https://github.com/Journeyman-dev/roguelike.h
                git submodule update -init

          - In your project's top level CMakeLists.txt:

                add_submodule(roguelike.h)
                target_add_link_libraries(YOUR_TARGET_NAME PUBLIC rlh)

    After roguelike.h is included in your project, you must implement the library before you can
    actually use it. To implement roguelike.h, create a new .c or .cpp file and write in it the
    following:

            #include <glad/glad.h> // you can use a different opengl loader here (see bellow)
            #include <glDebug.h> // this line is optional (see bellow)
            #define RLH_IMPLEMENTATION
            #include <roguelike.h>

    roguelike.h uses the OpenGL API to do its rendering. There are many open source libraries
    avaliable for loading OpenGL bindings, such as glad (https://github.com/Dav1dde/glad) and GLEW
    (http://glew.sourceforge.net/). It does not matter which loading library you use, but it must
    support at least OpenGL 3.3 core version. A pre-generated glad CMake project targeting this
    version of OpenGL can be found here: https://github.com/Journeyman-dev/glad-opengl33-core.

    If you want to debug the OpenGL API calls within roguelike.h, you can use the glDebug.h
    header only library by including it before implementing roguelike.h. The glDebug.h library is
    avaliable here: https://github.com/Journeyman-dev/glDebug.h. Look at the comment on top of
    that header for more information about its usage.

    HOW TO DEBUG
    Many functions in roguelike.h return an enum value of type rlhresult_t. Result codes with
    names that start with RLH_RESULT_ERROR_ are returned if an error occured in the function's
    execution. These values are all greater than RLH_RESULT_LAST_NON_ERROR.

    String descriptions of result codes are stored in the constant string array variable
    RLH_RESULT_DESCRIPTIONS. To get the string description of a specific result code from this
    array, index the array with the result code.

    This is an example of how to check for errors and print the result code from a function:

        rlhresult_t result = rlhFunction(arg0, arg1, arg2);
        printf("function result description: %s\n", RLH_RESULT_DESCRIPTIONS[result]);
        if (result > RLH_RESULT_LAST_NON_ERROR)
        {
            printf("that was an error!\n");
            return 1;
        }

     As explained above, the OpenGL calls in roguelike.h is also debuggable using glDebug.h library.
     Look in the "HOW TO SETUP" section for more details.

    HOW TO MAKE COLORS
    Colors in roguelike.h are 32 bit RGBA colors, with a each color channel represented by a uint8_t
    value between 0 and 255. Colors are stored in structs of type rlhColor32_s. To quickly create
    a color struct, you can use the included macros such as RLH_RED() or RLH_TRANSPARENT(). You can
    also use the RLH_C32() to quickly create a color with the given rgba values, like:

        rlmhColor32_s my_color = RLH_C32(127, 42, 245);

    HOW TO USE
    To use roguelike.h, you must bind it to an OpenGL context. There are many open source platform
    libraries for creating a window for rendering, including GLFW (https://www.glfw.org/) and SDL
    (https://www.libsdl.org/). An example of using GLFW is included in the roguelike.h repository on
    GitHub in the example folder.

    The general initialization steps for platform libraries are:

        - Initialize the library.
        - Create a window.
        - Load an OpenGL context in the window, usually by passing a function pointer from your
          OpenGL loading library as an argument (loading libraries are explained in the"HOW TO
          SETUP" section).
        - Make the window context "current" so that the next OpenGL function calls will use it.

    After the window is set up for rendering, you can initalize all of your roguelike.h objects. To
    render with roguelike.h, you require:

        - An atlas (rlhAtlas_h) object, which requires stpqp coordinates and bitmap pixel data of
          the atlas image.
        - A terminal object (rlhTerm_h), which represents the rectangle on the window you want to
          draw to.

    For the atlas, "stpqp" texture coordinates are floating point coordinates within the entire
    atlas texture that surround each glyph. The coordinates used go from (0,0) in the upper left
    corner of the entire atlas image, to (1,1) in the bottom right corner. Each glyph requires 5
    floating point coordinates, each represented by one of the letters in stpqp. The s coordinate is
    the left side of the glyph on the x axis, the t is the right side of the glyph on the x axis,
    the p is the top side of the glyph on the y axis, and the q is the bottom side of the glyph
    on the y axis. If you want to, you can utilize store multiple texture pages of the same size
    into the bitmap array. The final p coordinate is the page number that the glyph exists on. If
    you have only one page, this coordinate will always be 0, which is the index for the first page.
    For a detailed example of how to set up an atlas for a 16x16 glyph codepage atlas, look at the
    example on the roguelike.h GitHub repository.

    The atlas also needs raw pixel data for the texture atlas image. You can easily load data from
    an image file using a library such as stb_image.h
    (https://github.com/nothings/stb/blob/master/stb_image.h).

    For the terminal, you need to specify a default pixel size to render tiles to the screen in the
    arguments tile_width and tile_height. This will be the size of each tile on the screen when you
    draw them without resizing, and is the size of each cell in the console grid when you draw your
    glyphs at gridspace positions.

    Terminals need a ratio of a teriminal pixel size to screen pixel size, passed in as the
    pixel_scale argument. If you want evey teriminal pixel to take up a square of 4 screen pixels,
    you can use 2 for this value. If you want the pixels to match the screen pixels, use 1.
    You can use any value greater than 0 for this scalar, even decimal values such as 1.5f.
    However, if you choose to use decimal values, you glyphs may be distorted, so this is not
    reccomended.

    Terminals also require you to specify the size of its rendering space. You can do so by either
    specifying the tile dimensions of the tile grid (rlhTermCreateTileDimensions) or by specifying
    the pixel dimensions of the entire terminal's rect on the screen (rlhTermCreatePixelDimensions).
    If you choose to size your terminal based on pixels, you can pass RLH_TRUE in for the
    floor_pixels_to_tiles argument for the terminal pixel dimensions to automatically be floored to
    the next lowest tile dimension divisible values. This is great for if you have an arbitrarily
    sized window, and you want to have extra space past the edges of the terminal within the window
    where tiles don't quite fit.

    Next you need to create a "render loop", or a loop which will repeat over and over again until
    the window is closed. Usually, this kind of loop can look like the following (platform libary
    specific stuff is in pseudocode):

        while (!windowShouldClose(window)) // platform library specific function to keep looping
        // until the window is closed
        {
          pollEvents(); // Platform library specific function to update the window and its buttons
          // based on mouse clicks, mouse movements, etc.

          // Do your roguelike.h rendering stuff and game logic per frame here.
          // To draw tiles, use the rlhTermPushTileX functions.

          rlhViewport(0, 0, window_width, window_height); // Always do this before the actual
          // drawing so opengl knows the window size.

          rlhClearColor(RLH_BLACK); // Color the entire viewport to
          // clear it of anything left over from previous frames.

          rlhTermDraw(terminal, atlas); // You can use a different rlhTermDrawX function to draw the
          // terminal to the viewport in different ways.
          // When you call an rlhTermDrawX function, the tile data is automatically cleared of all
          // tiles.

          swapBuffers(window); // platform library specific function to swap the window's buffers
        }

    After this loop, you need to delete the resources from both roguelike.h and the platform
    library. Use rlhAtlasDestroy to free the atlas, and then use rlhTermDestroy to free the
    terminal. If you want, you can easily wrap all of this stuff in a C++ class, and
    automatically free all this automatically when the class goes out of scope using RAII
    (https://en.cppreference.com/w/cpp/language/raii).

    For more information about each roguelike.h function you can call, look for comments above their
    declarations further down in this header file.

    CHANGELOG
    - Version 1.2
        Features
            - Added automatic terminal clear after each draw so users don't have to remember
            clear the data themselves to prevent memory leaks.
            - Update the header comment in roguelike.h with a detailed explanation of usage.
            - Result codes have been reordered, and RLH_RESULT_LAST_NON_ERROR was added to make it
            easier to determine if a result code is an error or not.
        Tooling Changes
            - Example projects now expect dependency targets to exist already instead of finding
            them as packages.
    - Version 1.1
        Bugfixes
            - Added missing include to stddef.h
        Tooling Changes
            - Renamed the CMake project to 'rlh'
            - Changed the project's build system so it is no longer monolithic
            - Removed vcpkg.json
            - Removed vcpkg toolchain file setting from CMakeLists.txt
            - Created an interface target the library called 'rlh'
            - Added code linting using trunk
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

// If glDebug.h is not included, make the debug macros do nothing.
#ifndef GLD_H
#  define GLD_START()
#  define GLD_SET_CALLBACK(callback)
#  define GLD_CALL(glFunc) glFunc;
#  define GLD_COMPILE(shaderHandleVar) glCompileShader(shaderHandleVar);
#  define GLD_LINK(programHandleVar) glLinkProgram(programHandleVar);
#endif

  typedef struct rlhColor32_s
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  } rlhColor32_s;

// custom color macro
#define RLH_C32(red, green, blue, alpha) ((rlhColor32_s){(red), (green), (blue), (alpha)})

// standard color macros
#define RLH_RED ((rlhColor32_s){(255), (0), (0), (255)})
#define RLH_LIME ((rlhColor32_s){(0), (255), (0), (255)})
#define RLH_BLUE ((rlhColor32_s){(0), (0), (255), (255)})
#define RLH_WHITE ((rlhColor32_s){(255), (255), (255), (255)})
#define RLH_BLACK ((rlhColor32_s){(0), (0), (0), (255)})
#define RLH_SILVER ((rlhColor32_s){(192), (192), (192), (255)})
#define RLH_GRAY ((rlhColor32_s){(128), (128), (128), (255)})
#define RLH_MAROON ((rlhColor32_s){(128), (0), (0), (255)})
#define RLH_YELLOW ((rlhColor32_s){(255), (255), (0), (255)})
#define RLH_OLIVE ((rlhColor32_s){(128), (128), (0), (255)})
#define RLH_GREEN ((rlhColor32_s){(0), (128), (0), (255)})
#define RLH_AQUA ((rlhColor32_s){(0), (255), (255), (255)})
#define RLH_TEAL ((rlhColor32_s){(0), (128), (128), (255)})
#define RLH_NAVY ((rlhColor32_s){(0), (0), (128), (255)})
#define RLH_FUCHSIA ((rlhColor32_s){(255), (0), (255), (255)})
#define RLH_PURPLE ((rlhColor32_s){(128), (0), (128), (255)})
#define RLH_TRANSPARENT ((rlhColor32_s){(0), (0), (0), (0)})

  typedef enum rlhbool_t
  {
    RLH_FALSE = 0,
    RLH_TRUE = 1
  } rlhbool_t;

  typedef uint16_t rlhglyph_t;

  typedef struct rlhTerm_s* rlhTerm_h;
  typedef struct rlhAtlas_s* rlhAtlas_h;

  typedef enum rlhresult_t
  {
    RLH_RESULT_OK = 0,
    RLH_RESULT_TILE_OUT_OF_TERMINAL = 2,
    RLH_RESULT_LAST_NON_ERROR = RLH_RESULT_TILE_OUT_OF_TERMINAL,
    RLH_RESULT_ERROR_NULL_ARGUMENT = 3,
    RLH_RESULT_ERROR_INVALID_VALUE = 4,
    RLH_RESULT_ERROR_OUT_OF_MEMORY = 5,
    RLH_RESULT_COUNT = 5
  } rlhresult_t;

  extern const char* const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT];

  // Clear the color of the console area with a solid color.
  void rlhClearColor(const rlhColor32_s color);
  // Set viewport area to draw to.
  void rlhViewport(const int x, const int y, const int width, const int height);
  // Create atlas object.
  rlhresult_t rlhAtlasCreate(const int atlas_pixel_width, const int atlas_pixel_height,
                             const int atlas_page_count, const uint8_t* atlas_pixel_rgba,
                             const int atlas_glyph_count, const float* atlas_glyph_stpqp,
                             rlhAtlas_h* const atlas);
  // Destroy the atlas object and free all of its resources.
  void rlhAtlasDestroy(rlhAtlas_h const atlas);
  // Modify an atlas.
  rlhresult_t rlhAtlasSetData(rlhAtlas_h const atlas, const int atlas_pixel_width,
                              const int atlas_pixel_height, const int atlas_page_count,
                              const uint8_t* atlas_pixel_rgba, const int atlas_glyph_count,
                              const float* atlas_glyph_stpqp);
  // Get the pixel dimension and page count of an atlas.
  void rlhAtlasGetDimensions(rlhAtlas_h const atlas, int* const width, int* const height,
                             int* const pages);
  // Get the amount of glyphs in an atlas.
  void rlhAtlasGetGlyphCount(rlhAtlas_h const atlas, int* const glyph_count);
  // Create a terminal object sized based on tile dimensions.
  rlhresult_t rlhTermCreateTileDimensions(const int tiles_wide, const int tiles_tall,
                                          const float pixel_scale, const int tile_width,
                                          const int tile_height, rlhTerm_h* const term);
  // Create a terminal object sized with the specified pixel dimensions.
  rlhresult_t rlhTermCreatePixelDimensions(const int pixel_width, const int pixel_height,
                                           const float pixel_scale, const int tile_width,
                                           const int tile_height,
                                           const rlhbool_t floor_pixels_to_tiles,
                                           rlhTerm_h* const term);
  // Destroy a term object and free all of its resources.
  void rlhTermDestroy(rlhTerm_h const term);
  // Get the size ratio of a terminal pixel per screen pixel.
  float rlhTermGetPixelScale(rlhTerm_h const term);
  // Get the tile dimensions of a console.
  void rlhTermGetTileGridDimensions(rlhTerm_h const term, int* const tiles_wide,
                                    int* const tiles_tall);
  // Get the pixel dimensions of a console.
  void rlhTermGetPixelDimensions(rlhTerm_h const term, int* const pixel_width,
                                 int* const pixel_height);
  // Get the pixel dimensions of a single tile.
  void rlhTermGetTilePixelDimensions(rlhTerm_h const term, int* const tile_width,
                                     int* const tile_height);
  // Resize a terminal object sized based on tile dimensions.
  rlhresult_t rlhTermResizeTileDimensions(rlhTerm_h const term, const int tiles_wide,
                                          const int tiles_tall, const float pixel_scale,
                                          const int tile_width, const int tile_height);
  // Resize a terminal object sized based on pixel dimensions.
  rlhresult_t rlhTermResizePixelDimensions(rlhTerm_h const term, const int pixel_width,
                                           const int pixel_height, const float pixel_scale,
                                           const int tile_width, const int tile_height,
                                           const rlhbool_t floor_pixels_to_tiles);
  // Clear a terminal's data buffer.
  rlhresult_t rlhTermClearTileData(rlhTerm_h const term);
  // Get how many tiles have been set since the last clear.
  size_t rlhTermGetTileCount(rlhTerm_h const term);
  // Push a tile to the terminal that is stretched over the entire terminal area.
  rlhresult_t rlhTermPushFillTile(rlhTerm_h const term, const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg);
  // Push a tile to a terminal in a grid cell position with default pixel width and pixel height.
  rlhresult_t rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y,
                                  const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg);
  // Push a tile to a terminal in a grid cell position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y,
                                       const int tile_pixel_width, const int tile_pixel_height,
                                       const uint16_t glyph, const rlhColor32_s fg,
                                       const rlhColor32_s bg);
  // Push a tile to a terminal in a pixel position with a default pixel width and pixel height.
  rlhresult_t rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x,
                                  const int screen_pixel_y, const rlhglyph_t glyph,
                                  const rlhColor32_s fg, const rlhColor32_s bg);
  // Push a tile to a terminal in a pixel position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x,
                                       const int screen_pixel_y, const int tile_pixel_width,
                                       const int tile_pixel_height, const rlhglyph_t glyph,
                                       const rlhColor32_s fg, const rlhColor32_s bg);
  // Draw a terminal to the current bound framebuffer of the current graphics context. Draws it to
  // fit the viewport.
  rlhresult_t rlhTermDraw(rlhTerm_h const term, rlhAtlas_h const atlas);
  // Draw a terminal centered in a viewport.
  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term, rlhAtlas_h const atlas,
                                  const int viewport_width, const int viewport_height);
  // Draw a terminal translated to by a 2d pixel vector.
  rlhresult_t rlhTermDrawTranslated(rlhTerm_h const term, rlhAtlas_h const atlas,
                                    const int translate_x, const int translate_y,
                                    const int viewport_width, const int viewport_height);
  // Draw a terminal translated to by a 2d pixel vector and scaled by a 2d float vector.
  rlhresult_t rlhTermDrawTransformed(rlhTerm_h const term, rlhAtlas_h const atlas,
                                     const int translate_x, const int translate_y,
                                     const float scale_x, const float scale_y,
                                     const int viewport_width, const int viewport_height);
  // Draw a terminal transformed by a matrix 4x4 (with 16 floats)
  rlhresult_t rlhTermDrawMatrix(rlhTerm_h const term, rlhAtlas_h const atlas,
                                const float* const matrix_4x4);

#ifdef RLH_IMPLEMENTATION

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

  const char* const const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT] = {
      "no errors occured", "tile out of terminal", "unexpected null argument",
      "unexpected argument value", "out of memory"};

  const float RLH_OPENGL_SCREEN_MATRIX[4 * 4] = {2.0f, 0.0f, 0.0f, -1.0f, 0.0f, -2.0f, 0.0f, 1.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f};

  const char* RLH_VERTEX_SOURCE =
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
    float p = 1.0f - texelFetch(Fontmap, index).r;\n\
    index += 1;\n\
    float q = 1.0f - texelFetch(Fontmap, index).r;\n\
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

  const char* RLH_FRAGMENT_SOURCE =
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

  const size_t RLH_DATA_BYTES_PER_TILE = 18;
  const size_t RLH_ATLAS_CHANNELS_PER_PIXEL = 4;
  const size_t RLH_FONTMAP_ELEMENTS_PER_GLYPH = 5;
  const size_t RLH_VERTICES_PER_TILE = 6;
  const int RLH_TILE_POSITION_OFFSET = 16384;
  const int RLH_MAX_TILE_SIZE = 65565;

  typedef struct rlhTerm_s
  {
    size_t PixelWidth;
    size_t PixelHeight;
    size_t TilesWide;
    size_t TilesTall;
    float PixelScale;
    size_t TileWidth;
    size_t TileHeight;
    size_t TileDataCapacity;
    size_t TileDataCount;
    uint8_t* TileData;

    // Opengl33 handles
    GLuint Program;
    GLuint VAO;
    GLuint DataBUF;
    GLuint DataTEX;
  } rlhTerm_s;

  typedef struct rlhAtlas_s
  {
    size_t PixelWidth;
    size_t PixelHeight;
    size_t PageCount;
    size_t GlyphCount;

    // Opengl33 handles
    GLuint AtlasTEX;
    GLuint FontmapBUF;
    GLuint FontmapTEX;
  } rlhAtlas_s;

  static inline GLuint _rlhCreateGlTextureArray(const size_t pixel_width, const size_t pixel_height,
                                                const size_t page_count,
                                                const uint8_t* const pixel_rgba)
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
    GLD_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, pixel_width, pixel_height, page_count,
                          0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_rgba));

    return gl_texture_array;
  }

  void rlhClearColor(const rlhColor32_s color)
  {
    GLD_START();

    GLD_CALL(glClearColor((float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f,
                          (float)color.a / 255.0f));
    GLD_CALL(glClear(GL_COLOR_BUFFER_BIT));
  }

  void rlhViewport(const int x, const int y, const int width, const int height)
  {
    GLD_START();

    // set viewport
    GLD_CALL(glViewport(x, y, width, height));
  }

  rlhresult_t rlhAtlasCreate(const int atlas_pixel_width, const int atlas_pixel_height,
                             const int atlas_page_count, const uint8_t* atlas_pixel_rgba,
                             const int atlas_glyph_count, const float* atlas_glyph_stpqp,
                             rlhAtlas_h* const atlas)
  {
    if (atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (atlas_pixel_width <= 0 || atlas_pixel_height <= 0 || atlas_page_count <= 0 ||
        atlas_glyph_count <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    *atlas = (rlhAtlas_h)malloc(sizeof(rlhAtlas_s));
    if (*atlas == NULL)
    {
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }

    memset(*atlas, 0, sizeof(rlhAtlas_s));  // set all struct properties to 0 by default.

    (*atlas)->PixelWidth = (size_t)atlas_pixel_width;
    (*atlas)->PixelHeight = (size_t)atlas_pixel_height;
    (*atlas)->PageCount = (size_t)atlas_page_count;
    (*atlas)->GlyphCount = (size_t)atlas_glyph_count;

    const size_t atlas_size = sizeof(uint8_t) * (size_t)atlas_pixel_width *
                              (size_t)atlas_pixel_height * (size_t)atlas_page_count *
                              RLH_ATLAS_CHANNELS_PER_PIXEL;
    const size_t fontmap_size =
        sizeof(float) * (size_t)atlas_glyph_count * RLH_FONTMAP_ELEMENTS_PER_GLYPH;

    (*atlas)->AtlasTEX =
        _rlhCreateGlTextureArray((size_t)atlas_pixel_width, (size_t)atlas_pixel_height,
                                 (size_t)atlas_page_count, atlas_pixel_rgba);

    GLD_START();

    // load fontmap texture buffer object
    GLD_CALL(glGenBuffers(1, &(*atlas)->FontmapBUF));
    GLD_CALL(glBindBuffer(GL_TEXTURE_BUFFER, (*atlas)->FontmapBUF));
    GLD_CALL(
        glBufferData(GL_TEXTURE_BUFFER,
                     (GLsizei)atlas_glyph_count * RLH_FONTMAP_ELEMENTS_PER_GLYPH * sizeof(float),
                     atlas_glyph_stpqp, GL_DYNAMIC_DRAW));
    GLD_CALL(glGenTextures(1, &(*atlas)->FontmapTEX));
    GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, (*atlas)->FontmapTEX));
    GLD_CALL(glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, (*atlas)->FontmapBUF));

    return RLH_RESULT_OK;
  }

  void rlhAtlasDestroy(rlhAtlas_h const atlas)
  {
    if (atlas == NULL) return;

    // destroy fontmap
    if (atlas->FontmapBUF != 0)
    {
      GLD_CALL(glDeleteBuffers(1, &atlas->FontmapBUF));
      atlas->FontmapBUF = 0;
    }
    if (atlas->FontmapTEX != 0)
    {
      GLD_CALL(glDeleteTextures(1, &atlas->FontmapTEX));
      atlas->FontmapTEX = 0;
    }

    // destroy atlas texture
    if (atlas->AtlasTEX != 0)
    {
      GLD_CALL(glDeleteTextures(1, &atlas->AtlasTEX));
      atlas->AtlasTEX = 0;
    }

    // free the struct
    free(atlas);
  }

  rlhresult_t rlhAtlasSetData(rlhAtlas_h const atlas, const int atlas_pixel_width,
                              const int atlas_pixel_height, const int atlas_page_count,
                              const uint8_t* atlas_pixel_rgba, const int atlas_glyph_count,
                              const float* atlas_glyph_stpqp)
  {
    if (atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (atlas_pixel_width <= 0 || atlas_pixel_height <= 0 || atlas_page_count <= 0 ||
        atlas_glyph_count <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    GLD_START();

    if (atlas->AtlasTEX != 0)
    {
      GLD_CALL(glDeleteTextures(1, &atlas->AtlasTEX));
      atlas->AtlasTEX = 0;
    }

    atlas->AtlasTEX =
        _rlhCreateGlTextureArray((size_t)atlas_pixel_width, (size_t)atlas_pixel_height,
                                 (size_t)atlas_page_count, atlas_pixel_rgba);

    GLD_CALL(glBindBuffer(GL_TEXTURE_BUFFER, atlas->FontmapBUF));
    GLD_CALL(
        glBufferData(GL_TEXTURE_BUFFER,
                     sizeof(float) * (GLsizei)atlas_glyph_count * RLH_FONTMAP_ELEMENTS_PER_GLYPH,
                     atlas_glyph_stpqp, GL_DYNAMIC_DRAW));

    return RLH_RESULT_OK;
  }

  void rlhAtlasGetDimensions(rlhAtlas_h const atlas, int* const width, int* const height,
                             int* const pages)
  {
    if (atlas == NULL)
    {
      return;
    }

    if (width != NULL)
    {
      *width = atlas->PixelWidth;
    }
    if (height != NULL)
    {
      *height = atlas->PixelHeight;
    }
    if (pages != NULL)
    {
      *pages = atlas->PageCount;
    }
  }

  void rlhAtlasGetGlyphCount(rlhAtlas_h const atlas, int* const glyph_count)
  {
    if (atlas == NULL)
    {
      return;
    }

    if (glyph_count != NULL)
    {
      *glyph_count = (int)atlas->GlyphCount;
    }
  }

  rlhresult_t rlhTermCreateTileDimensions(const int tiles_wide, const int tiles_tall,
                                          const float pixel_scale, const int tile_width,
                                          const int tile_height, rlhTerm_h* const term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (tiles_wide <= 0 || tiles_tall <= 0 || pixel_scale <= 0.0f || tile_width <= 0 ||
        tile_height <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    const int pixel_width = tiles_wide * tile_width * pixel_scale;
    const int pixel_height = tiles_tall * tile_height * pixel_scale;

    return rlhTermCreatePixelDimensions(pixel_width, pixel_height, pixel_scale, tile_width,
                                        tile_height, RLH_FALSE, term);
  }

  rlhresult_t rlhTermCreatePixelDimensions(const int pixel_width, const int pixel_height,
                                           const float pixel_scale, const int tile_width,
                                           const int tile_height,
                                           const rlhbool_t floor_pixels_to_tiles,
                                           rlhTerm_h* const term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (pixel_width <= 0 || pixel_height <= 0 || pixel_scale <= 0.0f || tile_width <= 0 ||
        tile_height <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    *term = (rlhTerm_h)malloc(sizeof(rlhTerm_s));

    if (*term == NULL)
    {
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }

    memset(*term, 0, sizeof(rlhTerm_s));  // set all struct properties to 0 by default.

    (*term)->PixelScale = pixel_scale;
    (*term)->TilesWide = (size_t)pixel_width / ((size_t)tile_width * pixel_scale);
    (*term)->TilesTall = (size_t)pixel_height / ((size_t)tile_height * pixel_scale);
    (*term)->TileWidth = (size_t)tile_width;
    (*term)->TileHeight = (size_t)tile_height;
    if (floor_pixels_to_tiles)
    {
      (*term)->PixelWidth = (*term)->TilesWide * (*term)->TileWidth * pixel_scale;
      (*term)->PixelHeight = (*term)->TilesTall * (*term)->TileHeight * pixel_scale;
    }
    else
    {
      (*term)->PixelWidth = (size_t)pixel_width;
      (*term)->PixelHeight = (size_t)pixel_height;
    }

    (*term)->TileDataCapacity = (*term)->TilesWide * (*term)->TilesTall;
    const size_t reserved_data_size =
        sizeof(uint8_t) * (*term)->TileDataCapacity * RLH_DATA_BYTES_PER_TILE;
    (*term)->TileData = (uint8_t*)malloc(reserved_data_size);
    if ((*term)->TileData == NULL)
    {
      free(*term);
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }

    GLD_START();

    {  // load shader program
      int gl_vertex_shader, gl_fragment_shader;
      GLD_CALL(gl_vertex_shader = glCreateShader(GL_VERTEX_SHADER));
      GLD_CALL(glShaderSource(gl_vertex_shader, 1, &RLH_VERTEX_SOURCE, NULL));
      GLD_COMPILE(gl_vertex_shader, "rlh vertex shader");
      GLD_CALL(gl_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
      GLD_CALL(glShaderSource(gl_fragment_shader, 1, &RLH_FRAGMENT_SOURCE, NULL));
      GLD_COMPILE(gl_fragment_shader, "rlh fragment shader");
      GLD_CALL((*term)->Program = glCreateProgram());
      GLD_CALL(glAttachShader((*term)->Program, gl_vertex_shader));
      GLD_CALL(glAttachShader((*term)->Program, gl_fragment_shader));
      GLD_LINK((*term)->Program, "rlh shader program");
      GLD_CALL(glDetachShader((*term)->Program, gl_vertex_shader));
      GLD_CALL(glDetachShader((*term)->Program, gl_fragment_shader));
      GLD_CALL(glDeleteShader(gl_vertex_shader));
      GLD_CALL(glDeleteShader(gl_fragment_shader));
    }

    // generate and bind vertex array object
    GLD_CALL(glGenVertexArrays(1, &(*term)->VAO));
    GLD_CALL(glBindVertexArray((*term)->VAO));

    // set shader uniforms
    GLD_CALL(glUseProgram((*term)->Program));
    // texture slots
    GLD_CALL(glUniform1i(glGetUniformLocation((*term)->Program, "Atlas"), 2));
    GLD_CALL(glUniform1i(glGetUniformLocation((*term)->Program, "Fontmap"), 3));
    GLD_CALL(glUniform1i(glGetUniformLocation((*term)->Program, "Data"), 4));

    return RLH_RESULT_OK;
  }

  void rlhTermDestroy(rlhTerm_h const term)
  {
    if (term == NULL) return;

    // free allocated memory
    free(term->TileData);

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

    // free the struct
    free(term);
  }

  float rlhTermGetPixelScale(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0.0f;
    }

    return term->PixelScale;
  }

  void rlhTermGetTileGridDimensions(rlhTerm_h const term, int* const tiles_wide,
                                    int* const tiles_tall)
  {
    if (term == NULL)
    {
      return;
    }

    if (tiles_wide != NULL)
    {
      *tiles_wide = term->TilesWide;
    }
    if (tiles_tall != NULL)
    {
      *tiles_tall = term->TilesTall;
    }
  }

  void rlhTermGetPixelDimensions(rlhTerm_h const term, int* const pixel_width,
                                 int* const pixel_height)
  {
    if (term == NULL)
    {
      return;
    }

    if (pixel_width != NULL)
    {
      *pixel_width = (int)term->PixelWidth;
    }
    if (pixel_height != NULL)
    {
      *pixel_height = (int)term->PixelHeight;
    }
  }

  void rlhTermGetTileDimensions(rlhTerm_h const term, size_t* const tile_width,
                                size_t* const tile_height)
  {
    if (term == NULL)
    {
      return;
    }

    if (tile_width != NULL)
    {
      *tile_width = term->TileWidth;
    }
    if (tile_height != NULL)
    {
      *tile_height = term->TileHeight;
    }
  }

  rlhresult_t rlhTermClearTileData(rlhTerm_h term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    term->TileDataCount = 0;

    return RLH_RESULT_OK;
  }

  size_t rlhTermGetTileCount(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0;
    }

    return term->TileDataCount;
  }

  rlhresult_t rlhTermResizeTileDimensions(rlhTerm_h const term, const int tiles_wide,
                                          const int tiles_tall, const float pixel_scale,
                                          const int tile_width, const int tile_height)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (tiles_wide <= 0 || tiles_tall <= 0 || pixel_scale <= 0.0f || tile_width <= 0 ||
        tile_height <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    const int pixel_width = tiles_wide * tile_width * pixel_scale;
    const int pixel_height = tiles_tall * tile_height * pixel_scale;
    return rlhTermResizePixelDimensions(term, pixel_width, pixel_height, pixel_scale, tile_width,
                                        tile_height, RLH_FALSE);
  }

  rlhresult_t rlhTermResizePixelDimensions(rlhTerm_h const term, const int pixel_width,
                                           const int pixel_height, const float pixel_scale,
                                           const int tile_width, const int tile_height,
                                           const rlhbool_t floor_pixels_to_tiles)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    if (pixel_width <= 0 || pixel_height <= 0 || pixel_scale <= 0.0f || tile_width <= 0 ||
        tile_height <= 0)
    {
      return RLH_RESULT_ERROR_INVALID_VALUE;
    }

    term->PixelScale = pixel_scale;
    term->TilesWide = (size_t)pixel_width / ((size_t)tile_width * pixel_scale);
    term->TilesTall = (size_t)pixel_height / ((size_t)tile_height * pixel_scale);
    term->TileWidth = (size_t)tile_width;
    term->TileHeight = (size_t)tile_height;
    if (floor_pixels_to_tiles)
    {
      term->PixelWidth = term->TilesWide * term->TileWidth * pixel_scale;
      term->PixelHeight = term->TilesTall * term->TileHeight * pixel_scale;
    }
    else
    {
      term->PixelWidth = (size_t)pixel_width;
      term->PixelHeight = (size_t)pixel_height;
    }

    // clear tile data because it may no longer fit
    term->TileDataCount = 0;

    return RLH_RESULT_OK;
  }

  static inline uint8_t _rlhTermTryReserve(rlhTerm_h const term)
  {
    if (term->TileDataCount == term->TileDataCapacity)  // If we hit the reserved tile count, double
                                                        // the amount of reserved space.
    {
      size_t new_capacity = term->TileDataCapacity * 2;
      uint8_t* new_data_ptr =
          (uint8_t*)realloc(term->TileData, new_capacity * RLH_DATA_BYTES_PER_TILE);
      if (new_data_ptr == NULL)
      {
        return RLH_FALSE;  // out of memory
      }
      term->TileData = new_data_ptr;
      term->TileDataCapacity = new_capacity;
    }
    return RLH_TRUE;
  }

  static inline void _rlhTermPushTile(rlhTerm_h const term, const int pixel_x, const int pixel_y,
                                      const int pixel_w, const int pixel_h, const uint16_t glyph,
                                      const rlhColor32_s fg, const rlhColor32_s bg)
  {
    const unsigned int offset_pixel_x = (unsigned int)(pixel_x + RLH_TILE_POSITION_OFFSET);
    const unsigned int offset_pixel_y = (unsigned int)(pixel_y + RLH_TILE_POSITION_OFFSET);
    size_t index = term->TileDataCount * RLH_DATA_BYTES_PER_TILE;
    term->TileData[index++] = offset_pixel_x & 0xff;
    term->TileData[index++] = (offset_pixel_x >> 8) & 0xff;
    term->TileData[index++] = offset_pixel_y & 0xff;
    term->TileData[index++] = (offset_pixel_y >> 8) & 0xff;
    term->TileData[index++] = pixel_w & 0xff;
    term->TileData[index++] = (pixel_w >> 8) & 0xff;
    term->TileData[index++] = pixel_h & 0xff;
    term->TileData[index++] = (pixel_h >> 8) & 0xff;
    term->TileData[index++] = (glyph)&0xff;
    term->TileData[index++] = (glyph >> 8) & 0xff;
    term->TileData[index++] = fg.r;
    term->TileData[index++] = fg.g;
    term->TileData[index++] = fg.b;
    term->TileData[index++] = fg.a;
    term->TileData[index++] = bg.r;
    term->TileData[index++] = bg.g;
    term->TileData[index++] = bg.b;
    term->TileData[index++] = bg.a;
    term->TileDataCount++;
  }

  rlhresult_t rlhTermPushFillTile(rlhTerm_h const term, const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg)
  {
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, 0, 0, term->PixelWidth, term->PixelHeight, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y,
                                  const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg)
  {
    if (grid_x < 0 || grid_y < 0 || grid_x > term->TilesWide || grid_y > term->TilesTall)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    const int pixel_x = grid_x * term->TileWidth;
    const int pixel_y = grid_y * term->TileHeight;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, pixel_x, pixel_y, term->TileWidth, term->TileHeight, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y,
                                       const int tile_pixel_width, const int tile_pixel_height,
                                       const uint16_t glyph, const rlhColor32_s fg,
                                       const rlhColor32_s bg)
  {
    if (grid_x < 0 || grid_y < 0 || grid_x > term->TilesWide || grid_y > term->TilesTall)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (tile_pixel_width <= 0 || tile_pixel_height <= 0 || tile_pixel_width > RLH_MAX_TILE_SIZE ||
        tile_pixel_height > RLH_MAX_TILE_SIZE)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    const unsigned int pixel_x = (unsigned int)grid_x * term->TileWidth;
    const unsigned int pixel_y = (unsigned int)grid_y * term->TileHeight;
    const unsigned int pixel_w_u = (unsigned int)tile_pixel_width;
    const unsigned int pixel_h_u = (unsigned int)tile_pixel_height;
    _rlhTermPushTile(term, pixel_x, pixel_y, pixel_w_u, pixel_h_u, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x,
                                  const int screen_pixel_y, const uint16_t glyph,
                                  const rlhColor32_s fg, const rlhColor32_s bg)
  {
    const int negative_default_width = -(int)term->TileWidth;
    const int negative_default_height = -(int)term->TileHeight;
    if (screen_pixel_x < negative_default_width || screen_pixel_y < negative_default_height ||
        screen_pixel_x > (int)term->TileWidth || screen_pixel_y > (int)term->TileHeight)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, (unsigned int)screen_pixel_x, (unsigned int)screen_pixel_y,
                     term->TileWidth, term->TileHeight, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x,
                                       const int screen_pixel_y, const int tile_pixel_width,
                                       const int tile_pixel_height, const uint16_t glyph,
                                       const rlhColor32_s fg, const rlhColor32_s bg)
  {
    if (screen_pixel_x < -tile_pixel_width || screen_pixel_y < -tile_pixel_height ||
        screen_pixel_x > (int)term->PixelWidth || screen_pixel_y > (int)term->PixelHeight)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (tile_pixel_width <= 0 || tile_pixel_height <= 0 || tile_pixel_width > RLH_MAX_TILE_SIZE ||
        tile_pixel_height > RLH_MAX_TILE_SIZE)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, screen_pixel_x, screen_pixel_y, tile_pixel_width, tile_pixel_height,
                     glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDraw(rlhTerm_h term, rlhAtlas_h const atlas)
  {
    if (term == NULL || atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    rlhTermDrawMatrix(term, atlas, RLH_OPENGL_SCREEN_MATRIX);

    return RLH_RESULT_OK;
  }

  // Translate an opengl screen matrix so that a rectangle using it is flat facing the screen and
  // its 2d edges match the screen space pixel coordinates given.
  static inline void _TransformMatrix(float* matrix, int screen_width, int screen_height,
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

#  define MAX(x, y) ((x) > (y)) ? (x) : y

  static void _setTermScissor(const int translate_x, const int translate_y, const int width,
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

  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term, rlhAtlas_h const atlas,
                                  const int viewport_width, const int viewport_height)
  {
    if (term == NULL || atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    const int width_difference = viewport_width - term->PixelWidth;
    const int height_difference = viewport_height - term->PixelHeight;
    const int translate_x = width_difference / 2;
    const int translate_y = height_difference / 2;

    rlhTermDrawTranslated(term, atlas, translate_x, translate_y, viewport_width, viewport_height);

    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawTranslated(rlhTerm_h const term, rlhAtlas_h const atlas,
                                    const int translate_x, const int translate_y,
                                    const int viewport_width, const int viewport_height)
  {
    if (term == NULL || atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    float matrix[16];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * 16);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                     term->PixelWidth, term->PixelHeight);

    _setTermScissor(translate_x, translate_y, term->PixelWidth, term->PixelHeight);

    // draw
    rlhTermDrawMatrix(term, atlas, matrix);

    GLD_START();

    // unset the scissor
    GLD_CALL(glDisable(GL_SCISSOR_TEST));

    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawTransformed(rlhTerm_h const term, rlhAtlas_h const atlas,
                                     const int translate_x, const int translate_y,
                                     const float scale_x, const float scale_y,
                                     const int viewport_width, const int viewport_height)
  {
    if (term == NULL || atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    float matrix[16];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * 16);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                     term->PixelWidth * scale_x, term->PixelHeight * scale_y);

    _setTermScissor(translate_x, translate_y, term->PixelWidth * scale_x,
                    term->PixelHeight * scale_y);

    // draw
    rlhTermDrawMatrix(term, atlas, matrix);

    GLD_START();

    // unset the scissor
    GLD_CALL(glDisable(GL_SCISSOR_TEST));

    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermDrawMatrix(rlhTerm_h const term, rlhAtlas_h const atlas,
                                const float* const matrix_4x4)
  {
    if (term == NULL || atlas == NULL || matrix_4x4 == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    if (term->TileDataCount > 0)  // only render if there are tiles to render
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
      GLD_CALL(glBufferData(GL_TEXTURE_BUFFER, term->TileDataCount * RLH_DATA_BYTES_PER_TILE,
                            term->TileData, GL_STREAM_DRAW));
      if (term->DataTEX == 0)
      {
        GLD_CALL(glGenTextures(1, &term->DataTEX));
        GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->DataTEX));
        GLD_CALL(glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, term->DataBUF));
      }

      // bind the atlas texture
      GLD_CALL(glActiveTexture(GL_TEXTURE2));
      GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, atlas->AtlasTEX));

      // bind the fontmap
      GLD_CALL(glActiveTexture(GL_TEXTURE3));
      GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, atlas->FontmapTEX));

      // bind data texture buffer
      GLD_CALL(glActiveTexture(GL_TEXTURE4));
      GLD_CALL(glBindTexture(GL_TEXTURE_BUFFER, term->DataTEX));

      // set the matrix uniform
      GLD_CALL(glUniformMatrix4fv(glGetUniformLocation(term->Program, "Matrix"), 1, 0, matrix_4x4));

      // set the screen tile dimensions uniform
      GLD_CALL(glUniform2f(glGetUniformLocation(term->Program, "ConsolePixelUnitSize"),
                           term->PixelScale / (float)term->PixelWidth,
                           term->PixelScale / (float)term->PixelHeight));

      // set blend mode
      GLD_CALL(glEnable(GL_BLEND));
      GLD_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      // DRAW!!!
      GLD_CALL(glDrawArrays(GL_TRIANGLES, 0, term->TileDataCount * RLH_VERTICES_PER_TILE));

      // if we don't want to retain the tiles between each frame, clear them.
      #if !defined(RLH_RETAINED_MODE)
      rlhTermClearTileData(term);
      #endif
    }

    return RLH_RESULT_OK;
  }
#endif
#ifdef __cplusplus
}
#endif
#endif
