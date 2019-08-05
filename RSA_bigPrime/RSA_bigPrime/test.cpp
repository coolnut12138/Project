#include "RSA_bigPrime.h"
#include <iostream>

void testString()
{
	RSA_bigPrime RSA_bigPrime;
	Key key = RSA_bigPrime.getKey();
	/*while (1){*/
		std::string str;
		std::cout << "请输入字符串：" << std::endl;
		getchar();
		std::getline(std::cin, str);
		//加密后的
		std::vector<bm::int1024_t> str_ec = RSA_bigPrime.Ecrept(str, key.ekey, key.pkey);
		std::string str_de = RSA_bigPrime.Decrept(str_ec, key.dkey, key.pkey);
		std::cout << "密文:" << std::endl;
		RSA_bigPrime.printInfo(str_ec);
		//解密后的输出
		std::cout << "解密后的字符串" << std::endl;
		std::cout << str_de << std::endl;
		return;
	//}
}

void testFile()
{
	RSA_bigPrime RSA_bigPrime;
	Key key = RSA_bigPrime.getKey();
	std::string filename;
	std::cout << "输入文件名：" << std::endl;
	std::cin >> filename;
	RSA_bigPrime.ecrept(filename.c_str(), (filename + "ecrept.out.txt").c_str(), key.ekey, key.pkey);
	std::cout << "加密完成" << std::endl;
	RSA_bigPrime.decrept((filename + "ecrept.out.txt").c_str(), (filename + "decrept.out.txt").c_str(), key.dkey, key.pkey);
	std::cout << "解密完成" << std::endl;
	return;
}

int main()
{
	int n;
	do{
		std::cout << "请选择要加密的类型:" << std::endl;
		std::cout << "1、加密字符串" << std::endl;
		std::cout << "2、加密文件" << std::endl;
		std::cin >> n;
		switch (n){
		case 1:
			testString();
			break;
		case 2:
			testFile();
			break;
		default:
			std::cout << "输入错误!!!" << std::endl;
			break;
		}
	} while (n);
	return 0;
}