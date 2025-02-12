#include "menu.h"

HMENU GetAppMenu() {
	HMENU hMenu = CreateMenu();
	HMENU hMenuPopup = CreateMenu();

	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_NEW, TEXT("&New"));
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_OPEN, TEXT("&Open..."));
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_SAVE, TEXT("&Save"));
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_SAVEAS, TEXT("Save &As..."));
	AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_EXIT, TEXT("E&xit"));

	AppendMenu(hMenu, MF_POPUP, hMenuPopup, TEXT("&File"));

	hMenuPopup = CreateMenu();

	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_UNDO, TEXT("&Undo"));
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_REDO, TEXT("&Redo"));

	AppendMenu(hMenu, MF_POPUP, hMenuPopup, TEXT("&Edit"));

	hMenuPopup = CreateMenu();

	AppendMenu(hMenuPopup, MF_STRING, IDM_PREFS_PREFS, TEXT("&Preferences"));

	AppendMenu(hMenu, MF_POPUP, hMenuPopup, TEXT("&Settings"));

	hMenuPopup = CreateMenu();

	AppendMenu(hMenuPopup, MF_STRING, IDM_TEST_PLAY, TEXT("&Start Game"));

	AppendMenu(hMenu, MF_POPUP, hMenuPopup, TEXT("&Playtest"));

	return hMenu;
}