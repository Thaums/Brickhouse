#pragma once
#include <windows.h>
#include <stdio.h>

int embed(TCHAR* exePath, TCHAR* resourceFilePath, int resourceID);

int embed_nofile(TCHAR* exePath, void* pData, unsigned int data_size, int resourceID);