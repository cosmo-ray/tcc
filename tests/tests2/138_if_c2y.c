#include <stdio.h>

int ret_1()
{
    return 1;
}

int main()
{
    int i = 100;

    if (int i = 6) {
	printf("%d\n", i);
    } else {
	printf("bug\n");
    }

    printf("main scope i: %d\n", i);

    if (int i = 0) {
	printf("bug if here\n");
    } else {
	printf("in esle as i is %d\n", i);
    }

    if (void *i = main) {
	printf("main pointer\n");
    }


    if (int i = ret_1()) {
	printf("ret_1 return: %d\n", i);
    }

    if (int i = 10; i > 5) {
	printf("%d\n", i);
	if (i == 10) {
	    printf("i is 10: %d\n", i);
	}
    }

    if (char *char_ptr_ar[] = {}) {
	    printf("an array 0\n");
    }

    if (char char_ptr_ar[] = "a") {
	    printf("an array\n");
    }

    if (char char_ptr_ar[2] = "a"; char_ptr_ar) {
	printf("an array 2\n");
    }

    if (char *char_ptr_ar[] = {"a", "b"}; sizeof char_ptr_ar / sizeof *char_ptr_ar == 2) {
	    printf("array of 2\n");
    }
}
