#pragma once
typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	struct {
		float x;
		float y;
	} pos;
	struct {
		float w;
		float h;
	} dim;
} aabb;