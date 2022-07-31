#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <roguelike.h>
#include <stb_image.h>
#include <string.h>

int main()
{
  if (!glfwInit())
  {
    printf("GLFW failed to initialize!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  const int tiles_wide = 40;
  const int tiles_tall = 25;
  const int tile_size = 8;
  const float pixel_scale = 2;
  const int border_pixels = 32;
  const int w_width = (tiles_wide * tile_size * pixel_scale) + (border_pixels * 2);
  const int w_height = (tiles_tall * tile_size * pixel_scale) + (border_pixels * 2);

  GLFWwindow* window = glfwCreateWindow(w_width, w_height, "rlh test", NULL, NULL);
  glfwMakeContextCurrent(window);

  if (!window)
  {
    printf("window failed to be created!\n");
    glfwTerminate();
    return 2;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    glfwDestroyWindow(window);
    window = NULL;
    glfwTerminate();
    printf("failed to load opengl bindings!\n");
    return 3;
  }

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

  rlhTerm_h t = NULL;
  result =
      rlhTermCreateTileDimensions(tiles_wide, tiles_tall, pixel_scale, tile_size, tile_size, &t);
  if (result > RLH_RESULT_LAST_NON_ERROR)
  {
    rlhAtlasDestroy(a);
    a = NULL;
    glfwTerminate();
    printf("Error creating terminal: %s\n", RLH_RESULT_DESCRIPTIONS[result]);
    return 6;
  }

  while (!glfwWindowShouldClose(window))
  {
    // set the terminal background color to black by pushing a fill tile
    rlhTermPushFillTile(t, 0, RLH_TRANSPARENT, RLH_BLACK);

    // every glyph in codepage 437
    for (size_t x = 0; x < 16; x++)
    {
      for (size_t y = 0; y < 16; y++)
      {
        rlhTermPushTileGrid(t, x, y, (y * 16) + x, RLH_C32(255, 0, 255, 255),
                            RLH_C32(0, 0, 0, 255));
      }
    }

    // half tile offset face on bottom right corner of codepage
    rlhTermPushTileFree(t, 15 * tile_size + (tile_size / 2), 15 * tile_size + (tile_size / 2), 2,
                        RLH_WHITE, RLH_TRANSPARENT);

    // increasingly larger faces
    rlhTermPushTileGridSized(t, 18, 5, tile_size * 5, tile_size * 5, 2, RLH_WHITE, RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 23, 5, tile_size * 4, tile_size * 4, 2, RLH_WHITE, RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 27, 5, tile_size * 3, tile_size * 3, 2, RLH_WHITE, RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 30, 5, tile_size * 2, tile_size * 2, 2, RLH_WHITE, RLH_TRANSPARENT);
    rlhTermPushTileGridSized(t, 32, 5, tile_size, tile_size, 2, RLH_WHITE, RLH_TRANSPARENT);

    // big guy in corner going over edges
    rlhTermPushTileFreeSized(t, -(tile_size * 3), 18 * tile_size, 15 * tile_size, 8 * tile_size, 2,
                             RLH_RED, RLH_TRANSPARENT);

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
    rlhTermDrawCentered(t, a, w_width, w_height);

    glfwPollEvents();

    glfwSwapBuffers(window);
  }

  rlhAtlasDestroy(a);
  a = NULL;
  rlhTermDestroy(t);
  t = NULL;

  glfwDestroyWindow(window);
  window = NULL;
  glfwTerminate();

  return 0;
}
