#include "RSA_bigPrime.h"
#include <iostream>

void testString()
{
	RSA_bigPrime RSA_bigPrime;
	Key key = RSA_bigPrime.getKey();
	while (1){
		std::string str;
		std::cout << "�������ַ�����" << std::endl;
		std::cin >> str;
		//���ܺ��
		std::vector<bm::int1024_t> str_ec = RSA_bigPrime.Ecrept(str, key.ekey, key.pkey);
		std::string str_de = RSA_bigPrime.Decrept(str_ec, key.dkey, key.pkey);
		std::cout << "����:" << std::endl;
		RSA_bigPrime.printInfo(str_ec);
		//���ܺ�����
		std::cout << "���ܺ���ַ���" << std::endl;
		std::cout << str_de << std::endl;
	}
}

void testFile()
{
	RSA_bigPrime RSA_bigPrime;
	Key key = RSA_bigPrime.getKey();
	std::string filename;
	std::cout << "�����ļ�����" << std::endl;
	std::cin >> filename;
	RSA_bigPrime.ecrept(filename.c_str(), (filename + "ecrept.out.txt").c_str(), key.ekey, key.pkey);
	RSA_bigPrime.decrept((filename + "ecrept.out.txt").c_str(), (filename + "decrept.out.txt").c_str(), key.dkey, key.pkey);
}

int main()
{
	//testString();
	testFile();
	return 0;
}