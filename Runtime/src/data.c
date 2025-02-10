#include "data.h"

int loadGameResource(int resourceID, void** pData, unsigned int* pSize) {
    HRSRC hRes;
    HGLOBAL hResLoad;
    LPVOID pResLock;
    DWORD resSize;

    // Locate the resource in the executable
    hRes = FindResource(NULL, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (hRes == NULL) {
        printf("Could not find resource.\n");
        return 1;
    }

    // Load the resource
    hResLoad = LoadResource(NULL, hRes);
    if (hResLoad == NULL) {
        printf("Could not load resource.\n");
        return 1;
    }

    // Lock the resource in memory to get a pointer to it
    pResLock = LockResource(hResLoad);
    if (pResLock == NULL) {
        printf("Could not lock resource.\n");
        return 1;
    }

    // Get the size of the resource
    resSize = SizeofResource(NULL, hRes);
    
    *pData = pResLock;
    *pSize = resSize;

    /*
    // You now have access to the resource data in `pResLock`
    // For example, you could write it to a file:
    FILE* fp = fopen("extracted_data.bin", "wb");
    fwrite(pResLock, resSize, 1, fp);
    fclose(fp);

    printf("Resource extracted successfully.\n");
    */
    return 0;
}