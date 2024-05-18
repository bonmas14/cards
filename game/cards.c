#include <stdint.h>
#include "resource_manager.h"
#include "raylib.h"
#include "raymath.h"
#include "cards.h"
#include "game.h"

static uint8_t collided = 0;

void DrawCards(int32_t count, card_t* cards) {
	for (int32_t i = count - 1; i >= 0; i--) {
		card_t card = cards[i];

		Texture texture;
		if (!GetTextureByID(card.texture_handle, &texture))
			continue;

		Vector2 position = Vector2Add(Vector2Add(card.position, card.active_position), Vector2Scale(card.offset, card.scale));

		DrawTextureEx(texture, position, 0, card.scale, WHITE);
	}
}

void UpdateCards(int32_t count, card_t* cards, float speed, float delta_time) {
	for (int32_t i = 0; i < count; i++) {
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
					score_green += card.texture_handle;

					card.state = LOCKED;
				}
				else if (CheckCollisionPointRec(new_position, red_rect)) {
					card.position = new_position;
					score_red += card.texture_handle;

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
