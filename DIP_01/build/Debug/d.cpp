#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* args[]) {
	printf("pos %d\n", abs(3 % 5));
	printf("neg %d\n", abs(-3 % 5));
	printf("neg %d\n", abs(-3 % 4));
	printf("neg %d\n", abs(-5 % 4));
}
