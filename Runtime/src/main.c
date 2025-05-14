#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <debugapi.h>
#include <math.h>
#include <tchar.h>

#include <paldraw.h>
#include <pal.h>
#include <rle.h>
#include <defines.h>

#include "data.h"
#include "mapinfo.h"
#include "gameinfo.h"

#include <element.h>

#define TICK_INTERVAL 16

double appTime;
double tickStart = 0;

double GetTicks() {
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return (double)result.QuadPart - tickStart;
}
extern inline double GetTicksMs() {
	return GetTicks() / 10000.0f;
}

double time_left(void) {
	double now = GetTicksMs();
	if (appTime <= now) {
		appTime = now;
	}
	else {
		return appTime - now;
	}
	return 0;
}

int windowSizeX;
int windowSizeY;

typedef struct {
	uint32_t id;
	aabb collision;
	vec2 velocity;
	bool isStatic;
	struct {
		bool isTouchingGround;
		bool isTouchingCeiling;
	} flag;
} thing;

typedef struct {
	aabb collision;
	struct {
		uint32_t w;
		uint32_t h;
		uint16_t* arr;
	} dat;
	struct {
		uint16_t w;
		uint16_t h;
	} tex;
} grid;

thing* allRectsInGame = NULL;
int numRects = 5;

thing* player = NULL;

grid* level = NULL;
uint32_t levelw = 50;
uint32_t levelh = 37;

typedef struct {
	grid* data;
	struct {
		uint32_t w;
		uint32_t h;
	} dim;
} slevel;

bool keydown_w = false;
bool keydown_a = false;
bool keydown_s = false;
bool keydown_d = false;
bool keydown_e = false;
bool keydown_shift = false;
bool keydown_tilde = false;

void keydown_check(WPARAM key, bool check) {
	switch (key) {
	case 0x57:
		keydown_w = check;
		break;
	case 0x41:
		keydown_a = check;
		break;
	case 0x53:
		keydown_s = check;
		break;
	case 0x44:
		keydown_d = check;
		break;
	case 0x45:
		keydown_e = check;
		break;
	case 0x10:
		keydown_shift = check;
		break;
	case VK_OEM_3:
		keydown_tilde = check;
		break;
	}
}

bool canMoveUp = true;
bool canMoveDown = true;
bool canMoveLeft = true;
bool canMoveRight = true;

grid* allocateGrid(uint32_t w, uint32_t h) {
	grid* toAlloc = malloc(sizeof(grid));
	toAlloc->collision.pos.x = 0;
	toAlloc->collision.pos.y = 0;
	toAlloc->tex.w = 16;
	toAlloc->tex.h = 16;
	toAlloc->dat.w = w;
	toAlloc->dat.h = h;
	toAlloc->collision.dim.w = toAlloc->dat.w * toAlloc->tex.w;
	toAlloc->collision.dim.h = toAlloc->dat.h * toAlloc->tex.h;
	toAlloc->dat.arr = malloc(sizeof(uint16_t) * w * h * 2);
	return toAlloc;
}

void freeGrid(grid* toFree) {
	free(toFree->dat.arr);
	toFree->dat.arr = NULL;
	free(toFree);
	toFree = NULL;
	return;
}

void setGrid(grid* toSet, uint16_t* data) {
	for (int i = 0; i < toSet->dat.w * toSet->dat.h * 2; i++) {
		int x = i % toSet->dat.w;
		int y = i / toSet->dat.w;
		toSet->dat.arr[i] = data[i];
		//toSet->dat.arr[i] = ((i % 8) == 0) * 6;
	}
	return;
}

extern inline unsigned int xIntersect(float x1, float w1, float x2, float w2) {
	return x1 < x2 + w2 && x1 + w1 > x2;
}
extern inline unsigned int yIntersect(float y1, float h1, float y2, float h2) {
	return y1 < y2 + h2 && y1 + h1 > y2;
}

extern inline unsigned int horizontalRaycast(float lx, float ly, float lw, float bx, float by, float bw, float bh) {
	return lx < bx + bw && lx + lw > bx;
}
extern inline unsigned int verticalRaycast(float lx, float ly, float lh, float bx, float by, float bw, float bh) {
	return ly < by + bh && ly + lh > by;
}

extern inline unsigned int boundsCheckRR(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	return xIntersect(x1, w1, x2, w2) && yIntersect(y1, h1, y2, h2);
}
extern inline unsigned int boundsCheckX(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2, float vel) {
	bool direction = vel > 0;
	float lineOriginX = x1 + w1 * direction;
	float lineLength = 0;
	if (direction) {
		lineOriginX = x1 + w1;
		lineLength = vel;
	}
	else {
		lineOriginX = x1 + vel;
		lineLength = -vel;
	}
	return yIntersect(y1, h1, y2, h2) && horizontalRaycast(lineOriginX, y1, lineLength, x2, y2, w2, h2);
}
extern inline unsigned int boundsCheckY(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2, float vel) {
	bool direction = vel > 0;
	float lineOriginY = y1 + h1 * direction;
	float lineLength = 0;
	if (direction) {
		lineOriginY = y1 + h1;
		lineLength = vel;
	}
	else {
		lineOriginY = y1 + vel;
		lineLength = -vel;
	}
	return xIntersect(x1, w1, x2, w2) && verticalRaycast(x1, lineOriginY, lineLength, x2, y2, w2, h2);
}

extern inline unsigned int boundsCheckRR2(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	return x1 + w1 <= x2 + w2 && x1 >= x2 && y1 + h1 <= y2 + h2 && y1 >= y2;
}

int doBoundsCheck(aabb* moving, aabb* comp, vec2 vel) {
	if (vel.x != 0)
	if (boundsCheckX(moving->pos.x, moving->pos.y, moving->dim.w, moving->dim.h, comp->pos.x, comp->pos.y, comp->dim.w, comp->dim.h, vel.x)) {
		if (vel.x > 0)
			moving->pos.x = comp->pos.x - moving->dim.w;
		else
			moving->pos.x = comp->pos.x + comp->dim.w;
		return 0;
	}
	if (vel.y != 0)
	if (boundsCheckY(moving->pos.x, moving->pos.y, moving->dim.w, moving->dim.h, comp->pos.x, comp->pos.y, comp->dim.w, comp->dim.h, vel.y)) {
		if (vel.y > 0)
			moving->pos.y = comp->pos.y - moving->dim.h;
		else
			moving->pos.y = comp->pos.y + comp->dim.h;
		return 0;
	}
	return 1;
}

void doBoundsCheckProper(aabb moving, aabb stationary, vec2 vel, vec2* write) {
	write->x = vel.x;
	write->y = vel.y;
	if (vel.x != 0)
		if (boundsCheckX(moving.pos.x, moving.pos.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h, vel.x)) {
			if (vel.x > 0)
				write->x = (stationary.pos.x - moving.dim.w) - moving.pos.x;
			else
				write->x = (stationary.pos.x + stationary.dim.w) - moving.pos.x;
		}
	moving.pos.x += write->x;
	if (vel.y != 0)
		if (boundsCheckY(moving.pos.x, moving.pos.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h, vel.y)) {
			if (vel.y > 0)
				write->y = (stationary.pos.y - moving.dim.h) - moving.pos.y;
			else
				write->y = (stationary.pos.y + stationary.dim.h) - moving.pos.y;
		}
	return;
}
vec2 doBoundsCheckStruct(aabb moving, aabb stationary, vec2 vel) {
	vec2 write = {0};
	write.x = vel.x;
	write.y = vel.y;
	if (vel.x != 0)
		if (boundsCheckX(moving.pos.x, moving.pos.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h, vel.x)) {
			if (vel.x > 0)
				write.x = (stationary.pos.x - moving.dim.w) - moving.pos.x;
			else
				write.x = (stationary.pos.x + stationary.dim.w) - moving.pos.x;
		}
	moving.pos.x += write.x;
	if (vel.y != 0)
		if (boundsCheckY(moving.pos.x, moving.pos.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h, vel.y)) {
			if (vel.y > 0)
				write.y = (stationary.pos.y - moving.dim.h) - moving.pos.y;
			else
				write.y = (stationary.pos.y + stationary.dim.h) - moving.pos.y;
		}
	return write;
}

/*
int doBoundsCheck2(aabb* moving, aabb* comp, vec2 vel) {
	if (vel.x != 0)
	if (yIntersect(moving->pos.y, moving->dim.h, comp->pos.y, comp->dim.h)) {
		bool destIsToTheRight = moving->pos.x < comp->pos.x;
		float dist = moving->pos.x + vel.x
	}
	return 1;
}
*/
int doBoundsCheckInverted(aabb moving, aabb stationary, vec2 vel, vec2* write) {
	if (vel.x != 0)
	if (!boundsCheckRR2(moving.pos.x + vel.x, moving.pos.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h)) {
		if (vel.x > 0)
			write->x = (stationary.dim.w - moving.dim.w + stationary.pos.x) - moving.pos.x;
		else
			write->x = (stationary.pos.x) - moving.pos.x;
	}
	if (vel.y != 0)
	if (!boundsCheckRR2(moving.pos.x, moving.pos.y + vel.y, moving.dim.w, moving.dim.h, stationary.pos.x, stationary.pos.y, stationary.dim.w, stationary.dim.h)) {
		if (vel.y > 0)
			write->y = (stationary.dim.h - moving.dim.h + stationary.pos.y) - moving.pos.y;
		else
			write->y = (stationary.pos.y) - moving.pos.y;
	}
	return;
}

vec2 moveCharacterInScene2(thing* moving, vec2 vel) {
	vec2 finalVelocity = { vel.x, vel.y };
	vec2 bVelocity = { vel.x, vel.y };
	for (int i = 0; i < numRects; i++) {
		if (moving->id != (allRectsInGame + i)->id) {
			doBoundsCheckProper(moving->collision, (allRectsInGame + i)->collision, bVelocity, &bVelocity);
		}
	}

	
	int leftOnMap = (moving->collision.pos.x - level->collision.pos.x) / level->tex.w;
	int topOnMap = (moving->collision.pos.y - level->collision.pos.y) / level->tex.h;
	int rightOnMap = (moving->collision.pos.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	int bottomOnMap = (moving->collision.pos.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	int leftOnMap2 = (moving->collision.pos.x + bVelocity.x - level->collision.pos.x) / level->tex.w;
	int topOnMap2 = (moving->collision.pos.y + bVelocity.y - level->collision.pos.y) / level->tex.h;
	int rightOnMap2 = (moving->collision.pos.x + bVelocity.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	int bottomOnMap2 = (moving->collision.pos.y + bVelocity.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	int numOfCollisionsX = rightOnMap - leftOnMap + 1;
	int numOfCollisionsY = bottomOnMap - topOnMap + 1;
	int repeatCollisionX = abs(leftOnMap2 - leftOnMap) + 1;
	int repeatCollisionY = abs(topOnMap2 - topOnMap) + 1;
	if (vel.x != 0)
		for (int j = 0; j <= repeatCollisionX; j++) {
			for (int i = 0; i < numOfCollisionsY; i++) {
				int indexX = 0;
				int indexY = 0;
				if (vel.x > 0) {
					indexX = rightOnMap + j;
					indexY = topOnMap + i;
				}
				else {
					indexX = leftOnMap - j;
					indexY = topOnMap + i;
				}
				int index = indexX + indexY * level->dat.w;
				if (indexX < level->dat.w && indexY < level->dat.h) //here
					if (level->dat.arr[index + (level->dat.w * level->dat.h)] != 0) {
						int tileWidth = level->tex.w;
						int tileHeight = level->tex.h;
						int tileX = indexX * tileWidth + level->collision.pos.x;
						int tileY = indexY * tileHeight + level->collision.pos.y;
						//This code will have bugs in a staggered/atypical grid but this engine will not support that, though its possible tiles with unique colliders might have issues
						//doBoundsCheckProper(moving->collision, (aabb) { tileX, tileY, tileWidth, tileHeight }, bVelocity, &bVelocity);
						vec2 tempVelocity = doBoundsCheckStruct(moving->collision, (aabb) { tileX, tileY, tileWidth, tileHeight }, bVelocity);
						bVelocity.x = tempVelocity.x;
						bVelocity.y = tempVelocity.y;
						leftOnMap2 = (moving->collision.pos.x + bVelocity.x - level->collision.pos.x) / level->tex.w;
						repeatCollisionX = abs(leftOnMap2 - leftOnMap) + 1;
					}
			}
		}
	/*
		refactor all this shit you fuck
	*/
	leftOnMap = (moving->collision.pos.x - level->collision.pos.x) / level->tex.w;
	topOnMap = (moving->collision.pos.y - level->collision.pos.y) / level->tex.h;
	rightOnMap = (moving->collision.pos.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	bottomOnMap = (moving->collision.pos.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	leftOnMap2 = (moving->collision.pos.x + bVelocity.x - level->collision.pos.x) / level->tex.w;
	topOnMap2 = (moving->collision.pos.y + bVelocity.y - level->collision.pos.y) / level->tex.h;
	rightOnMap2 = (moving->collision.pos.x + bVelocity.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	bottomOnMap2 = (moving->collision.pos.y + bVelocity.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	numOfCollisionsX = rightOnMap - leftOnMap + 1;
	numOfCollisionsY = bottomOnMap - topOnMap + 1;
	repeatCollisionX = abs(leftOnMap2 - leftOnMap) + 1;
	repeatCollisionY = abs(topOnMap2 - topOnMap) + 1;
	for (int i = 0; i < numOfCollisionsX; i++) {
		int indexX = 0;
		int indexY = 0;
			indexX = leftOnMap + i;
			indexY = bottomOnMap;
		int index = indexX + indexY * level->dat.w;
		if (indexX < level->dat.w && indexY < level->dat.h) //here
			if (level->dat.arr[index + (level->dat.w * level->dat.h)] != 0) {
				moving->flag.isTouchingGround = true;
				break;
			}
			else {
				moving->flag.isTouchingGround = false;
				break;
			}
	}
	leftOnMap = (moving->collision.pos.x - level->collision.pos.x) / level->tex.w;
	topOnMap = (moving->collision.pos.y - level->collision.pos.y) / level->tex.h;
	rightOnMap = (moving->collision.pos.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	bottomOnMap = (moving->collision.pos.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	leftOnMap2 = (moving->collision.pos.x + bVelocity.x - level->collision.pos.x) / level->tex.w;
	topOnMap2 = (moving->collision.pos.y + bVelocity.y - level->collision.pos.y) / level->tex.h;
	rightOnMap2 = (moving->collision.pos.x + bVelocity.x + moving->collision.dim.w - level->collision.pos.x) / level->tex.w;
	bottomOnMap2 = (moving->collision.pos.y + bVelocity.y + moving->collision.dim.h - level->collision.pos.y) / level->tex.h;
	numOfCollisionsX = rightOnMap - leftOnMap + 1;
	numOfCollisionsY = bottomOnMap - topOnMap + 1;
	repeatCollisionX = abs(leftOnMap2 - leftOnMap) + 1;
	repeatCollisionY = abs(topOnMap2 - topOnMap) + 1;
	for (int j = 0; j <= repeatCollisionY; j++) {
		for (int i = 0; i < numOfCollisionsX; i++) {
			int indexX = 0;
			int indexY = 0;
			if (vel.y > 0) {
				indexX = leftOnMap + i;
				indexY = bottomOnMap + j;
			}
			else {
				indexX = leftOnMap + i;
				indexY = topOnMap - j;
			}
			int index = indexX + indexY * level->dat.w;
			if (indexX < level->dat.w && indexY < level->dat.h) //here
				if (level->dat.arr[index + (level->dat.w * level->dat.h)] != 0) {
					int tileWidth = level->tex.w;
					int tileHeight = level->tex.h;
					int tileX = indexX * tileWidth + level->collision.pos.x;
					int tileY = indexY * tileHeight + level->collision.pos.y;
					//doBoundsCheckProper(moving->collision, (aabb) { tileX, tileY, tileWidth, tileHeight }, bVelocity, & bVelocity);
					vec2 tempVelocity = doBoundsCheckStruct(moving->collision, (aabb) { tileX, tileY, tileWidth, tileHeight }, bVelocity);
					bVelocity.x = tempVelocity.x;
					bVelocity.y = tempVelocity.y;
					topOnMap2 = (moving->collision.pos.y + bVelocity.y - level->collision.pos.y) / level->tex.h;
					repeatCollisionY = abs(topOnMap2 - topOnMap) + 1;
				}
		}
	}
	//arena
	doBoundsCheckInverted(moving->collision, (aabb) { 0, 0, levelw * 16, levelh * 16 }, bVelocity, &bVelocity);
	if (vel.y != bVelocity.y) {
		if (vel.y >= 0)
			moving->flag.isTouchingGround = true;
		else
			moving->flag.isTouchingCeiling = true;
	}

	moving->collision.pos.x += bVelocity.x;
	moving->collision.pos.y += bVelocity.y;

	return bVelocity;
}

float speed = 3.5f;

float grav = 0.1f;
float fall = 0.0f;

void game() {
	static bool didMove = false;



	if (keydown_w && player->flag.isTouchingGround) {
		fall = -3.5f;
		player->flag.isTouchingGround = false;
	}
	else if (player->flag.isTouchingCeiling) {
		fall = 0.0f;
		player->flag.isTouchingCeiling = false;
	}

	vec2 vec_player = (vec2){ 0, fall };

	if (keydown_a) {
		vec_player.x = -speed;
	}
	else if (keydown_d) {
		vec_player.x = speed;
	}

	vec2 vec_final = moveCharacterInScene2(player, vec_player);

	if (vec_final.y != 0.0f) {
		player->flag.isTouchingGround = false;
	}
	if (!player->flag.isTouchingGround) {
		fall += grav;
	}
	else {
		fall = 0.0f;
	}

	fall = fminf(fall, 7.0f);

	return;
}

char stringTest[] = "0001";

uint32_t test = "0001";
uint32_t test2 = 0;

typedef struct PaletteResource {
	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} color;
} PaletteResource;

void fillPaletteStruct(RGBQUAD* epalette, PaletteResource* rpalette) {
	for (int i = 0; i < 256; i++) {
		epalette[i].rgbRed = rpalette[i].color.r;
		epalette[i].rgbGreen = rpalette[i].color.g;
		epalette[i].rgbBlue = rpalette[i].color.b;
		epalette[i].rgbReserved = 0;
	}
	return;
}

int GetStartingMapResource() {
	printf("A: %d\n", LaunchInfo.resources.indexMapResource + LaunchInfo.player.map);
	return LaunchInfo.resources.indexMapResource + LaunchInfo.player.map;
}

int GetStartingMapID() {
	return LaunchInfo.player.map;
}

MapInfo* GetMapFromID(int id) {
	return MapCollection + id;
}



RGBQUAD* selectedGamePalette;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

_Use_decl_annotations_ int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow) {
	tickStart = GetTicks();
	test2 = stringTest;

	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	PALDRAW_isLoaded();

	printf("Loading MAPINFO\n");

	char* loadMapInfo = NULL;
	unsigned int loadMapInfoSize = 0;
	loadGameResource(RRID_MAPINFO, &loadMapInfo, &loadMapInfoSize);
	InitializeMapInfoList(loadMapInfo);

	printf("Loading LAUNCHINFO\n");

	char* loadLaunchInfo = NULL;
	unsigned int loadLaunchInfoSize = 0;
	loadGameResource(RRID_GAMEINFO, &loadLaunchInfo, &loadLaunchInfoSize);
	InitializeGameState(loadLaunchInfo);

	printf("Loading MAP DATA\n");

	uint16_t* loadMapDataTest = NULL;
	unsigned int loadMapDataSize = 0;
	loadGameResource(GetStartingMapResource(), &loadMapDataTest, &loadMapDataSize);
	uint16_t* writeBufferUncompressedRLE = RunLengthDecode(loadMapDataTest);

	printf("Loading PALETTE DATA\n");

	unsigned char* loadPaletteDataTest = NULL;
	unsigned int loadPaletteDataSize = 0;
	loadGameResource(112, &loadPaletteDataTest, &loadPaletteDataSize);

	selectedGamePalette = malloc(sizeof(RGBQUAD) * 256);
	fillPaletteStruct(selectedGamePalette, (PaletteResource*)loadPaletteDataTest);

	//_tprintf(TEXT("%s"), szCmdLine);
	/*
	MessageBox(NULL, szCmdLine, szCmdLine, MB_ICONERROR);
	printf("%s\n", szCmdLine);
	printf("Canprint");
	*/
	allRectsInGame = malloc(sizeof(thing) * numRects);
	for (int i = 0; i < numRects; i++) {
		allRectsInGame[i].id = i;
		allRectsInGame[i].collision.pos.x = (float)(i % 7 * 160 + 50);
		allRectsInGame[i].collision.pos.y = 0;
		allRectsInGame[i].collision.dim.w = 16;
		allRectsInGame[i].collision.dim.h = 16;
		allRectsInGame[i].isStatic = true;
		allRectsInGame[i].velocity.x = 0;
		allRectsInGame[i].velocity.y = 0;
		allRectsInGame[i].flag.isTouchingGround = false;
		allRectsInGame[i].flag.isTouchingCeiling = false;
	}
	player = allRectsInGame;
	player->collision.pos.x = LaunchInfo.player.x;
	player->collision.pos.y = LaunchInfo.player.y;

	printf("Making Level\n");
	printf("first map: %d\n", GetStartingMapID());
	levelw = GetMapFromID(GetStartingMapID())->dim.w;
	levelh = GetMapFromID(GetStartingMapID())->dim.h;
	printf("Allocating Grid\n");
	level = allocateGrid(levelw, levelh);
	printf("Level Made\n");
	setGrid(level, writeBufferUncompressedRLE);
	printf("Grid set\n");

	//grid* smalltest = allocateGrid(100, 100);

	RECT clientAreaRect;
	clientAreaRect.left = 0;
	clientAreaRect.top = 0;
	clientAreaRect.right = 1600;
	clientAreaRect.bottom = 1200;
	AdjustWindowRect(&clientAreaRect, WS_OVERLAPPEDWINDOW, false);

	int CorrectScreenSizeX, CorrectScreenSizeY;
	CorrectScreenSizeX = clientAreaRect.right + abs(clientAreaRect.left);
	CorrectScreenSizeY = clientAreaRect.bottom + abs(clientAreaRect.top);

	static TCHAR szAppName[] = TEXT("Enshitten");
	HWND hwnd;
	WNDCLASS wndclass = { 0 };
	MSG message;
	wndclass.lpszClassName = szAppName;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndclass);
	hwnd = CreateWindow(
		szAppName,
		TEXT("Runtime"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CorrectScreenSizeX, CorrectScreenSizeY,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	appTime = GetTicksMs() + TICK_INTERVAL;


	timeBeginPeriod(1);
	while (TRUE) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			//game logic
			game();
			InvalidateRect(hwnd, NULL, FALSE);
			Sleep(time_left());
			appTime += TICK_INTERVAL;
		}
	}
	timeEndPeriod(1);
	return message.wParam;
}

typedef struct TexData {
	struct {
		int w;
		int h;
	} dim;
	unsigned char* data;
} TexData;

TexData LoadedTilesetTexture;

void unpackTexture(unsigned char* packed, TexData* tex) {

	int* readDimensions = (int*)packed;
	//printf("\nREADING TEXTURE DATA\n");
	tex->dim.w = readDimensions[0];
	tex->dim.h = readDimensions[1];
	tex->data = malloc(tex->dim.w * tex->dim.h);
	//printf("WIDTH: %d\nHEIGHT: %d\n", width, height);
	unsigned char* readData = (unsigned char*)(readDimensions + 2);
	for (int i = 0; i < tex->dim.w * tex->dim.h; i++) {
		tex->data[i] = readData[i];
	}
	return;
}

extern inline void renderTextASCIIToBuffer(HDC hdcDest, HDC hdcGlyphAtlas, char* text, unsigned int numOfCharacters, unsigned int textBoxWidth, int x, int y) {
	for (int i = 0; i < numOfCharacters; i++) {
		unsigned char ascii_code = text[i] - 32;
		if (text[i] == 0)
			ascii_code = 0;
		int glyphMapWidth = 128 / 8;
		int glyphMapHeight = 48 / 8;
		int iglyph_x = ascii_code % glyphMapWidth;
		int iglyph_y = ascii_code / glyphMapWidth;
		BitBlt(hdcDest, i % textBoxWidth * 8 + x, i / textBoxWidth * 8 + y, 8, 8, hdcGlyphAtlas, iglyph_x * 8, iglyph_y * 8, SRCCOPY);
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	void* testPointer;
	static HBITMAP hBackbuffer;
	static HBITMAP hTileset;
	static HBITMAP hVersionText;
	static HBITMAP hGlyphAtlas;
	static HBITMAP hBackground;
	switch (message) {
	case WM_CREATE: {
		HDC hdc = GetWindowDC(hwnd);
		hBackbuffer = BitmapFill(hdc, 800, 600, &testPointer, 0, selectedGamePalette);
		hBackground = BitmapFill(hdc, 800, 600, &testPointer, 16, selectedGamePalette);
		int tilesetSiz = 0;
		unsigned char* packedTextureData;
		loadGameResource(113, &packedTextureData, &tilesetSiz);
		unpackTexture(packedTextureData, &LoadedTilesetTexture);

		hVersionText = BitmapFill(hdc, 256, 8, &testPointer, 0, selectedGamePalette);
		hGlyphAtlas = BitmapNormal(hdc, 128, 48, &testPointer, textureData_Glyph, selectedGamePalette);

		HDC hdcVersionText = CreateCompatibleDC(hdc);
		HDC hdcGlyphAtlas = CreateCompatibleDC(hdc);
		SelectObject(hdcVersionText, hVersionText);
		SelectObject(hdcGlyphAtlas, hGlyphAtlas);

		renderTextASCIIToBuffer(hdcVersionText, hdcGlyphAtlas, "Brickhouse a.1.0.0", 32, 32, 0, 0);

		DeleteDC(hdcGlyphAtlas);
		DeleteDC(hdcVersionText);

		//printf("imgay");
		hTileset = BitmapNormal(hdc, LoadedTilesetTexture.dim.w, LoadedTilesetTexture.dim.h, &testPointer, LoadedTilesetTexture.data, selectedGamePalette);
		//convertBMPToEngineFormat(hdc, TEXT("res/outdoor.bmp"), default_palette, (unsigned char*)testPointer);
		DeleteDC(hdc);
		break;
	}
	case WM_SIZE:
		windowSizeX = LOWORD(lParam);
		windowSizeY = HIWORD(lParam);
		break;
	case WM_KEYDOWN:
		keydown_check(wParam, true);
		break;
	case WM_KEYUP:
		keydown_check(wParam, false);
		break;
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		//Rectangle(hdcPaint, 0, 0, windowSizeX, windowSizeY);
		

		HDC hdcBackbuffer = CreateCompatibleDC(hdcPaint);
		HDC hdcTileset = CreateCompatibleDC(hdcPaint);
		HDC hdcVersionText = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcBackbuffer, hBackbuffer);
		SelectObject(hdcTileset, hTileset);
		SelectObject(hdcVersionText, hVersionText);

		HDC hdcBackground = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcBackground, hBackground);
		BitBlt(hdcBackbuffer, 0, 0, 800, 600, hdcBackground, 0, 0, SRCCOPY);
		DeleteDC(hdcBackground);
		
		//if (allRectsInGame != NULL)
		for (int y = 0; y < levelh; y++) {
			for (int x = 0; x < levelw; x++) {
				int sx = x * level->tex.w - player->collision.pos.x + 800 / 2;
				int sy = y * level->tex.h - player->collision.pos.y + 600 / 2;
				BitBlt(hdcBackbuffer, sx, sy, level->tex.w, level->tex.h, hdcTileset, (level->dat.arr[x + y * levelw] % (LoadedTilesetTexture.dim.w / level->tex.w)) * level->tex.w, (level->dat.arr[x + y * levelw] / (LoadedTilesetTexture.dim.w / level->tex.w)) * level->tex.h, SRCCOPY);
			}
		}
		for (int i = 0; i < numRects; i++) {
			//Rectangle(hdcPaint, allRectsInGame[i].collision.pos.x, allRectsInGame[i].collision.pos.y, allRectsInGame[i].collision.dim.w + allRectsInGame[i].collision.pos.x, allRectsInGame[i].collision.dim.h + allRectsInGame[i].collision.pos.y);
			BitBlt(hdcBackbuffer, allRectsInGame[i].collision.pos.x - player->collision.pos.x + 800 / 2, allRectsInGame[i].collision.pos.y - player->collision.pos.y + 600 / 2, allRectsInGame[i].collision.dim.w, allRectsInGame[i].collision.dim.h, hdcTileset, 16, 48, SRCCOPY);
		}

		BitBlt(hdcBackbuffer, 0, 0, 128, 8, hdcVersionText, 0, 0, SRCPAINT);

		//TODO: Check if device can StretchBlt
		StretchBlt(hdcPaint, 0, 0, windowSizeX, windowSizeY, hdcBackbuffer, 0, 0, 800, 600, SRCCOPY);

		DeleteDC(hdcBackbuffer);
		DeleteDC(hdcTileset);
		DeleteDC(hdcVersionText);

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}