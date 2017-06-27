#!/usr/local/bin/tcc -run
#include <tcclib.h>

const int a = 0;

#define gen_sw(a) _Generic(a, const char *: 1, default: 8, const int: 123);

int main()
{
	int i = 0;
	long long *b[] = {NULL};

	i = _Generic(a, const char *: 1, const int: 8);
	printf("Hello World %d\n", i);
	i = gen_sw(a);
	printf("Hello World %d\n", i);
	i = _Generic(a, char:1, const int:2, default:20);
	printf("Hello World %d\n", i);
	i = _Generic(a, char:1, int[4]:2, default:5);
	printf("Hello World %d\n", i);
	i = _Generic(17, int :1, int **:2);
	printf("Hello World %d\n", i);
	i = _Generic("17, io", const char *:1, const int :2);
	printf("Hello World %d\n", i);
	return 0;
}
