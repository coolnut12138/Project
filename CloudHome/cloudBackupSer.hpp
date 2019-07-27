#pragma once
#include <fstream>
#include <boost/filesystem.hpp>
#include "httplib.h"
#include "model.hpp"
#include "view.hpp"

#define BASE_DIR "wwwroot"
#define BACKUP_DIR "wwwroot/backup/"
#define SUB_DIR "/backup/"
#define SER_ADDR "0.0.0.0"
#define SER_PORT 9999

namespace htp = httplib;
namespace bfs = boost::filesystem;

class cloudBackupSer
{
    private:
	htp::Server server;
    public:
	cloudBackupSer()
	{
	    Init();
	    server.set_base_dir(BASE_DIR);
	}

	//备份目录不存在则创建
	void Init()
	{
	    bfs::path base_dir(BASE_DIR);
	    CheckFileExist(base_dir);
	    bfs::path sub_dir(BACKUP_DIR);
	    CheckFileExist(sub_dir);
	}

	//判断备份路径是否存在
	static void CheckFileExist(const bfs::path& path)
	{
	    if(!bfs::exists(path)) {
		bfs::create_directory(path);
	    }
	    else {
		return;
	    }
	}

	void Run()
	{
	    server.Get("/(backup(/){0,1}){0,1}", ShowCloudFiles);
	    server.Get("/backup/(.*)",DownloadCloudFile);
	    server.Get("/img/(.*)",DownloadCloudFile);
	    server.listen(SER_ADDR, SER_PORT);
	}
    private:	
	//客户端上传文件到服务端
	static void UploadFiles(const htp::Request& req, htp::Response& resp)
	{
	    //根据客户端请求中的Range字段来确定文件的大小
	}

	//展示备份文件的所有文件名到客户端，遍历该目录下的文件即可
	static void ShowCloudFiles(const htp::Request& req, htp::Response& resp)
	{
	    FileModel model;
	    model.Load();
	    
	    std::vector<FileInfo> all_files;
	    model.GetAllFiles(&all_files);

	    std::string body;
	    View::RenderFiles(all_files, &body);

	    resp.set_content(body, "text/html");
	    return;
	}
	static void DownloadCloudFile(const htp::Request& req, htp::Response& resp)
	{
	    //客户点击相应的文件名，服务器会得到一个请求，如req.path == "/back/a.txt"
	    std::string downloadFileName = BASE_DIR + req.path;
	    if(!bfs::exists(downloadFileName)){
		resp.status = 404;
		return;
	    }
	    
	    std::ifstream file(downloadFileName, std::ios::binary);
	    if(!file.is_open()){
		std::cerr << "file" << downloadFileName << "open error" << std::endl;
		resp.status = 500;
		return;
	    }
	    
	    int64_t fileSize = bfs::file_size(downloadFileName);
	    std::string body;
	    body.resize(fileSize);
	    file.read((char*)body.c_str(), fileSize);
	    if(!file.good()){
		std::cerr << downloadFileName << "read error" << std::endl;
		resp.status = 500;
		file.close();
		return;
	    }
	    resp.set_content(body, "application/octet-stream");
	    file.close();
	    return;
	}
};

