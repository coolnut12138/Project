#include "RSA.h"
#include <iostream>

void testString()
{
	RSA rsa;
	Key key = rsa.getKey();
	std::string str;
	std::cout << "�������ַ�����" << std::endl;
	std::cin >> str;
	//���ܺ��
	std::vector<long> str_ec = rsa.Ecrept(str, key.ekey, key.pkey);
	std::string str_de = rsa.Decrept(str_ec, key.dkey, key.pkey);
	std::cout << "����:" << std::endl;
	rsa.printInfo(str_ec);
	//���ܺ�����
	std::cout << "���ܺ���ַ���" << std::endl;
	std::cout << str_de << std::endl;
}

void testFile()
{
	RSA rsa;
	Key key = rsa.getKey();
	std::string filename;
	std::cout << "�����ļ�����" << std::endl;
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