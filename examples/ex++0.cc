//#include <stdio.h>
int printf(const char *format, ...);

class Head {
public:
	void init();
	const char *c_str();
	void set(const char *str);
private:
	const char *h_;
};

void Head::set(class Head *h, const char *to_set)
{
	h->h_ = to_set;
}

void Head::init(class Head *h)
{
	h->h_ = "  ___\n"
		" /   \\\n"
		"(|O O|)\n"
		" | ^ |\n"
		" \\ - /\n"
		"  ---";
}

const char *Head::c_str(class Head *h)
{
	printf("c_str ret: %p\n", h);
	return  h->h_;
}

int main()
{
	class Head h;

	h.init();
	// printf("%s\n", h.c_str());
	printf("- %p -\n%s\n", &h, h.c_str());
	h.set("=<oo>=     />\n"
		"    {====} \n"
		"    /     \\");
	printf("%s\n", h.c_str());
}
