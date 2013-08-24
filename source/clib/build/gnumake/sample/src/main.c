#include <stdio.h>
#include "math/add.h"
#include "math/sub.h"

#define ADD(x, y) ((x) + (y))
#define EXPRESSION(OP, p1, p2) OP(p1, p2)

int x = EXPRESSION(ADD, 12, 23);

void inner() {
	printf("inner\n");

	printf("%d\n", x);

	return;
}

void function() {
	printf("hi, tangel\n");
	return inner();
}

int main() {
	function();

	printf("%d\n", add(1, 2));
	printf("%d\n", sub(5, 2));
}
