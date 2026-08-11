#include "raylib.h"
#include <stdio.h>
#include <vector>

#define RES_PATH(X) "./resources/" X

int window_width = 1200;
int window_height = 800;

enum class FontMode
{
    NONE,
    BOLD
};

struct CustomFont
{
    float size;
    FontMode mode;
    Font font;
};

std::vector<CustomFont> font_pool;

Font get_font(float size, FontMode mode)
{
    for (const CustomFont &font : font_pool)
    {
        if (font.size == size && font.mode == mode)
        {
            return font.font;
        }
    }
    std::vector<int> codepoints;
    for (int i = 32; i <= 126; i++)
        codepoints.push_back(i);
    for (int i = 160; i <= 255; i++)
        codepoints.push_back(i);

    const char *font_path = "";
    if (mode == FontMode::NONE)
    {
        font_path = RES_PATH("consolas.ttf");
    }
    else if (mode == FontMode::BOLD)
    {
        font_path = RES_PATH("consolas_bold.ttf");
    }

    CustomFont custom_font;
    custom_font.size = size;
    custom_font.mode = mode;
    custom_font.font = LoadFontEx(font_path, size, codepoints.data(), codepoints.size());
    font_pool.push_back(custom_font);
    return custom_font.font;
}

int main()
{
    InitWindow(window_width, window_height, "GuiGuix");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawTextEx(get_font(20.0f, FontMode::NONE), "Hello", {100.0f, 100.0f}, 20.0f, 0.0f, WHITE);
        DrawTextEx(get_font(20.0f, FontMode::BOLD), "World", {100.0f, 150.0f}, 20.0f, 0.0f, WHITE);

        EndDrawing();
    }

    for (const CustomFont &custom_font : font_pool)
    {
        UnloadFont(custom_font.font);
    }

    CloseWindow();

    return 0;
}