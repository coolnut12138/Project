#pragma once
#include <iostream>
#include <vector>
#include <pthread.h>
#include "compressTool.hpp"

#define BACKUP_DIR "wwwroot/backup/"
#define SUB_DIR	   "/backup/"

namespace bfs = boost::filesystem;
CompressUtil cutl;

struct FileInfo
{
    std::string filename;
    std::string url;
};


class FileModel
{
    private:
	std::vector<FileInfo> _files;
	pthread_rwlock_t _rwlock;
    public:
	FileModel(){
	    pthread_rwlock_init(&_rwlock, nullptr);
	}
	~FileModel() {
	    pthread_rwlock_destroy(&_rwlock);
	}
	bool Load(){
	    //将目录中的文件加载到vector中
	    bfs::path backupDir(BACKUP_DIR);
	    
	    std::vector<std::string> normalFile;
	    cutl.GetFileList(normalFile);
	    for(size_t i = 0; i < normalFile.size(); i++){
		bfs::path realpath(normalFile[i]);

		FileInfo f;
		f.filename = realpath.filename().string();
		f.url = SUB_DIR + f.filename;

		_files.push_back(f);
	    }
	    std::cout << "load " << _files.size() << " files" << std::endl;
	}

	bool GetAllFiles(std::vector<FileInfo>* files) {
	    pthread_rwlock_rdlock(&_rwlock);
	    files->clear();
	    for(const auto& e : _files){
		//std::cerr << e.filename << std::endl;
		files->push_back(e);
	    }
	    pthread_rwlock_unlock(&_rwlock);
	    return true;
	}
};
