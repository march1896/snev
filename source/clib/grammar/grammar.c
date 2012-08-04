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

void test_const(const int* x) {
	int* y = (int*)x;
	*y = 5;
}

int main() {
	int mm = 8;
	test0();

	test_const(&mm);
	printf("%d", mm);
	return 0;
}
