#pragma once
#include <Windows.h>

HICON LoadIconWithSize(HINSTANCE hInstance, int iconID, int width, int height);

HWND CreateButton(HWND window, int x, int y, int w, int h, HICON icon, DWORD flags, int id);

HWND CreateColorButton(HWND window, int x, int y, int w, int h, HICON icon);

HWND CreateCheckbox(HWND window, int x, int y, int w, int h, TCHAR* text, int id);

HWND CreateBackground(HWND window);