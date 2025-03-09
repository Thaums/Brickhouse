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

#include <coolmath.h>

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


HICON LoadIconWithSize(HINSTANCE hInstance, int iconID, int width, int height) {
	return (HICON)LoadImage(
		hInstance,
		MAKEINTRESOURCE(iconID),
		IMAGE_ICON,
		width,
		height,
		LR_DEFAULTCOLOR
	);
}

HWND CreateButton(HWND window, int x, int y, int w, int h, HICON icon, DWORD flags, int id) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		NULL,
		flags,
		x, y,
		w, h,
		window,
		(HMENU)id,
		GetWindowLong(window, GWL_HINSTANCE),
		NULL
	);
	if (icon != NULL)
		SendMessage(result, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
	return result;
}
HWND CreateColorButton(HWND window, int x, int y, int w, int h, HICON icon) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		NULL,
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		x, y,
		w, h,
		window,
		NULL,
		GetWindowLong(window, GWL_HINSTANCE),
		NULL
	);
	if (icon != NULL)
		SendMessage(result, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
	return result;
}
HWND CreateCheckbox(HWND window, int x, int y, int w, int h, TCHAR* text) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		text,
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,
		x, y,
		w, h,
		window,
		NULL,
		GetWindowLong(window, GWL_HINSTANCE),
		NULL
	);
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(result, WM_SETFONT, (WPARAM)hFont, TRUE);
	return result;
}
HWND CreateBackground(HWND window) {
	RECT rect;
	GetClientRect(window, &rect);
	//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	HWND result = CreateWindowEx(0, TEXT("STATIC"),
		NULL,
		WS_CHILD | WS_VISIBLE | SS_SUNKEN,
		0, 0,
		rect.right - rect.left, rect.bottom - rect.top,
		window,
		NULL,
		GetWindowLong(window, GWL_HINSTANCE),
		NULL
	);
	SetWindowLong(window, GWL_USERDATA, (LONG)result);
	return result;
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



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndTilepicker;
	static HWND hwndViewport;
	static HWND hwndObjects;
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
		
		//LaunchInfo Init
		LaunchInfo.player.map = 0;
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
		MoveWindow(hwndTilepicker, 0, 43, 256, windowSizeH - 43, TRUE);
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
			embed_nofile(executablePath, SerializeGameInfo(&LaunchInfo), sizeof(GameInfo), RRID_GAMEINFO);
			embed_nofile(executablePath, SerializeMapInfoList(MapCollection), MapCollectionLength * sizeof(MapInfo) + sizeof(unsigned int), RRID_MAPINFO);

			uint16_t* writeBuffer = malloc(map_width * map_height * sizeof(uint16_t));
			if (!writeBuffer) {
				break;
			}
			for (int i = 0; i < (map_width * map_height); i++) {
				writeBuffer[i] = map_data[i];
			}
			uint16_t* writeBufferCompressedRLE = RunLengthEncode(writeBuffer, map_width * map_height);
			size_t* getRLELength = (size_t*)writeBufferCompressedRLE;
			size_t RLELength = *getRLELength;
			embed_nofile(executablePath, writeBufferCompressedRLE, sizeof(size_t) + sizeof(uint16_t) * 2 * RLELength, LaunchInfo.resources.indexMapResource);
			free(writeBufferCompressedRLE);
			writeBufferCompressedRLE = NULL;

			unsigned char* palette_data = paletteToResource(paletteInMemory);
			embed_nofile(executablePath, palette_data, 256 * 3, 112);
			free(palette_data);

			unsigned char* TilesetDataPacked = packTexture(TilesetDataWidth, TilesetDataHeight, TilesetDataToSend);
			embed_nofile(executablePath, TilesetDataPacked, 256 * 256, 113);
			free(TilesetDataPacked);

			RunExecutableWithArguments(executablePath, arguments);
			break;
		}
		case IDM_FILE_SAVE: {
			uint16_t* writeBuffer = malloc(map_width * map_height * sizeof(uint16_t));
			if (!writeBuffer) {
				break;
			}
			for (int i = 0; i < (map_width * map_height); i++) {
				writeBuffer[i] = map_data[i];
			}
			WriteToFile(TEXT("test.map"), (char*)writeBuffer, map_width * map_height * sizeof(uint16_t));

			uint16_t* writeBufferCompressedRLE = RunLengthEncode(writeBuffer, map_width * map_height);

			size_t* getRLELength = (size_t*)writeBufferCompressedRLE;
			size_t RLELength = *getRLELength;
			//writeBufferCompressedRLE = getRLELength + 1;

			WriteToFile(TEXT("testCompressed.map"), (char*)writeBufferCompressedRLE, sizeof(size_t) + sizeof(uint16_t) * 2 * RLELength);

			uint16_t* writeBufferUncompressedRLE = RunLengthDecode(writeBufferCompressedRLE);
			WriteToFile(TEXT("testUncompressed.map"), writeBufferUncompressedRLE, map_width * map_height * sizeof(uint16_t));

			//embed_nofile(executablePath, writeBuffer, map_width * map_height, 101);
			free(writeBuffer);
			writeBuffer = NULL;
			free(writeBufferCompressedRLE);
			writeBufferCompressedRLE = NULL;
			free(writeBufferUncompressedRLE);
			writeBufferUncompressedRLE = NULL;

			/*int length = CMDLINELEN;
			TCHAR commandLine[CMDLINELEN];
			//_tprintf(arguments);
			_sntprintf_s(commandLine, CMDLINELEN, 100, TEXT("\"%s\""), executablePath);*/

			//embed(executablePath, arguments, 101);
			/*
			char modulePath[1000];
			GetCurrentDirectoryA(1000, modulePath);
			printf(modulePath);
			*/
			break;
		}
		}
		if (lParam == TMenuButtons.zoomOut) {
			int izoom = zoomLevel;
			zoomLevel = max(zoomLevel - 1, 1);
			SendMessage(viewportTest, WM_SIZE, wParam, lParam);
			if (zoomLevel != izoom)
				InvalidateRect(viewportTest, NULL, FALSE);
		}
		else if (lParam == TMenuButtons.zoomIn) {
			int izoom = zoomLevel;
			zoomLevel = min(zoomLevel + 1, 5);
			SendMessage(viewportTest, WM_SIZE, wParam, lParam);
			if (zoomLevel != izoom)
				InvalidateRect(viewportTest, NULL, FALSE);
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