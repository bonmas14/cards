#include <stdint.h>
#include "raylib.h"
#include "raymath.h"

typedef enum {
	IDLE,
	LOCKED,
	FOCUSED,
	ANIM_HOVER_IN,
	HOVERED,
	ANIM_HOVER_OUT,
	PICKED,
};

typedef struct {
	Vector2 position;
	Vector2 active_position;

	Vector2 offset;

	int32_t width;
	int32_t height;

	float scale;

	int state;
	int16_t texture_handle;
	float time;
} card_t;

typedef struct { 
	Texture texture;
	int16_t id;
} TextureResource_t;

static struct {
	size_t textures_count;
	TextureResource_t *textures;
} resources;

bool GetTextureByID(int16_t id, Texture* texture) {
	for (size_t i = 0; i < resources.textures_count; i++) {
		if (resources.textures[i].id == id) {
			*texture = resources.textures[i].texture;
			return true;
		}
	}

	TraceLog(LOG_ERROR, "Texture doesn't exist, but it was requested");
	return false;
}

static Camera2D cam;
static uint8_t collided = 0; // required by UpdateCards, remove it later

static Rectangle green_rect = { 0 };
static Rectangle red_rect = { 0 };

void DrawCards(size_t count, card_t* cards) {
	for (int32_t i = count - 1; i >= 0; i--) {
		card_t card = cards[i];

		Texture texture;
		if (!GetTextureByID(card.texture_handle, &texture))
			continue;
		Vector2 position = Vector2Add(Vector2Add(card.position, card.active_position), Vector2Scale(card.offset, card.scale));

		DrawTextureEx(texture, position, 0, card.scale, WHITE);
	}
}

void UpdateCards(size_t count, card_t* cards, float speed, float delta_time) {
	for (size_t i = 0; i < count; i++) {
		card_t card = cards[i];

		Rectangle rect = {
			.x = (card.position.x + card.active_position.x + card.offset.x) * cam.zoom,
			.y = (card.position.y + card.active_position.y + card.offset.y) * cam.zoom,
			.width = card.width * cam.zoom,
			.height = card.height * cam.zoom
		};

		switch (card.state) {
		case LOCKED:
			break;
		case IDLE:
			if (collided)
				break;

			if (!CheckCollisionPointRec(GetMousePosition(), rect))
				break;

			card.state = FOCUSED;
			card.time = 0;
			break;
		case FOCUSED:
			card.state = ANIM_HOVER_IN;
			break;
		case ANIM_HOVER_IN:
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !collided) {
				card.scale = 2;
				card.state = PICKED;
				collided = 1;
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
					card.state = PICKED;
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
		case PICKED:
			card.active_position = Vector2Add(card.active_position, Vector2Scale(GetMouseDelta(), 1 / cam.zoom));
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

				Vector2 new_position = Vector2Add(card.position, card.active_position);

				if (CheckCollisionPointRec(new_position, green_rect)) {
					card.position = new_position;
					card.state = LOCKED;
				}
				else if (CheckCollisionPointRec(new_position, red_rect)) {
					card.position = new_position;
					card.state = LOCKED;
				}
				else {
					card.state = HOVERED;
				}

				card.active_position = (Vector2){ 0 };
				collided = 0;
			}
			break;
		}

		switch (card.state) {
		case FOCUSED:
		case PICKED:
			for (int32_t j = i - 1; j >= 0; j--) {
				cards[j + 1] = cards[j];
			}
			cards[0] = card;
			break;
		default:
			cards[i] = card;
			break;
		}

	}
}

int main(void) {
	SetTraceLogLevel(LOG_WARNING);

	InitWindow(800, 600, "Cards");

	SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
	cam = (Camera2D){ .offset = (Vector2){400, 300}, .target = (Vector2){0, 0}, .zoom = 2 };

	card_t* cards = MemAlloc(10 * sizeof(card_t));

	resources.textures_count = 3;
	resources.textures = MemAlloc(3 * sizeof(TextureResource_t));

	resources.textures[0].texture = LoadTexture("resources/A.png");
	resources.textures[0].id = 0;

	resources.textures[1].texture = LoadTexture("resources/B.png");
	resources.textures[1].id = 10;

	resources.textures[2].texture = LoadTexture("resources/C.png");
	resources.textures[2].id = 20;

	for (int i = 0; i < 10; i++)
	{
		cards[i].texture_handle = (i % 3) * 10;

		Texture texture = { 0 };
		if (!GetTextureByID(cards[i].texture_handle, &texture))
			continue;

		cards[i].width = texture.width;
		cards[i].height = texture.height;

		cards[i].position = (Vector2){ .x = (i - 5) * 30, .y = 100 };
		cards[i].offset = (Vector2){ -cards[i].width / 2, -cards[i].height / 2 };

		cards[i].state = IDLE;
		cards[i].scale = 1;
		cards[i].time = 0;
	}

	red_rect = (Rectangle){ .x = -400 / cam.zoom, .y = -300 / cam.zoom, .width = 300 / cam.zoom, .height = 400 / cam.zoom };
	green_rect = (Rectangle){ .x = 100 / cam.zoom, .y = -300 / cam.zoom, .width = 300 / cam.zoom, .height = 400 / cam.zoom };

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

			DrawRectangle(green_rect.x, green_rect.y, green_rect.width, green_rect.height, GREEN);
			DrawRectangle(red_rect.x, red_rect.y, red_rect.width, red_rect.height, RED);

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

	for (size_t i = 0; i < resources.textures_count; i++) {
		UnloadTexture(resources.textures[i].texture);
	}

	MemFree(resources.textures);
	resources.textures_count = 0;
	MemFree(cards);
}
