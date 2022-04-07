#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define RLH_IMPLEMENTATION
#include <roguelike.h>

#include <string.h>

int main()
{
    if (!glfwInit())
    {
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
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        window = NULL;
        glfwTerminate();
        return 2;
    }

    int i_width, i_height;
    stbi_set_flip_vertically_on_load(1);
    uint8_t* pixels = stbi_load("cp_8x8_rgba_bg_alpha.png", &i_width, &i_height, NULL, 4);

    if (!pixels)
    {
        glfwDestroyWindow(window);
        window = NULL;
        glfwTerminate();
        return 3;
    }

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
                        stpqp[i++] = uvtilesize * x;
                        break;
                    case 1:
                        stpqp[i++] = uvtilesize * x + uvtilesize;
                        break;
                    case 2:
                        stpqp[i++] = 1.0f - (uvtilesize * y + uvtilesize);
                        break;
                    case 3:
                        stpqp[i++] = 1.0f - (uvtilesize * y);
                        break;
                    case 4:
                        stpqp[i++] = 0;
                    }
                }
            }
        }
    }

    rlhAtlas_h a = rlhAtlasCreate(i_width, i_height, 1, pixels, 256, stpqp);
    rlhTerm_h t = rlhTermCreate(tiles_wide, tiles_tall, pixel_scale, tile_size, tile_size);

    free(stpqp);
    stbi_image_free(pixels);

    while (!glfwWindowShouldClose(window))
    {
        rlhTermClear(t);

        // every glyph in codepage 437
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 16; y++)
            {
                rlhTermPushTileGrid(t, x, y, (y * 16) + x, RLH_C32(255, 0, 255, 255), RLH_C32(0, 0, 0, 255));
            }
        }

        //half tile offset face on bottom right corner of codepage
        rlhTermPushTileFree(t, 15 * tile_size + (tile_size / 2), 15 * tile_size + (tile_size / 2), 2, RLH_WHITE(), RLH_TRANSPARENT());

        // increasingly larger faces
        rlhTermPushTileGridSized(t, 18, 5, tile_size * 5, tile_size * 5, 2, RLH_WHITE(), RLH_TRANSPARENT());
        rlhTermPushTileGridSized(t, 23, 5, tile_size * 4, tile_size * 4, 2, RLH_WHITE(), RLH_TRANSPARENT());
        rlhTermPushTileGridSized(t, 27, 5, tile_size * 3, tile_size * 3, 2, RLH_WHITE(), RLH_TRANSPARENT());
        rlhTermPushTileGridSized(t, 30, 5, tile_size * 2, tile_size * 2, 2, RLH_WHITE(), RLH_TRANSPARENT());
        rlhTermPushTileGridSized(t, 32, 5, tile_size, tile_size, 2, RLH_WHITE(), RLH_TRANSPARENT());

        // big guy in corner going over edges
        rlhTermPushTileFreeSized(t, -(tile_size * 3), 18 * tile_size, 15 * tile_size, 8 * tile_size, 2, RLH_RED(), RLH_TRANSPARENT());

        const size_t label_x = 18;

        const char* label_1 = "roguelike.h";
        const size_t label_1_y = 20;
        const size_t label_1_length = strlen(label_1);
        for (size_t x = 0; x < label_1_length; x++)
        {
            rlhTermPushTileGrid(t, x + label_x, label_1_y, label_1[x], RLH_NAVY(), RLH_YELLOW());
        }

        const char* label_2 = "by journeyman";
        const size_t label_2_y = label_1_y + 1;
        const size_t label_2_length = strlen(label_2);
        for (size_t x = 0; x < label_2_length; x++)
        {
            rlhTermPushTileGrid(t, x + label_x, label_2_y, label_2[x], RLH_RED(), RLH_GREEN());
        }

        rlhViewport(0, 0, w_width, w_height);
        rlhClearColor(RLH_BLACK());
        rlhTermDrawTranslated(t, a, border_pixels, border_pixels, w_width, w_height);

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