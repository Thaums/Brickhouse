#include "tilepicker.h"

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