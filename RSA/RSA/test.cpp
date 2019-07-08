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
	//解密后的输出
	std::string str_de = rsa.Decrept(str_ec, key.dkey, key.pkey);
	std::cout << "密文:" << std::endl;
	rsa.printInfo(str_ec);
	std::cout << "解密后的字符串" << std::endl;
	std::cout << str_de << std::endl;
}

int main()
{
	testString();
	return 0;
}