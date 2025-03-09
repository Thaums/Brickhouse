#include "viewport.h"

LRESULT CALLBACK LayerManagerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		for (int i = 0; i < map_layers->size; i++) {
			CreateCheckbox(hwnd, 0, i * 25, 100, 25, TEXT("Layer"));
		}
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
					 //case WM_NCACTIVATE:
						 // Prevent the non-client area from deactivating
						 //return DefWindowProc(hwnd, message, TRUE, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK OverviewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		overviewHWND = hwnd;
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
					 //case WM_NCACTIVATE:
						 // Prevent the non-client area from deactivating
						 //return DefWindowProc(hwnd, message, TRUE, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}