
#include "palettemanager.h"
HWND* paletteButtons;

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
