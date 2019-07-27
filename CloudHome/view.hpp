#pragma once
#include <ctemplate/template.h>
#include "model.hpp"

class View
{
    public:
	static void RenderFiles(const std::vector<FileInfo>& all_files, std::string* html){
	    ctemplate::TemplateDictionary dict("all_files");
	    for(const auto& file : all_files) {
		ctemplate::TemplateDictionary* table_dict = dict.AddSectionDictionary("file");
		table_dict->SetValue("filename", file.filename);
		table_dict->SetValue("url", file.url);
	    }
	    ctemplate::Template* tp1;
	    tp1 = ctemplate::Template::GetTemplate("./template/home.html", ctemplate::DO_NOT_STRIP);
	    tp1->Expand(html, &dict);

	}
};
