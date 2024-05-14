#include <stdio.h>
#include "raylib.h"

int main(void)
{
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(800, 600, "Cards");
    
    SetWindowState(FLAG_VSYNC_HINT 
        | FLAG_WINDOW_ALWAYS_RUN);

    Texture tex = LoadTexture("resources/A.png");

    Image image = LoadImageFromTexture(tex);

    SetWindowIcon(image);

    while (true)
    {
        if (WindowShouldClose())
            return;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(tex, 400 - tex.width / 2, 300 - tex.height / 2, WHITE);

        char* message = "Hit 'ESC' to exit";
        int fontSize = 16;

        DrawText(message, 800 - MeasureText(message, fontSize), 600 - fontSize, fontSize, WHITE);

        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();

    UnloadTexture(tex);
    UnloadImage(image);
}
