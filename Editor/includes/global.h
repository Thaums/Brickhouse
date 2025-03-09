#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdbool.h>
#include "layer.h"
#include "mapinfo.h"
#include "gameinfo.h"
#include "celement.h"
#include "stack.h"

#define TICK_INTERVAL 16

#define VMODE_DRAW 0
#define VMODE_ELEM 1
#define VMODE_COL  2


#ifndef BRICKHOUSE_GLOBALS
#define BRICKHOUSE_GLOBALS
double appTime;
double tickStart;

unsigned int selectedTileIndex;
unsigned int zoomLevel;
HWND viewportTest;
HWND tilepickerHWND;
HWND overviewHWND;
bool CanDrawViewport;

unsigned char* TilesetDataToSend;
int TilesetDataWidth;
int TilesetDataHeight;

unsigned int windowSizeW, windowSizeH;

uint16_t* map_data;
unsigned int map_width;
unsigned int map_height;

MapLayerStack* map_layers;

extern const TCHAR szTilesetName[];
extern const TCHAR szViewportName[];
extern const TCHAR szLayerManagerName[];
extern const TCHAR szPaletteManagerName[];
extern const TCHAR szOverviewName[];

RGBQUAD paletteInMemory[256];

int g_viewport_mode;

Stack* elementStack;

#endif // !_BRICKHOUSE_GLOBALS


void initGlobals();