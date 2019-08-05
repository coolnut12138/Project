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
	//���ܺ�����
	std::string str_de = rsa.Decrept(str_ec, key.dkey, key.pkey);
	std::cout << "����:" << std::endl;
	rsa.printInfo(str_ec);
	std::cout << "���ܺ���ַ���" << std::endl;
	std::cout << str_de << std::endl;
}

int main()
{
	testString();
	return 0;
}