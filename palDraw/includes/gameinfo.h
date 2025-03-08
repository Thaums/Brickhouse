#pragma once
#include <stdint.h>

typedef struct GameInfo {
	struct {
		uint32_t indexMapResource;
	} resources;
	struct {
		uint32_t map;
		float x;
		float y;
	} player;
} GameInfo;

GameInfo LaunchInfo;

void InitializeGameState(char* serialized);

char* SerializeGameInfo(GameInfo* gInfo);
