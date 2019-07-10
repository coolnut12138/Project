#include "RSA_bigPrime.h"
#include <iostream>


RSA_bigPrime::RSA_bigPrime()
{
	produce_keys();
}
Key RSA_bigPrime::getKey()
{
	return _key;
}

void RSA_bigPrime::ecrept(const char* plain_file_in, const char* ecrept_file_out, bm::int1024_t ekey, bm::int1024_t pkey)
{
	std::ifstream fin(plain_file_in, std::ifstream::binary);
	std::ofstream fout(ecrept_file_out, std::ofstream::binary);
	if (!fin.is_open()){
		std::cout << "file open filed" << std::endl;
		return;
	}
	const int NUM = 256;
	char buf[NUM];
	bm::int1024_t buf_out[NUM];
	int curNum;
	//���ļ��������ȡ����μ���
	while (!fin.eof())
	{
		fin.read(buf, NUM);
		curNum = fin.gcount();	//��ǰ��ȡ���ֽ���
		for (int i = 0; i < curNum; i++){
			buf_out[i] = ecrept((bm::int1024_t)buf[i], ekey, pkey);
		}
		fout.write((char*)buf_out, curNum * sizeof(bm::int1024_t));
	}

	fin.close();
	fout.close();
}
void RSA_bigPrime::decrept(const char* ecrept_file_in, const char* plain_file_out, bm::int1024_t dkey, bm::int1024_t pkey)
{
	std::ifstream fin(ecrept_file_in, std::ifstream::binary);
	std::ofstream fout(plain_file_out, std::ofstream::binary);
	if (!fin.is_open()){
		std::cout << "file open filed" << std::endl;
		return;
	}
	const int NUM = 256;
	bm::int1024_t buf[NUM];		//Ҫ���ܵ�
	char buf_out[NUM];	//���
	int curNum;
	//���ļ��������ȡ����μ���
	while (!fin.eof())
	{
		fin.read((char*)buf, NUM * sizeof(bm::int1024_t));
		curNum = fin.gcount();	//��ǰ��ȡ���ֽ���
		curNum /= sizeof(bm::int1024_t);
		for (int i = 0; i < curNum; i++){
			buf_out[i] = (char)ecrept(buf[i], dkey, pkey);
		}
		fout.write(buf_out, curNum);
	}

	fin.close();
	fout.close();
}

std::vector<bm::int1024_t> RSA_bigPrime::Ecrept(std::string& str_in, bm::int1024_t ekey, bm::int1024_t pkey)
{
	std::vector<bm::int1024_t> msg_des;
	for (const auto& e : str_in){
		msg_des.push_back(ecrept(e, ekey, pkey));
	}
	return msg_des;
}
std::string RSA_bigPrime::Decrept(std::vector<bm::int1024_t>& ecrept_str, bm::int1024_t dkey, bm::int1024_t pkey)
{
	std::string msg_out;
	for (const auto& e : ecrept_str){
		msg_out.push_back((char)ecrept(e, dkey, pkey));
	}
	return msg_out;
}

void RSA_bigPrime::printInfo(std::vector<bm::int1024_t>& ecrept_str)
{
	for (const auto& e : ecrept_str)
	{
		std::cout << e << " ";
	}
	std::cout << std::endl;
}

bm::int1024_t RSA_bigPrime::produce_prime()
{
	//mt19937: һ�������������
	rm::mt19937 gen(time(nullptr));
	//ָ��������ķ�Χ��2 ~ ��1 << 768��
	rm::uniform_int_distribution<bm::int1024_t> dist(2, bm::int1024_t(1) << 128);
	bm::int1024_t prime = 0;
	while (1){
		prime = dist(gen);
		if (is_bigPrime(prime)){
			break;
		}
	}
	return prime;
}

bool RSA_bigPrime::is_bigPrime(bm::int1024_t digit)
{
	rm::mt11213b gen(time(nullptr));
	if (miller_rabin_test(digit, 25, gen))
	{
		if (miller_rabin_test((digit - 1) / 2, 25, gen))
		{
			return true;
		}
	}
	return false;
}

//ģ������
bm::int1024_t RSA_bigPrime::ecrept(bm::int1024_t msg, bm::int1024_t key, bm::int1024_t pkey)
{
	bm::int1024_t msg_out = 1;
	bm::int1024_t a = msg;	// a:��Ҫ���ܵ���Ϣ  key: b  pkey: c
	bm::int1024_t b = key;
	bm::int1024_t c = pkey;
	while (b){
		if (b & 1){
			msg_out = (msg_out * a) % c;
		}
		b >>= 1;
		a = (a * a) % c;
	}
	return msg_out;
}

//������Ա������Կ��˽Կ
void RSA_bigPrime::produce_keys()
{
	bm::int1024_t prime1 = produce_prime();
	bm::int1024_t prime2 = produce_prime();
	while (prime1 == prime2){
		prime2 = produce_prime();
	}
	std::cout << "���������ɹ�" << std::endl;
	//��Կ(e, n) ˽Կ(d, n)
	_key.pkey = produce_pkey(prime1, prime2);	//n
	std::cout << "n �����ɹ�" << std::endl;
	bm::int1024_t orla = produce_orla(prime1, prime2);	//f(n)
	std::cout << "orla �����ɹ�" << std::endl;
	_key.ekey = produce_ekey(orla);			//e
	std::cout << "e �����ɹ�" << std::endl;
	_key.dkey = produce_dkey(_key.ekey, orla);	//d
	std::cout << "d �����ɹ�" << std::endl;
}

//��n
bm::int1024_t RSA_bigPrime::produce_pkey(bm::int1024_t prime1, bm::int1024_t prime2)
{
	return prime1 * prime2;
}

//��f(n)
bm::int1024_t RSA_bigPrime::produce_orla(bm::int1024_t prime1, bm::int1024_t prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

//��e
bm::int1024_t RSA_bigPrime::produce_ekey(bm::int1024_t orla)
{
	srand((unsigned int)time(nullptr));
	bm::int1024_t e;
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
bm::int1024_t RSA_bigPrime::produce_dkey(bm::int1024_t ekey, bm::int1024_t orla)
{
	bm::int1024_t x, y;
	exgcd(ekey, orla, x, y);
	return (x % orla + orla) % orla;
}

bm::int1024_t RSA_bigPrime::produce_gcd(bm::int1024_t ekey, bm::int1024_t orla)
{
	bm::int1024_t a = ekey;
	bm::int1024_t b = orla;
	bm::int1024_t tmp;
	while (tmp = a % b){
		a = b;
		b = tmp;
	}
	return b;
}

bm::int1024_t RSA_bigPrime::exgcd(bm::int1024_t ekey, bm::int1024_t orla
	, bm::int1024_t& x, bm::int1024_t& y)
{
	if (orla == 0){
		x = 1;
		y = 0;
		return ekey;
	}
	bm::int1024_t gcd = exgcd(orla, ekey % orla, x, y);
	bm::int1024_t x1 = x, y1 = y;
	x = y1;
	y = x1 - (ekey / orla) * y1;
	return gcd;
}