#include "viewport.h"

void UpdateMapInfoCollection(unsigned int id, unsigned int w, unsigned int h) {
	MapCollectionLength++;
	MapInfo* updated = realloc(MapCollection, sizeof(MapInfo) * MapCollectionLength);
	if (updated == NULL) {
		return;
	}
	if (MapCollection != NULL)
		free(MapCollection);
	MapCollection = updated;
	for (int i = 0; i < MapCollectionLength - 1; i++) {
		updated++;
	}
	updated->info.id = id;
	updated->dim.w = w;
	updated->dim.h = h;
	return;
}

void CreateMap() {
	map_width = 50;
	map_height = 50;
	map_data = malloc(sizeof(int) * map_width * map_height);
	if (map_data == NULL)
		return;
	for (int i = 0; i < map_width * map_height; i++) {
		map_data[i] = 0;
	}
	UpdateMapInfoCollection(MapCollectionLength, map_width, map_height);
	return;
}

HBITMAP CreateAlphaBitmap(HDC hdc, int width, int height)
{
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // Negative for a top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // 32-bit for alpha
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pvBits = NULL;
	HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);

	if (hBitmap && pvBits)
	{
		// Initialize bitmap to fully transparent
		memset(pvBits, 255, width * height * 4);
	}

	return hBitmap;
}

LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	static HBITMAP hTileset;
	static HBITMAP hTilesetBackbuffer;
	static HBITMAP hElementBackbuffer;
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

	static HBRUSH hElementBrush;

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

		CreateMap();

		//hTilesetBackbuffer = CreateCompatibleBitmap(hdc, map_width * 16, map_height * 16);
		hTilesetBackbuffer = BitmapFill(hdc, map_width * 16, map_height * 16, &pTileset, 0, paletteInMemory);
		hElementBackbuffer = CreateAlphaBitmap(hdc, map_width * 16, map_height * 16);

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

		ElementEditor* etest1 = malloc(sizeof(ElementEditor));
		etest1->collider.pos.x = 0.0f;
		etest1->collider.pos.y = 0.0f;
		etest1->collider.dim.w = 16.0f;
		etest1->collider.dim.h = 16.0f;

		ElementEditor* etest2 = malloc(sizeof(ElementEditor));
		etest2->collider.pos.x = 50.0f;
		etest2->collider.pos.y = 50.0f;
		etest2->collider.dim.w = 100.0f;
		etest2->collider.dim.h = 100.0f;

		stackPush(elementStack, nstackCreate(etest1));
		stackPush(elementStack, nstackCreate(etest2));

		break;
	}
	case WM_ERASEBKGND: {
		break;
	}
	case WM_SIZE: {
		realClientX = (map_width * 16 * zoomLevel);
		realClientY = (map_height * 16 * zoomLevel);
		MoveWindow(hwnd, 266, 66, min(realClientX, windowSizeW - 266), min(realClientY, windowSizeH - 43 - 23), TRUE);
		clientX = LOWORD(lParam);
		clientY = HIWORD(lParam);
		showScrollV = realClientY > (windowSizeH - 43 - 23);
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
		if (!(wParam & MK_LBUTTON))
			lMouseDown = false;

		int offsetScreenY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43 - 23)));
		int offsetScreenX = ((float)iHScrollPos / 1000.0f) * ((realClientX - (windowSizeW - 266)));
		int selIndY = (mouseY + (offsetScreenY)) / (16 * zoomLevel);
		int selIndX = (mouseX + (offsetScreenX)) / (16 * zoomLevel);

		int ix = selIndX * (16 * zoomLevel);
		int iy = selIndY * (16 * zoomLevel);
		if (g_viewport_mode == VMODE_DRAW) {
			if (mapIndexX != ix || mapIndexY != iy) {
				mapIndexX = ix;
				mapIndexY = iy;
				CanDrawViewport = true;
				InvalidateRect(hwnd, &focusrect, FALSE);
				focusrect.left = ix - (offsetX * zoomLevel);
				focusrect.right = focusrect.left + (16 * zoomLevel);
				//int yOffsetCorrected = offsetScreenY / (16 * zoomLevel) * (16 * zoomLevel);
				focusrect.top = iy - (offsetY * zoomLevel);
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
		}
		else if (g_viewport_mode == VMODE_ELEM) {
			if (lMouseDown) {
				ElementEditor* elemNew = malloc(sizeof(ElementEditor));
				int ex = mouseX * zoomLevel - (offsetX * (zoomLevel));
				int ey = mouseY * zoomLevel - (offsetY * (zoomLevel));
				elemNew->collider.pos.x = mouseX / zoomLevel + (offsetX / (zoomLevel));
				elemNew->collider.pos.y = mouseY / zoomLevel + (offsetY / zoomLevel);
				elemNew->collider.dim.w = 16.0f;
				elemNew->collider.dim.h = 16.0f;
				stackPush(elementStack, nstackCreate(elemNew));
				InvalidateRect(hwnd, NULL, FALSE);
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
		HDC hdcElementBackbuffer = CreateCompatibleDC(hdcPaint);
		SelectObject(hdcTileset, hTileset);
		SelectObject(hdcBackbuffer, hTilesetBackbuffer);
		SelectObject(hdcElementBackbuffer, hElementBackbuffer);
		SetDIBColorTable(hdcTileset, 0, 256, paletteInMemory);
		offsetY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43 - 23)) / zoomLevel);
		offsetX = ((float)iHScrollPos / 1000.0f) * ((realClientX - (windowSizeW - 266)) / zoomLevel);
		if (CanDrawViewport) {
			backbufferRender(ps.rcPaint, hdcBackbuffer, hdcTileset, offsetY);
		}
		CanDrawViewport = false; //false
		int paintWidth = ps.rcPaint.right - ps.rcPaint.left;
		int paintHeight = ps.rcPaint.bottom - ps.rcPaint.top;

		RECT testEntRect = { 0 };
		testEntRect.left = 1;
		testEntRect.right = 3;
		testEntRect.top = 1;
		testEntRect.bottom = 3;

		testEntRect.left = testEntRect.left * 16 * zoomLevel - (offsetX * (zoomLevel));
		testEntRect.right = testEntRect.right * 16 * zoomLevel - (offsetX * (zoomLevel));
		testEntRect.top = testEntRect.top * 16 * zoomLevel - (offsetY * (zoomLevel));
		testEntRect.bottom = testEntRect.bottom * 16 * zoomLevel - (offsetY * (zoomLevel));


		SetDIBColorTable(hdcBackbuffer, 0, 256, paletteInMemory);
		//BitBlt(hdcPaint, 0, 0, map_width * 16, map_height * 16, hdcBackbuffer, 0, 0, SRCCOPY);
		StretchBlt(hdcPaint, ps.rcPaint.left, ps.rcPaint.top, paintWidth - (paintWidth % zoomLevel), paintHeight - (paintHeight % zoomLevel), hdcBackbuffer, ps.rcPaint.left / zoomLevel + offsetX, ps.rcPaint.top / zoomLevel + offsetY, paintWidth / zoomLevel, paintHeight / zoomLevel, SRCCOPY);
		
		POINT slope[4];
		slope[0].x = testEntRect.left;
		slope[0].y = testEntRect.bottom;
		slope[1].x = testEntRect.right;
		slope[1].y = testEntRect.bottom;
		slope[2].x = testEntRect.right;
		slope[2].y = testEntRect.top;
		slope[3].x = testEntRect.left;
		slope[3].y = testEntRect.bottom;


		SetBkMode(hdcPaint, TRANSPARENT);
		
		if (g_viewport_mode == VMODE_ELEM) {
			StackNode* cur = elementStack->bottom;
			float testfloat = 5.0f;
			while (cur != NULL) {
				ElementEditor* elem = (ElementEditor*)cur->data;
				RECT elemRect = { 0 };

				elemRect.left = elem->collider.pos.x * zoomLevel - (offsetX * (zoomLevel));
				elemRect.right = (elem->collider.pos.x + elem->collider.dim.w) * zoomLevel - (offsetX * (zoomLevel));
				elemRect.top = elem->collider.pos.y * zoomLevel - (offsetY * (zoomLevel));
				elemRect.bottom = (elem->collider.pos.y + elem->collider.dim.h) * zoomLevel - (offsetY * (zoomLevel));

				hElementBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 0, 0));
				HPEN elementBorder = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcPaint, hElementBrush);
				HPEN hOldPen = (HPEN)SelectObject(hdcPaint, elementBorder);
				Rectangle(hdcPaint, elemRect.left, elemRect.top, elemRect.right, elemRect.bottom);
				//Polygon(hdcPaint, slope, 4);
				DeleteObject(hElementBrush);
				DeleteObject(elementBorder);
				//restore brush & pen
				SelectObject(hdcPaint, hOldBrush);
				SelectObject(hdcPaint, hOldPen);
				cur = cur->up;
			}
		}

		SetBkMode(hdcPaint, OPAQUE);

		DrawFocusRect(hdcPaint, &focusrect);

		DeleteDC(hdcTileset);
		DeleteDC(hdcBackbuffer);
		DeleteDC(hdcElementBackbuffer);
		EndPaint(hwnd, &ps);
		break;
	}

	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}