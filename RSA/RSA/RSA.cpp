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
	//���ļ��������ȡ����μ���
	while (!fin.eof())
	{
		fin.read(buf, NUM);
		curNum = fin.gcount();	//��ǰ��ȡ���ֽ���
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
	long buf[NUM];		//Ҫ���ܵ�
	char buf_out[NUM];	//���
	int curNum;
	//���ļ��������ȡ����μ���
	while (!fin.eof())
	{
		fin.read((char*)buf, NUM * sizeof(long));
		curNum = fin.gcount();	//��ǰ��ȡ���ֽ���
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

//ģ������
long RSA::ecrept(long msg, long key, long pkey)
{
	long msg_out = 1;
	long a = msg;	// a:��Ҫ���ܵ���Ϣ  key: b  pkey: c
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

//������Ա������Կ��˽Կ
void RSA::produce_keys()
{
	long prime1 = produce_prime();
	long prime2 = produce_prime();
	while (prime1 == prime2){
		prime2 = produce_prime();
	}
	//��Կ(e, n) ˽Կ(d, n)
	_key.pkey = produce_pkey(prime1, prime2);	//n
	long orla = produce_orla(prime1, prime2);	//f(n)
	_key.ekey = produce_ekey(orla);				//e
	_key.dkey = produce_dkey(_key.ekey, orla);	//d
}

//��n
long RSA::produce_pkey(long prime1, long prime2)
{
	return prime1 * prime2;
}

//��f(n)
long RSA::produce_orla(long prime1, long prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

//��e
long RSA::produce_ekey(long orla)
{
	srand((unsigned int)time(nullptr));
	long e;
	//ѡȡe����(1, f(n))--->[2, f(n)-1]�м�ѡһ����f(n)���ʵ���������һ������
	while (1){
		e = rand() % orla;
		if (e > 1 && produce_gcd(e, orla) == 1){
			break;
		}
	}
	return e;
}

//��d
long RSA::produce_dkey(long ekey, long orla)
{
	//(dkey * ekey) % orla == 1
	//�Ż����� orla/ekey ��ʼ�ң�(5 * x) % 20 == 1;	��ô�ʹ� 4 ��ʼ�ң���20��Ŀ�ʼ��
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
