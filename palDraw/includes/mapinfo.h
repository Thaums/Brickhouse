#pragma once
#include <stdint.h>

typedef struct MapInfo {
	struct {
		uint32_t w;
		uint32_t h;
	} dim;
	struct {
		uint32_t id;
	} info;
} MapInfo;

MapInfo* MapCollection;
unsigned int MapCollectionLength;

void InitializeMapInfoList(char* serialized);
char* SerializeMapInfoList(MapInfo* mInfoList);
