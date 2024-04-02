#ifndef LawrenceLib_RSA_H
#define LawrenceLib_RSA_H
 
#define RSA_P 9973
#define RSA_Q 10007
#define RSA_E 65537

#include<stdint.h>
#include <stdio.h>

class RSA
{
public:
	/** RSA參數 */
	struct Params
	{
		uint64_t p;         /**< 質數p */
		uint64_t q;         /**< 質數q */
		uint64_t n;         /**< RSA模數 */
		uint64_t e;         /**< 公鑰e */
		uint64_t d;         /**< 私鑰d */
		uint64_t lambda_n;  /**< λ(n)，即(p-1)(q-1)的最小公倍數 */
	};

	/** 建構子
	* @param p 質數p
	* @param q 質數q
	* @param e 公鑰e
	*/
	RSA(uint64_t p, uint64_t q, uint64_t e);

	/** 加密
	* @param message 原始訊息
	* @return 加密的訊息
	*/
	uint64_t Encrypt(uint32_t message);

	/** 解密
	* @param cipher 加密的訊息
	* @return 原始訊息
	*/
	uint64_t Decrypt(uint64_t cipher);

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

	/** 計算最小公倍數
	* @param a 數字 a
	* @param b 數字 b
	* @return 最小公倍數
	*/
	uint64_t _lcm(uint64_t a, uint64_t b);

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

	/** RSA參數 */
	Params _params;
};

extern RSA rsa;

#endif