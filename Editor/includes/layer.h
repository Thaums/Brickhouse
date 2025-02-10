#pragma once

#ifndef MAPLAYER_INCLUDES
#define MAPLAYER_INCLUDES

#include <stdio.h>
#include <stdbool.h>

typedef struct MapLayerStackNodule {
	struct MapLayerStackNodule* down;
	struct MapLayerStackNodule* up;
	unsigned int* data;
	unsigned int width;
	unsigned int height;
} MapLayerStackNodule;

typedef struct {
	MapLayerStackNodule* top;
	MapLayerStackNodule* bottom;
	unsigned int size;
} MapLayerStack;

MapLayerStack* initMLS();

MapLayerStackNodule* mlsNodeCreate(unsigned int width, unsigned int height);

void mlsFree(MapLayerStack* list);

void mlsNodeFree(MapLayerStackNodule* node);

bool mlsPush(MapLayerStack* list, MapLayerStackNodule* node);

bool mlsPop(MapLayerStack* list, int index);

#endif