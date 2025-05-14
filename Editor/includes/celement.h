#pragma once
#include <element.h>
#include <Windows.h>


typedef struct ElementEditor {
	aabb collider;
	RGBQUAD color;
	int bgStyle;
	TCHAR* label;
	int id;
	HWND checkbox;
} ElementEditor;

extern const int hatchStyles[];