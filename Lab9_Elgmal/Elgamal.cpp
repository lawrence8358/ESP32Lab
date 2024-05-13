#include "Elgamal.h"
#include <stdlib.h>

// Elgamal類別的建構函式
Elgamal::Elgamal(uint64_t p, uint64_t g)
{
    // 初始化 Elgamal 參數
    _params.p = p;
    _params.g = g;

    // 印出金鑰生成詳細資訊
    printf("------------金鑰生成--------------\r\n");
    printf("Elgamal: %5s: %8llu \r\n", "p", this->_params.p);
    printf("Elgamal: %5s: %8llu \r\n", "g", this->_params.g);
    printf("------------金鑰生成完成----------\r\n");
}

// 加密函式
uint64_t* Elgamal::Encrypt(uint32_t message, uint64_t Y, uint64_t r)
{
    uint64_t* ret = (uint64_t*)malloc(sizeof(uint64_t) * 2);  // 需要free(ret)
    uint64_t X = _powof(Y, r, _params.p);
    uint64_t c = (X * message) % (_params.p);

    ret[0] = X;
    ret[1] = c;
    return ret;  // return [X, c]
}

// 解密函式 Decryption:D(c)= m = X * c^(-1) mod p
uint64_t Elgamal::Decrypt(uint64_t cipher, uint64_t X)
{
    return (cipher * _mod_inverse(X, _params.p)) % _params.p;
}

uint64_t Elgamal::GenY(uint64_t x)
{
    return _powof(_params.g, x, _params.p);
}

// 計算最大公因數
uint64_t Elgamal::_gcd(uint64_t a, uint64_t b)
{
    if (a == 0) return b;
    return _gcd(b % a, a);
}

// 計算模冪運算結果
uint64_t Elgamal::_powof(uint64_t b, uint64_t pow, uint64_t mod_size)
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

// 使用擴展歐幾里得算法計算模反元素
uint64_t Elgamal::_ext_gcd(uint32_t mod_size, uint32_t r, uint32_t d1, uint32_t d2)
{
    int q = mod_size / r;
    return (r == 1) ? d2 : _ext_gcd(r, (mod_size % r), d2, (d1 - q * d2)); // 使用擴展歐幾里得算法計算模反元素
}

// 計算模反元素
uint64_t Elgamal::_mod_inverse(uint32_t num, uint32_t mod_size)
{
    int ret = 0;
    int g = _gcd(num, mod_size);
    if (g == 1)
    {
        ret = g ? _ext_gcd(mod_size, num, 0, 1) : 0; // 如果 num 和 mod_size 互質，則計算模反元素
    }
    return (ret < 0) ? (ret + mod_size) : ret; // 如果 ret 為負數，則將其轉換為正數
}

Elgamal elgamal(Elgamal_P, Elgamal_G); // 初始化 Elgamal 實例