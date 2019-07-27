#pragma once
#include <iostream>
#include <vector>

#define BACKUP_DIR "wwwroot/backup/"
#define SUB_DIR	   "/backup/"

namespace bfs = boost::filesystem;

struct FileInfo
{
    std::string filename;
    std::string url;
};

class FileModel
{
    private:
	std::vector<FileInfo> _files;
    public:
	bool Load(){
	    //将目录中的文件加载到vector中
	    bfs::path backupDir(BACKUP_DIR);

	    bfs::directory_iterator files_begin(backupDir);
	    bfs::directory_iterator files_end;
	    for(; files_begin != files_end; ++files_begin){
		FileInfo f;
		f.filename = files_begin->path().filename().string();
		f.url = SUB_DIR + f.filename;

		_files.push_back(f);
	    }
	    std::cout << "load " << _files.size() << " files" << std::endl;
	}

	bool GetAllFiles(std::vector<FileInfo>* files) const{
	    files->clear();
	    for(const auto& e : _files){
		files->push_back(e);
	    }
	    return true;
	}
};
