#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <tchar.h>
#include <stdint.h>
#include <math.h>

#include "global.h"

#include "layer.h"
#include "embed.h"

#include <pal.h>
#include <paldraw.h>
#include <rle.h>
#include <io.h>
#include <defines.h>

#include "resource.h"

#include "menu.h"

#include "layermanager.h"
#include "palettemanager.h"
#include "viewport.h"
#include "tilepicker.h"
#include "tree.h"

#include <coolmath.h>

#include "button.h"

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

#define CMDLINELEN 512
void RunExecutableWithArguments(const TCHAR* executablePath, const TCHAR* arguments) {
	// Prepare the command line
	int length = CMDLINELEN;
	TCHAR commandLine[CMDLINELEN];
	//_tprintf(arguments);
	_sntprintf_s(commandLine, CMDLINELEN, 100, TEXT("\"%s\" %s"), executablePath, arguments);
	_tprintf(commandLine);
	// Set up the process information structures
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Create the process
	if (!CreateProcess(
		NULL,          // No module name (use command line)
		commandLine,   // Command line
		NULL,          // Process handle not inheritable
		NULL,          // Thread handle not inheritable
		FALSE,         // Set handle inheritance to FALSE
		0,             // No creation flags
		NULL,          // Use parent's environment block
		NULL,          // Use parent's starting directory
		&si,           // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		) {
		fprintf(stderr, "CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}




unsigned char* paletteToResource(RGBQUAD* palette) {
	int paletteSize = 256 * 3;
	unsigned char* result = malloc(paletteSize);
	for (int i = 0; i < 256; i++) {
		int ires = i * 3;
		result[ires] = palette[i].rgbRed;
		result[ires + 1] = palette[i].rgbGreen;
		result[ires + 2] = palette[i].rgbBlue;
	}
	return result;
}

struct TopMenuButtons {
	HWND drawButton; //tilemap editor
	HWND elementButton; //element editor for dynamic game objects 
	HWND collisionButton; //tilemap collision editor
	HWND pasteButton;
	HWND zoomOut;
	HWND zoomIn;
};

struct TopMenuButtons TMenuButtons;
int TMenuIcons[] = {
	IDI_ICON1,
	IDI_ICON1,
	IDI_ICON3,
	IDI_ICON2,
	IDI_ICON5,
	IDI_ICON4
};

DWORD TMenuButtonFlags[] = {
	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_PUSHLIKE | BS_ICON,
	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_PUSHLIKE | BS_ICON,
	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_PUSHLIKE | BS_ICON,
	WS_CHILD | WS_VISIBLE | BS_ICON,
	WS_CHILD | WS_VISIBLE | BS_ICON,
	WS_CHILD | WS_VISIBLE | BS_ICON
};

int TMenuIDs[] = {
	1001,
	1002,
	1003,
	NULL,
	NULL,
	NULL
};

void game() {
	return;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void copyPaletteToPalette(RGBQUAD* src, RGBQUAD* dest, int length, int offset) {
	for (int i = offset; i < length; i++) {
		dest[i].rgbRed = src[i].rgbRed;
		dest[i].rgbGreen = src[i].rgbGreen;
		dest[i].rgbBlue = src[i].rgbBlue;
	}
	return;
}

HMENU CreateTilesetMenu() {
	HMENU result = CreateMenu();
	HMENU subMenu = CreateMenu();
	AppendMenu(subMenu, MF_STRING, 6001, TEXT("&Load Bitmap"));
	AppendMenu(result, MF_POPUP, subMenu, TEXT("&File"));
	return result;
}



_Use_decl_annotations_ int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	initGlobals();
	g_hInst = hInstance;
	tickStart = GetTicks();

	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	map_layers = initMLS();
	mlsPush(map_layers, mlsNodeCreate(5, 20));
	mlsPush(map_layers, mlsNodeCreate(50, 22));
	mlsPush(map_layers, mlsNodeCreate(500, 500));

	//mlsPop(map_layers, -1);

	MapLayerStackNodule* cur = map_layers->top;
	for (int i = 0; i < map_layers->size; i++) {
		if (cur != NULL) {
			printf("%d, %d\n", cur->width, cur->height);
			cur = cur->down;
		}
	}

	copyPaletteToPalette(outdoor_pal, paletteInMemory, 256, 0);

	static TCHAR szAppName[] = TEXT("Brickhouse2D");

	HWND hwnd;
	WNDCLASS wndclass = {0};
	MSG message;
	wndclass.lpszClassName = szAppName;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APPLICATION));
	wndclass.hInstance = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndclass);

	wndclass.lpfnWndProc = TilePickerWndProc;
	wndclass.lpszClassName = szTilesetName;
	wndclass.hIcon = NULL;
	//wndclass.hbrBackground = NULL;
	RegisterClass(&wndclass);
	wndclass.lpfnWndProc = ViewportWndProc;
	wndclass.lpszClassName = szViewportName;
	RegisterClass(&wndclass);
	wndclass.lpfnWndProc = LayerManagerWndProc;
	wndclass.lpszClassName = szLayerManagerName;
	RegisterClass(&wndclass);
	wndclass.lpfnWndProc = PaletteManagerWndProc;
	wndclass.lpszClassName = szPaletteManagerName;
	RegisterClass(&wndclass);
	wndclass.lpfnWndProc = OverviewWndProc;
	wndclass.lpszClassName = szOverviewName;
	RegisterClass(&wndclass);
	wndclass.lpfnWndProc = TreeWndProc;
	wndclass.lpszClassName = szTreeWindowName;
	RegisterClass(&wndclass);
	
	hwnd = CreateWindow(
		szAppName,
		TEXT("Brickhouse2D"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL,
		GetAppMenu(),
		hInstance,
		NULL
	);
	HWND hLayerManager = CreateWindowEx(
		WS_EX_TOOLWINDOW, szLayerManagerName,
		TEXT("Layers"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		hwnd,
		NULL,
		hInstance,
		NULL
	);
	HWND hPaletteManager = CreateWindowEx(
		WS_EX_TOOLWINDOW, szPaletteManagerName,
		TEXT("Palette"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		hwnd,
		NULL,
		hInstance,
		NULL
	);


	//SetWindowPos(hLayerManager, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	/*
	appTime = GetTicksMs() + TICK_INTERVAL;
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
	*/
	return message.wParam;
}




void backbufferRender(RECT rcPaint, HDC dest, HDC source, int offsetY) {
	int x_val = rcPaint.left / 16 / zoomLevel;
	int w_val = rcPaint.right / 16 / zoomLevel;
	int y_val = rcPaint.top / 16 / zoomLevel;
	int h_val = rcPaint.bottom / 16 / zoomLevel;

	if (map_data != NULL) {
		for (int y = y_val; y < h_val; y++) {
			for (int x = x_val; x < w_val; x++) {
				unsigned int img_width = TilesetDataWidth / 16;
				unsigned int index = x + y * map_width;
				unsigned int ix = map_data[index] % img_width;
				unsigned int iy = map_data[index] / img_width;
				BitBlt(dest, x * 16, y * 16, 16, 16, source, ix * 16, iy * 16, SRCCOPY);
			}
		}
	}
}

void backbufferPencil(int x, int y, HDC dest, HDC source) {
	RECT square;
	square.left = x * 16 * zoomLevel;
	square.right = square.left + (16 * zoomLevel);
	square.top = y * 16 * zoomLevel;
	square.bottom = square.top + (16 * zoomLevel);
	backbufferRender(square, dest, source, 0);
	return;
}

void bucketFill(unsigned int index, unsigned int color, unsigned int replaceColor) {
	map_data[index] = replaceColor;
	int x = index % map_width;
	int y = index / map_width;
	if (x < map_width - 1)
		if (map_data[index + 1] == color) {
			bucketFill(index + 1, color, replaceColor);
		}
	if (x > 0)
		if (map_data[index - 1] == color) {
			bucketFill(index - 1, color, replaceColor);
		}
	if (y < map_height - 1)
		if (map_data[index + map_width] == color) {
			bucketFill(index + map_width, color, replaceColor);
		}
	if (y > 0)
		if (map_data[index - map_width] == color) {
			bucketFill(index - map_width, color, replaceColor);
		}
	return;
}


unsigned char* packTexture(int width, int height, unsigned char* data) {
	unsigned char* result = malloc(sizeof(int) * 2 + width * height);
	int* writeDimensions = (int*)result;
	writeDimensions[0] = width;
	writeDimensions[1] = height;
	unsigned char* writeData = (unsigned char*)(writeDimensions + 2);
	for (int i = 0; i < width * height; i++) {
		writeData[i] = data[i];
	}
	return result;
}

void readPackedTexture(unsigned char* texture) {
	int* readDimensions = (int*)texture;
	printf("\nREADING TEXTURE DATA\n");
	int width = readDimensions[0];
	int height = readDimensions[1];
	printf("WIDTH: %d\nHEIGHT: %d\n", width, height);
	unsigned char* readData = (unsigned char*)(readDimensions + 2);
	for (int i = 0; i < width * height; i++) {
		printf("%d", readData[i]);
	}
	return;
}

void _UpdateRuntimeSettings(const TCHAR* executablePath) {
	embed_nofile(executablePath, SerializeGameInfo(&LaunchInfo), sizeof(GameInfo), RRID_GAMEINFO);
}

void _UpdateRuntimeMapCollection(const TCHAR* executablePath) {
	embed_nofile(executablePath, SerializeMapInfoList(MapCollection), MapCollectionLength * sizeof(MapInfo) + sizeof(unsigned int), RRID_MAPINFO);
}

void _UpdateRuntimePalette(const TCHAR* executablePath, int resourceID, RGBQUAD* palette) {
	unsigned char* palette_data = paletteToResource(palette);
	embed_nofile(executablePath, palette_data, 256 * 3, resourceID);
	free(palette_data);
}

void _UpdateRuntimeBMP(const TCHAR* executablePath, int resourceID, int width, int height, char* bitmap) {
	unsigned char* TilesetDataPacked = packTexture(width, height, bitmap);
	embed_nofile(executablePath, TilesetDataPacked, width * height, resourceID);
	free(TilesetDataPacked);
}

#define NUMOFMAPLAYERS 2
#define _RLE_MAPDATATYPE uint16_t
#define RLE_MAPDATA _RLE_MAPDATATYPE*

size_t RLE_MAPDATA_GetLength(RLE_MAPDATA md) {
	return *(size_t*)md;
}

#define RLE_MAPDATA_SIZEPLUSHEADER(enc) sizeof(size_t) + sizeof(_RLE_MAPDATATYPE) * NUMOFMAPLAYERS * RLE_MAPDATA_GetLength(enc)

int _UpdateRuntimeMap(const TCHAR* executablePath, EditorMap* map) {
	MapInfo* record = MapCollection + map->id;
	int area = record->dim.w * record->dim.h;
	int length = area * NUMOFMAPLAYERS;
	uint16_t* writeBuffer = malloc(length * sizeof(uint16_t));
	if (!writeBuffer) {
		return 0;
	}
	uint16_t* write = writeBuffer;

	/*
	* Write visual background tiles
	*/
	uint16_t* bg_layer = map->tilemap;
	for (int i = 0; i < area; i++) {
		*write++ = *bg_layer++;
	}
	bg_layer = NULL;

	/*
	* Write collision tiles
	*/
	uint16_t* col_layer = map->collision;
	for (int i = 0; i < area; i++) {
		*write++ = *col_layer++;
	}
	col_layer = NULL;

	write = NULL;

	/*
	* Compress & patch runtime w/ all tile data for this map
	*/
	RLE_MAPDATA writeBufferCompressedRLE = RunLengthEncode(writeBuffer, length);
	embed_nofile(executablePath, writeBufferCompressedRLE, RLE_MAPDATA_SIZEPLUSHEADER(writeBufferCompressedRLE), LaunchInfo.resources.indexMapResource + map->id);
	free(writeBufferCompressedRLE);
	writeBufferCompressedRLE = NULL;

	return 1;
}

void UpdateRuntimeResources() {
	const TCHAR* executablePath = TEXT("runtime.exe");
	const TCHAR* arguments = TEXT("test.map");

	// Settings
	_UpdateRuntimeSettings(executablePath);

	// Map Records
	_UpdateRuntimeMapCollection(executablePath);

	StackNode* cur_map = mapStack->bottom;
	for (int i = 0; i < mapStack->size; i++) {
		if (!_UpdateRuntimeMap(executablePath, (EditorMap*)cur_map->data))
			return 0;
		cur_map = cur_map->up;
	}
	// Map that gets immediately loaded on launch
	//if (!_UpdateRuntimeMap(executablePath, LaunchInfo.resources.indexMapResource + LaunchInfo.player.map))
	//	return 0;

	// Only Palette for now
	_UpdateRuntimePalette(executablePath, 112, paletteInMemory);

	// The Tileset
	_UpdateRuntimeBMP(executablePath, 113, TilesetDataWidth, TilesetDataHeight, TilesetDataToSend);
	return 1;
}

HANDLE OpenFileForWriting(TCHAR* filePath) {
	// Create or open the file
	HANDLE hFile = CreateFile(
		filePath,                // File path
		GENERIC_WRITE,           // Desired access
		0,                       // Share mode
		NULL,                    // Security attributes
		CREATE_ALWAYS,           // Creation disposition
		FILE_ATTRIBUTE_NORMAL,   // Flags and attributes
		NULL                     // Template file handle
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not create or open file (error %lu)\n", GetLastError());
		return;
	}

	return hFile;
}

HANDLE OpenFileForReading(TCHAR* filePath) {
	// Create or open the file
	HANDLE hFile = CreateFile(
		filePath,                // File path
		GENERIC_READ,           // Desired access
		FILE_SHARE_READ,                       // Share mode
		NULL,                    // Security attributes
		OPEN_EXISTING,           // Creation disposition
		FILE_ATTRIBUTE_NORMAL,   // Flags and attributes
		NULL                     // Template file handle
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not open file (error %lu)\n", GetLastError());
		return;
	}

	return hFile;
}

void CloseFileHandle(HANDLE hFile) {
	CloseHandle(hFile);
}

void AppendToFile(HANDLE hFile, const char* data, unsigned int length) {
	// Write data to the file
	DWORD bytesWritten;
	BOOL result = WriteFile(
		hFile,                  // Handle to the file
		data,                   // Data to write
		length,    // Number of bytes to write
		&bytesWritten,          // Number of bytes written
		NULL                    // Overlapped structure
	);

	if (!result) {
		printf("Failed to write to file (error %lu)\n", GetLastError());
	}
	else {
		printf("Successfully written %lu bytes to file\n", bytesWritten);
	}
}

char* ReadChunkFromFile(HANDLE hFile, DWORD offset, DWORD sizeToRead) {
	// Move the file pointer to the desired offset
	DWORD result = SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
	if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
		printf("Failed to set file pointer. Error: %lu\n", GetLastError());
		return NULL;
	}

	// Allocate a buffer
	//char* buffer = (char*)HeapAlloc(GetProcessHeap(), 0, sizeToRead + 1);
	char* buffer = malloc(sizeToRead);
	if (!buffer) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	DWORD bytesRead = 0;
	if (!ReadFile(hFile, buffer, sizeToRead, &bytesRead, NULL)) {
		printf("ReadFile failed. Error: %lu\n", GetLastError());
		//HeapFree(GetProcessHeap(), 0, buffer);
		free(buffer);
		return NULL;
	}

	//buffer[bytesRead] = '\0'; // Null-terminate for safe string usage
	return buffer;
}

TCHAR* FileDialogOpen(HWND hwnd) {
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH] = TEXT("");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("All Files\0*.*\0Text Files\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)) {
		MessageBox(hwnd, ofn.lpstrFile, TEXT("Selected File"), MB_OK);
	}
	size_t len = _tcslen(ofn.lpstrFile);
	TCHAR* result = malloc(MAX_PATH * sizeof(TCHAR));
	if (!result)
		return NULL;
	_tcscpy_s(result, MAX_PATH, ofn.lpstrFile);
	return result;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndTilepicker;
	static HWND hwndViewport;
	static HWND hwndObjects;
	static HWND hwndTree;
	PAINTSTRUCT ps;
	switch (message) {
	case WM_CREATE: {
		CreateBackground(hwnd);

		HWND* PTMenuButtons = &TMenuButtons;

		for (int i = 0; i < sizeof(struct TopMenuButtons) / sizeof(HWND); i++) {
			*(PTMenuButtons + i) = CreateButton(hwnd, 23 * i + 266, 43, 23, 23, LoadIconWithSize((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TMenuIcons[i], 16, 16), TMenuButtonFlags[i], TMenuIDs[i]);
		}
		hwndViewport = CreateWindow(
			szViewportName,
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			1,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);

		hwndTilepicker = CreateWindow(
			szTilesetName,
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			2,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);

		hwndObjects = CreateWindow(
			szOverviewName,
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			3,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);
		hwndTree = CreateWindow(
			szTreeWindowName,
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			4,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);
		
		//LaunchInfo Init
		LaunchInfo.player.map = 1;
		LaunchInfo.player.x = 16 * 10;
		LaunchInfo.player.y = 16 * 10;
		LaunchInfo.resources.indexMapResource = 104;
		

		break;
	}
	case WM_SIZE: {
		windowSizeH = HIWORD(lParam);
		windowSizeW = LOWORD(lParam);
		HWND bkg = (HWND)GetWindowLong(hwnd, GWL_USERDATA);
		MoveWindow(bkg, 0, 0, windowSizeW, windowSizeH, TRUE);
		MoveWindow(hwndTilepicker, 5, 43, 256, windowSizeH / 2 - 43, TRUE);
		MoveWindow(hwndTree, 5, 43 + (windowSizeH / 2 - 43) + 43 / 2, 256, windowSizeH / 2 - 43, TRUE);
		//MoveWindow(hwndObjects, 0, 43, 256, windowSizeH - 43, TRUE);
		MoveWindow(hwndObjects, windowSizeW - 266, 66, windowSizeW, windowSizeH - 43 - 23 - 100, TRUE);
		int viewportX = 266;
		//MoveWindow(hwndViewport, viewportX, 43, (map_width * 16 * zoomLevel) - viewportX, (map_height * 16 * zoomLevel) - 43, TRUE);
		SendMessage(hwndViewport, WM_SIZE, wParam, lParam);
		break;
	}
	case WM_LBUTTONUP: {
		SendMessage(viewportTest, WM_LBUTTONUP, wParam, lParam);
		break;
	}
	case WM_RBUTTONUP: {
		SendMessage(viewportTest, WM_RBUTTONUP, wParam, lParam);
		break;
	}
	case WM_ERASEBKGND: {
		break;
	}
	case WM_COMMAND: {
		const TCHAR* executablePath = TEXT("runtime.exe");
		const TCHAR* arguments = TEXT("test.map");

		switch (LOWORD(wParam)) {
			case IDM_FILE_EXIT: {
				PostQuitMessage(0);
				break;
			}
			case IDM_TEST_PLAY: {
				UpdateRuntimeResources();

				RunExecutableWithArguments(executablePath, arguments);
				break;
			}
			case IDM_FILE_SAVE: {

				HANDLE project = OpenFileForWriting(TEXT("test.bhproj"));
				char* buffer = SerializeMapInfoList(MapCollection);
				AppendToFile(project, buffer, MapCollectionLength * sizeof(MapInfo) + sizeof(unsigned int));
				free(buffer);
				StackNode* current = mapStack->bottom;
				for (int i = 0; i < mapStack->size; i++) {
					EditorMap* cur_map = current->data;
					MapInfo* cur_map_info = MapCollection + cur_map->id;
					AppendToFile(project, cur_map->tilemap, cur_map_info->dim.w * cur_map_info->dim.h * sizeof(uint16_t));
					AppendToFile(project, cur_map->collision, cur_map_info->dim.w * cur_map_info->dim.h * sizeof(uint16_t));
					current = current->up;
				}

				CloseFileHandle(project);
				/*
				char modulePath[1000];
				GetCurrentDirectoryA(1000, modulePath);
				printf(modulePath);
				*/
				break;
			}
			case IDM_FILE_OPEN: {

				TCHAR* project_path = FileDialogOpen(hwnd);

				printf("Open");
				HANDLE project = OpenFileForReading(project_path);
				if (project == NULL)
					break;
				unsigned int* pMapRecords = (unsigned int*)ReadChunkFromFile(project, 0, sizeof(unsigned int));
				unsigned int iMapRecords = *pMapRecords;
				free(pMapRecords);
				unsigned int sizMapRecords = iMapRecords * sizeof(MapInfo) + sizeof(unsigned int);
				char* serializedMapRecords = ReadChunkFromFile(project, 0, sizMapRecords);
				StackNode* cur = mapStack->top;
				while (cur != NULL) {
					StackNode* next = cur->down;
					//nstackFree(cur);
					EditorMap* innerMap = cur->data;
					free(innerMap->tilemap);
					free(innerMap->collision);
					free(innerMap);
					cur = next;
				}
				stackFree(mapStack);
				InitializeMapInfoList(serializedMapRecords);
				size_t beginIndex = sizMapRecords;
				for (int i = 0; i < MapCollectionLength; i++) {
					MapInfo* mapRecord = MapCollection + i;
					EditorMap* newMapData = EditorCreateMap(i, mapRecord->dim.w, mapRecord->dim.h);
					size_t mapSize = mapRecord->dim.w * mapRecord->dim.h;
					size_t mapSizeBytes = mapSize * sizeof(uint16_t);
					uint16_t* mapPointer = (uint16_t*)ReadChunkFromFile(project, beginIndex, mapSizeBytes);
					beginIndex += mapSizeBytes;
					for (int j = 0; j < mapSize; j++) {
						newMapData->tilemap[j] = mapPointer[j];
					}
					free(mapPointer);
					mapPointer = (uint16_t*)ReadChunkFromFile(project, beginIndex, mapSizeBytes);
					beginIndex += mapSizeBytes;
					for (int j = 0; j < mapSize; j++) {
						newMapData->collision[j] = mapPointer[j];
					}
					free(mapPointer);
					map_data = newMapData->tilemap;
					collision_data = newMapData->collision;
					map_width = mapRecord->dim.w;
					map_height = mapRecord->dim.h;
					//beginIndex += mapRecord->dim.w * mapRecord->dim.h * 2 * sizeof(uint16_t);
				}
				CanDrawViewport = TRUE;
				InvalidateRect(viewportTest, NULL, FALSE);
				CloseFileHandle(project);
				break;
			}
		}
		if (lParam == TMenuButtons.zoomOut) {
			int izoom = zoomLevel;
			zoomLevel = max(zoomLevel - 1, 1);
			SendMessage(viewportTest, WM_SIZE, wParam, lParam);
			if (zoomLevel != izoom) {
				CanDrawViewport = TRUE;
				InvalidateRect(viewportTest, NULL, FALSE);
			}
		}
		else if (lParam == TMenuButtons.zoomIn) {
			int izoom = zoomLevel;
			zoomLevel = min(zoomLevel + 1, 5);
			SendMessage(viewportTest, WM_SIZE, wParam, lParam);
			if (zoomLevel != izoom) {
				CanDrawViewport = TRUE;
				InvalidateRect(viewportTest, NULL, FALSE);
			}
		}
		else if (lParam == TMenuButtons.drawButton) {
			CanDrawViewport = !CanDrawViewport; //???

			CheckDlgButton(hwnd, TMenuIDs[1], BST_UNCHECKED);
			CheckDlgButton(hwnd, TMenuIDs[2], BST_UNCHECKED);

			g_viewport_mode = VMODE_DRAW;
			InvalidateRect(viewportTest, NULL, FALSE);
		}
		else if (lParam == TMenuButtons.elementButton) {
			CheckDlgButton(hwnd, TMenuIDs[0], BST_UNCHECKED);
			CheckDlgButton(hwnd, TMenuIDs[2], BST_UNCHECKED);

			g_viewport_mode = VMODE_ELEM;
			InvalidateRect(viewportTest, NULL, FALSE);
		}
		else if (lParam == TMenuButtons.collisionButton) {
			CheckDlgButton(hwnd, TMenuIDs[1], BST_UNCHECKED);
			CheckDlgButton(hwnd, TMenuIDs[0], BST_UNCHECKED);

			g_viewport_mode = VMODE_COL;
			InvalidateRect(viewportTest, NULL, FALSE);
		}
		break;
	}
	/*
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	*/
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}