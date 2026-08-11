#include "raylib.h"
#include <stdio.h>
#include <vector>

#define RES_PATH(X) "./resources/" X

int window_width = 1200;
int window_height = 800;
int cursor = MOUSE_CURSOR_DEFAULT;

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

void Frame(Rectangle rect, Color frame_color, float frame_width = 1.0f)
{
    if (frame_width > 0.0f)
    {
        DrawRectangle(rect.x, rect.y, rect.width, frame_width, frame_color);
        DrawRectangle(rect.x, rect.y, frame_width, rect.height, frame_color);
        DrawRectangle(rect.x, rect.y + rect.height - frame_width, rect.width, frame_width, frame_color);
        DrawRectangle(rect.x + rect.width - frame_width, rect.y, frame_width, rect.height, frame_color);
    }
}

bool Button(Rectangle rect, const char *text = "Default", bool selected = false)
{
    const float font_size = 20.0f;
    const Font font = get_font(font_size, FontMode::BOLD);
    const float frame_width = 1.0f;

    const bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    const bool clicked = hovered && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE));

    Color color = BLACK;
    Color font_color = WHITE;
    if (selected || hovered)
    {
        color = WHITE;
        font_color = BLACK;
    }

    if (hovered)
    {
        cursor = MOUSE_CURSOR_POINTING_HAND;
    }

    DrawRectangleRec(rect, color);
    Frame(rect, WHITE, frame_width);

    // Draw Text
    const Vector2 text_size = MeasureTextEx(font, text, font_size, 0);
    Vector2 text_pos;
    text_pos.x = rect.x + rect.width / 2 - text_size.x / 2;
    text_pos.y = rect.y + rect.height / 2 - text_size.y / 2 + 3.0f;
    DrawTextEx(font, text, text_pos, font_size, 0, font_color);
    return clicked;
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

        if (Button({200.0f, 100.0f, 100.0f, 50.0f}, "Hello"))
        {
            printf("Hello World\n");
        }

        EndDrawing();
    }

    for (const CustomFont &custom_font : font_pool)
    {
        UnloadFont(custom_font.font);
    }

    CloseWindow();

    return 0;
}