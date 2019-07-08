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
{}
void RSA::decrept(const char* ecrept_file_in, const char* plain_file_out, long dkey, long pkey)
{}

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
		//msg_out += (std::string)(ecrept(e, dkey, pkey));
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
	/*
	long msg_out = 1;
	long a = msg;
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
	*/
	long msg_des = 1;
	//a:��Ҫ���ܵ���Ϣ
	long a = msg;
	//key: b pkey: c
	long index = key;
	//������
	while (index){
		if (index & 1){
			msg_des = (msg_des * a) % pkey;
		}
		index >>= 1;
		a = (a * a) % pkey;
	}
	return msg_des;
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
	_key.ekey = produce_ekey(orla);		//e
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
		if (e > 1 && is_prime(e)){
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
	for (long dkey = orla / ekey;; dkey++){
		if ((dkey * ekey) % orla == 1){
			break;
		}
	}
	return dkey;
}

