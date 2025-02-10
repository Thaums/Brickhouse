#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BMP_8BPP_H_INCLUDED
#define BMP_8BPP_H_INCLUDED


__declspec(dllexport) HBITMAP BMP8bpp(HDC hdc, int width, int height, void** ppixel);

__declspec(dllexport) HBITMAP BMPTextureIndexed(HDC hdc, int width, int height, void** ppixel, unsigned char* texture);

__declspec(dllexport) HBITMAP BMPOneColorIndexed(HDC hdc, int width, int height, void** ppixel, unsigned char color);

__declspec(dllexport) HBITMAP BitmapNormal(HDC hdc, int width, int height, void** ppixel, unsigned char* texture, RGBQUAD* table);

__declspec(dllexport) HBITMAP BitmapFill(HDC hdc, int width, int height, void** ppixel, unsigned char color, RGBQUAD* table);

__declspec(dllexport) HBITMAP img(TCHAR* str);

__declspec(dllexport) HBITMAP convertBMPToEngineFormat(HDC hdc, char* file, RGBQUAD* palette, void** output);

__declspec(dllexport) unsigned char PALDRAW_isLoaded();


#endif