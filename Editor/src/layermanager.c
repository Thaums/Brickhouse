#include "viewport.h"
#include "menu.h"

LRESULT CALLBACK LayerManagerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		for (int i = 0; i < map_layers->size; i++) {
			CreateCheckbox(hwnd, 0, i * 25, 100, 25, TEXT("Layer"), NULL);
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
	static HWND hLastRightClicked = NULL;
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
	case WM_CONTEXTMENU: {
		HWND hCtrl = (HWND)wParam;
		printf("%d %d\n", hCtrl, GetMenu(hCtrl));
		if (hCtrl != hwnd) {
			hLastRightClicked = hCtrl;
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			HMENU hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, IDM_OVERVIEW_RENAME, TEXT("Rename"));
			AppendMenu(hMenu, MF_STRING, IDM_OVERVIEW_MOVEUP, TEXT("Move Up"));
			AppendMenu(hMenu, MF_STRING, IDM_OVERVIEW_MOVEDN, TEXT("Move Down"));
			AppendMenu(hMenu, MF_STRING, IDM_OVERVIEW_DELETE, TEXT("Delete"));
			AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hMenu, MF_STRING, IDM_OVERVIEW_CANCEL, TEXT("Cancel"));

			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
			DestroyMenu(hMenu);
		}
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDM_OVERVIEW_RENAME:
			MessageBox(hwnd, TEXT("Option 1 Selected"), TEXT("Info"), MB_OK);
			printf("C: %d\n", GetMenu(hLastRightClicked));
			SetWindowLongPtr(hLastRightClicked, GWL_ID, ((int)GetMenu(hLastRightClicked) - 1));
			break;
		case IDM_OVERVIEW_DELETE: {
			if (hLastRightClicked != NULL) {
				StackNode* cur = elementStack->bottom;
				ElementEditor* elem = NULL;
				int index = 0;
				while (cur != NULL) {
					elem = cur->data;
					if (elem != NULL)
						if (elem->checkbox == hLastRightClicked) {
							cur = cur->up;
							stackPop(elementStack, index);
							free(elem);
							continue;
							//removeElement(index, elementStack->size);
						}
						else {
							MoveWindow(elem->checkbox, 0, index * 25, 100, 25, TRUE);
						}
						
					index++;
					cur = cur->up;
				}
				DestroyWindow(hLastRightClicked);
				InvalidateRect(viewportTest, NULL, FALSE);
			}
			break;
		}
		case IDM_OVERVIEW_MOVEUP: {
			if (hLastRightClicked != NULL) {
				StackNode* cur = elementStack->bottom;
				ElementEditor* elem = NULL;
				int index = 0;
				while (cur != NULL) {
					elem = cur->data;
					if (elem != NULL)
						if (elem->checkbox == hLastRightClicked) {

							StackNode* outerUp = NULL;
							StackNode* outerDown = NULL;
							StackNode* down = NULL;
							
							outerUp = cur->up;
							if (cur->down)
								outerDown = cur->down->down;
							down = cur->down;

							
							cur->up = down;
							if (down)
								cur->down = down->down;

							if (down) {
								down->up = outerUp;
								down->down = cur;
							}

							if (outerUp)
								outerUp->down = down;
							if (outerDown)
								outerDown->up = cur;

							if (!outerDown)
								elementStack->top = cur;
							if (!outerUp)
								elementStack->bottom = cur->up;
							
							MoveWindow(elem->checkbox, 0, (index - 1) * 25, 100, 25, TRUE);
							if (cur->up != NULL) {
								ElementEditor* other = cur->up->data;
								MoveWindow(other->checkbox, 0, (index) * 25, 100, 25, TRUE);
							}
							break;
							//removeElement(index, elementStack->size);
						}

					index++;
					cur = cur->up;
				}
				InvalidateRect(viewportTest, NULL, FALSE);
			}
			break;
		}
		}
		break;
	}
					 //case WM_NCACTIVATE:
						 // Prevent the non-client area from deactivating
						 //return DefWindowProc(hwnd, message, TRUE, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}