#include "RSA.h" 
 
void setup() 
{
  Serial.begin(115200);      
  
  int t = 25, h = 60;
  printf("加密前溫度 : %d\n", t);
  printf("加密前濕度 : %d\n", h);

  uint64_t cipher_humid = rsa.Encrypt(h);
  uint64_t cipher_temp = rsa.Encrypt(t);
  printf("加密後溫度 : %llu\n", cipher_temp);
  printf("加密後濕度 : %llu\n", cipher_humid);

  uint64_t decrypt_humid = rsa.Decrypt(cipher_humid);
  uint64_t decrypt_temp = rsa.Decrypt(cipher_temp);
  printf("解密後溫度 : %llu\n", decrypt_temp);
  printf("解密後濕度 : %llu\n", decrypt_humid);
}

void loop() 
{ 
}
 
