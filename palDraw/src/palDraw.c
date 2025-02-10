#include "paldraw.h"

HBITMAP BMP8bpp(HDC hdc, int width, int height, void** ppixel) {
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biBitCount = 8;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biClrUsed = 256;
	bmpInfo.bmiHeader.biClrImportant = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	HBITMAP result;
	result = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, ppixel, NULL, 0);
	return result;
}

HBITMAP BMPTextureIndexed(HDC hdc, int width, int height, void** ppixel, unsigned char* texture) {
	HBITMAP result = BMP8bpp(hdc, width, height, ppixel);
	unsigned char* pixels = *ppixel;
	for (int i = 0; i < (width * height); i++) {
		pixels[i] = texture[i];
	}
	return result;
}

HBITMAP BMPOneColorIndexed(HDC hdc, int width, int height, void** ppixel, unsigned char color) {
	HBITMAP result = BMP8bpp(hdc, width, height, ppixel);
	unsigned char* pixels = *ppixel;
	for (int i = 0; i < (width * height); i++) {
		pixels[i] = color;
	}
	return result;
}

HBITMAP BitmapNormal(HDC hdc, int width, int height, void** ppixel, unsigned char* texture, RGBQUAD* table) {
	HBITMAP result = BMPTextureIndexed(hdc, width, height, ppixel, texture);
	HDC deviceContextTexture = CreateCompatibleDC(hdc);
	SelectObject(deviceContextTexture, result);
	SetDIBColorTable(deviceContextTexture, 0, 256, table);
	DeleteDC(deviceContextTexture);
	return result;
}

HBITMAP BitmapFill(HDC hdc, int width, int height, void** ppixel, unsigned char color, RGBQUAD* table) {
	HBITMAP result = BMPOneColorIndexed(hdc, width, height, ppixel, color);
	HDC deviceContextTexture = CreateCompatibleDC(hdc);
	SelectObject(deviceContextTexture, result);
	SetDIBColorTable(deviceContextTexture, 0, 256, table);
	DeleteDC(deviceContextTexture);
	return result;
}

HBITMAP img(TCHAR* str) {
	return (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

HBITMAP convertBMPToEngineFormat(HDC hdc, char* file, RGBQUAD* palette, void** output) {
	HBITMAP bitmapTest = img(file);
	if (bitmapTest == NULL) {
		MessageBox(NULL, TEXT("File not found"), TEXT("Failed to convert bitmap to engine format (Could not find file)"), MB_ICONERROR);
		return;
	}
	BITMAP bmpInfo;
	GetObject(bitmapTest, sizeof(bmpInfo), &bmpInfo);

	HBITMAP handleImport = BitmapFill(hdc, bmpInfo.bmWidth, bmpInfo.bmHeight, output, 0, palette);
	HDC importBuffer = CreateCompatibleDC(hdc);
	HDC importData = CreateCompatibleDC(hdc);
	SelectObject(importBuffer, handleImport);
	SelectObject(importData, bitmapTest);
	BitBlt(importBuffer, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, importData, 0, 0, SRCCOPY);
	SetDIBColorTable(importBuffer, 0, 256, palette);
	/*
	for (int i = 0; i < bmpInfo.bmHeight; i++) {
		for (int j = 0; j < bmpInfo.bmWidth; j++) {
			printf("%d, ", output[j + (i * bmpInfo.bmWidth)]);
		}
		printf("\n");
	}
	*/
	DeleteDC(importBuffer);
	DeleteDC(importData);
	DeleteObject(bitmapTest);
	return handleImport;
}

unsigned char PALDRAW_isLoaded() {
	printf("PalDraw is loaded!");
	return 1;
}