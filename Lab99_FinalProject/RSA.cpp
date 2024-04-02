#include "RSA.h"

// RSA類別的建構函式
RSA::RSA(uint64_t p, uint64_t q, uint64_t e)
{
	// 初始化 RSA 參數
	_params.p = p; // 質數 p
	_params.q = q; // 質數 q
	_params.n = _params.p * _params.q; // RSA 模數 n = p * q

	// 計算 λ(n)，即 (p-1)(q-1) 的最小公倍數
	_params.lambda_n = _lcm(_params.p - 1, _params.q - 1);

	_params.e = e; // 公鑰 e

	// 使用模反元素計算私鑰 d
	_params.d = _mod_inverse(_params.e, _params.lambda_n);

	// 印出金鑰生成詳細資訊
	printf("------------金鑰生成--------------\r\n");
	printf("RSA: %5s: %8llu \r\n", "p", this->_params.p);
	printf("RSA: %5s: %8llu \r\n", "q", this->_params.q);
	printf("RSA: %5s: %8llu \r\n", "n", this->_params.n);
	printf("RSA: %5s: %8llu \r\n", "e", this->_params.e);
	printf("RSA: %5s: %8llu \r\n", "λ(n)", this->_params.lambda_n);
	printf("RSA: %5s: %8llu \r\n", "d", this->_params.d);
	printf("------------金鑰生成完成----------\r\n");
}

// 加密函式
uint64_t RSA::Encrypt(uint32_t message)
{
	// 使用模冪運算進行加密
	return _powof(message, _params.e, _params.n);
}

// 解密函式
uint64_t RSA::Decrypt(uint64_t cipher)
{
	// 使用模冪運算進行解密
	return _powof(cipher, _params.d, _params.n);
}

// 計算最大公因數
uint64_t RSA::_gcd(uint64_t a, uint64_t b)
{
	if (a == 0) return b;
	return _gcd(b % a, a);
}

// 計算模冪運算結果
uint64_t RSA::_powof(uint64_t b, uint64_t pow, uint64_t mod_size)
{
	uint64_t ret = 1;
	while (pow != 0)
	{
		if ((pow & 1) == 1)
		{
			ret = (ret * b) % mod_size; // 模幂運算：計算 b^pow mod mod_size
		}
		pow >>= 1;
		b = (b * b) % mod_size; // 將底數 b 進行平方取模
	}
	return ret % mod_size; // 返回結果取模 mod_size
}

// 計算最小公倍數
uint64_t RSA::_lcm(uint64_t a, uint64_t b)
{
	return (a * b) / _gcd(a, b); // 使用公式計算最小公倍數
}

// 使用擴展歐幾里得算法計算模反元素
uint64_t RSA::_ext_gcd(uint32_t mod_size, uint32_t r, uint32_t d1, uint32_t d2)
{
	int q = mod_size / r;
	return (r == 1) ? d2 : _ext_gcd(r, (mod_size % r), d2, (d1 - q * d2)); // 使用擴展歐幾里得算法計算模反元素
}

// 計算模反元素
uint64_t RSA::_mod_inverse(uint32_t num, uint32_t mod_size)
{
	int ret = 0;
	int g = _gcd(num, mod_size);
	if (g == 1)
	{
		ret = g ? _ext_gcd(mod_size, num, 0, 1) : 0; // 如果 num 和 mod_size 互質，則計算模反元素
	}
	return (ret < 0) ? (ret + mod_size) : ret; // 如果 ret 為負數，則將其轉換為正數
}

RSA rsa(RSA_P, RSA_Q, RSA_E); // 初始化 RSA 實例