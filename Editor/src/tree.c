#include "tree.h"
#include "resource.h"

int g_nOpen;
int g_nClosed;
int g_nDocument;

BOOL InitTreeViewImageLists(HWND tree) {
	HIMAGELIST himl;
	HICON hbmp;
	if ((himl = ImageList_Create(
		16, 16,
		ILC_COLOR32 | ILC_MASK,
		3,
		0
	)) == NULL)
		return FALSE;
	hbmp = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON_APPLICATION), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	g_nOpen = ImageList_AddIcon(himl, hbmp);
	DeleteObject(hbmp);
	hbmp = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	g_nClosed = ImageList_AddIcon(himl, hbmp);
	DeleteObject(hbmp);
	hbmp = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	g_nDocument = ImageList_AddIcon(himl, hbmp);
	DeleteObject(hbmp);
	int fuck = ImageList_GetImageCount(himl);
	if (ImageList_GetImageCount(himl) < 3)
		return FALSE;
	TreeView_SetImageList(tree, himl, TVSIL_NORMAL);
	return TRUE;
}

HTREEITEM AddItemToTree(HWND tree, LPSTR lpszItem, int nLevel) {
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hPrevRootItem = NULL;
	static HTREEITEM hPrevLev2Item = NULL;
	HTREEITEM hti;
	tvi.mask = 
		TVIF_TEXT |
		TVIF_IMAGE |
		TVIF_SELECTEDIMAGE |
		TVIF_PARAM
		;
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(TCHAR) * 3;
	
	tvi.iImage = g_nDocument;
	tvi.iSelectedImage = g_nDocument;

	tvi.lParam = (LPARAM)nLevel;
	tvins.item = tvi;
	tvins.hInsertAfter = hPrev;

	if (nLevel == 1)
		tvins.hParent = TVI_ROOT;
	else if (nLevel == 2)
		tvins.hParent = hPrevRootItem;
	else
		tvins.hParent = hPrevLev2Item;

	hPrev = (HTREEITEM)SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (hPrev == NULL)
		return NULL;

	if (nLevel == 1)
		hPrevRootItem = hPrev;
	else if (nLevel == 2)
		hPrevLev2Item = hPrev;

	if (nLevel > 1) {
		hti = TreeView_GetParent(tree, hPrev);
		tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.hItem = hti;
		tvi.iImage = g_nClosed;
		tvi.iSelectedImage = g_nClosed;
		TreeView_SetItem(tree, &tvi);
	}

	return hPrev;
}

BOOL InitTreeViewItems(HWND tree) {
	HTREEITEM hti;
	for (int i = 0; i < mapStack->size; i++) {
		TCHAR* newLabel = malloc(sizeof(TCHAR) * 64);
		wsprintf(newLabel, TEXT("Map %d"), i);
		hti = AddItemToTree(tree, newLabel, i + 1);
		if (hti == NULL)
			return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK TreeWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	RECT rcClient;
	static HWND hwndTreeView;
	switch (message) {
	case WM_CREATE: {
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&icex);
		GetClientRect(hwnd, &rcClient);
		hwndTreeView = CreateWindowEx(
			0,
			WC_TREEVIEW,
			TEXT("Tree View"),
			WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
			0,
			0,
			rcClient.right,
			rcClient.bottom,
			hwnd,
			(HMENU)ID_TREEVIEW,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL
		);
		if (!InitTreeViewImageLists(hwndTreeView) || 
			!InitTreeViewItems(hwndTreeView)) 
		{
			DestroyWindow(hwndTreeView);
		}

		break;
	}
	case WM_SIZE: {
		GetClientRect(hwnd, &rcClient);
		MoveWindow(hwndTreeView, 0, 0, rcClient.right, rcClient.bottom, TRUE);

		break;
	}
	case WM_MOUSEMOVE: {
		break;
	}
	case WM_LBUTTONDOWN: {
		break;
	}
	case WM_LBUTTONUP: {
		break;
	}
	case WM_RBUTTONUP: {
		break;
	}
	case WM_PAINT: {
		HDC hdcPaint = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}