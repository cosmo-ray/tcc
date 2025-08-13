#include <stdio.h>

struct s {
	int a;
};

int main()
{
	switch (int i = 3) {
	case 3:
		printf("i is 3\n");
		break;
	default:
		printf("wrong\n");
	}

	switch (struct s s = (struct s){.a=10}; s.a) {
	case 10:
		printf("s.a is 10\n");
		break;
	default:
		printf("wrong\n");
	}

}
