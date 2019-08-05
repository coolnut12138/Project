#include <ctemplate/template.h>

class View
{
    public:
	static void Render(std::string* html){
	    ctemplate::TemplateDictionary dict("example");
	}
};
