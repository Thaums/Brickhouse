#pragma once
#include <Windows.h>

#ifndef PAL_PREPROCESSOR_H_INCLUDES
#define PAL_PREPROCESSOR_H_INCLUDES
RGBQUAD default_palette[] = {
	//1
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//2
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//3
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//4
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//5
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//6
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//7
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//8
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//9
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//10
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//11
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//12
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//13
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//14
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//15
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//16
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//17
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//18
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
};
RGBQUAD outdoor_pal[] = {
	255, 0, 255, 0,
	32, 53, 80, 0,
	112, 99, 27, 0,
	49, 91, 130, 0,
	91, 108, 193, 0,
	70, 150, 24, 0,
	81, 143, 35, 0,
	80, 171, 27, 0,
	65, 191, 101, 0,
	82, 199, 14, 0,
	115, 153, 232, 0,
	119, 189, 240, 0,
	98, 224, 155, 0,
	160, 128, 65, 0,
	187, 173, 116, 0,
	228, 249, 255, 0,
	//3
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//4
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	30, 29, 27, 0, //glinting pink
	82, 38, 61, 0, //pink
	120, 64, 189, 0, //orange
	120, 64, 189, 0, //yellow
	30, 29, 27, 0, //glinting black
	30, 29, 27, 0, //glinting orange
	//5
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//6
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//7
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//8
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	30, 29, 27, 0, //pink
	114, 38, 249, 0, //orange
	31, 151, 253, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//9
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//10
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//11
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//12
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	114, 38, 249, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	30, 29, 27, 0, //glinting black
	31, 151, 253, 0, //glinting orange
	//13
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//14
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//15
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//16
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	166, 108, 0, 0, //glinting pink
	166, 108, 0, 0, //pink
	89, 183, 212, 0, //orange
	230, 255, 235, 0, //yellow
	30, 29, 27, 0, //glinting black
	89, 183, 212, 0, //glinting orange
	//17
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	//18
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	30, 29, 27, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
};

RGBQUAD conversion_palette[] = {
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
	0, 0, 0, 0, //transparent
	30, 29, 27, 0, //black
	242, 248, 248, 0, //glinting pink
	114, 38, 249, 0, //pink
	31, 151, 253, 0, //orange
	116, 219, 230, 0, //yellow
	239, 217, 102, 0, //glinting black
	255, 129, 174, 0, //glinting orange
};


#endif

RGBQUAD vga_pal[] = {
	0, 0, 0, 0,
	0, 0, 170, 0,
	0, 170, 0, 0,
	0, 170, 170, 0,
	170, 0, 0, 0,
	170, 0, 170, 0,
	170, 85, 0, 0,
	170, 170, 170, 0,
	85, 85, 85, 0,
	85, 85, 255, 0,
	85, 255, 85, 0,
	85, 255, 255, 0,
	255, 85, 85, 0,
	255, 85, 255, 0,
	255, 255, 85, 0,
	255, 255, 255, 0,
	0, 0, 0, 0,
	16, 16, 16, 0,
	32, 32, 32, 0,
	53, 53, 53, 0,
	69, 69, 69, 0,
	85, 85, 85, 0,
	101, 101, 101, 0,
	117, 117, 117, 0,
	138, 138, 138, 0,
	154, 154, 154, 0,
	170, 170, 170, 0,
	186, 186, 186, 0,
	202, 202, 202, 0,
	223, 223, 223, 0,
	239, 239, 239, 0,
	255, 255, 255, 0,
	0, 0, 255, 0,
	65, 0, 255, 0,
	130, 0, 255, 0,
	190, 0, 255, 0,
	255, 0, 255, 0,
	255, 0, 190, 0,
	255, 0, 130, 0,
	255, 0, 65, 0,
	255, 0, 0, 0,
	255, 65, 0, 0,
	255, 130, 0, 0,
	255, 190, 0, 0,
	255, 255, 0, 0,
	190, 255, 0, 0,
	130, 255, 0, 0,
	65, 255, 0, 0,
	0, 255, 0, 0,
	0, 255, 65, 0,
	0, 255, 130, 0,
	0, 255, 190, 0,
	0, 255, 255, 0,
	0, 190, 255, 0,
	0, 130, 255, 0,
	0, 65, 255, 0,
	130, 130, 255, 0,
	158, 130, 255, 0,
	190, 130, 255, 0,
	223, 130, 255, 0,
	255, 130, 255, 0,
	255, 130, 223, 0,
	255, 130, 190, 0,
	255, 130, 158, 0,
	255, 130, 130, 0,
	255, 158, 130, 0,
	255, 190, 130, 0,
	255, 223, 130, 0,
	255, 255, 130, 0,
	223, 255, 130, 0,
	190, 255, 130, 0,
	158, 255, 130, 0,
	130, 255, 130, 0,
	130, 255, 158, 0,
	130, 255, 190, 0,
	130, 255, 223, 0,
	130, 255, 255, 0,
	130, 223, 255, 0,
	130, 190, 255, 0,
	130, 158, 255, 0,
	186, 186, 255, 0,
	202, 186, 255, 0,
	223, 186, 255, 0,
	239, 186, 255, 0,
	255, 186, 255, 0,
	255, 186, 239, 0,
	255, 186, 223, 0,
	255, 186, 202, 0,
	255, 186, 186, 0,
	255, 202, 186, 0,
	255, 223, 186, 0,
	255, 239, 186, 0,
	255, 255, 186, 0,
	239, 255, 186, 0,
	223, 255, 186, 0,
	202, 255, 186, 0,
	186, 255, 186, 0,
	186, 255, 202, 0,
	186, 255, 223, 0,
	186, 255, 239, 0,
	186, 255, 255, 0,
	186, 239, 255, 0,
	186, 223, 255, 0,
	186, 202, 255, 0,
	0, 0, 113, 0,
	28, 0, 113, 0,
	57, 0, 113, 0,
	85, 0, 113, 0,
	113, 0, 113, 0,
	113, 0, 85, 0,
	113, 0, 57, 0,
	113, 0, 28, 0,
	113, 0, 0, 0,
	113, 28, 0, 0,
	113, 57, 0, 0,
	113, 85, 0, 0,
	113, 113, 0, 0,
	85, 113, 0, 0,
	57, 113, 0, 0,
	28, 113, 0, 0,
	0, 113, 0, 0,
	0, 113, 28, 0,
	0, 113, 57, 0,
	0, 113, 85, 0,
	0, 113, 113, 0,
	0, 85, 113, 0,
	0, 57, 113, 0,
	0, 28, 113, 0,
	57, 57, 113, 0,
	69, 57, 113, 0,
	85, 57, 113, 0,
	97, 57, 113, 0,
	113, 57, 113, 0,
	113, 57, 97, 0,
	113, 57, 85, 0,
	113, 57, 69, 0,
	113, 57, 57, 0,
	113, 69, 57, 0,
	113, 85, 57, 0,
	113, 97, 57, 0,
	113, 113, 57, 0,
	97, 113, 57, 0,
	85, 113, 57, 0,
	69, 113, 57, 0,
	57, 113, 57, 0,
	57, 113, 69, 0,
	57, 113, 85, 0,
	57, 113, 97, 0,
	57, 113, 113, 0,
	57, 97, 113, 0,
	57, 85, 113, 0,
	57, 69, 113, 0,
	81, 81, 113, 0,
	89, 81, 113, 0,
	97, 81, 113, 0,
	105, 81, 113, 0,
	113, 81, 113, 0,
	113, 81, 105, 0,
	113, 81, 97, 0,
	113, 81, 89, 0,
	113, 81, 81, 0,
	113, 89, 81, 0,
	113, 97, 81, 0,
	113, 105, 81, 0,
	113, 113, 81, 0,
	105, 113, 81, 0,
	97, 113, 81, 0,
	89, 113, 81, 0,
	81, 113, 81, 0,
	81, 113, 89, 0,
	81, 113, 97, 0,
	81, 113, 105, 0,
	81, 113, 113, 0,
	81, 105, 113, 0,
	81, 97, 113, 0,
	81, 89, 113, 0,
	0, 0, 65, 0,
	16, 0, 65, 0,
	32, 0, 65, 0,
	49, 0, 65, 0,
	65, 0, 65, 0,
	65, 0, 49, 0,
	65, 0, 32, 0,
	65, 0, 16, 0,
	65, 0, 0, 0,
	65, 16, 0, 0,
	65, 32, 0, 0,
	65, 49, 0, 0,
	65, 65, 0, 0,
	49, 65, 0, 0,
	32, 65, 0, 0,
	16, 65, 0, 0,
	0, 65, 0, 0,
	0, 65, 16, 0,
	0, 65, 32, 0,
	0, 65, 49, 0,
	0, 65, 65, 0,
	0, 49, 65, 0,
	0, 32, 65, 0,
	0, 16, 65, 0,
	32, 32, 65, 0,
	40, 32, 65, 0,
	49, 32, 65, 0,
	57, 32, 65, 0,
	65, 32, 65, 0,
	65, 32, 57, 0,
	65, 32, 49, 0,
	65, 32, 40, 0,
	65, 32, 32, 0,
	65, 40, 32, 0,
	65, 49, 32, 0,
	65, 57, 32, 0,
	65, 65, 32, 0,
	57, 65, 32, 0,
	49, 65, 32, 0,
	40, 65, 32, 0,
	32, 65, 32, 0,
	32, 65, 40, 0,
	32, 65, 49, 0,
	32, 65, 57, 0,
	32, 65, 65, 0,
	32, 57, 65, 0,
	32, 49, 65, 0,
	32, 40, 65, 0,
	45, 45, 65, 0,
	49, 45, 65, 0,
	53, 45, 65, 0,
	61, 45, 65, 0,
	65, 45, 65, 0,
	65, 45, 61, 0,
	65, 45, 53, 0,
	65, 45, 49, 0,
	65, 45, 45, 0,
	65, 49, 45, 0,
	65, 53, 45, 0,
	65, 61, 45, 0,
	65, 65, 45, 0,
	61, 65, 45, 0,
	53, 65, 45, 0,
	49, 65, 45, 0,
	45, 65, 45, 0,
	45, 65, 49, 0,
	45, 65, 53, 0,
	45, 65, 61, 0,
	45, 65, 65, 0,
	45, 61, 65, 0,
	45, 53, 65, 0,
	45, 49, 65, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};