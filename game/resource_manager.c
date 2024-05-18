#include "resource_manager.h"
#include <stdio.h>

static struct {
	size_t textures_count;
	TextureResource_t *textures;
} resources;

static int16_t texture_id_counter = 10;

bool GetTextureByID(int16_t id, Texture* texture) {
	for (size_t i = 0; i < resources.textures_count; i++) {
		if (resources.textures[i].id == id) {
			*texture = resources.textures[i].texture;
			return true;
		}
	}

	char buffer[128];
	sprintf_s(buffer, 128, "Texture doesn't exist, but it was requested. req: %d", id);

	TraceLog(LOG_ERROR, buffer);
	return false;
}

bool FindTextureID(Texture texture, int16_t* id) {
	for (size_t i = 0; i < resources.textures_count; i++) {
		if (resources.textures[i].texture.id == texture.id) {
			*id = resources.textures[i].id;
			return true;
		}
	}

	char buffer[128];
	sprintf_s(buffer, 128, "TextureResource doesn't exist, but it was requested.");

	TraceLog(LOG_ERROR, buffer);
	return false;
};

void InitTextureResource(size_t count, Texture* textures) {
	resources.textures_count = count;
    resources.textures = MemAlloc(count * sizeof(TextureResource_t));

	for (int32_t i = 0; i < 9; i++) {
		resources.textures[i].texture = textures[i];
		resources.textures[i].id = texture_id_counter++;
	}
}

void ClearTextureResource(void) {
	for (size_t i = 0; i < resources.textures_count; i++) {
		UnloadTexture(resources.textures[i].texture);
	}

	MemFree(resources.textures);
	resources.textures_count = 0;
}
