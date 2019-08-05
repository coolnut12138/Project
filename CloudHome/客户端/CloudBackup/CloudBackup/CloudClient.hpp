#ifndef __M_CLOUD_H__
#define __M_CLOUD_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>	//ʹ�����е�split
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#define CLIENT_BACKUP_DIR		"backup"
#define CLIENT_BACKUP_INFO_FINE "back.list"
#define CERT					"cert.pem"
//10 << 20 = 10M
#define RANGE_MAX_SIZE (10 << 20)
#define SERVER_IP "192.168.160.150"
#define SERVER_PORT 9999

#define BACKUP_URI "/backup/"

namespace bf = boost::filesystem;

class ThrBackUp
{
private:
	std::string _file;
	int64_t _range_start;
	int64_t _range_len;
public:
	bool _res;
public:
	ThrBackUp(const std::string &file, int64_t start, int64_t len)
		:_res(true)
		,_file(file)
		,_range_start(start)
		,_range_len(len)
	{}
	void Start()
	{
		std::cout << "into thr_start\n";
		//��ȡ�ļ���range�ֿ�����
		std::ifstream path(_file, std::ios::binary);
		if (!path.is_open()) {
			std::cerr << "range backup file " << _file << "filed\n";
			_res = false;
			return;
		}
		//��ת��range����ʼλ��
		path.seekg(_range_start, std::ios::beg);
		std::string body;
		body.resize(_range_len);
		//��ȡ�ļ���range�ֿ���ļ�����
		path.read(&body[0], _range_len);
		std::cerr << "ready to read!!!" << std::endl;
		if (!path.good()) {
			std::cerr << "read file " << _file << " range data failed\n";
			_res = false;
			return;
		}
		std::cerr << "read over!!!" << std::endl;
		path.close();

		//�ϴ�range����
		bf::path name(_file);
		//��֯�ϴ���url·��	method url version
		//PUT /list/filename HTTP/1.1
		std::string url = BACKUP_URI + name.filename().string();
		//ʵ����һ��httplib�Ŀͻ��˶���
		httplib::SSLClient cli(SERVER_IP, SERVER_PORT);
		cli.set_ca_cert_path(CERT);
		cli.enable_server_certificate_verification(true);
		//����http����ͷ��Ϣ
		httplib::Headers hdr;
		hdr.insert(std::make_pair("Content-Length", std::to_string(_range_len)));
		std::stringstream tmp;
		tmp << "bytes=" << _range_start << "-" << (_range_start + _range_len - 1);
		hdr.insert(std::make_pair("Range", tmp.str().c_str()));

		//ͨ��ʵ������client�����˷���PUT����
		auto rsp = cli.Put(url.c_str(), hdr, body, "text/plain");
		if (rsp && rsp->status != 200 || rsp == nullptr) {
			std::cerr << "connect error~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			_res = false;
			return;
		}

		std::stringstream ss;
		ss << "backup file" << _file << "] range:["<< _range_start <<"-" << _range_len << "] backup success\n";
		std::cout << ss.str();
		return;
	}
};

class CloudClient
{
private:
	std::unordered_map<std::string, std::string> _backup_list;	//���filname-etag
private:
	//��ȡ�ļ�������Ϣ
	bool GetBackupInfo() {
		//��ȡ���������ָ�ʽ��
		//filename etag\n
		//filename etsg\n
		bf::path path(CLIENT_BACKUP_INFO_FINE);
		if (!bf::exists(path)) {
			std::cerr << "list file" << path.string() << "is not exists\n";
			return false;
		}
		int64_t fsize = bf::file_size(path);
		if (fsize == 0) {
			std::cerr << "have no backup info\n";
			return false;
		}
		std::string body;
		body.resize(fsize);
		std::ifstream file(CLIENT_BACKUP_INFO_FINE, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "list file open error\n";
			return false;
		}
		file.read(&body[0], fsize);
		if (!file.good()) {
			std::cerr << "read list file body error\n";
			file.close();
			return false;
		}
		file.close();

		std::vector<std::string> list;
		boost::split(list, body, boost::is_any_of("\n"));
		for (auto& e : list) {
			//filename2 etag
			size_t pos = e.find(" ");
			if (pos == std::string::npos) {
				continue;
			}
			std::string key = e.substr(0, pos);
			std::string val = e.substr(pos + 1);
			_backup_list[key] = val;
		}

		return true;
	}
	//��������Ϣ���浽ָ���ļ���,���ǽ��µ��޸ĵ���Ϣ������ļ����൱��ˢ���ļ�
	bool SetBackupInfo() {
		std::string body;
		for (auto it : _backup_list) {
			body += it.first + " " + it.second + "\n";
		}
		std::ofstream file(CLIENT_BACKUP_INFO_FINE, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "open list file error\n";
			return false;
		}
		file.write(&body[0], body.size());
		if (!file.good()) {
			std::cerr << "set backup info error\n";
			file.close();
			return false;
		}
		file.close();
		return true;
	}
	//���Ŀ¼�µ��ļ��Ƿ���Ҫ����
	bool BackupDirListen(const std::string &path)
	{
		bf::path file(path);
		bf::directory_iterator item_begin(file);
		bf::directory_iterator item_end;
		for (; item_begin != item_end; ++item_begin) {
			if (bf::is_directory(item_begin->status())) {
				BackupDirListen(item_begin->path().string());
				continue;
			}
			if (FileIsNeedBackup(item_begin->path().string()) == false) {
				continue;
			}
			std::cerr << "file:[" << item_begin->path().string() << "need  backup\n";
			if (PutFileData(item_begin->path().string()) == false) {
				continue;
			}
			//�ļ��ϴ��ɹ���Ӧ��¼���ļ���etag��Ϣ
			AddBackInfo(item_begin->path().string());
		}
		return true;
	}

	//���etag��Ϣ
	bool AddBackInfo(const std::string &file) {
		//etag = "mtime-fsize"
		std::string etag;
		if (GetFileEtag(file, etag) == false) {
			return false;
		}
		_backup_list[file] = etag;
		return true;
	}

	//�ж��ļ��Ƿ���Ҫ����
	bool FileIsNeedBackup(const std::string &file) {
		std::string etag;
		if (GetFileEtag(file, etag) == false) {
			return false;
		}
		auto it = _backup_list.find(file);
		if (it != _backup_list.end() && it->second == etag) {
			return false;
		}
		return true;
	}

	//��ȡ�ļ�etag��Ϣ
	bool GetFileEtag(const std::string &file, std::string &etag) {
		bf::path path(file);
		if (!bf::exists(path)) {
			std::cerr << "get file " << file << "etag error\n";
			return false;
		}
		int64_t fsize = bf::file_size(path);
		int64_t mtime = bf::last_write_time(path);
		std::stringstream tmp;
		tmp << std::hex << fsize << "-" << std::hex << mtime;	//ע���޸�Ϊmtime-fsize
		etag = tmp.str();
		return true;
	}

	//���ļ������ϴ�(�����ļ�)
	bool PutFileData(const std::string &file) {
		//���ֿ��С����(10M)���ļ����ݽ��зֿ鴫��
		//ͨ����ȡ�ֿ鴫���Ƿ�ɹ��ж������ļ��Ƿ��ϴ��ɹ�
		//ѡ����̴߳���
		//1����ȡ�ļ���С
		int64_t fsize = bf::file_size(file);
		if (fsize <= 0) {
			std::cerr << "file " << file << " unnecessary backup\n";
			return false;
		}
		//2�������ܹ���Ҫ�ֶ��ٿ飬�õ�ÿ���С�Լ���ʵλ��
		//3��ѭ�������̣߳����߳����ϴ��ļ�����
		int count = (int)(fsize / RANGE_MAX_SIZE);
		std::vector<ThrBackUp> thr_res;
		std::vector<std::thread> thr_list;
		std::cerr << "file:[" << file << "] fsize:[" << fsize << "] count:[" << count + 1 << "]" << std::endl;
		for (int i = 0; i <= count; ++i) {
			int64_t range_start = i * RANGE_MAX_SIZE;
			int64_t range_end = ((i + 1) * RANGE_MAX_SIZE - 1);
			if (i == count) {
				range_end = fsize - 1;
			}
			int64_t range_len = range_end - range_start + 1;
			ThrBackUp backup_info(file, range_start, range_len);
			std::cerr << "file:[" << file << "] range:[" << range_start << "-" << range_end << "] " << range_len << std::endl;
			thr_res.push_back(backup_info);
		}
		for (int i = 0; i <= count; ++i) {
			thr_list.push_back(std::thread(thr_start, &thr_res[i]));
		}
		//4���ȴ������߳��˳����ж��ļ��ϴ����
		bool ret = true;
		for (int i = 0; i <= count; ++i) {
			thr_list[i].join();
			if (thr_res[i]._res == true) {
				continue;
			}
			ret = false;
		}
		//5���ϴ��ɹ���������ļ��ı�����Ϣ��¼
		if (ret == false) {
			std::cerr << "file:[" << file << "] backup failed!!!!!!!" << std::endl;
			return false;
		}
		else {
			std::cerr << "file:[" << file << "] backup success\n";
		}
		return true;
	}

	static void thr_start(ThrBackUp *back_info)
	{
		back_info->Start();
		return;
	}
public:
	CloudClient()
	{
		bf::path file(CLIENT_BACKUP_DIR);
		if (!bf::exists(file)) {
			bf::create_directory(file);
		}
	}
	bool Start() {
		GetBackupInfo();
		while (1) {
			BackupDirListen(CLIENT_BACKUP_DIR);
			SetBackupInfo();
			Sleep(1000);
		}
		return true;
	}
};

#endif
