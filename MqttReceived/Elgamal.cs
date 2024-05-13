using System;
using System.Numerics; 

namespace MqttReceived
{
    public class Elgamal
    {
        #region Members

        /// <summary>
        /// 大質數
        /// </summary>
        private BigInteger p;

        /// <summary>
        /// 公開選定的數(任意)
        /// </summary>
        private BigInteger g;

        #endregion

        #region Constructor

        public Elgamal(BigInteger p, BigInteger g)
        {
            this.p = p;
            this.g = g;

            Console.WriteLine("------------金鑰生成--------------");
            Console.WriteLine($"Elgamal: {"p",5}: {p}");
            Console.WriteLine($"Elgamal: {"g",5}: {g}");
            Console.WriteLine("------------金鑰生成完成----------");
        }

        #endregion

        #region Public Methods

        public BigInteger[] Encrypt(BigInteger message, BigInteger Y, BigInteger x)
        {
            BigInteger K = BigInteger.ModPow(Y, x, p);
            BigInteger c = (K * message) % p;

            return new BigInteger[] { K, c };
        }

        public BigInteger Decrypt(BigInteger cipher, BigInteger K)
        {
            BigInteger modInverse = ModInverse(K, p);
            return (cipher * modInverse) % p;
        }

        /// <summary>
        /// 產生接收方公鑰
        /// </summary>
        /// <param name="x">接收方的私鑰</param>
        /// <returns>g^x mod p</returns>
        public BigInteger GenY(BigInteger x)
        {
            return BigInteger.ModPow(g, x, p);
        }

        #endregion

        #region Private Methods

        private BigInteger Gcd(BigInteger a, BigInteger b)
        {
            if (a == 0) return b;
            return Gcd(b % a, a);
        }

        private BigInteger PowOf(BigInteger b, BigInteger pow, BigInteger modSize)
        {
            BigInteger ret = 1;
            while (pow != 0)
            {
                if ((pow & 1) == 1)
                {
                    ret = (ret * b) % modSize;
                }
                pow >>= 1;
                b = BigInteger.ModPow(b, 2, modSize);
            }
            return ret % modSize;
        }

        private BigInteger ExtGcd(BigInteger modSize, BigInteger r, BigInteger d1, BigInteger d2)
        {
            BigInteger q = modSize / r;
            return (r == 1) ? d2 : ExtGcd(r, (modSize % r), d2, (d1 - q * d2));
        }

        private BigInteger ModInverse(BigInteger num, BigInteger modSize)
        {
            BigInteger g = Gcd(num, modSize);
            if (g == 1)
            {
                BigInteger ret = g == 1 ? ExtGcd(modSize, num, 0, 1) : 0;
                return (ret < 0) ? (ret + modSize) : ret;
            }
            throw new Exception("Modular inverse does not exist.");
        }

        #endregion
    }
}
