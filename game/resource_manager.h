#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"

typedef struct { 
	Texture texture;
	int16_t id;
} TextureResource_t;


bool GetTextureByID(int16_t id, Texture* texture);

bool FindTextureID(Texture texture, int16_t* id);

void InitTextureResource(size_t count, Texture* textures);

void ClearTextureResource(void);
