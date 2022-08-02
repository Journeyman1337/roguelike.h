#include <glad/glad.h>
#include <glDebug.h>
#include <GLFW/glfw3.h>
#include <roguelike.h>
#include <stb_image.h>
#include <string.h>

// if glDebug.h comes across an error, we want it sent to this callback for printing
void gld_callback(const char* error, const char* func) { printf("%s: %s\n", error, func); }

int main()
{
  // calculate some constant values that we will need to size the terminal and the window
  const int tiles_wide = 40;  // the amount of tiles wide and tall we want drawn in the terminal
  const int tiles_tall = 25;
  const int tile_width = 8;  // the width and height of each terimal tile
  const int tile_height = 8;
  const float pixel_scale = 2;  // the ratio of terminal pixels to window pixels
  const int border_pixels =
      32;  // this is the size of the gap between edges of the terminal and the window edges
  const int w_width = (tiles_wide * tile_width * pixel_scale) + (border_pixels * 2);
  const int w_height = (tiles_tall * tile_height * pixel_scale) + (border_pixels * 2);

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
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  // create the window
  GLFWwindow* window = glfwCreateWindow(w_width, w_height, "rlh test", NULL, NULL);
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
    glfwDestroyWindow(window);
    window = NULL;
    glfwTerminate();
    printf("failed to load opengl bindings!\n");
    return 3;
  }
  // load the atlas image from secondary storage
  int i_width, i_height;
  stbi_set_flip_vertically_on_load(1);
  uint8_t* pixels = stbi_load("cp_8x8_rgba_bg_alpha.png", &i_width, &i_height, NULL, 4);
  if (!pixels)
  {
    glfwDestroyWindow(window);
    window = NULL;
    glfwTerminate();
    printf(
        "Failed to load texture atlas image. Make sure that the image is in the same directory as "
        "the built executable.\n");
    return 4;
  }

  // setup the stpqp coordinates for the atlas
  // note that the atlas is a 16x16 grid of glyphs, with a total of 256
  const int sheet_sprite_dimensions = 16;
  const int sheet_sprite_count = sheet_sprite_dimensions * sheet_sprite_dimensions;
  const size_t stpqp_arr_size = sheet_sprite_count * sizeof(float) * 5;
  float* stpqp = (float*)malloc(stpqp_arr_size);
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
            stpqp[i++] = uvtilesize * x;  // the s coordinate (left x)
            break;
          case 1:
            stpqp[i++] = uvtilesize * x + uvtilesize;  // the t coordinate (right x)
            break;
          case 2:
            stpqp[i++] = uvtilesize * y + uvtilesize;  // the p coordinate (top y)
            break;
          case 3:
            stpqp[i++] = uvtilesize * y;  // the q coordinate (bottom y)
            break;
          case 4:
            stpqp[i++] = 0;  // the 2nd p coordinate (texture page id)
          }
        }
      }
    }
  }
  // create the roguelike.h atlas object
  rlhAtlas_h a = NULL;
  rlhresult_t result = rlhAtlasCreate(i_width, i_height, 1, pixels, 256, stpqp, &a);
  free(stpqp);
  stbi_image_free(pixels);
  if (result > RLH_RESULT_LAST_NON_ERROR)
  {
    glfwTerminate();
    printf("Error creating atlas: %s\n", RLH_RESULT_DESCRIPTIONS[result]);
    return 5;
  }
  // create the roguelike.h terminal object
  rlhTerm_h t = NULL;
  result =
      rlhTermCreateTileDimensions(tiles_wide, tiles_tall, pixel_scale, tile_width, tile_height, &t);
  if (result > RLH_RESULT_LAST_NON_ERROR)
  {
    rlhAtlasDestroy(a);
    a = NULL;
    glfwTerminate();
    printf("Error creating terminal: %s\n", RLH_RESULT_DESCRIPTIONS[result]);
    return 6;
  }
  // the game loop
  while (!glfwWindowShouldClose(window))  // keep looping until the window is closed.
  {
    glfwPollEvents();  // poll for platform events to update the window with user input stuff

    // set the terminal background color to black by pushing a fill tile
    rlhTermPushFillTile(t, 0, RLH_TRANSPARENT, RLH_BLACK);

    // every glyph in codepage 437 drawn to the upper left corner
    for (size_t x = 0; x < 16; x++)
    {
      for (size_t y = 0; y < 16; y++)
      {
        rlhTermPushTileGrid(t, x, y, (y * 16) + x, RLH_C32(255, 0, 255, 255),
                            RLH_C32(0, 0, 0, 255));
      }
    }

    // half tile offset face on bottom right corner of codepage
    rlhTermPushTileFree(t, 15 * tile_width + (tile_height / 2), 15 * tile_width + (tile_height / 2),
                        2, RLH_WHITE, RLH_TRANSPARENT);

    // increasingly larger faces
    rlhTermPushTileGridSized(t, 18, 5, tile_width * 5, tile_height * 5, 2, RLH_WHITE,
                             RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 23, 5, tile_width * 4, tile_height * 4, 2, RLH_WHITE,
                             RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 27, 5, tile_width * 3, tile_height * 3, 2, RLH_WHITE,
                             RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 30, 5, tile_width * 2, tile_height * 2, 2, RLH_WHITE,
                             RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 32, 5, tile_width, tile_height, 2, RLH_WHITE, RLH_TRANSPARENT);

    // big guy in corner going over edges
    rlhTermPushTileFreeSized(t, -(tile_width * 3), 18 * tile_height, 15 * tile_width,
                             8 * tile_height, 2, RLH_RED, RLH_TRANSPARENT);

    // print some text to the screen
    // this is easy to do because CP437 glyphs for characters collide with ascii character codes
    const size_t label_x = 18;
    const char* label_1 = "roguelike.h";
    const size_t label_1_y = 20;
    const size_t label_1_length = strlen(label_1);
    for (size_t x = 0; x < label_1_length; x++)
    {
      rlhTermPushTileGrid(t, x + label_x, label_1_y, label_1[x], RLH_NAVY, RLH_YELLOW);
    }

    const char* label_2 = "by journeyman";
    const size_t label_2_y = label_1_y + 1;
    const size_t label_2_length = strlen(label_2);
    for (size_t x = 0; x < label_2_length; x++)
    {
      rlhTermPushTileGrid(t, x + label_x, label_2_y, label_2[x], RLH_RED, RLH_GREEN);
    }

    // set the viewport to the window dimensions
    rlhViewport(0, 0, w_width, w_height);
    // draw clear color as silver
    rlhClearColor(RLH_SILVER);
    // draw the terminal centered within the viewport.
    // note that we made the terminal smaller than the window, so there will be space outside of it
    rlhTermDrawCentered(t, a, w_width, w_height);
    // swap the window buffers. this presents the frame to the screen and reuses the framebuffer
    // from the previous frame for the next draw
    glfwSwapBuffers(window);
  }

  // destroy all roguelike.h resources
  rlhAtlasDestroy(a);
  a = NULL;
  rlhTermDestroy(t);
  t = NULL;

  // destroy all platform library resources
  glfwDestroyWindow(window);
  window = NULL;
  glfwTerminate();

  return 0;
}
