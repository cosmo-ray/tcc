int printf(const char *format, ...);
/* #include <tcclib.h> */

/*
 * Self can be in 2 very diferent way, on a struct
 * or on a function pointer member of a struct
 *
 * on a function pointer, it's here to indicate that a ptr
 * on the struct contianing the ptr is automatically add as the 1rst param of of function (example, printer and pri_int)
 *
 * on a struct it's here to tell tcc that self qualified field inside self qualified struct can be reach from a parent struct containing a pointer on thequalified struct (example vtable)
 */

struct __attribute__((self)) caller_table {
	__attribute__((self)) void (*far_print)(void);
	__attribute__((self)) void (*far_pi)(int i);
};

struct caller {
	int nb;
	struct caller_table *vtable;
	__attribute__((self)) void (*printer)(void);
	__attribute__((self)) void (*pri_int)(int i);
};

void printer(struct caller *c0)
{
	printf("printer: %d\n", c0->nb);
}

void print2(struct caller *c0)
{
	printf("other: %d\n", -c0->nb);
}

void p_i(struct caller *c1, int i)
{
	printf("%d - %d\n", c1->nb, i);
}


struct caller_table caller_ops = { printer, p_i };
struct caller_table caller_o2 = { print2, p_i };

int main(void)
{
	struct caller cs = {3,
			    &caller_ops,
			    printer, p_i};
	struct caller c2 = {3,
			    &caller_o2,
			    printer, p_i};

	cs.printer();
	c2.far_print();
	cs.nb = 10;
	cs.far_print();
	cs.nb = 9999;
	cs.pri_int(9);
	cs.far_pi(101);
}
