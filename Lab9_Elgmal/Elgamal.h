#ifndef LawrenceLib_Elgamal_H
#define LawrenceLib_Elgamal_H

#define Elgamal_P 2147483647  
#define Elgamal_G 1983

#include <stdint.h>
#include <stdio.h>

class Elgamal
{
public:
	/** Elgamal 參數 */
	struct Params
	{
		uint64_t p;
		uint64_t g;
	};

	/** 建構子 */
	Elgamal(uint64_t p, uint64_t g);

	/** 加密 */
	uint64_t* Encrypt(uint32_t message, uint64_t Y, uint64_t r);

	/** 解密 */
	uint64_t Decrypt(uint64_t cipher, uint64_t X);

	uint64_t GenY(uint64_t x);

private:
	/** 計算最大公因數，歐基里德算法
	* @param a 數字 a
	* @param b 數字 b
	* @return 最大公因數
	*/
	uint64_t _gcd(uint64_t a, uint64_t b);

	/** 計算次方
	* @param b 底數
	* @param pow 次方
	* @param mod_size 模數
	* @return b^pow mod mod_size
	*/
	uint64_t _powof(uint64_t b, uint64_t pow, uint64_t mod_size);

	/** 使用擴展歐幾里得算法計算模反元素
	* @param mod_size 模數
	* @param r 模數的餘數
	* @param d1 第一個除數
	* @param d2 第二個除數
	* @return d1、d2在模mod_size下的反元素
	*/
	uint64_t _ext_gcd(uint32_t mod_size, uint32_t r, uint32_t d1, uint32_t d2);

	/** 使用擴展歐幾里得算法計算模反元素
	* @param num 數字
	* @param mod_size 模數
	* @return num在模mod_size下的反元素
	*/
	uint64_t _mod_inverse(uint32_t num, uint32_t mod_size);

	/** Elgamal 參數 */
	Params _params;
};

extern Elgamal elgamal;

#endif