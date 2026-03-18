int aB(int a, int b)
{
	int *c = &b;
	return a + *c;
}

int main(void)
{
	return 0;
}
