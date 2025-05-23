#include "viewport.h"
#include "button.h"



void UpdateMapInfoCollection(unsigned int id, unsigned int w, unsigned int h) {
	MapCollectionLength++;
	MapInfo* updated = realloc(MapCollection, sizeof(MapInfo) * MapCollectionLength);
	if (updated == NULL) {
		return;
	}
	//if (MapCollection != NULL)
	//	free(MapCollection);
	MapCollection = updated;
	//for (int i = 0; i < MapCollectionLength - 1; i++) {
	//	updated++;
	//}
	updated += (MapCollectionLength - 1);
	updated->info.id = id;
	updated->dim.w = w;
	updated->dim.h = h;
	return;
}

EditorMap* EditorCreateMap(int id, int w, int h) {
	EditorMap* newMap = malloc(sizeof(EditorMap));
	if (!newMap)
		return NULL;
	//newMap->info.dim.w = w;
	//newMap->info.dim.h = h;
	newMap->tilemap = malloc(sizeof(uint16_t) * w * h);
	newMap->collision = malloc(sizeof(uint16_t) * w * h);
	if (newMap->tilemap == NULL)
		return NULL;
	if (newMap->collision == NULL)
		return NULL;
	for (int i = 0; i < w * h; i++) {
		newMap->tilemap[i] = 0;
		newMap->collision[i] = 0;
	}
	newMap->id = id;
	stackPush(mapStack, nstackCreate(newMap));
	//newMap->info.info.id = mapStack->size;
	return newMap;
}

EditorMap* CreateMap(unsigned int w, unsigned int h) {
	map_width = w;
	map_height = h;
	EditorMap* a_map = EditorCreateMap(MapCollectionLength, map_width, map_height);
	map_data = a_map->tilemap;
	collision_data = a_map->collision;
	UpdateMapInfoCollection(MapCollectionLength, map_width, map_height);
	return a_map;
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

void addElement(ElementEditor* element, int index) {
	stackPush(elementStack, nstackCreate(element));
	element->checkbox = CreateCheckbox(overviewHWND, 0, index * 25, 100, 25, element->label, index);
}

void removeElement(int index, int length) {
	ElementEditor* elem = stackPop(elementStack, index);
	free(elem);
	for (int i = index + 1; i < length; i++) {
		HWND button = GetDlgItem(overviewHWND, i);
		int newID = (int)GetMenu(button) - 1;
		SetWindowLongPtr(button, GWL_ID, newID);
		MoveWindow(button, 0, newID * 25, 100, 25, TRUE);
	}
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
	static bool lMouseWasPressed = false;
	static int iVScrollPos = 0;
	static int iHScrollPos = 0;
	static int offsetY;
	static int offsetX;

	static ElementEditor* elemNew = NULL;
	static RECT redraw;
	static RECT cursorRect;

	static int brushSize = 1;

	static HBRUSH hElementBrush;

	static int numOfElements;
	static int labelNumber;

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

		CreateMap(50, 10);
		CreateMap(50, 50);

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

		break;
	}
	case WM_ERASEBKGND: {
		break;
	}
	case WM_SIZE: {
		realClientX = (map_width * 16 * zoomLevel);
		realClientY = (map_height * 16 * zoomLevel);
		MoveWindow(hwnd, 266, 66, min(realClientX, windowSizeW - 266 - 266), min(realClientY, windowSizeH - 43 - 23 - 100), TRUE);
		clientX = LOWORD(lParam);
		clientY = HIWORD(lParam);
		showScrollV = realClientY > (windowSizeH - 43 - 23 - 100);
		showScrollH = realClientX > (windowSizeW - 266 - 266);
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

		int offsetScreenY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43 - 23 - 100)));
		int offsetScreenX = ((float)iHScrollPos / 1000.0f) * ((realClientX - (windowSizeW - 266 - 266)));
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
		if (g_viewport_mode == VMODE_COL) {
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
					if (collision_data[mx + my * map_width] != selectedTileIndex) {
						collision_data[mx + my * map_width] = selectedTileIndex;
						CanDrawViewport = true;
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
		}
		else if (g_viewport_mode == VMODE_ELEM) {
			/*
			cursorRect.left--;
			cursorRect.right+=2;
			cursorRect.top--;
			cursorRect.bottom+=2;
			InvalidateRect(hwnd, &cursorRect, FALSE);
			cursorRect.left = mouseX - 20;
			cursorRect.right = mouseX + 20;
			cursorRect.top = mouseY - 20;
			cursorRect.bottom = mouseY + 20;
			InvalidateRect(hwnd, &cursorRect, FALSE);
			*/

			if (lMouseDown && !lMouseWasPressed && (wParam & MK_CONTROL)) {
				elemNew = malloc(sizeof(ElementEditor));
				lMouseWasPressed = true;
				if (elemNew != NULL) {
					elemNew->collider.pos.x = mouseX / zoomLevel + (offsetX);
					elemNew->collider.pos.y = mouseY / zoomLevel + (offsetY);
					elemNew->color.rgbRed = randInt(0, 255);
					elemNew->color.rgbGreen = randInt(0, 255);
					elemNew->color.rgbBlue = randInt(0, 255);
					elemNew->bgStyle = hatchStyles[randInt(0, 5)];
					elemNew->id = numOfElements++;
					TCHAR* newLabel = malloc(sizeof(TCHAR) * 64);
					wsprintf(newLabel, TEXT("Element %d"), labelNumber++);
					elemNew->label = newLabel;
					//stackPush(elementStack, nstackCreate(elemNew));
					addElement(elemNew, elementStack->size);
				}
			}
			if (lMouseDown && elemNew != NULL) {
				elemNew->collider.dim.w = mouseX / zoomLevel + (offsetX) - elemNew->collider.pos.x;
				elemNew->collider.dim.h = mouseY / zoomLevel + (offsetY) - elemNew->collider.pos.y;
				/*
				InvalidateRect(hwnd, &redraw, FALSE);
				redraw.left = elemNew->collider.pos.x * zoomLevel - (offsetX * (zoomLevel));
				redraw.right = (elemNew->collider.pos.x + elemNew->collider.dim.w) * zoomLevel - (offsetX * (zoomLevel));
				redraw.top = elemNew->collider.pos.y * zoomLevel - (offsetY * (zoomLevel));
				redraw.bottom = (elemNew->collider.pos.y + elemNew->collider.dim.h) * zoomLevel - (offsetY * (zoomLevel));
				if (elemNew->collider.dim.w < 0) {
					redraw.right += -1;
					redraw.left += -1;
				}
				if (elemNew->collider.dim.h < 0) {
					redraw.bottom += -1;
					redraw.top += -1;
				}
				InvalidateRect(hwnd, &redraw, FALSE);
				*/
				InvalidateRect(hwnd, NULL, FALSE);
			}
			if (!lMouseDown && elemNew != NULL) {
				lMouseWasPressed = false;
				if (elemNew->collider.dim.w < 0) {
					elemNew->collider.pos.x = elemNew->collider.pos.x + elemNew->collider.dim.w;
					elemNew->collider.dim.w = -elemNew->collider.dim.w;
				}
				if (elemNew->collider.dim.h < 0) {
					elemNew->collider.pos.y = elemNew->collider.pos.y + elemNew->collider.dim.h;
					elemNew->collider.dim.h = -elemNew->collider.dim.h;
				}

				elemNew = NULL;
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
		offsetY = ((float)iVScrollPos / 1000.0f) * ((realClientY - (windowSizeH - 43 - 23 - 100)) / zoomLevel);
		offsetX = ((float)iHScrollPos / 1000.0f) * ((realClientX - (windowSizeW - 266 - 266)) / zoomLevel);
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

		if (g_viewport_mode == VMODE_COL) {
			SetTextColor(hdcPaint, RGB(255, 255, 255));
			for (int y = 0; y < map_height; y++) {
				for (int x = 0; x < map_width; x++) {
					int index = y * map_width + x;
					TCHAR buffer[6];
					wsprintf(buffer, TEXT("%d  "), collision_data[index]);
					if (collision_data[index] != 0)
						SetTextColor(hdcPaint, RGB(255, 255, 255));
					else
						SetTextColor(hdcPaint, RGB(255, 0, 0));
					TextOut(hdcPaint, x * 16 * zoomLevel - (offsetX * (zoomLevel)), y * 16 * zoomLevel - (offsetY * (zoomLevel)), buffer, 3);
				}
			}
			//TextOut(hdcPaint, 0, 0, TEXT("Collision Mode"), 14);
		}
		SetTextColor(hdcPaint, RGB(0, 0, 0));
		
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

				hElementBrush = CreateHatchBrush(elem->bgStyle, RGB(elem->color.rgbRed, elem->color.rgbGreen, elem->color.rgbBlue));
				HPEN elementBorder = CreatePen(PS_SOLID, 2, RGB(elem->color.rgbRed, elem->color.rgbGreen, elem->color.rgbBlue));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcPaint, hElementBrush);
				HPEN hOldPen = (HPEN)SelectObject(hdcPaint, elementBorder);
				Rectangle(hdcPaint, elemRect.left + 1, elemRect.top + 1, elemRect.right, elemRect.bottom);
				//Polygon(hdcPaint, slope, 4);
				DeleteObject(hElementBrush);
				DeleteObject(elementBorder);
				//restore brush & pen
				SelectObject(hdcPaint, hOldBrush);
				SelectObject(hdcPaint, hOldPen);
				cur = cur->up;
			}
		}
		//Rectangle(hdcPaint, (mouseX / zoomLevel + (offsetX))* zoomLevel - (offsetX * zoomLevel) - 1, (mouseY / zoomLevel + (offsetY)) * zoomLevel - (offsetY * (zoomLevel)) - 1, (mouseX / zoomLevel + (offsetX))* zoomLevel - (offsetX * zoomLevel) + 1, (mouseY / zoomLevel + (offsetY))* zoomLevel - (offsetY * (zoomLevel)) + 1);
		DrawFocusRect(hdcPaint, &cursorRect);

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