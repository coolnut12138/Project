#pragma once
#include <string>
#include <vector>
#include <time.h>
#include <fstream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
namespace bm = boost::multiprecision;
namespace rm = boost::random;

struct Key
{
	//公钥(ekey, pkey): (e,n)
	bm::int1024_t pkey;		//n
	bm::int1024_t ekey;		//e
	//私钥(dkey, pkey): (d, n)
	bm::int1024_t dkey;		//d
};

class RSA_bigPrime
{
public:
	RSA_bigPrime();
	Key getKey();
	void ecrept(const char* plain_file_in, const char* ecrept_file_out,
		bm::int1024_t ekey, bm::int1024_t pkey);
	void decrept(const char* ecrept_file_in, const char* plain_file_out,
		bm::int1024_t dkey, bm::int1024_t pkey);

	std::vector<bm::int1024_t> Ecrept(std::string& str_in, bm::int1024_t ekey, bm::int1024_t pkey);
	std::string Decrept(std::vector<bm::int1024_t>& ecrept_str, bm::int1024_t dkey, bm::int1024_t pkey);

	void printInfo(std::vector<bm::int1024_t>& ecrept_str);
private:
	//加密解密单个信息，加密时key传e，pkey传n；解密时key传d，pkey传n
	bm::int1024_t ecrept(bm::int1024_t msg, bm::int1024_t key, bm::int1024_t pkey);
	bm::int1024_t produce_prime();
	bool is_bigPrime(bm::int1024_t prime);
	void produce_keys();
	bm::int1024_t produce_pkey(bm::int1024_t prime1, bm::int1024_t prime2);
	bm::int1024_t produce_orla(bm::int1024_t prime1, bm::int1024_t prime2);
	bm::int1024_t produce_ekey(bm::int1024_t orla);
	bm::int1024_t exgcd(bm::int1024_t ekey, bm::int1024_t orla, bm::int1024_t& x, bm::int1024_t& y);
	bm::int1024_t produce_dkey(bm::int1024_t ekey, bm::int1024_t orla);
	bm::int1024_t produce_gcd(bm::int1024_t ekey, bm::int1024_t orla);
private:
	Key _key;
};