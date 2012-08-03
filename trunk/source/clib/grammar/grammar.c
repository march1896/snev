#include <stdio.h>

typedef struct {
	void *a, *b;
} S;

void test0() {
	int x = 5;
	int a = 3, b = 4;
	int c = x == 5 ? a : b;
	S e = { &a, &b };
	S f = { NULL, NULL };
	S temp = x == 5 ? e : f;
	if (NULL == NULL) x = 7;
}

int main() {
	test0();
	return 0;
}
