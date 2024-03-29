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

#include <glad/glad.h>
#include <gld/gl_debug.h>
#include <GLFW/glfw3.h>
#include <rlh/roguelike.h>
#include <cp/cp437.h>
#include <pngw/png_wrapper.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// if glDebug.h comes across an error, we want it sent to this callback for printing
void gld_callback(const char *error) { printf("%s\n", error); }

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  rlhTerm_h term = glfwGetWindowUserPointer(window);
  rlhTermSizeInfo_t size_info;
  size_info.width = width;
  size_info.height = height;
  size_info.pixel_scale = rlhTermGetPixelScale(term);
  size_info.size_mode = RLH_SIZE_SCALED_PIXELS;
  size_info.floor_pixels_to_tiles = RLH_TRUE;
  rlhTermGetTileSize(term, &size_info.tile_width, &size_info.tile_height);
  rlhTermSetSize(term, &size_info);
}

int main()
{
  // calculate some constant values that we will need to size the terminal and the window
  const int tiles_wide = 40; // the amount of tiles wide and tall we want drawn in the terminal
  const int tiles_tall = 25;
  const int tile_width = 8; // the width and height of each terimal tile
  const int tile_height = 8;
  const float pixel_scale = 2; // the ratio of terminal pixels to window pixels
  const int border_pixels =
      32; // this is the size of the gap between edges of the terminal and the window edges

  // set the debug callback for glDebug.h
  GLD_SET_CALLBACK(gld_callback);

  // initialize the platform library
  if (!glfwInit())
  {
    printf("GLFW failed to initialize!\n");
    return 1;
  }

  // configure the window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  // create the window
  GLFWwindow *window = glfwCreateWindow(1, 1, "rlh test", NULL, NULL);
  glfwMakeContextCurrent(window);
  if (!window)
  {
    printf("window failed to be created!\n");
    glfwTerminate();
    return 2;
  }
  // load opengl bindings
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("failed to load opengl bindings!\n");
    return 3;
  }
  // load the atlas image from secondary storage
  const char *image_path = "cp_8x8_rgba_bg_alpha.png";
  size_t i_width, i_height, file_depth;
  pngwcolor_t file_color;
  pngwresult_t pngw_result = pngwFileInfo(image_path, &i_width, &i_height, &file_depth, &file_color);
  if (pngw_result != PNGW_RESULT_OK)
  {
    printf("an error has occured getting png file info: %s\n", PNGW_RESULT_DESCRIPTIONS[pngw_result]);
    return 4;
  }
  size_t size;
  // If you change the channel_size to 2 and it should work fine
  // roguelike.h also supports a channel size of 4, but png_wrapper.h can not load images like that.
  const size_t image_load_channel_size = 1;
  const size_t bits_per_byte = 8;
  const size_t image_load_depth = image_load_channel_size * bits_per_byte;
  // if you change the load_color to PNGW_COLOR_G or PNGW_COLOR_GA and it will work fine with roguelike.h.
  const pngwcolor_t image_load_color = PNGW_COLOR_RGBA;
  pngw_result = pngwDataSize(i_width, i_height, image_load_depth, image_load_color, &size);
  if (pngw_result != PNGW_RESULT_OK)
  {
    printf("an error has occured getting image data size: %s\n", PNGW_RESULT_DESCRIPTIONS[pngw_result]);
    return 5;
  }
  pngwb_t *pixels = malloc(size);
  if (!pixels)
  {
    printf("an error has occured reading png file data: out of memory\n");
    return 6;
  }
  pngw_result = pngwReadFile(image_path, pixels, PNGW_DEFAULT_ROW_OFFSET, i_width, i_height, image_load_depth,
                             image_load_color);
  if (pngw_result != PNGW_RESULT_OK)
  {
    printf("an error has occured loading image bytes: %s\n", PNGW_RESULT_DESCRIPTIONS[pngw_result]);
    return 7;
  }

  // setup the stpqp coordinates for the atlas
  // note that the atlas is a 16x16 grid of glyphs, with a total of 256
  const int sheet_sprite_dimensions = 16;
  const int sheet_sprite_count = sheet_sprite_dimensions * sheet_sprite_dimensions;
  const size_t stpqp_arr_size = sheet_sprite_count * sizeof(float) * 5;
  float *stpqp = (float *)malloc(stpqp_arr_size);
  {
    const float uvtilesize = 1.0f / (float)sheet_sprite_dimensions;
    size_t i = 0;
    for (float y = 0; y < 16; y++)
    {
      for (float x = 0; x < 16; x++)
      {
        for (int q = 0; q < 5; q++)
        {
          switch (q)
          {
          case 0:
            stpqp[i++] = uvtilesize * x; // the s coordinate (left x)
            break;
          case 1:
            stpqp[i++] = uvtilesize * x + uvtilesize; // the t coordinate (right x)
            break;
          case 2:
            stpqp[i++] = uvtilesize * y; // the p coordinate (top y)
            break;
          case 3:
            stpqp[i++] = uvtilesize * y + uvtilesize; // the q coordinate (bottom y)
            break;
          case 4:
            stpqp[i++] = 0; // the 2nd p coordinate (texture page id)
          }
        }
      }
    }
  }
  // create the atlas info
  rlhAtlasCreateInfo_t atlas_info;
  memset(&atlas_info, 0, sizeof(rlhAtlasCreateInfo_t));
  atlas_info.width = i_width;
  atlas_info.height = i_height;
  atlas_info.pages = 1;
  atlas_info.channel_size = image_load_channel_size;
  switch (image_load_color)
  {
  case PNGW_COLOR_G:
    atlas_info.color = RLH_COLOR_G;
    break;
  case PNGW_COLOR_GA:
    atlas_info.color = RLH_COLOR_GA;
    break;
  case PNGW_COLOR_RGBA:
    atlas_info.color = RLH_COLOR_RGBA;
    break;
  default:
    printf("invalid image color type");
    return 8;
  }
  atlas_info.pixel_data = pixels;
  atlas_info.glyph_count = sheet_sprite_count;
  atlas_info.glyph_stpqp = stpqp;
  // create the size info
  rlhTermSizeInfo_t size_info;
  memset(&size_info, 0, sizeof(rlhTermSizeInfo_t));
  size_info.width = tiles_wide;
  size_info.height = tiles_tall;
  size_info.pixel_scale = pixel_scale;
  size_info.size_mode = RLH_SIZE_TILES;
  size_info.tile_width = i_width / sheet_sprite_dimensions;
  size_info.tile_height = i_height / sheet_sprite_dimensions;
  // create the term info
  rlhTermCreateInfo_t term_info;
  term_info.atlas_info = &atlas_info;
  term_info.size_info = &size_info;
  // create the terminal
  rlhTerm_h t = NULL;
  rlhresult_t result = rlhTermCreate(&term_info, &t);
  int t_width, t_height;
  rlhTermGetScaledPixelSize(t, &t_width, &t_height);
  int w_width = t_width + border_pixels * 2;
  int w_height = t_height + border_pixels * 2;
  glfwSetWindowSize(window, w_width, w_height);
  glfwShowWindow(window);
  if (result != RLH_RESULT_OK)
  {
    glfwTerminate();
    printf("Error creating terminal: %s\n", RLH_RESULT_DESCRIPTIONS[result]);
    return 6;
  }
  glfwSetWindowUserPointer(window, t);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // the game loop
  while (!glfwWindowShouldClose(window)) // keep looping until the window is closed.
  {
    glfwPollEvents(); // poll for platform events to update the window with user input stuff

    // set the terminal background color to black by pushing a fill tile
    rlhTermPushFill(t, 0, RLH_TRANSPARENT, RLH_BLACK);

    // every glyph in codepage 437 drawn to the upper left corner
    for (size_t x = 0; x < 16; x++)
    {
      for (size_t y = 0; y < 16; y++)
      {
        rlhTermPushGrid(t, x, y, (y * 16) + x, RLH_COLOR(1, 0, 1, 1),
                        RLH_COLOR(0, 0, 0, 1));
      }
    }

    // half tile offset face on bottom right corner of codepage
    rlhTermPushFree(t, 15 * tile_width + (tile_height / 2), 15 * tile_width + (tile_height / 2),
                    CP_SMILE, RLH_WHITE, RLH_TRANSPARENT);

    // increasingly larger faces
    rlhTermPushGridSized(t, 18, 5, tile_width * 5, tile_height * 5, CP_SMILE, RLH_WHITE,
                         RLH_TRANSPARENT);
    rlhTermPushGridSized(t, 23, 5, tile_width * 4, tile_height * 4, CP_SMILE, RLH_WHITE,
                         RLH_TRANSPARENT);
    rlhTermPushGridSized(t, 27, 5, tile_width * 3, tile_height * 3, CP_SMILE, RLH_WHITE,
                         RLH_TRANSPARENT);
    rlhTermPushGridSized(t, 30, 5, tile_width * 2, tile_height * 2, CP_SMILE, RLH_WHITE,
                         RLH_TRANSPARENT);
    rlhTermPushGridSized(t, 32, 5, tile_width, tile_height, CP_SMILE, RLH_WHITE, RLH_TRANSPARENT);

    // big guy in corner going over edges
    rlhTermPushFreeSized(t, -(tile_width * 3), 18 * tile_height, 15 * tile_width,
                         8 * tile_height, CP_SMILE, RLH_RED, RLH_TRANSPARENT);

    // print some text to the screen
    // this is easy to do because CP437 glyphs for characters collide with ascii character codes
    const size_t label_x = 18;
    const char *label_1 = "roguelike.h";
    const size_t label_1_y = 20;
    const size_t label_1_length = strlen(label_1);
    for (size_t x = 0; x < label_1_length; x++)
    {
      rlhTermPushGrid(t, x + label_x, label_1_y, label_1[x], RLH_NAVY, RLH_YELLOW);
    }

    const char *label_2 = "by journeyman";
    const size_t label_2_y = label_1_y + 1;
    const size_t label_2_length = strlen(label_2);
    for (size_t x = 0; x < label_2_length; x++)
    {
      rlhTermPushGrid(t, x + label_x, label_2_y, label_2[x], RLH_RED, RLH_GREEN);
    }

    // get the current window size
    glfwGetFramebufferSize(window, &w_width, &w_height);
    // set the viewport to the window dimensions
    rlhViewport(0, 0, w_width, w_height);
    // draw clear color as silver
    rlhClearColor(RLH_SILVER);
    // draw the terminal aligned within the viewport.
    // note that we made the terminal smaller than the window, so there will be space outside of it
    rlhTermDrawAligned(t, w_width, w_height, RLH_HALIGN_LEFT, RLH_VALIGN_TOP);
    // swap the window buffers. this presents the frame to the screen and reuses the framebuffer
    // from the previous frame for the next draw
    glfwSwapBuffers(window);
  }

  // destroy all roguelike.h resources
  rlhTermDestroy(t);
  t = NULL;

  // destroy all platform library resources
  glfwDestroyWindow(window);
  window = NULL;
  glfwTerminate();

  return 0;
}
