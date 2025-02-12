#pragma once
#include <Windows.h>
#include <stdint.h>

uint16_t* RunLengthEncode(uint16_t* uncompressed, size_t siz);

uint16_t* RunLengthDecode(uint16_t* compressed);