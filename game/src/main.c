#include <stdio.h>
#include <stdint.h>
#include "cards.h"
#include "resource_manager.h"
#include "raylib.h"
#include "raymath.h"
#include "game.h"

int main(void) {
	SetTraceLogLevel(LOG_WARNING);

	InitWindow(800, 600, "Cards");

	SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);

	card_t* cards = MemAlloc(9 * sizeof(card_t));

	Texture* textures = MemAlloc(9 * sizeof(Texture));

	for (int32_t i = 0; i < 9; i++) {
		char buff[40];
		sprintf_s(buff, 40, "resources/%d.png", i + 1);

		textures[i] = LoadTexture(buff);
	}

	InitTextureResource(9, textures);

	for (int i = 0; i < 9; i++)
	{
		if (!FindTextureID(textures[i], &cards[i].texture_handle))
			continue;

		Texture texture = { 0 };
		if (!GetTextureByID(cards[i].texture_handle, &texture))
			continue;

		cards[i].width = texture.width;
		cards[i].height = texture.height;

		cards[i].position = (Vector2){ .x = ((float)i - 4.5f) * 30, .y = 100 };
		cards[i].offset = (Vector2){ -(float)cards[i].width / 2, -(float)cards[i].height / 2 };

		cards[i].state = IDLE;
		cards[i].scale = 1;
		cards[i].time = 0;
	}

	MemFree(textures);

	cam = (Camera2D){ .offset = (Vector2){400, 300}, .target = (Vector2){0, 0}, .zoom = 2 };
	red_rect = (Rectangle){ .x = -400 / cam.zoom, .y = -300 / cam.zoom, .width = 300 / cam.zoom, .height = 400 / cam.zoom };
	green_rect = (Rectangle){ .x = 100 / cam.zoom, .y = -300 / cam.zoom, .width = 300 / cam.zoom, .height = 400 / cam.zoom };

	SetMouseOffset(-400, -300);

	while (true) {
		if (WindowShouldClose())
			return;

		float deltaTime = GetFrameTime();
		float speed = 8;

		UpdateCards(9, cards, 8, deltaTime);

		BeginDrawing();
		{
			ClearBackground(BLACK);

			BeginMode2D(cam);

			DrawRectangle((int)green_rect.x, (int)green_rect.y, (int)green_rect.width, (int)green_rect.height, GREEN);
			DrawRectangle((int)red_rect.x, (int)red_rect.y, (int)red_rect.width, (int)red_rect.height, RED);

			DrawCards(9, cards);

			EndMode2D();

			int fontSize = 16;
			char* message = "Hit 'ESC' to exit";

			DrawText(message, 800 - MeasureText(message, fontSize), 600 - fontSize, fontSize, WHITE);

			char buffer[40];

			sprintf_s(buffer, 40, "red: %d", score_red);

			DrawText(buffer, 400 - MeasureText(buffer, fontSize) / 2, 300 - fontSize, fontSize, WHITE);
			sprintf_s(buffer, 40, "green: %d", score_green);

			DrawText(buffer, 400 - MeasureText(buffer, fontSize) / 2, 300 + fontSize / 4, fontSize, WHITE);

			DrawFPS(0, 0);
		}
		EndDrawing();
	}

	CloseWindow();

	MemFree(cards);
}
