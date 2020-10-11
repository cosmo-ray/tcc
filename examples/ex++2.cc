class Forme {
public:
	virtual const char *c_str() = 0;
	virtual ~Forme(){}
};

class Head : public Forme {
public:
	const char *c_str() {
		return  " ___\n"
			"/   \\\n"
			"|O O|\n"
			"| V |\n"
			"\\ - /\n"
			" --";
	}
};

class Cat : public Forme {
public:
	const char *c_str() {
		return  "=<oo>=     />\n"
			"    \{====} \n"
			"    /     \\";
	}
};

int main()
{
	Forme *h = new Head;
	Cat c;

	printf("%s\n\n\n%s\n", h->c_str(), c.c_str());

	delete h;
}
