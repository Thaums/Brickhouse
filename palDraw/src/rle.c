
#include "rle.h"

uint16_t* RunLengthEncode(uint16_t* uncompressed, size_t siz) {
	if (siz == 0)
		return NULL;
	uint16_t* compressed;
	int count = 1;
	uint16_t cur = uncompressed[0];
	for (int i = 1; i < siz; i++) {
		if (cur != uncompressed[i]) {
			cur = uncompressed[i];
			count++;
		}
	}
	compressed = malloc(sizeof(uint16_t) * 2 * count + sizeof(size_t)); //prepend the new size
	if (!compressed) {
		MessageBox(NULL, TEXT("Could not allocate memory for RLE encode"), TEXT("Error"), MB_ICONERROR);
		return NULL;
	}
	size_t* writeSize = (size_t*)compressed;
	compressed = writeSize + 1;
	count = 0;
	size_t total_size = 1;
	cur = uncompressed[0];
	for (int i = 0; i < siz; i++) {
		if (i)
			count++;
		if (cur != uncompressed[i] || count == MAXUINT16) {
			*compressed = count;
			compressed++;
			*compressed = cur;
			compressed++;
			cur = uncompressed[i];
			count = 0;
			total_size++;
		}
	}
	count++;
	*compressed = count;
	compressed++;
	*compressed = cur;
	compressed++;
	*writeSize = total_size;
	return (uint16_t*)writeSize;
}

uint16_t* RunLengthDecode(uint16_t* compressed) {
	size_t cSize = *(size_t*)compressed;
	compressed = ((size_t*)compressed) + 1;
	uint16_t* iter = compressed;
	size_t uSize = 0;
	for (int i = 0; i < cSize; i++) {
		uSize += *iter;
		iter += 2;
	}
	iter = NULL;
	if (!uSize) {
		return NULL;
	}
	uint16_t* uncompressed = malloc(sizeof(uint16_t) * uSize);
	if (!uncompressed) {
		return NULL;
	}
	uint16_t* result = uncompressed;
	iter = compressed;
	for (int i = 0; i < cSize; i++) {
		uint16_t repeat = *iter;
		iter++;
		uint16_t character = *iter;
		iter++;
		for (int r = 0; r < repeat; r++) {
			*uncompressed = character;
			uncompressed++;
		}
	}
	iter = NULL;
	return result;
}