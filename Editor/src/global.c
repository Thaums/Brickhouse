#include "global.h"

const TCHAR szTilesetName[] = TEXT("TilesetPicker");
const TCHAR szViewportName[] = TEXT("Viewport");
const TCHAR szLayerManagerName[] = TEXT("Layer Manager");
const TCHAR szPaletteManagerName[] = TEXT("Palette Manager");
const TCHAR szOverviewName[] = TEXT("Overview");
const TCHAR szTreeWindowName[] = TEXT("Map Tree");

void initGlobals() {
	tickStart = 0;

	selectedTileIndex = 0;
	zoomLevel = 3;
	viewportTest = NULL;
	overviewHWND = NULL;
	tilepickerHWND = NULL;
	CanDrawViewport = true;
	TilesetDataWidth = 0;
	TilesetDataHeight = 0;

	map_data = NULL;
	collision_data = NULL;
	map_width = 0;
	map_height = 0;
	
	//MapCollection needs to be NULL for realloc
	MapCollection = NULL;
	MapCollectionLength = 0;
	
	//todo: store in project file
	g_viewport_mode = VMODE_DRAW;

	elementStack = stackInit();
	mapStack = stackInit();
}
