#pragma once
#include <fstream>
#include <boost/filesystem.hpp>
//使用httplib库中的SSL加密
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "view.hpp"
#include "compressTool.hpp"
#include "model.hpp"

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
	htp::SSLServer server;
    public:
	cloudBackupSer(const char *cert, const char *dkey)
	    :server(cert, dkey)
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
	    server.Get("/(backup(/){0,1}){0,1}", ShowCloudFiles);   //将文件列表展示在网页上
	    server.Get("/img/(.*)", DownloadCloudFile);
	    server.Get("/backup/(.*)",DownloadCloudFile);   //客户端下载文件
	    server.Put("/backup/(.*)",UploadFiles); //客户端上传文件
	    server.listen(SER_ADDR, SER_PORT);
	}
    private:	
	//客户端上传文件到服务端
	static void UploadFiles(const htp::Request& req, htp::Response& resp)
	{
	    //根据客户端请求中的Range字段来确定文件的大小
	    std::cout << "client is uploading..." << std::endl;
	    //请求中没有Range字段直接报错
	    if(!req.has_header("Range")){
		resp.status = 400;
		return;
	    }

	    std::string range = req.get_header_value("Range");
	    int64_t range_start;
	    if(RangeParse(range, range_start) == false) {
		resp.status = 400;
		return;
	    }
	    std::string file = BASE_DIR + req.path;
	    cutl.StorgeFileData(file, req.body, range_start);
	    std::cout << "client upload success!!!" << std::endl;
	    return;
	}

	static bool RangeParse(std::string &range, int64_t &start) {
	    //传过来的Range格式是:bytes=start-end
	    //根据格式进行分割
	    size_t pos1 = range.find("=");
	    size_t pos2 = range.find("-");
	    if(pos1 == std::string::npos || pos2 == std::string::npos) {
		std::cerr << "range format error!!!" << std::endl;
		return false;
	    }
	    std::stringstream s;    //字符串转数字
	    s << range.substr(pos1+1, pos2-pos1-1);
	    s >> start;
	    return true;

	}

	//展示备份文件的所有文件名到客户端，遍历该目录下的文件即可
	static void ShowCloudFiles(const htp::Request& req, htp::Response& resp)
	{
	    std::cerr << "Showing Files..." <<std::endl;
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
	    
	    std::string body;
	    cutl.GetFileData(downloadFileName, body);

	    resp.set_content(body, "application/octet-stream");
	    return;
	}
};
