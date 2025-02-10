
#include "layer.h"


MapLayerStack* initMLS() {
	MapLayerStack* result = malloc(sizeof(MapLayerStack));
	result->size = 0;
	result->bottom = NULL;
	result->top = NULL;
	return result;
}

MapLayerStackNodule* mlsNodeCreate(unsigned int width, unsigned int height) {
	MapLayerStackNodule* result = malloc(sizeof(MapLayerStackNodule));
	result->data = malloc(sizeof(int) * width * height);
	result->height = height;
	result->width = width;
	result->up = NULL;
	result->down = NULL;
	return result;
}

void mlsNodeFree(MapLayerStackNodule* node) {
	free(node->data);
	node->data = NULL;
	free(node);
	node = NULL;
	return;
}

void mlsFree(MapLayerStack* list) {
	MapLayerStackNodule* cur = list->top;
	while (cur != NULL) {
		MapLayerStackNodule* next = cur->down;
		mlsNodeFree(cur);
		cur = next;
	}
	list->size = 0;
	list->bottom = NULL;
	list->top = NULL;
	return;
}

bool mlsPush(MapLayerStack* list, MapLayerStackNodule* node) {
	if (list->size == 0) {
		list->bottom = node;
		list->top = node;
	}
	else {
		list->top->up = node;
		node->down = list->top;
		list->top = node;
	}
	list->size++;
	return true;
}

bool mlsPop(MapLayerStack* list, int index) {
	if (list->size == 0)
		return false;
	if (index >= list->size)
		return false;
	if (list->size == 1) {
		mlsNodeFree(list->top);
		list->top = NULL;
		list->bottom = NULL;
	}
	else {
		if (index < 0) {
			printf("TBD\n");
		}
		else {
			MapLayerStackNodule* cur = list->bottom;
			for (int i = 0; i < index; i++) {
				cur = cur->up;
			}
			if (index == 0) {
				list->bottom = cur->up;
				cur->up->down = NULL;
			}
			else if (index == (list->size - 1)) {
				list->top = cur->down;
				cur->down->up = NULL;
			}
			else {
				cur->down->up = cur->up;
				cur->up->down = cur->down;
			}
			mlsNodeFree(cur);
			cur = NULL;
		}
	}
	list->size--;
	return true;
}