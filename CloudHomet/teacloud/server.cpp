#include <boost/filesystem.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include "httplib.h"
#include "compress.hpp"

#define SERVER_BASE_DIR "www"
#define SERVER_ADDR "0.0.0.0"
#define SERVER_PORT 9000
#define SERVER_BACKUP_DIR SERVER_BASE_DIR"/list/"

using namespace httplib;
namespace bf = boost::filesystem;

CompressStore cstor;

class CloudServer
{
    private:
	httplib::Server srv;
    public:
	CloudServer(){
	    bf::path base_path(SERVER_BASE_DIR);
	    if(!bf::exists(base_path)){
		bf::create_directory(base_path);
	    }
	    bf::path list_path(SERVER_BACKUP_DIR);
	    if(!bf::exists(list_path)){
		bf::create_directory(list_path);
	    }
	}
	bool Start(){
	    srv.set_base_dir(SERVER_BASE_DIR);
	    srv.Get("/(list(/){0,1}){0,1}", GetFileList);  //请求list时才去获取文件目录
	    srv.Get("/list/(.*)", GetFileData);    // . 表示匹配任意一个字符，  * 匹配上一个字符任意次， .* 就表示匹配任意字符任意次
	    srv.Put("/list/(.*)", PutFileData);	    //上传文件
	    srv.listen(SERVER_ADDR, SERVER_PORT);
	    return true;
	}
    private:
	static void PutFileData(const Request& req, Response& rsp){
	    std::cout << "hello wg" << std::endl;
	    if(!req.has_header("Range")) {
		rsp.status = 400;
		return;
	    }

	    std::string range = req.get_header_value("Range");
	    int64_t range_start;
	    if(RangeParse(range, range_start) == false) {
		rsp.status = 400;
		return;
	    }
	    std::string real= SERVER_BASE_DIR + req.path;

	    cstor.SetFileData(real, req.body, range_start);
	    return;   
	}

	static bool RangeParse(std::string &range, int64_t &start) {
	    //Range格式: bytes=start-end
	    size_t pos1 = range.find("=");
	    size_t pos2 = range.find("-");
	    if(pos1 == std::string::npos || pos2 == std::string::npos){
		std::cerr << "range:[" << range << "] format error\n";
		return false;
	    }
	    std::stringstream rs;   //字符串转数字
	    rs << range.substr(pos1+1, pos2 - pos1 - 1);
	    rs >> start;
	    return true;
	}

	static void GetFileList(const Request& req, Response& rsp){
	    std::vector<std::string> list;
	    cstor.GetFileList(list);

	    std::string body;	//即将给客户端响应的正文
	    body += "<html><body><ol><hr />";
	    for(auto i : list) {
		bf::path path(i);
		std::string file = path.filename().string();  //请求的路径
		//请求路径应该是"/list/filename",请求路径是它时，会在它前面加上服务器地址，请求的就是这个文件名称
		std::string uri = "/list/" + file;
		body += "<h4><li>";
		body += "<a href='";
		body += uri;
		body += "'>";
		body += file;//filename显示文件名称，不显示全路径，但是这也是一个path对象，所以也要加上.string()
		body += "</a>";
		body += "</li></h4>";
		//std::string file = item_begin->path().string();	//这块boost库中会给获取的文件名加一个双引号，所以要处理下;使用.string()是因为item_begin->path()是一个path对象，要对他转型
		//std::cerr << "file:" << file << std::endl;
	    }
	    body += "<hr /></ol></body></html>";
	    rsp.set_content(&body[0], "text/html"); //第二个参数写成/text/html会直接下载
	    return;
	}
	static void GetFileData(const Request& req, Response& rsp){
	    //此时的req.path == "/list/a.txt"
	    std::string real = SERVER_BASE_DIR + req.path;  //这样就是相对路径了
	    std::string body;
	    cstor.GetFileData(real, body);
	    rsp.set_content(body, "application/octet-stream");
	}
};

void thr_start(){
    cstor.LowHeatFileStore();
}

int main()
{
    std::thread thr(thr_start);
    thr.detach();
    CloudServer srv;
    srv.Start();
    return 0;
}
