#pragma once
#ifndef GAMETEXTURE_INCLUDES
#define GAMETEXTURE_INCLUDES

typedef struct GameTexture {
	struct {
		int width;
		int height;
	} dim;
	unsigned char* data;
} GameTexture;

GameTexture* LoadedTextureArray;
int LoadedTextureArrayLength;

#endif // !GAMETEXTURE_INCLUDES
