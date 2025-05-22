#pragma once
#include "global.h"
#include <mapinfo.h>
#include <coolmath.h>
LRESULT CALLBACK ViewportWndProc(HWND, UINT, WPARAM, LPARAM);

void removeElement(int index, int length);
EditorMap* CreateMap(unsigned int w, unsigned int h);
EditorMap* EditorCreateMap(int id, int w, int h);