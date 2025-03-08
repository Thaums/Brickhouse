#include "gameinfo.h"

char* SerializeGameInfo(GameInfo* gInfo) {
	char* result = malloc(sizeof(GameInfo));
	uint32_t* writeGameResourceIndices = (uint32_t*)result;
	*writeGameResourceIndices = gInfo->resources.indexMapResource;
	writeGameResourceIndices++;
	uint32_t* writeStartingMap = (uint32_t*)writeGameResourceIndices;
	*writeStartingMap = gInfo->player.map;
	writeStartingMap++;
	float* writePlayerCoords = (float*)writeStartingMap;
	*writePlayerCoords = gInfo->player.x;
	writePlayerCoords++;
	*writePlayerCoords = gInfo->player.y;
	return result;
}

void InitializeGameState(char* serialized) {
	uint32_t* gameResourceIndices = (uint32_t*)serialized;
	LaunchInfo.resources.indexMapResource = *gameResourceIndices;
	gameResourceIndices++;
	uint32_t* startingMap = (uint32_t*)gameResourceIndices;
	LaunchInfo.player.map = *startingMap;
	startingMap++;
	float* playerCoords = (float*)startingMap;
	LaunchInfo.player.x = *playerCoords;
	playerCoords++;
	LaunchInfo.player.y = *playerCoords;
	return;
}