#include "stack.h"

Stack* stackInit() {
	Stack* result = malloc(sizeof(Stack));
	result->size = 0;
	result->bottom = NULL;
	result->top = NULL;
	return result;
}

StackNode* nstackCreate(void* data) {
	StackNode* result = malloc(sizeof(StackNode));
	result->data = data;
	result->up = NULL;
	result->down = NULL;
	return result;
}

void nstackFree(StackNode* node) {
	//node->data = NULL;
	free(node);
	node = NULL;
	return;
}

void stackFree(Stack* list) {
	StackNode* cur = list->top;
	while (cur != NULL) {
		StackNode* next = cur->down;
		nstackFree(cur);
		cur = next;
	}
	list->size = 0;
	list->bottom = NULL;
	list->top = NULL;
	return;
}

bool stackPush(Stack* list, StackNode* node) {
	if (list->size == 0) {
		list->bottom = node;
		list->top = node;
	}
	else {
		list->top->up = node;
		node->down = list->top;
		list->top = node;
	}
	list->size++;
	return true;
}

//remember to free whatever data was inside the node before calling this function
bool stackPop(Stack* list, int index) {
	if (list->size == 0)
		return false;
	if (index >= list->size)
		return false;
	if (list->size == 1) {
		nstackFree(list->top);
		list->top = NULL;
		list->bottom = NULL;
	}
	else {
		if (index < 0) {
			printf("TBD\n");
		}
		else {
			StackNode* cur = list->bottom;
			for (int i = 0; i < index; i++) {
				cur = cur->up;
			}
			if (index == 0) {
				list->bottom = cur->up;
				cur->up->down = NULL;
			}
			else if (index == (list->size - 1)) {
				list->top = cur->down;
				cur->down->up = NULL;
			}
			else {
				cur->down->up = cur->up;
				cur->up->down = cur->down;
			}
			nstackFree(cur);
			cur = NULL;
		}
	}
	list->size--;
	return true;
}