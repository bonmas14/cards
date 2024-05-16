#include <stdint.h>
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
} card_t;

static Camera2D cam;

void DrawCards(size_t count, card_t* cards) {
    for (int32_t i = count - 1; i >= 0; i--) {
        card_t card = cards[i];

		DrawTextureEx(card.texture, Vector2Add(card.position, Vector2Scale(card.offset, card.scale)), 0, card.scale, WHITE);
    }
}

void UpdateCards(size_t count, card_t* cards, float speed, float delta_time) {
    uint8_t collided = 0;

    for (size_t i = 0; i < count; i++) {
        card_t card = cards[i];

        Rectangle rect = {
            .x = (card.position.x + card.offset.x) * cam.zoom,
            .y = (card.position.y + card.offset.y) * cam.zoom,
            .width = card.texture.width * cam.zoom,
            .height = card.texture.height * cam.zoom
        };

        switch (card.state) {
        case IDLE:
            if (collided)
                break;

            if (!CheckCollisionPointRec(GetMousePosition(), rect))
                break;

            card.state = ANIM_HOVER_IN;
            card.time = 0;
            break;
        case ANIM_HOVER_IN:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                card.scale = 2;
                card.state = ANIM_PICKED;
                break;
            }

            card.time += delta_time * speed;
            card.scale = 1 + card.time * card.time;

            if (card.time > 1) {
                card.scale = 2;
                card.state = HOVERED;
            }
            break;
        case HOVERED:
            card.scale = 2;
            if (CheckCollisionPointRec(GetMousePosition(), rect)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !collided)
                {
					card.state = ANIM_PICKED;
					collided = 1;
                }
                break;
            }

            card.state = ANIM_HOVER_OUT;
            card.time = 0;
            break;
        case ANIM_HOVER_OUT:
            card.time += delta_time * speed;

            card.scale = 2 - 2 * card.time + card.time * card.time;

            if (card.time > 1) {
                card.scale = 1;

                card.state = IDLE;
            }
            break;
        case ANIM_PICKED:
            card.position = Vector2Add(card.position, Vector2Scale(GetMouseDelta(), 1 / cam.zoom));
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                card.state = HOVERED;
                collided = 0;
            }
            break;
        }

        cards[i] = card;
    }
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(800, 600, "Cards");
    
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);

    card_t* cards = MemAlloc(10 * sizeof(card_t));
    Texture tex = LoadTexture("resources/A.png");
    for (size_t i = 0; i < 10; i++)
    {
		cards[i].texture = tex; // no texture manager
		cards[i].offset = (Vector2){ -cards[i].texture.width / 2, -cards[i].texture.height / 2 };
		cards[i].state = IDLE;
		cards[i].scale = 1;
		cards[i].time = 0;
        cards[i].position = (Vector2){ .x = 200 - (int)i * 40 };
    }

	cam = (Camera2D){ .offset = (Vector2){400, 300}, .target = (Vector2){0, 0}, .zoom = 1 };

    SetMouseOffset(-400, -300);

    while (true) {
        if (WindowShouldClose())
            return;

        float deltaTime = GetFrameTime();
        float speed = 8;

        UpdateCards(10, cards, 8, deltaTime);

		BeginDrawing(); 
		{
			ClearBackground(BLACK);

			BeginMode2D(cam);

			DrawCards(10, cards);

			EndMode2D();

			int fontSize = 16;
			char* message = "Hit 'ESC' to exit";

			DrawText(message, 800 - MeasureText(message, fontSize), 600 - fontSize, fontSize, WHITE);

			DrawFPS(0, 0);
		}
		EndDrawing();
    }

    CloseWindow();

    UnloadTexture(cards[0].texture);
    MemFree(cards);
}
