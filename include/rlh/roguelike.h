// SPDX-FileCopyrightText: 2021-2022 Daniel Aimé Valcour <fosssweeper@gmail.com>
//
// SPDX-License-Identifier: MIT

/*
    Copyright (c) 2021-2022  Daniel Aimé Valcour
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
    The roguelike.h library can be included in your project in one of three different ways:
        - Copy and paste the roguelike.h file directly into your source tree.
        - Clone the GitHub as a git submodule to your project's repository.
          1. In bash console from the root directory of your project's repository:

                git submodule add https://github.com/Journeyman-dev/roguelike.h
                git submodule update --init

          2. Add the submodule folder as a subdirectory in your project's CMakeLists.txt:

                add_subdirectory(roguelike.h)

          3. Link it with the CMake target of your project:

                target_add_link_libraries(YOUR_TARGET_NAME PRIVATE rlh::rlh)

        - Fetch it using CMake FetchContent:
         1. Fetch it in your project's CMakeLists.txt:

                Include(FetchContent)
                FetchContent_Declare(
                        rlh
                        GIT_REPOSITORY https://github.com/Journeyman-dev/roguelike.h
                        GIT_TAG        v1.0.0 # replace with the version of the library that you want
                        FIND_PACKAGE_ARGS
                )
                set(RLH_BUILD_EXAMPLE OFF) # set this to ON if you want to build the rlh example target
                FetchContent_MakeAvailable(rlh)

          2. Link it with the CMake target of your project:

              target_link_libraries(YOUR_TARGET_NAME PRIVATE rlh::rlh)

    After roguelike.h is included in your project, you must implement the library before you can
    actually use it. To implement roguelike.h, create a new .c or .cpp file and write in it the
    following:

            #include <glad/glad.h> // you can use a different opengl loader here (see bellow)
            #include <glDebug.h> // this line is optional (see bellow)
            #define RLH_RETAINED_MODE // optional (see bellow)
            #define RLH_IMPLEMENTATION
            #include <rlh/roguelike.h>

    roguelike.h uses the OpenGL API to do its rendering. There are many open source libraries
    avaliable for loading OpenGL bindings, such as glad (https://github.com/Dav1dde/glad) and GLEW
    (http://glew.sourceforge.net/). It does not matter which loading library you use, but it must
    support at least OpenGL 3.3 core version. A pre-generated glad CMake project targeting this
    version of OpenGL can be found here: https://github.com/Journeyman-dev/glad-opengl33-core.

    If you want to debug the OpenGL API calls within roguelike.h, you can use the glDebug.h
    header only library by including it before implementing roguelike.h. The glDebug.h library is
    avaliable here: https://github.com/Journeyman-dev/glDebug.h. Look at the comment on top of
    that header for more information about its usage.

    By default, roguelike.h will clear the tile buffer after each draw. This requires you to set
    all tiles over again the next frame from scratch. If you want to instead retain tiles from
    previous draws unless you explicitly clear the tile buffer with the function
    rlhTermClearTileData(), define RLH_RETAINED_MODE before implementing the header. If you do this,
    be very careful! If you forget to clear the tile buffer and keep adding tiles to it over time,
    this can result in a nasty memory leak.

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

    Terminals need a ratio of a terminal pixel size to screen pixel size, passed in as the
    pixel_scale argument. If you want evey teriminal pixel to take up a square of 4 screen pixels,
    you can use 2 for this value. If you want the pixels to match the screen pixels, use 1.
    You can use any value greater than 0 for this scalar, even decimal values such as 1.5f.
    However, if you choose to use decimal values, you glyphs may be distorted, so this is not
    recomended.

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
#  define GLD_START()
#  define GLD_SET_CALLBACK(callback)
#  define GLD_CALL(glFunc) glFunc;
#  define GLD_COMPILE(shaderHandleVar, identifier) glCompileShader(shaderHandleVar);
#  define GLD_LINK(programHandleVar, identifier) glLinkProgram(programHandleVar);
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

  typedef enum rlhresult_t
  {
    RLH_RESULT_OK = 0,
    RLH_RESULT_TILE_OUT_OF_TERMINAL = 1,
    RLH_RESULT_LAST_NON_ERROR = RLH_RESULT_TILE_OUT_OF_TERMINAL,
    RLH_RESULT_ERROR_NULL_ARGUMENT = 2,
    RLH_RESULT_ERROR_INVALID_VALUE = 3,
    RLH_RESULT_ERROR_OUT_OF_MEMORY = 4,
    RLH_RESULT_COUNT
  } rlhresult_t;

  extern const char* const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT];

  typedef enum rlhcolortype_t
  {
    RLH_COLOR_G,
    RLH_COLOR_GA,
    RLH_COLOR_RGBA,
    RLH_COLOR_BGRA,
    RLH_COLOR_TYPE_COUNT
  } rlhcolortype_t;

  extern const char* const RLH_COLOR_TYPE_NAMES[RLH_COLOR_TYPE_COUNT];

  // Clear the color of the console area with a solid color.
  void rlhClearColor(const rlhColor32_s color);
  // Set viewport area to draw to.
  void rlhViewport(int x, int y, int width, int height);
  // Create a terminal object sized based on tile dimensions.
  rlhresult_t rlhTermCreateTileDimensions(const int tiles_wide, const int tiles_tall, const float pixel_scale, const int tile_width, const int tile_height, rlhTerm_h* const term);
  // Create a terminal object sized with the specified pixel dimensions.
  rlhresult_t rlhTermCreatePixelDimensions(const int pixel_width, const int pixel_height, const float pixel_scale, const int tile_width, const int tile_height, const rlhbool_t floor_pixels_to_tiles, rlhTerm_h* const term);
  // Destroy a term object and free all of its resources.
  void rlhTermDestroy(rlhTerm_h const term);
  // Set the atlas of a terminal.
  rlhresult_t rlhTermSetAtlas(rlhTerm_h const term, const int width, const int height, const int pages, const uint8_t* pixels, rlhcolortype_t color, const int glyph_count, const float* glyph_stpqp);
  // Get the pixel dimension and page count of an atlas.
  void rlhTermGetAtlasDimensions(rlhTerm_h const term, int* const width, int* const height, int* const pages);
  // Get the amount of glyphs in a terminal's atlas.
  int rlhTermGetAtlasGlyphCount(rlhTerm_h const term, int* const glyph_count);
  // Get the size ratio of a terminal pixel per screen pixel.
  float rlhTermGetPixelScale(rlhTerm_h const term);
  // Get the tile dimensions of a console.
  void rlhTermGetTileGridDimensions(rlhTerm_h const term, int* const tiles_wide, int* const tiles_tall);
  // Get the pixel dimensions of a console.
  void rlhTermGetPixelDimensions(rlhTerm_h const term, int* const pixel_width, int* const pixel_height);
  // Get the pixel dimensions of a single tile.
  void rlhTermGetTilePixelDimensions(rlhTerm_h const term, int* const tile_width, int* const tile_height);
  // Resize a terminal object sized based on tile dimensions.
  rlhresult_t rlhTermResizeTileDimensions(rlhTerm_h const term, const int tiles_wide, const int tiles_tall, const float pixel_scale, const int tile_width, const int tile_height);
  // Resize a terminal object sized based on pixel dimensions.
  rlhresult_t rlhTermResizePixelDimensions(rlhTerm_h const term, const int pixel_width, const int pixel_height, const float pixel_scale, const int tile_width, const int tile_height, const rlhbool_t floor_pixels_to_tiles);
  // Clear a terminal's data buffer.
  rlhresult_t rlhTermClearTileData(rlhTerm_h const term);
  // Get how many tiles have been set since the last clear.
  size_t rlhTermGetTileCount(rlhTerm_h const term);
  // Push a tile to the terminal that is stretched over the entire terminal area.
  rlhresult_t rlhTermPushFillTile(rlhTerm_h const term, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
  // Push a tile to a terminal in a grid cell position with default pixel width and pixel height.
  rlhresult_t rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
  // Push a tile to a terminal in a grid cell position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y, const int tile_pixel_width, const int tile_pixel_height, const uint16_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
  // Push a tile to a terminal in a pixel position with a default pixel width and pixel height.
  rlhresult_t rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const rlhglyph_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
  // Push a tile to a terminal in a pixel position with a custom pixel width and pixel height.
  rlhresult_t rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x, const int screen_pixel_y, const int tile_pixel_width, const int tile_pixel_height, const rlhglyph_t glyph, const rlhColor32_s fg, const rlhColor32_s bg);
  // Draw a terminal to the current bound framebuffer of the current graphics context. Draws it to fit the viewport, which might distort pixels.
  rlhresult_t rlhTermDraw(rlhTerm_h const term, , rlhbool_t retained);
  // Draw a terminal pixel perfect, centered in the viewport.
  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term, const int viewport_width, const int viewport_height, rlhbool_t retained );
  // Draw a terminal pixel perfect, translated to by a 2d pixel vector.
  rlhresult_t rlhTermDrawTranslated(rlhTerm_h const term, const int translate_x, const int translate_y, const int viewport_width, const int viewport_height, rlhbool_t retained);
  // Draw a terminal translated to by a 2d pixel vector and scaled by a 2d float vector.
  rlhresult_t rlhTermDrawTransformed(rlhTerm_h const term, const int translate_x, const int translate_y, const float scale_x, const float scale_y, const int viewport_width, const int viewport_height, rlhbool_t retained);
  // Draw a terminal transformed by a matrix 4x4 (with 16 floats)
  rlhresult_t rlhTermDrawMatrix(rlhTerm_h const term, const float* const matrix_4x4, rlhbool_t retained);

#ifdef RLH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define MAX(x, y) ((x) > (y)) ? (x) : y
#endif

  const char* const RLH_RESULT_DESCRIPTIONS[RLH_RESULT_COUNT] = {
      "no errors occured", "tile out of terminal", "unexpected null argument",
      "unexpected argument value", "out of memory"};

  const float RLH_OPENGL_SCREEN_MATRIX[4 * 4] = {2.0f, 0.0f, 0.0f, -1.0f, 0.0f, -2.0f, 0.0f, 1.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f};

  const char* RLH_VERTEX_SOURCE =
    "#version 330 core\n"
    "in vec3 a_pos;\n"
    "in vec3 a_uvp;\n"
    "in vec4 a_color;\n"
    "out vec3 v_uvp;\n"
    "out vec4 v_color;\n"
    "uniform mat4 u_matrix;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = u_matrix * vec4(a_pos, 1.0);\n"
    "  v_uvp = a_uvp;\n"
    "  v_color = a_color;\n"
    "}";

  const char* RLH_FRAGMENT_SOURCE =
    "#version 330 core\n"
    "in vec3 v_uvp;\n"
    "in vec4 v_color;\n"
    "out vec4 f_color;\n"
    "uniform sampler2DArray u_atlas;\n"
    "void main()\n"
    "{\n"
    "  vec4 tex_color = texture(u_atlas, v_uvp);\n"
    "  f_color = tex_color * v_color;\n"
    "}";

  const size_t RLH_DATA_ATTRIBUTES_PER_TILE = 10;
  const size_t RLH_FONTMAP_ELEMENTS_PER_GLYPH = 5;
  const size_t RLH_VERTICES_PER_TILE = 6;
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
    size_t tile_data_capacity;
    size_t tile_data_count;
    float* tile_data;
    size_t atlas_width;
    size_t atlas_height;
    size_t atlas_pages;
    size_t glyph_count;
    float* glyph_stpqp;
    size_t element_count;
    unsigned int* element_data;

    // OpenGL
    GLuint gl_program;
    GLuint gl_vertex_array;
    GLuint gl_vertex_buffer;
    GLuint gl_element_buffer;
    GLuint matrix_uniform_location;
    GLuint gl_atlas_texture_2d_array;
  } rlhTerm_s;

  static inline GLuint _rlhColorTypeToGlFormat(const rlhcolortype_t color)
  {
    switch (color)
    {
      case RLH_COLOR_G:
        return GL_R;
      case RLH_COLOR_GA:
        return GL_RA;
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
        return GL_RA8;
      case RLH_COLOR_RGBA:
        return GL_RGBA8;
      case RLH_COLOR_BGRA:
        return GL_BGRA8;
      default:
        return GL_NONE;
    }
  }

  static inline GLuint _rlhCreateGlTextureArray(const size_t width, const size_t height, const size_t pages, const uint8_t* const pixel_bytes, const rlhcolortype_t color)
  {
    GLint gl_texture_2d_array = 0;
    const GLenum format = _rlhColorTypeToGlFormat(color);
    const GLenum internal_format = _rlhColorTypeToGlInternalFormat(color);
    GLD_START();
    GLD_CALL(glGenTextures(1, &gl_texture_2d_array));
    GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, gl_texture_2d_array));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0));
    GLD_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0));
    GLD_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal_format, width, height, pages, 0, format, GL_UNSIGNED_BYTE, pixel_rgba));
    return gl_texture_2d_array;
  }

  static inline GLuint _rlhCreateGlProgram()
  {
      GLint gl_program, gl_vertex_shader, gl_fragment_shader;
      GLD_CALL(gl_vertex_shader = glCreateShader(GL_VERTEX_SHADER));
      GLD_CALL(glShaderSource(gl_vertex_shader, 1, &RLH_VERTEX_SOURCE, NULL));
      GLD_COMPILE(gl_vertex_shader, "rlh vertex shader");
      GLD_CALL(gl_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
      GLD_CALL(glShaderSource(gl_fragment_shader, 1, &RLH_FRAGMENT_SOURCE, NULL));
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

  void rlhClearColor(const rlhColor32_s color)
  {
    GLD_START();
    GLD_CALL(glClearColor((float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f));
    GLD_CALL(glClear(GL_COLOR_BUFFER_BIT));
  }

  void rlhViewport(const int x, const int y, const int width, const int height)
  {
    GLD_START();
    GLD_CALL(glViewport(x, y, width, height));
  }

  rlhresult_t rlhTermCreateTileDimensions(const int tiles_wide, const int tiles_tall, const float pixel_scale, const int tile_width, const int tile_height, rlhTerm_h* const term)
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

    return rlhTermCreatePixelDimensions(pixel_width, pixel_height, pixel_scale, tile_width, tile_height, RLH_FALSE, term);
  }

  rlhresult_t rlhTermCreatePixelDimensions(const int pixel_width, const int pixel_height, const float pixel_scale, const int tile_width, const int tile_height, const rlhbool_t floor_pixels_to_tiles, rlhTerm_h* const term)
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
    (*term)->pixel_scale = pixel_scale;
    (*term)->tiles_wide = (size_t)pixel_width / ((size_t)tile_width * pixel_scale);
    (*term)->tiles_tall = (size_t)pixel_height / ((size_t)tile_height * pixel_scale);
    (*term)->tile_width = (size_t)tile_width;
    (*term)->tile_height = (size_t)tile_height;
    if (floor_pixels_to_tiles)
    {
      (*term)->pixel_width = (*term)->tiles_wide * (*term)->tile_width * pixel_scale;
      (*term)->pixel_height = (*term)->tiles_tall * (*term)->tile_height * pixel_scale;
    }
    (*term)->tile_data_capacity = (*term)->tiles_wide * (*term)->tiles_tall;
    const size_t reserved_data_size =
        sizeof(uint8_t) * (*term)->tile_data_capacity * RLH_DATA_BYTES_PER_TILE;
    (*term)->tile_data = (uint8_t*)malloc(reserved_data_size);
    if ((*term)->tile_data == NULL)
    {
      free(*term);
      return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    }
    GLD_START();
    (*term->gl_program) = _rlhCreateGlProgram();
    GLD_CALL(glGenVertexArrays(1, &(*term)->gl_vertex_array));
    GLD_CALL(glBindVertexArray((*term)->gl_vertex_array));
    GLD_CALL(glUseProgram((*term)->gl_program));
    GLD_CALL(glUniform1i(glGetUniformLocation((*term)->gl_program, "u_atlas"), RLH_ATLAS_TEXTURE_SLOT));
    (*term)->matrix_uniform_location = glGetUniformLocation((*term)->gl_program, "u_matrix");
    return RLH_RESULT_OK;
  }

  void rlhTermDestroy(rlhTerm_h const term)
  {
    if (term == NULL) return;
    free(term->tile_data);
    GLD_START();
    if (term->gl_vertex_array != GL_NONE)
    {
      GLD_CALL(glDeleteVertexArrays(1, &term->gl_vertex_array));
      term->gl_vertex_array = GL_NONE;
    }
    if (term->gl_program != GL_NONE)
    {
      GLD_CALL(glDeleteProgram(term->gl_program));
    }
    free(term);
  }

  float rlhTermGetPixelScale(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0.0f;
    }
    return term->pixel_scale;
  }

  void rlhTermGetTileGridDimensions(rlhTerm_h const term, int* const tiles_wide, int* const tiles_tall)
  {
    if (term == NULL)
    {
      return;
    }
    if (tiles_wide != NULL)
    {
      *tiles_wide = term->tiles_wide;
    }
    if (tiles_tall != NULL)
    {
      *tiles_tall = term->tiles_tall;
    }
  }

  void rlhTermGetPixelDimensions(rlhTerm_h const term, int* const pixel_width, int* const pixel_height)
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

  void rlhTermGetTileDimensions(rlhTerm_h const term, size_t* const tile_width, size_t* const tile_height)
  {
    if (term == NULL)
    {
      return;
    }
    if (tile_width != NULL)
    {
      *tile_width = term->tile_width;
    }
    if (tile_height != NULL)
    {
      *tile_height = term->tile_height;
    }
  }

  rlhresult_t rlhTermClearTileData(rlhTerm_h term)
  {
    if (term == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }
    term->tile_data_count = 0;
    return RLH_RESULT_OK;
  }

  size_t rlhTermGetTileCount(rlhTerm_h const term)
  {
    if (term == NULL)
    {
      return 0;
    }
    return term->tile_data_count;
  }

  rlhresult_t rlhTermResizeTileDimensions(rlhTerm_h const term, const int tiles_wide, const int tiles_tall, const float pixel_scale, const int tile_width, const int tile_height)
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

  rlhresult_t rlhTermResizePixelDimensions(rlhTerm_h const term, const int pixel_width, const int pixel_height, const float pixel_scale, const int tile_width, const int tile_height, const rlhbool_t floor_pixels_to_tiles)
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
    term->pixel_scale = pixel_scale;
    term->tiles_wide = (size_t)pixel_width / ((size_t)tile_width * pixel_scale);
    term->tiles_tall = (size_t)pixel_height / ((size_t)tile_height * pixel_scale);
    term->tile_width = (size_t)tile_width;
    term->tile_height = (size_t)tile_height;
    if (floor_pixels_to_tiles)
    {
      term->pixel_width = term->tiles_wide * term->tile_width * pixel_scale;
      term->pixel_height = term->tiles_tall * term->tile_height * pixel_scale;
    }
    else
    {
      term->pixel_width = (size_t)pixel_width;
      term->pixel_height = (size_t)pixel_height;
    }
    term->tile_data_count = 0;
    return RLH_RESULT_OK;
  }

  static inline uint8_t _rlhTermTryReserve(rlhTerm_h const term)
  {
    if (term->tile_data_count == term->tile_data_capacity)  // If we hit the reserved tile count, double
                                                        // the amount of reserved space.
    {
      size_t new_capacity = term->tile_data_capacity * 2;
      float* new_data_ptr =
          (float**)realloc(term->tile_data, new_capacity * RLH_DATA_ATTRIBUTES_PER_TILE * sizeof(float));
      if (new_data_ptr == NULL)
      {
        return RLH_FALSE;  // out of memory
      }
      term->tile_data = new_data_ptr;
      term->tile_data_capacity = new_capacity;
    }
    return RLH_TRUE;
  }

  static inline void _rlhTermPushTile(rlhTerm_h const term, const int pixel_x, const int pixel_y,
                                      const int pixel_w, const int pixel_h, const uint16_t glyph,
                                      const rlhColor32_s fg, const rlhColor32_s bg)
  {
    if (glyph > term->atlas_glyph_count) return;
    const float position_s = (float)pixel_x / (float)term->pixel_width;
    const float position_t = pos_s + ((float)pixel_w / (float)term->pixel_width);
    const float position_p = (float)pixel_y / (float)term->pixel_height;
    const float position_q = pos_p + ((float)pixel_h / (float)term->pixel_height);
    size_t atlas_stpqp_i = glyph * RLH_ATLAS_COORDINATES_PER_GLYPH;
    const float atlas_s = term->atlas_stpqp[atlas_stpqp_i++];
    const float atlas_t = term->atlas_stpqp[atlas_stpqp_i++];
    const float atlas_p = term->atlas_stpqp[atlas_stpqp_i++];
    const float atlas_q = term->atlas_stpqp[atlas_stpqp_i++];
    const float atlas_page = term->atlas_stpqp[atlas_stpqp_i];
    const float fg_r = (float)fg.r / 255.0f;
    const float fg_g = (float)fg.g / 255.0f;
    const float fg_b = (float)fg.b / 255.0f;
    const float fg_a = (float)fg.a / 255.0f;
    const float bg_r = (float)bg.r / 255.0f;
    const float bg_g = (float)bg.g / 255.0f;
    const float bg_b = (float)bg.b / 255.0f;
    const float bg_a = (float)bg.a / 255.0f;
    size_t index = term->TileDataCount * RLH_ATTRIBUTES_PER_TILE;
    // v0
    term->tile_data[index++] = position_s;
    term->tile_data[index++] = position_p;
    term->tile_data[index++] = atlas_s;
    term->tile_data[index++] = atlas_p;
    term->tile_data[index++] = atlas_page;
    term->tile_data[index++] = fg_r;
    term->tile_data[index++] = fg_g;
    term->tile_data[index++] = fg_b;
    term->tile_data[index++] = fg_a;
    term->tile_data[index++] = bg_r;
    term->tile_data[index++] = bg_g;
    term->tile_data[index++] = bg_b;
    term->tile_data[index++] = bg_a;
    // v1
    term->tile_data[index++] = position_t;
    term->tile_data[index++] = position_p;
    term->tile_data[index++] = atlas_t;
    term->tile_data[index++] = atlas_p;
    term->tile_data[index++] = atlas_page;
    term->tile_data[index++] = fg_r;
    term->tile_data[index++] = fg_g;
    term->tile_data[index++] = fg_b;
    term->tile_data[index++] = fg_a;
    term->tile_data[index++] = bg_r;
    term->tile_data[index++] = bg_g;
    term->tile_data[index++] = bg_b;
    term->tile_data[index++] = bg_a;
    // v2
    term->tile_data[index++] = position_s;
    term->tile_data[index++] = position_q;
    term->tile_data[index++] = atlas_s;
    term->tile_data[index++] = atlas_q;
    term->tile_data[index++] = atlas_page;
    term->tile_data[index++] = fg_r;
    term->tile_data[index++] = fg_g;
    term->tile_data[index++] = fg_b;
    term->tile_data[index++] = fg_a;
    term->tile_data[index++] = bg_r;
    term->tile_data[index++] = bg_g;
    term->tile_data[index++] = bg_b;
    term->tile_data[index++] = bg_a;
    // v3
    term->tile_data[index++] = position_t;
    term->tile_data[index++] = position_q;
    term->tile_data[index++] = atlas_t;
    term->tile_data[index++] = atlas_q;
    term->tile_data[index++] = atlas_page;
    term->tile_data[index++] = fg_r;
    term->tile_data[index++] = fg_g;
    term->tile_data[index++] = fg_b;
    term->tile_data[index++] = fg_a;
    term->tile_data[index++] = bg_r;
    term->tile_data[index++] = bg_g;
    term->tile_data[index++] = bg_b;
    term->tile_data[index++] = bg_a;
    term->TileDataCount++;
  }

  rlhresult_t rlhTermPushFillTile(rlhTerm_h const term, const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg)
  {
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, 0, 0, term->pixel_width, term->pixel_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileGrid(rlhTerm_h const term, const int grid_x, const int grid_y,
                                  const uint16_t glyph, const rlhColor32_s fg,
                                  const rlhColor32_s bg)
  {
    if (grid_x < 0 || grid_y < 0 || grid_x > term->tiles_wide || grid_y > term->tiles_tall)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    const int pixel_x = grid_x * term->tile_width;
    const int pixel_y = grid_y * term->tile_height;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, pixel_x, pixel_y, term->tile_width, term->tile_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileGridSized(rlhTerm_h const term, const int grid_x, const int grid_y,
                                       const int tile_pixel_width, const int tile_pixel_height,
                                       const uint16_t glyph, const rlhColor32_s fg,
                                       const rlhColor32_s bg)
  {
    if (grid_x < 0 || grid_y < 0 || grid_x > term->tiles_wide || grid_y > term->tiles_tall)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (tile_pixel_width <= 0 || tile_pixel_height <= 0 || tile_pixel_width > RLH_MAX_TILE_SIZE ||
        tile_pixel_height > RLH_MAX_TILE_SIZE)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    const unsigned int pixel_x = (unsigned int)grid_x * term->tile_width;
    const unsigned int pixel_y = (unsigned int)grid_y * term->tile_height;
    const unsigned int pixel_w_u = (unsigned int)tile_pixel_width;
    const unsigned int pixel_h_u = (unsigned int)tile_pixel_height;
    _rlhTermPushTile(term, pixel_x, pixel_y, pixel_w_u, pixel_h_u, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileFree(rlhTerm_h const term, const int screen_pixel_x,
                                  const int screen_pixel_y, const uint16_t glyph,
                                  const rlhColor32_s fg, const rlhColor32_s bg)
  {
    const int negative_default_width = -(int)term->tile_width;
    const int negative_default_height = -(int)term->tile_height;
    if (screen_pixel_x < negative_default_width || screen_pixel_y < negative_default_height ||
        screen_pixel_x > (int)term->tile_width || screen_pixel_y > (int)term->tile_height)
      return RLH_RESULT_TILE_OUT_OF_TERMINAL;
    if (!_rlhTermTryReserve(term)) return RLH_RESULT_ERROR_OUT_OF_MEMORY;
    _rlhTermPushTile(term, (unsigned int)screen_pixel_x, (unsigned int)screen_pixel_y,
                     term->tile_width, term->tile_height, glyph, fg, bg);
    return RLH_RESULT_OK;
  }

  rlhresult_t rlhTermPushTileFreeSized(rlhTerm_h const term, const int screen_pixel_x,
                                       const int screen_pixel_y, const int tile_pixel_width,
                                       const int tile_pixel_height, const uint16_t glyph,
                                       const rlhColor32_s fg, const rlhColor32_s bg)
  {
    if (screen_pixel_x < -tile_pixel_width || screen_pixel_y < -tile_pixel_height ||
        screen_pixel_x > (int)term->pixel_width || screen_pixel_y > (int)term->pixel_height)
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
  static inline void _rlhTransformMatrix(float* matrix, int screen_width, int screen_height,
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

  rlhresult_t rlhTermDrawCentered(rlhTerm_h const term, rlhAtlas_h const atlas,
                                  const int viewport_width, const int viewport_height)
  {
    if (term == NULL || atlas == NULL)
    {
      return RLH_RESULT_ERROR_NULL_ARGUMENT;
    }

    const int width_difference = viewport_width - term->pixel_width;
    const int height_difference = viewport_height - term->pixel_height;
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

    float matrix[RLH_MATRIX_FLOAT_COUNT];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * RLH_MATRIX_FLOAT_COUNT);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                     term->pixel_width, term->pixel_height);

    _setTermScissor(translate_x, translate_y, term->pixel_width, term->pixel_height);

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

    float matrix[RLH_MATRIX_FLOAT_COUNT];
    memcpy(matrix, RLH_OPENGL_SCREEN_MATRIX, sizeof(float) * RLH_MATRIX_FLOAT_COUNT);
    _TransformMatrix(matrix, viewport_width, viewport_height, translate_x, translate_y,
                     term->pixel_width * scale_x, term->pixel_height * scale_y);

    _setTermScissor(translate_x, translate_y, term->pixel_width * scale_x,
                    term->pixel_height * scale_y);

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

      // Update vertex buffer and element buffer data. Create objects if they don't exist yet.
      // TODO

      // Bind objects
      GLD_CALL(glBindVertexArray(term->gl_vertex_array));
      GLD_CALL(glUseProgram(term->gl_program));

      // bind the atlas texture
      GLD_CALL(glActiveTexture(GL_TEXTURE0 + RLH_ATLAS_TEXTURE_SLOT));
      GLD_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, atlas->gl_atlas_texture_2d_array));

      // set the matrix uniform
      GLD_CALL(glUniformMatrix4fv(term->MatrixUniformLocation, 1, 0, matrix_4x4));

      // set blend mode
      GLD_CALL(glEnable(GL_BLEND));
      GLD_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      // DRAW!!!
      GLD_CALL(glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, NULL));
    }

    return RLH_RESULT_OK;
  }
#endif
#ifdef __cplusplus
}
#endif
#endif
