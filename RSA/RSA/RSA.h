#pragma once
#include <string>
#include <vector>
#include <time.h>
struct Key
{
	//��Կ(ekey, pkey): (e,n)
	long pkey;		//n
	long ekey;		//e
	//˽Կ(dkey, pkey): (d, n)
	long dkey;		//d
};

class RSA
{
public:
	RSA();
	Key getKey();
	void ecrept(const char* plain_file_in, const char* ecrept_file_out,
		long ekey, long pkey);
	void decrept(const char* ecrept_file_in, const char* plain_file_out,
		long dkey, long pkey);

	std::vector<long> Ecrept(std::string& str_in, long ekey, long pkey);
	std::string Decrept(std::vector<long>& ecrept_str, long dkey, long pkey);

	void printInfo(std::vector<long>& ecrept_str);
private:
	//���ܽ��ܵ�����Ϣ������ʱkey��e��pkey��n������ʱkey��d��pkey��n
	long ecrept(long msg, long key, long pkey);
	long produce_prime();
	bool is_prime(long prime);
	void produce_keys();
	long produce_pkey(long prime1, long prime2);
	long produce_orla(long prime1, long prime2);
	long produce_ekey(long orla);
	//long produce_gcd(long ekey, long orla);
	long produce_dkey(long ekey, long orla);

private:
	Key _key;
};