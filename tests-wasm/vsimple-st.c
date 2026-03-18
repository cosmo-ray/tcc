struct s {
	int i;
	int j;
	int k;
};

int main(void)
{
	int i;
	struct s s = {4, 6};
	return s.j;
}
