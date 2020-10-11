// #include <stdio.h>

int printf(const char *format, ...);

class Forme {
public:
	virtual const char *c_str();
};

const char *Forme::c_str()
{
	return "[XX - BROKEN - XX]";
}

class Head : public Forme {
public:
	const char *c_str();
};

const char *Head::c_str()
{
	return " ___\n"
		"/   \\\n"
		"|O O|\n"
		"| ^ |\n"
		"\\ - /\n"
		" --";
}

class Cat : public Forme {
public:
	const char *c_str();
};

const char *Cat::c_str()
{
	return  "=<oo>=     />\n"
		"    \{====} \n"
		"    /     \\";
}

int main()
{
	class Forme f;
	class Head h;
	class Cat c;

	printf("%s\n%s\n%s\n", f.c_str(), h.c_str(), c.c_str());
}
