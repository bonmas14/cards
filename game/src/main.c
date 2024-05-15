#include <stdio.h>
#include "raylib.h"
#include "raymath.h"


typedef enum {
IDLE,
ANIM_HOVER_IN,
HOVERED,
ANIM_HOVER_OUT,
ANIM_PICKED,
} GraphicsState;

typedef struct {
    Vector2 position;
    Vector2 offset;
    Texture texture;
    GraphicsState state;
    float scale;
    float time;
} GraphicsComponent;

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(800, 600, "Cards");
    
    SetWindowState(FLAG_VSYNC_HINT 
        | FLAG_WINDOW_ALWAYS_RUN);

    GraphicsComponent gr = { 0 };

    gr.texture = LoadTexture("resources/A.png");
    gr.offset = (Vector2){ -gr.texture.width / 2, -gr.texture.height / 2 };
    gr.state = IDLE;
    gr.scale = 1;
    gr.time = 0;


    Camera2D cam = {.offset = (Vector2){400, 300}, .target = (Vector2){0, 0}, .zoom = 1};

    SetMouseOffset(-400, -300);

    while (true) {
        if (WindowShouldClose())
            return;

        Rectangle rect = {
			.x = (gr.position.x + gr.offset.x) * cam.zoom,
			.y = (gr.position.y + gr.offset.y) * cam.zoom,
			.width = gr.texture.width * cam.zoom,
			.height = gr.texture.height * cam.zoom
        };

        float deltaTime = GetFrameTime();
        float speed = 8;

        switch (gr.state) {
        case IDLE:
            if (!CheckCollisionPointRec(GetMousePosition(), rect))
                break;

			gr.state = ANIM_HOVER_IN;
			gr.time = 0;
            break;
        case ANIM_HOVER_IN:
            gr.time += deltaTime * speed;

            gr.scale = 1 + gr.time * gr.time;


            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                gr.scale = 2;
				gr.state = ANIM_PICKED;
                break;
            }

            if (gr.time > 1)
            {
                gr.scale = 2;

				gr.state = HOVERED;
            }
            break;
        case HOVERED:
            gr.scale = 2;
            if (CheckCollisionPointRec(GetMousePosition(), rect))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					gr.state = ANIM_PICKED;
                break;
            }
			gr.state = ANIM_HOVER_OUT;
			gr.time = 0;
            break;
        case ANIM_HOVER_OUT:
            gr.time += deltaTime * speed;

            gr.scale = 2 - 2 * gr.time + gr.time * gr.time;

            if (gr.time > 1)
            {
                gr.scale = 1;

				gr.state = IDLE;
            }
            break;
        case ANIM_PICKED:
            gr.position = Vector2Add(gr.position, Vector2Scale(GetMouseDelta(), 1 / cam.zoom));
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                gr.state = HOVERED;
            }
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        //DrawRectangle(rect.x + 400, rect.y + 300, rect.width, rect.height, RAYWHITE);

        BeginMode2D(cam);

		DrawTextureEx(gr.texture, Vector2Add(gr.position, Vector2Scale(gr.offset, gr.scale)), 0, gr.scale, WHITE);

        EndMode2D();

        int fontSize = 16;
        char* message = "Hit 'ESC' to exit";

        DrawText(message, 800 - MeasureText(message, fontSize), 600 - fontSize, fontSize, WHITE);

        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();

    UnloadTexture(gr.texture);
}
