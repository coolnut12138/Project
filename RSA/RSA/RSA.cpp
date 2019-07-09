#define _CRT_SECURE_NO_WARNINGS 1
#include "RSA.h"
#include <iostream>

RSA::RSA()
{
	produce_keys();
}
Key RSA::getKey()
{
	return _key;
}

void RSA::ecrept(const char* plain_file_in, const char* ecrept_file_out, long ekey, long pkey)
{
	std::ifstream fin(plain_file_in);
	std::ofstream fout(ecrept_file_out, std::ofstream::app);
	if (!fin.is_open()){
		std::cout << "file open filed" << std::endl;
		return;
	}
	const int NUM = 256;
	char buf[NUM];
	long buf_out[NUM];
	int curNum;
	//打开文件，按块读取，逐段加密
	while (!fin.eof())
	{
		fin.read(buf, NUM);
		curNum = fin.gcount();	//当前读取的字节数
		for (int i = 0; i < curNum; i++){
			buf_out[i] = ecrept((long)buf[i], ekey, pkey);
		}
		fout.write((char*)buf_out, curNum * sizeof(long));
	}

	fin.close();
	fout.close();
}
void RSA::decrept(const char* ecrept_file_in, const char* plain_file_out, long dkey, long pkey)
{
	std::ifstream fin(ecrept_file_in);
	std::ofstream fout(plain_file_out, std::ofstream::app);
	if (!fin.is_open()){
		std::cout << "file open filed" << std::endl;
		return;
	}
	const int NUM = 256;
	long buf[NUM];		//要解密的
	char buf_out[NUM];	//输出
	int curNum;
	//打开文件，按块读取，逐段加密
	while (!fin.eof())
	{
		fin.read((char*)buf, NUM * sizeof(long));
		curNum = fin.gcount();	//当前读取的字节数
		curNum /= sizeof(long);
		for (int i = 0; i < curNum; i++){
			buf_out[i] = (char)ecrept(buf[i], dkey, pkey);
		}
		fout.write(buf_out, curNum);
	}

	fin.close();
	fout.close();
}

std::vector<long> RSA::Ecrept(std::string& str_in, long ekey, long pkey)
{
	std::vector<long> msg_des;
	for (const auto& e : str_in){
		msg_des.push_back(ecrept(e, ekey, pkey));
	}
	return msg_des;
}
std::string RSA::Decrept(std::vector<long>& ecrept_str, long dkey, long pkey)
{
	std::string msg_out;
	for (const auto& e : ecrept_str){
		msg_out.push_back((char)ecrept(e, dkey, pkey));
	}
	return msg_out;
}

void RSA::printInfo(std::vector<long>& ecrept_str)
{
	for (const auto& e : ecrept_str)
	{
		std::cout << e << " ";
	}
	std::cout << std::endl;
}

long RSA::produce_prime()
{
	long num;
	srand((unsigned int)time(nullptr));
	while (1){
		num = rand() % 100 + 2;
		if (is_prime(num)){
			break;
		}
	}
	return num;
}

bool RSA::is_prime(long prime)
{
	if (prime < 2){
		return false;
	}
	for (int i = 2; i <= sqrt(prime); i++){
		if (prime % i == 0){
			return false;
		}
	}
	return true;
}

//模幂运算
long RSA::ecrept(long msg, long key, long pkey)
{
	long msg_out = 1;
	long a = msg;	// a:需要加密的信息  key: b  pkey: c
	long b = key;
	long c = pkey;
	while(b){
		if(b & 1){
			msg_out = (msg_out * a) % c;
		}
		b >>= 1;
		a = (a * a) % c;
	}
	return msg_out;
}

//产生成员变量公钥和私钥
void RSA::produce_keys()
{
	long prime1 = produce_prime();
	long prime2 = produce_prime();
	while (prime1 == prime2){
		prime2 = produce_prime();
	}
	//公钥(e, n) 私钥(d, n)
	_key.pkey = produce_pkey(prime1, prime2);	//n
	long orla = produce_orla(prime1, prime2);	//f(n)
	_key.ekey = produce_ekey(orla);				//e
	_key.dkey = produce_dkey(_key.ekey, orla);	//d
}

//求n
long RSA::produce_pkey(long prime1, long prime2)
{
	return prime1 * prime2;
}

//求f(n)
long RSA::produce_orla(long prime1, long prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

//求e
long RSA::produce_ekey(long orla)
{
	srand((unsigned int)time(nullptr));
	long e;
	//选取e，在(1, f(n))--->[2, f(n)-1]中间选一个和f(n)互质的数，即找一个素数
	while (1){
		e = rand() % orla;
		if (e > 1 && produce_gcd(e, orla) == 1){
			break;
		}
	}
	return e;
}

//求d
long RSA::produce_dkey(long ekey, long orla)
{
	//(dkey * ekey) % orla == 1
	//优化：从 orla/ekey 开始找：(5 * x) % 20 == 1;	那么就从 4 开始找，比20大的开始找
	long dkey = orla / ekey;
	for (;; dkey++){
		if ((dkey * ekey) % orla == 1){
			break;
		}
	}
	return dkey;
}

long RSA::produce_gcd(long ekey, long orla)
{
	long a = ekey;
	long b = orla;
	int tmp;
	while (tmp = a % b){
		a = b;
		b = tmp;
	}
	return b;
}
