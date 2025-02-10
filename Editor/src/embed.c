#include "embed.h"

int embed(TCHAR* exePath, TCHAR* resourceFilePath, int resourceID) {
    HANDLE hUpdateRes;
    //const char* exePath = "target.exe";
    //const char* resourceFilePath = "data.bin";
    //const int resourceID = 101;

    // Open the target executable for updating resources
    hUpdateRes = BeginUpdateResourceW(exePath, FALSE);
    if (hUpdateRes == NULL) {
        printf("Could not open file for update.\n");
        return 1;
    }

    // Open the resource file to embed
    FILE* fp;
    _wfopen_s(&fp, resourceFilePath, TEXT("rb"));
    if (!fp) {
        printf("Could not open resource file.\n");
        EndUpdateResource(hUpdateRes, TRUE);
        return 1;
    }

    // Read the resource data
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    BYTE* buffer = (BYTE*)malloc(size);
    if (buffer == 0) {
        printf("Could not allocate buffer.\n");
        return 1;
    }
    fread(buffer, size, 1, fp);
    fclose(fp);

    // Update the executable with the new resource
    if (!UpdateResource(hUpdateRes, RT_RCDATA, MAKEINTRESOURCE(resourceID), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, size)) {
        printf("Could not update resource.\n");
        free(buffer);
        EndUpdateResource(hUpdateRes, TRUE);
        return 1;
    }

    // Finalize the update
    if (!EndUpdateResource(hUpdateRes, FALSE)) {
        printf("Could not write changes.\n");
        free(buffer);
        return 1;
    }

    free(buffer);
    printf("Resource embedded successfully.\n");
    return 0;
}
int embed_nofile(TCHAR* exePath, void* pData, unsigned int data_size, int resourceID) {
    HANDLE hUpdateRes;
    //const char* exePath = "target.exe";
    //const char* resourceFilePath = "data.bin";
    //const int resourceID = 101;

    // Open the target executable for updating resources
    hUpdateRes = BeginUpdateResourceW(exePath, FALSE);
    if (hUpdateRes == NULL) {
        printf("Could not open file for update.\n");
        return 1;
    }
    // Update the executable with the new resource
    if (!UpdateResource(hUpdateRes, RT_RCDATA, MAKEINTRESOURCE(resourceID), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pData, data_size)) {
        printf("Could not update resource.\n");
        EndUpdateResource(hUpdateRes, TRUE);
        return 1;
    }

    // Finalize the update
    if (!EndUpdateResource(hUpdateRes, FALSE)) {
        printf("Could not write changes.\n");
        return 1;
    }
    printf("Resource embedded successfully.\n");
    return 0;
}
