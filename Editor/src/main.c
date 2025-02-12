#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <tchar.h>
#include <stdint.h>
#include <math.h>

#include "layer.h"
#include "embed.h"

#include <pal.h>
#include <paldraw.h>
#include <rle.h>
#include <io.h>

#include "resource.h"

#include "menu.h"

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

int randInt(int min, int max) {
	return (rand() % (max - min + 1)) + min;
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

unsigned int windowSizeW, windowSizeH;

uint16_t* map_data = NULL;
unsigned int map_width = 0;
unsigned int map_height = 0;

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

HWND CreateButton(HWND window, int x, int y, int w, int h, HICON icon) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		NULL,
		WS_CHILD | WS_VISIBLE | BS_ICON,
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
	HWND newButton;
	HWND saveButton;
	HWND copyButton;
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

MapLayerStack* map_layers;


void game() {
	return;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TilePickerWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ViewportWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK LayerManagerWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PaletteManagerWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szTilesetName[] = TEXT("TilesetPicker");
TCHAR szViewportName[] = TEXT("Viewport");
TCHAR szLayerManagerName[] = TEXT("Layer Manager");
TCHAR szPaletteManagerName[] = TEXT("Palette Manager");

RGBQUAD paletteInMemory[256];

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

unsigned int selectedTileIndex = 0;
unsigned int zoomLevel = 3;
HWND viewportTest = NULL;
HWND tilepickerHWND = NULL;
bool CanDrawViewport = true;

unsigned char* TilesetDataToSend;
int TilesetDataWidth = 0;
int TilesetDataHeight = 0;


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
	PAINTSTRUCT ps;
	switch (message) {
	case WM_CREATE: {
		CreateBackground(hwnd);

		HWND* PTMenuButtons = &TMenuButtons;

		for (int i = 0; i < sizeof(struct TopMenuButtons) / sizeof(HWND); i++) {
			*(PTMenuButtons + i) = CreateButton(hwnd, 23 * i + 10, 10, 23, 23, LoadIconWithSize((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TMenuIcons[i], 16, 16));
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
		

		

		break;
	}
	case WM_SIZE: {
		windowSizeH = HIWORD(lParam);
		windowSizeW = LOWORD(lParam);
		HWND bkg = (HWND)GetWindowLong(hwnd, GWL_USERDATA);
		MoveWindow(bkg, 0, 0, windowSizeW, windowSizeH, TRUE);
		MoveWindow(hwndTilepicker, 0, 43, 256, windowSizeH - 43, TRUE);
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
			embed_nofile(executablePath, writeBufferCompressedRLE, sizeof(size_t) + sizeof(uint16_t) * 2 * RLELength, 101);
			free(writeBufferCompressedRLE);
			writeBufferCompressedRLE = NULL;

			unsigned char* palette_data = paletteToResource(paletteInMemory);
			embed_nofile(executablePath, palette_data, 256 * 3, 102);
			free(palette_data);

			unsigned char* TilesetDataPacked = packTexture(TilesetDataWidth, TilesetDataHeight, TilesetDataToSend);
			embed_nofile(executablePath, TilesetDataPacked, 256 * 256, 103);
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
		else if (lParam == TMenuButtons.newButton) {
			CanDrawViewport = !CanDrawViewport;
			break;
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


LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	static HBITMAP hTileset;
	static HBITMAP hTilesetBackbuffer;
	static int mouseX, mouseY;
	static int mapIndexX, mapIndexY;
	static int clientX, clientY;
	static int realClientX, realClientY;
	static RECT focusrect;
	static bool lMouseDown = false;
	static bool rMouseDown = false;
	static bool showScrollV = false;
	static bool showScrollH = false;
	static int iVScrollPos = 0;
	static int iHScrollPos = 0;
	static int offsetY;
	static int offsetX;

	static void* imgay;

	static int brushSize = 1;

	static void* pTileset;
	switch (message) {
	case WM_CREATE: {
		viewportTest = hwnd;
		//CreateButton(hwnd, 0, 0, 100, 100, NULL);
		HDC hdc = GetWindowDC(hwnd);
		//hTileset = LoadImage(NULL, TEXT("res/outdoor.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		
		hTileset = convertBMPToEngineFormat(hdc, TEXT("res/space.bmp"), paletteInMemory, (void**)&TilesetDataToSend);
		BITMAP TilesetBMPI;
		GetObject(hTileset, sizeof(TilesetBMPI), &TilesetBMPI);
		TilesetDataWidth = TilesetBMPI.bmWidth;
		TilesetDataHeight = TilesetBMPI.bmHeight;

		//SendMessage(tilepickerHWND, WM_CREATE, wParam, lParam);

		map_width = 50;
		map_height = 50;
		//hTilesetBackbuffer = CreateCompatibleBitmap(hdc, map_width * 16, map_height * 16);
		hTilesetBackbuffer = BitmapFill(hdc, map_width * 16, map_height * 16, &pTileset, 0, paletteInMemory);
		map_data = malloc(sizeof(int) * map_width * map_height);
		for (int i = 0; i < map_width * map_height; i++) {
			map_data[i] = 0;
		}
		RECT blitRect = { 0 };
		blitRect.right = map_width * 16 * zoomLevel;
		blitRect.bottom = map_height * 16 * zoomLevel;
		HDC hdcBackbuffer = CreateCompatibleDC(hdc);
		HDC hdcTileset = CreateCompatibleDC(hdc);
		SelectObject(hdcTileset, hTileset);
		SelectObject(hdcBackbuffer, hTilesetBackbuffer);
		backbufferRender(blitRect, hdcBackbuffer, hdcTileset, offsetY);
		DeleteDC(hdcBackbuffer);
		DeleteDC(hdcTileset);
		//ShowScrollBar(hwnd, SB_HORZ, TRUE);
		DeleteDC(hdc);

		SetScrollRange(hwnd, SB_HORZ, 0, 1000, FALSE);
		SetScrollRange(hwnd, SB_VERT, 0, 1000, FALSE);

		break;
	}
	case WM_ERASEBKGND: {
		break;
	}
	case WM_SIZE: {
		realClientX = (map_width * 16 * zoomLevel);
		realClientY = (map_height * 16 * zoomLevel);
		MoveWindow(hwnd, 266, 43, min(realClientX, windowSizeW - 266), min(realClientY, windowSizeH - 43), TRUE);
		clientX = LOWORD(lParam);
		clientY = HIWORD(lParam);
		showScrollV = realClientY > (windowSizeH - 43);
		showScrollH = realClientX > (windowSizeW - 266);
		iVScrollPos *= showScrollV;
		iHScrollPos *= showScrollH;
		SendMessage(hwnd, WM_VSCROLL, wParam, lParam);
		SendMessage(hwnd, WM_HSCROLL, wParam, lParam);
		ShowScrollBar(hwnd, SB_VERT, showScrollV);
		ShowScrollBar(hwnd, SB_HORZ, showScrollH);
		break;
	}
	case WM_VSCROLL: {
		switch LOWORD(wParam) {
		case SB_THUMBTRACK:
			iVScrollPos = HIWORD(wParam);
		}
		if (iVScrollPos != GetScrollPos(hwnd, SB_VERT)) {
			SetScrollPos(hwnd, SB_VERT, iVScrollPos, FALSE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_HSCROLL: {
		switch LOWORD(wParam) {
		case SB_THUMBTRACK:
			iHScrollPos = HIWORD(wParam);
		}
		if (iHScrollPos != GetScrollPos(hwnd, SB_HORZ)) {
			SetScrollPos(hwnd, SB_HORZ, iHScrollPos, FALSE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_MOUSEMOVE: {
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);

		int offsetScreenY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43)));
		int selIndY = (mouseY + offsetScreenY) / (16 * zoomLevel);
		int selIndX = (mouseX) / (16 * zoomLevel);

		int ix = mouseX / (16 * zoomLevel) * (16 * zoomLevel);
		int iy = mouseY / (16 * zoomLevel) * (16 * zoomLevel);
		if (mapIndexX != ix || mapIndexY != iy) {
			mapIndexX = ix;
			mapIndexY = selIndY * 16 * zoomLevel;
			CanDrawViewport = true;
			InvalidateRect(hwnd, &focusrect, FALSE);
			focusrect.left = ix;
			focusrect.right = ix + (16 * zoomLevel);
			//int yOffsetCorrected = offsetScreenY / (16 * zoomLevel) * (16 * zoomLevel);
			focusrect.top = selIndY * (16 * zoomLevel) - (offsetY * zoomLevel);
			focusrect.bottom = focusrect.top + (16 * zoomLevel);
			CanDrawViewport = true;
			InvalidateRect(hwnd, &focusrect, FALSE);
		}
		if (lMouseDown) {
			int mx = mapIndexX / (16 * zoomLevel);
			int my = selIndY;
			if (map_data != NULL && mx >= 0 && mx < map_width && my >= 0 && my < map_height) {
				if (map_data[mx + my * map_width] != selectedTileIndex) {
					map_data[mx + my * map_width] = selectedTileIndex;
					CanDrawViewport = true;
					//focusrect.top = selIndY * (16 * zoomLevel);
					//focusrect.bottom = focusrect.top + (16 * zoomLevel);
					//InvalidateRect(hwnd, &focusrect, FALSE); //focusrect
					HDC hdcSubWindow = GetDC(hwnd);
					HDC hdcBackbuffer = CreateCompatibleDC(hdcSubWindow);
					HDC hdcTileset = CreateCompatibleDC(hdcSubWindow);
					SelectObject(hdcTileset, hTileset);
					SelectObject(hdcBackbuffer, hTilesetBackbuffer);

					backbufferPencil(selIndX, selIndY, hdcBackbuffer, hdcTileset);
					InvalidateRect(hwnd, &focusrect, FALSE);

					ReleaseDC(hwnd, hdcSubWindow);
					DeleteDC(hdcBackbuffer);
					DeleteDC(hdcTileset);
				}
			}
		}
		if (rMouseDown) {
			int mx = mapIndexX / (16 * zoomLevel);
			int my = selIndY;
			if (map_data != NULL && mx >= 0 && mx < map_width && my >= 0 && my < map_height) {
				if (map_data[mx + my * map_width] != selectedTileIndex) {
					bucketFill(mx + my * map_width, map_data[mx + my * map_width], selectedTileIndex, hwnd, focusrect);
					RECT blitRect = { 0 };
					blitRect.right = map_width * 16 * zoomLevel;
					blitRect.bottom = map_height * 16 * zoomLevel;
					HDC hdcSubWindow = GetDC(hwnd);
					HDC hdcBackbuffer = CreateCompatibleDC(hdcSubWindow);
					HDC hdcTileset = CreateCompatibleDC(hdcSubWindow);
					SelectObject(hdcTileset, hTileset);
					SelectObject(hdcBackbuffer, hTilesetBackbuffer);
					backbufferRender(blitRect, hdcBackbuffer, hdcTileset, offsetY);
					ReleaseDC(hwnd, hdcSubWindow);
					DeleteDC(hdcBackbuffer);
					DeleteDC(hdcTileset);
					//CanDrawViewport = true;
					CanDrawViewport = true;
					InvalidateRect(hwnd, NULL, FALSE);
				}
			}
		}
		break;
	}
	case WM_LBUTTONDOWN: {
		lMouseDown = true;
		SendMessage(hwnd, WM_MOUSEMOVE, wParam, lParam);
		break;
	}
	case WM_LBUTTONUP: {
		lMouseDown = false;
		break;
	}
	case WM_RBUTTONDOWN: {
		rMouseDown = true;
		SendMessage(hwnd, WM_MOUSEMOVE, wParam, lParam);
		break;
	}
	case WM_RBUTTONUP: {
		rMouseDown = false;
		break;
	}
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		HDC hdcBackbuffer = CreateCompatibleDC(hdcPaint);
		HDC hdcTileset = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcTileset, hTileset);
		SelectObject(hdcBackbuffer, hTilesetBackbuffer);
		SetDIBColorTable(hdcTileset, 0, 256, paletteInMemory);
		offsetY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43)) / zoomLevel);
		offsetX = ((float)iHScrollPos / 1000.0f) * ((realClientX - (windowSizeW - 266)) / zoomLevel);
		if (CanDrawViewport) {
			backbufferRender(ps.rcPaint, hdcBackbuffer, hdcTileset, offsetY);
		}
		CanDrawViewport = false; //false
		int paintWidth = ps.rcPaint.right - ps.rcPaint.left;
		int paintHeight = ps.rcPaint.bottom - ps.rcPaint.top;

		SetDIBColorTable(hdcBackbuffer, 0, 256, paletteInMemory);
		//BitBlt(hdcPaint, 0, 0, map_width * 16, map_height * 16, hdcBackbuffer, 0, 0, SRCCOPY);
		StretchBlt(hdcPaint, ps.rcPaint.left, ps.rcPaint.top, paintWidth - (paintWidth % zoomLevel), paintHeight - (paintHeight % zoomLevel), hdcBackbuffer, ps.rcPaint.left / zoomLevel + offsetX, ps.rcPaint.top / zoomLevel + offsetY, paintWidth / zoomLevel, paintHeight / zoomLevel, SRCCOPY);
		DrawFocusRect(hdcPaint, &focusrect);
		DeleteDC(hdcTileset);
		DeleteDC(hdcBackbuffer);
		EndPaint(hwnd, &ps);
		break;
	}
	
	}
	
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK TilePickerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	static HBITMAP hTileset;
	static HBITMAP hTilesetBackbuffer;
	static int mouseX, mouseY;
	static RECT focusrect;
	static int tilesInRow;
	static int tilesInCol;
	static int tileAmount;
	switch (message) {
	case WM_CREATE: {
		tilepickerHWND = hwnd;
		HDC hdc = GetWindowDC(hwnd);
		hTileset = LoadImage(NULL, TEXT("res/space.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hTilesetBackbuffer = CreateCompatibleBitmap(hdc, TilesetDataWidth, TilesetDataHeight);
		tilesInCol = TilesetDataHeight / 16;
		tilesInRow = TilesetDataWidth / 16;
		tileAmount = tilesInCol * tilesInRow;
		DeleteDC(hdc);
		break;
	}
	case WM_MOUSEMOVE: {
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN: {
		int sx = mouseX / 16;
		int sy = mouseY / 16;
		if (sx >= tilesInRow)
			break;
		if (sy >= tilesInCol)
			break;
		selectedTileIndex = sx + sy * tilesInRow; // should be width
		InvalidateRect(hwnd, &focusrect, FALSE);
		focusrect.left = sx * 16;
		focusrect.right = sx * 16 + 16;
		focusrect.top = sy * 16;
		focusrect.bottom = sy * 16 + 16;
		InvalidateRect(hwnd, &focusrect, FALSE);
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
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		HDC hdcBackbuffer = CreateCompatibleDC(hdcPaint);
		HDC hdcTileset = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcBackbuffer, hTilesetBackbuffer);
		SelectObject(hdcTileset, hTileset);
		BitBlt(hdcBackbuffer, 0, 0, TilesetDataWidth, TilesetDataHeight, hdcTileset, 0, 0, SRCCOPY);
		DrawFocusRect(hdcBackbuffer, &focusrect);

		BitBlt(hdcPaint, 0, 0, TilesetDataWidth, TilesetDataHeight, hdcBackbuffer, 0, 0, SRCCOPY);
		DeleteDC(hdcBackbuffer);
		DeleteDC(hdcTileset);
		EndPaint(hwnd, &ps);
		break;
	}

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK LayerManagerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		for (int i = 0; i < map_layers->size; i++) {
			CreateCheckbox(hwnd, 0, i * 25, 100, 25, TEXT("Layer"));
		}
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
	//case WM_NCACTIVATE:
		// Prevent the non-client area from deactivating
		//return DefWindowProc(hwnd, message, TRUE, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}



void HSVtoRGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b) {
	int i = (int)(h * 6);
	float f = h * 6.0f - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i % 6) {
	case 0: {
		*r = (int)(v * 255.0f);
		*g = (int)(t * 255.0f);
		*b = (int)(p * 255.0f);
		break;
	}
	case 1: {
		*r = (int)(q * 255.0f);
		*g = (int)(v * 255.0f);
		*b = (int)(p * 255.0f);
		break;
	}
	case 2: {
		*r = (int)(p * 255.0f);
		*g = (int)(v * 255.0f);
		*b = (int)(t * 255.0f);
		break;
	}
	case 3: {
		*r = (int)(p * 255.0f);
		*g = (int)(q * 255.0f);
		*b = (int)(v * 255.0f);
		break;
	}
	case 4: {
		*r = (int)(t * 255.0f);
		*g = (int)(p * 255.0f);
		*b = (int)(v * 255.0f);
		break;
	}
	case 5: {
		*r = (int)(v * 255.0f);
		*g = (int)(p * 255.0f);
		*b = (int)(q * 255.0f);
		break;
	}
	}
	return;
}


HWND* paletteButtons;

LRESULT CALLBACK PaletteManagerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HBRUSH hBrushButton;
	
	static CHOOSECOLOR cc;
	static COLORREF custClr[16];
	static DWORD rgbCurrent;

	static HBITMAP colorWheel;
	static void* colorWheelData;

	static PAINTSTRUCT ps;

	switch (message) {
	case WM_CREATE: {
		/*for (int i = 0; i < map_layers->size; i++) {
			CreateCheckbox(hwnd, 0, i * 25, 100, 25, TEXT("Layer"));
		}*/
		paletteButtons = malloc(sizeof(HWND) * 256);
		if (!paletteButtons) {
			SendMessage(hwnd, WM_DESTROY, wParam, lParam);
			return 0;
		}
		for (int i = 0; i < 256; i++) {
			int dim_x = (i % 16) * 17;
			int dim_y = (i / 16) * 13;
			paletteButtons[i] = CreateColorButton(hwnd, dim_x, dim_y, 16, 12, NULL);
		}
		hBrushButton = CreateSolidBrush(RGB(255, 0, 0));

		cc.lStructSize = sizeof(cc);
		cc.hwndOwner = hwnd;
		cc.lpCustColors = (LPDWORD)custClr;
		cc.rgbResult = rgbCurrent;
		cc.Flags = CC_FULLOPEN | CC_RGBINIT;

		HDC hdc = GetWindowDC(hwnd);
		BITMAPINFO bmpInfo;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 0;
		bmpInfo.bmiHeader.biWidth = 256;
		bmpInfo.bmiHeader.biHeight = -16;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		colorWheel = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, &colorWheelData, NULL, 0);

		uint8_t* rgb = (uint8_t*)colorWheelData;
		uint32_t* pix = (uint32_t*)colorWheelData;
		for (int y = 0; y < 16; y++) {
			for (int x = 0; x < 256; x++) {
				float hsvx = (float)x / 255.0f;
				HSVtoRGB(hsvx, 1.0f, 1.0f, rgb, rgb + 1, rgb + 2);
				pix++;
				rgb = pix;
			}
		}

		DeleteDC(hdc);
		break;
	}
	case WM_CTLCOLORBTN: {
		int index = 0;
		for (int i = 0; i < 256; i++) {
			if (lParam == paletteButtons[i]) {
				index = i;
				break;
			}
		}
		RGBQUAD* cpal = paletteInMemory + index;
		DeleteObject(hBrushButton);
		hBrushButton = CreateSolidBrush(RGB(cpal->rgbRed, cpal->rgbGreen, cpal->rgbBlue));
		return (LRESULT)hBrushButton;
	}
	case WM_COMMAND: {
		int index = 0;
		for (int i = 0; i < 256; i++) {
			if (lParam == paletteButtons[i]) {
				index = i;
				break;
			}
		}
		RGBQUAD* cpal = paletteInMemory + index;
		cc.rgbResult = RGB(cpal->rgbRed, cpal->rgbGreen, cpal->rgbBlue);
		if (ChooseColor(&cc) == TRUE) {
			paletteInMemory[index].rgbRed = GetRValue(cc.rgbResult);
			paletteInMemory[index].rgbGreen = GetGValue(cc.rgbResult);
			paletteInMemory[index].rgbBlue = GetBValue(cc.rgbResult);
			InvalidateRect(hwnd, NULL, FALSE);
			InvalidateRect(viewportTest, NULL, FALSE);
		}

		break;
	}
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		HDC hdcColorWheel = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcColorWheel, colorWheel);

		BitBlt(hdcPaint, 0, 256, 256, 16, hdcColorWheel, 0, 0, SRCCOPY);

		DeleteDC(hdcColorWheel);
		EndPaint(hwnd, &ps);
	}
	case WM_LBUTTONUP: {
		SendMessage(viewportTest, WM_LBUTTONUP, wParam, lParam);
		break;
	}
	case WM_RBUTTONUP: {
		SendMessage(viewportTest, WM_RBUTTONUP, wParam, lParam);
		break;
	}
	case WM_DESTROY:
		// Clean up the brush when the window is destroyed
		DeleteObject(hBrushButton);
		//PostQuitMessage(0);
		return 0;
	//case WM_NCACTIVATE:
		// Prevent the non-client area from deactivating
		//return DefWindowProc(hwnd, message, TRUE, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
