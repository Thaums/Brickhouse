#include "button.h"

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

HWND CreateButton(HWND window, int x, int y, int w, int h, HICON icon, DWORD flags, int id) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		NULL,
		flags,
		x, y,
		w, h,
		window,
		(HMENU)id,
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
HWND CreateCheckbox(HWND window, int x, int y, int w, int h, TCHAR* text, int id) {
	HWND result = CreateWindow(
		TEXT("BUTTON"),
		text,
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,
		x, y,
		w, h,
		window,
		(HMENU)id,
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