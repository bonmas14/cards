#pragma once
#include "raylib.h"

enum {
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

void DrawCards(int32_t count, card_t* cards);

void UpdateCards(int32_t count, card_t* cards, float speed, float delta_time);
