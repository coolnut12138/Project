#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <zlib.h>

//未压缩文件的存放位置
#define NORMALFILE_PATH "wwwroot/backup/"
//已经压缩的文件存放位置
#define COMPRESSEDFILE_PATH "wwwroot/zip/"
//时间超过DEADLINE就会自动压缩
#define DEADLINE 30
//记录普通文件和压缩文件的信息的文件
#define FILEINFO "fileinfo.list"

namespace bfs = boost::filesystem;

class CompressUtil
{
    public:
	CompressUtil(){
	    Init();
	}
	~CompressUtil() {
	    pthread_rwlock_destroy(&_rwlock);
	}

	//对热度低的文件进行压缩存储
	bool CompressLowHeatFiles() {
	    //获取文件列表信息
	    ObtainFromlist();
	    while(1){
		//遍历列表压缩需要压缩的文件
		TraveDirAndCompressFiles();
		//将操作完的文件信息加入 fileinfo.list 中
		UpdateListFile();
		sleep(5);
	    }
	}
	
	//向外提供获取文件列表功能
	bool GetFileList(std::vector<std::string>& list){
	    pthread_rwlock_rdlock(&_rwlock);
	    for(const auto& e : _allfile_list) {
		list.push_back(e.first);
	    }
	    pthread_rwlock_unlock(&_rwlock);
	    return true;
	}


	//通过文件名获取文件对应的压缩包名称
	bool GetGzipFilename(std::string& file, std::string& gzip) {
	    pthread_rwlock_rdlock(&_rwlock);
	    auto it = _allfile_list.find(file);
	    if(it == _allfile_list.end()) {
		pthread_rwlock_unlock(&_rwlock);
		return false;
	    }
	    gzip = it->second;
	    pthread_rwlock_unlock(&_rwlock);
	    return true;
	}

	//向外提供文件数据获取的功能（未压缩文件直接获取到数据，压缩文件先解压缩，然后获取到文件数据）
	bool GetFileData(std::string& file, std::string& body) {
	    if(!bfs::exists(file)){
		//文件是压缩文件，先解压，再获取文件数据
		std::string gzipFilename;
		GetGzipFilename(file, gzipFilename);
		UnCompressFile(gzipFilename, file);
		ObtainNormalFileData(file, body);
	    }
	    else{
		ObtainNormalFileData(file, body);
	    }
	}

	//用户上传文件时将文件内容存储到服务器上
	bool StorgeFileData(const std::string& file, const std::string& body, const int64_t offset) {
	    int fd = open(file.c_str(), O_CREAT | O_WRONLY, 0664);
	    if(fd < 0) {
		std::cerr << "Storge file " << file << " open error" << std::endl;
		return false;
	    }
	    flock(fd, LOCK_EX);
	    lseek(fd, offset, SEEK_SET);
	    int ret = write(fd, &body[0], body.size());
	    if(ret < 0){
		std::cerr << "storge file " << file << " failed!!" << std::endl;
		flock(fd, LOCK_UN);
		close(fd);
		return false;
	    }
	    flock(fd, LOCK_UN);
	    close(fd);
	    AddFileRecord(file, "");
	    return true;
	}
	private:
	bool Init(){
	    pthread_rwlock_init(&_rwlock, nullptr);
	    if(!bfs::exists(COMPRESSEDFILE_PATH)) {
		bfs::create_directory(COMPRESSEDFILE_PATH);
	    }
	}
	
	//遍历目录中的文件并且将需要压缩的文件进行压缩
	//1、遍历目录
	//2、判断是否需要压缩
	//3、需要压缩则压缩，不需要则不压缩
	bool TraveDirAndCompressFiles() {
	    //将Windows下的gbk格式转为utf-8
	    std::string cmd = "bash gbk2utf-8";
	    FILE* fd = popen(cmd.c_str(), "r");
	    pclose(fd);

	    bfs::directory_iterator files_begin(NORMALFILE_PATH);
	    bfs::directory_iterator files_end;
	    while(files_begin != files_end){
		if(bfs::is_directory(files_begin->status())){
		    continue;
		}
		std::string name = files_begin->path().string();
		if(IsNeedCompress(name)) {
		    std::string gzip = COMPRESSEDFILE_PATH + files_begin->path().filename().string() + ".gz"; 
		    if(CompressFile(name, gzip)){
			std::cout << "file " << name  << " compress success!!!" << std::endl;
			AddFileRecord(name, gzip);
		    }else{
			std::cerr << "file " << name << " compress failed !!!" << std::endl;
		    }
		}
		++files_begin;
	    }
	    return true;
	}

	//将文件名和压缩文件名存入_allfile_list 中
	bool AddFileRecord(const std::string& file, const std::string& gzipfile) {
	    pthread_rwlock_wrlock(&_rwlock);
	    _allfile_list[file] = gzipfile;
	    pthread_rwlock_unlock(&_rwlock);
	}
	//每次启动压缩线程，都要从fileinfo.list文件中读取文件列表信息，将文件信息存入_allfile_list中
	bool ObtainFromlist() {
	    bfs::path listFile(FILEINFO);
	    if(!bfs::exists(listFile)){
		std::cerr << "fileinfo.list not exist!!!" << std::endl;
		return false;
	    }

	    std::ifstream rdlistFile(FILEINFO, std::ios::binary);
	    if(!rdlistFile.is_open()){
		std::cerr << "fileinfo.list open error" << std::endl;
		return false;
	    }

	    int64_t fsize = bfs::file_size(listFile);
	    std::string body;
	    body.resize(fsize);
	    //将文件内容读到body中
	    rdlistFile.read((char*)body.c_str(), fsize);
	    if(!rdlistFile.good()){
		std::cerr << "fileinfo.list read to body error" << std::endl;
		rdlistFile.close();
		return false;
	    }
	    rdlistFile.close();

	    //fileinfo.list存放文件的信息格式是：filename gzipfilename\n
	    //使用boost库中的split函数进行分割
	    std::vector<std::string> res;   //分割后的信息放在这里
	    boost::split(res, body, boost::is_any_of("\n"));
	    
	    for(const auto& e : res) {
		//分割后的格式是 filename gzipfilename
		//所以要找到两者之间的空格进行分割分别作为key和val
		size_t pos = e.find(" ");
		if(pos != std::string::npos){
		    std::string key = e.substr(0, pos);
		    std::string val = e.substr(pos + 1);
		    _allfile_list[key] = val;
		}
	    }
	    return true;
	}

	//每次压缩完将_allfile_list中的文件信息再次存放到fileinfo.list中
	bool UpdateListFile() {
	    std::stringstream text_line;
	    for(const auto& e : _allfile_list) {
		text_line << e.first << " " << e.second << "\n";
	    }

	    std::ofstream wtfile(FILEINFO, std::ios::binary | std::ios::trunc);
	    if(!wtfile.is_open()) {
		std::cerr << "fileinfo.list open error" << std::endl;
		return false;
	    }
	    wtfile.write(text_line.str().c_str(), text_line.str().size());
	    if(!wtfile.good()) {
		std::cerr << "map write to file failed!!" << std::endl;
		wtfile.close();
		return false;
	    }
	    wtfile.close();
	    return true;
	}

	//判断文件是否需要压缩
	bool IsNeedCompress(std::string& file) {
	    //stat这个系统调用接口可以获取文件相关信息，其中就有最后一次访问时间
	    //通过判别最后一次访问时间到当前时间是否超过我们约定的dead_line
	    struct stat info;
	    if(stat(file.c_str(), &info) < 0) {
		std::cerr << "stat get info error" << std::endl;
		return false;
	    }
	    time_t cur_time = time(nullptr);	//当前时间
	    time_t acc_time = info.st_atime;	//最后一次修改时间
	    if((cur_time - acc_time) > DEADLINE) {
		return true;
	    }
	    else {
		return false;
	    }
	}

	//压缩文件
	//zlib库压缩文件流程：1、打开普通文件 2、将压缩文件以二进制写的方式打开(gzopen) 3、
	bool CompressFile(const std::string& file, const std::string& gzipfile){
	    int fd = open(file.c_str(), O_RDONLY);
	    if(fd < 0) {
		std::cerr << "file open error" << std::endl;
		return false;
	    }

	    //使用zlib库进行压缩文件
	    gzFile gf = gzopen(gzipfile.c_str(), "wb");
	    if(gf == nullptr){
		std::cerr << "gzfile " << gzipfile << " open error" << std::endl;
		return false;
	    }
	    int ret;
	    char buf[1024];
	    flock(fd, LOCK_SH);
	    while(1) {
		ret = read(fd, buf, sizeof(buf) - 1);
		if(ret > 0) {
		    gzwrite(gf, buf, ret);
		}else{
		    break;
		}
	    }
	    flock(fd, LOCK_UN);
	    close(fd);
	    gzclose(gf);
	    //压缩完后将普通文件删除，unlink这个接口相对安全，如果一个文件已经被打开并且正在操作中，那么就删除不了，会报错EBUSY
	    unlink(file.c_str());	
	    return true;
	}

	//解压缩文件
	bool UnCompressFile(const std::string& gzipfile, const std::string& file) {
	    int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
	    if(fd < 0) {
		std::cerr << "uncompress open file error!!" << std::endl;
		return false;
	    }
	    gzFile gf = gzopen(gzipfile.c_str(), "rb");
	    if(gf == nullptr) {
		std::cerr << "uncompress gzfile open error!!!" << std::endl;
		return false;
	    }

	    int ret;
	    char buf[1024];
	    flock(fd, LOCK_EX);
	    while((ret = gzread(gf, buf, sizeof(buf) - 1)) > 0){
		int len = write(fd, buf, ret);
		if(len < 0) {
		    std::cerr << "uncompress write file filed!!!" << std::endl;
		    flock(fd, LOCK_UN);
		    gzclose(gf);
		    close(fd);
		    return false;
		}
	    }
	    flock(fd, LOCK_UN);
	    gzclose(gf);
	    close(fd);
	    unlink(gzipfile.c_str());
	    return true;
	}

	//获取非压缩文件的数据
	bool ObtainNormalFileData(const std::string& file, std::string& buf) {
	    int fd = open(file.c_str(), O_RDONLY);
	    if(fd < 0) {
		std::cerr << "open Normal file error" << std::endl;
		return false;
	    }
	    int64_t fsize = bfs::file_size(file);
	    buf.resize(fsize);

	    flock(fd, LOCK_SH);
	    int ret = read(fd, &buf[0], fsize);
	    flock(fd, LOCK_UN);

	    if(ret != fsize) {
		std::cerr << "read NormalFile Data failed!!!" << std::endl;
		close(fd);
		return false;
	    }
	    close(fd);
	    return true;
	}
    private:
	//存放所有文件，key是未压缩文件名，val是压缩文件名
	std::unordered_map<std::string, std::string> _allfile_list;
	pthread_rwlock_t _rwlock;   //读写锁
};

