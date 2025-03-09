#include "coolmath.h"

int randInt(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}