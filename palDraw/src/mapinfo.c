#include "mapinfo.h"

MapCollectionLength = 0;

char* SerializeMapInfoList(MapInfo* mInfoList) {
	void* result = malloc(MapCollectionLength * sizeof(MapInfo) + sizeof(unsigned int));
	unsigned int* writeLength = (unsigned int*)result;
	*writeLength = MapCollectionLength;
	writeLength++;
	uint32_t* cur = (uint32_t*)writeLength;
	for (int i = 0; i < MapCollectionLength; i++) {
		*cur = mInfoList[i].info.id;
		cur++;
		*cur = mInfoList[i].dim.w;
		cur++;
		*cur = mInfoList[i].dim.h;
		cur++;
	}
	return (char*)result;
}

void InitializeMapInfoList(char* serialized) {
	unsigned int* pLength = (unsigned int*)serialized;
	unsigned int len = *pLength;
	printf("Number of Maps: %d\n", len);
	pLength++;
	uint32_t* cur = (uint32_t*)pLength;
	MapInfo* result = malloc(sizeof(MapInfo) * len);
	MapCollectionLength = len;
	for (int i = 0; i < len; i++) {
		result[i].info.id = *cur;
		printf("ID: %d\n", result[i].info.id);
		cur++;
		result[i].dim.w = *cur;
		printf("w: %d\n", result[i].dim.w);
		cur++;
		result[i].dim.h = *cur;
		printf("h: %d\n", result[i].dim.h);
		cur++;
	}
	MapCollection = result;
}