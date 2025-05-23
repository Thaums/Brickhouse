#pragma once
#include <Windows.h>

#define IDM_FILE_NEW		40001
#define IDM_FILE_OPEN		40002
#define IDM_FILE_SAVE		40003
#define IDM_FILE_SAVEAS		40004
#define IDM_FILE_EXIT		40011
#define IDM_EDIT_UNDO		40005
#define IDM_EDIT_REDO		40006
#define IDM_HELP_ABOUT		40007
#define IDM_HELP_DOCS		40008
#define IDM_TEST_PLAY		40009
#define IDM_PREFS_PREFS		40010

#define IDM_OVERVIEW_RENAME	50001
#define IDM_OVERVIEW_MOVEUP	50002
#define IDM_OVERVIEW_MOVEDN	50003
#define IDM_OVERVIEW_DELETE	50004
#define IDM_OVERVIEW_CANCEL	50005

HMENU GetAppMenu();