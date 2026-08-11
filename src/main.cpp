#include "raylib.h"
#include <stdio.h>

int window_width = 1200;
int window_height = 800;

int main()
{
    InitWindow(window_width, window_height, "GuiGuix");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawText("Hello World", 100, 100, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}