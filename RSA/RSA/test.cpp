#include "RSA.h"
#include <iostream>

void testString()
{
	RSA rsa;
	Key key = rsa.getKey();
	std::string str;
	std::cout << "请输入字符串：" << std::endl;
	std::cin >> str;
	//加密后的
	std::vector<long> str_ec = rsa.Ecrept(str, key.ekey, key.pkey);
	std::string str_de = rsa.Decrept(str_ec, key.dkey, key.pkey);
	std::cout << "密文:" << std::endl;
	rsa.printInfo(str_ec);
	//解密后的输出
	std::cout << "解密后的字符串" << std::endl;
	std::cout << str_de << std::endl;
}

void testFile()
{
	RSA rsa;
	Key key = rsa.getKey();
	std::string filename;
	std::cout << "输入文件名：" << std::endl;
	std::cin >> filename;
	rsa.ecrept(filename.c_str(), (filename+"ecrept.out.txt").c_str(), key.ekey, key.pkey);
	rsa.decrept((filename + "ecrept.out.txt").c_str(), (filename+"decrept.out.txt").c_str(), key.dkey, key.pkey);
}

int main()
{
	//testString();
	testFile();
	return 0;
}