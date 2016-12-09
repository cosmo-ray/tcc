#!/usr/local/bin/tcc -run
#include <tcclib.h>

int a = 0;

int main()
{
	int i = 0;
	long long *b[] = {NULL};

	i = _Generic(a, const char *: 1, int: 8);
	printf("Hello World %d\n", i);
	i = _Generic(a, char:1, int:2, default:20);
	printf("Hello World %d\n", i);
	i = _Generic(a, char:1, int[4]:2, default:5);
	printf("Hello World %d\n", i);
	/* i = _Generic(17, char:1, int **:2); */
	/* printf("Hello World %d\n", i); */
	return 0;
}
