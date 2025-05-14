#pragma once
#include <stdio.h>
#include <stdbool.h>

typedef struct StackNode {
	struct StackNode* down;
	struct StackNode* up;
	void* data;
} StackNode;

typedef struct {
	StackNode* top;
	StackNode* bottom;
	unsigned int size;
} Stack;

Stack* stackInit();

StackNode* nstackCreate(void* data);

void* nstackFree(StackNode* node);

void stackFree(Stack* list);

bool stackPush(Stack* list, StackNode* node);

//remember to free whatever data was inside the node before calling this function
void* stackPop(Stack* list, int index);