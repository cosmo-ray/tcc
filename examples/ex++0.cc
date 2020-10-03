//#include <stdio.h>
int printf(const char *format, ...);

class Head {
public:
	const char *c_str();
private:
	char *h_;
};

const char *Head::c_str(class Head *h)
{
	return  "  ___\n"
		" /   \\\n"
		"(|O O|)\n"
		" | ^ |\n"
		" \\ - /\n"
		"  ---";
}

int main()
{
	class Head h;

	// printf("%s\n", h.c_str());
	printf("%s\n", _s_Head_c_str(&h));
}
