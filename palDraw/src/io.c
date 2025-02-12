#include "io.h"

void WriteToFile(const TCHAR* filePath, const char* data, unsigned int length) {
	// Create or open the file
	HANDLE hFile = CreateFile(
		filePath,                // File path
		GENERIC_WRITE,           // Desired access
		0,                       // Share mode
		NULL,                    // Security attributes
		CREATE_ALWAYS,           // Creation disposition
		FILE_ATTRIBUTE_NORMAL,   // Flags and attributes
		NULL                     // Template file handle
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Could not create or open file (error %lu)\n", GetLastError());
		return;
	}

	// Write data to the file
	DWORD bytesWritten;
	BOOL result = WriteFile(
		hFile,                  // Handle to the file
		data,                   // Data to write
		length,    // Number of bytes to write
		&bytesWritten,          // Number of bytes written
		NULL                    // Overlapped structure
	);

	if (!result) {
		printf("Failed to write to file (error %lu)\n", GetLastError());
	}
	else {
		printf("Successfully written %lu bytes to file\n", bytesWritten);
	}

	// Close the file handle
	CloseHandle(hFile);
}