#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define JM_RLH_IMPLEMENTATION
#include <jm/roguelike.h>

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

    const int tiles_wide = 80;
    const int tiles_tall = 50;
    const int tile_size = 8 * 2;
    const int pixel_scale = 1;
    const int w_width = (tiles_wide * tile_size * pixel_scale) + 64;
    const int w_height = (tiles_tall * tile_size * pixel_scale) + 64;

    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "jm test", NULL, NULL);
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

    jmTerm_h t = jmTermCreate(tiles_wide, tiles_tall, pixel_scale, tile_size, tile_size, i_width, i_height, 1, pixels, 256, stpqp);

    free(stpqp);
    stbi_image_free(pixels);

    while (!glfwWindowShouldClose(window))
    {
        jmTermClear(t);

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 16; y++)
            {
                jmTermPushTileGrid(t, x, y, (y * 16) + x, JM_C32(255, 0, 255, 255), JM_C32(0, 0, 0, 255));
            }
        }

        jmTermPushTileFree(t, 15 * tile_size + (tile_size / 2), 15 * tile_size + (tile_size / 2), 2, JM_WHITE(), JM_TRANSPARENT());

        jmTermPushTileGridSized(t, 25, 25, tile_size * 3, tile_size * 3, 2, JM_WHITE(), JM_TRANSPARENT());

        jmTermPushTileFreeSized(t, 30 * tile_size + (tile_size / 2), 30 * tile_size, 15 * tile_size, 8 * tile_size, 2, JM_WHITE(), JM_TRANSPARENT());

        jmTermSetupStretchedDraw(t, w_width, w_height);
        jmClearColor(JM_BLACK());
        jmTermSetupOffsetDraw(t, 32, 32, w_height);
        jmTermDraw(t);

        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    window = NULL;
    glfwTerminate();

    return 0;
}