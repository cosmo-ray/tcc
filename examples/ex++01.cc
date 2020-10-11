/* This seems to work */
//#include <stdio.h>
int printf(const char *format, ...);

class Forme {
public:
    const char *h_;
};

class Head : public Forme {
public:
    void init();
    const char *c_str();
    void set(const char *str);
private:
};

void Head::set(const char *to_set)
{
    this->h_ = to_set;
}

void Head::init()
{
    this->h_ =
	"  ___\n"
	" /   \\\n"
	"(|O O|)\n"
	" | ^ |\n"
	" \\ - /\n"
	"  ---";
}

const char *Head::c_str()
{
    return this->h_;
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
