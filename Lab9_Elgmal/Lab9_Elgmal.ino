#include "Elgamal.h" 
 
void setup() 
{
	Serial.begin(115200);      

    double t = 25.31, h = 75.12;
    printf("加密前溫度 : %f\n", t);
    printf("加密前濕度 : %f\n", h); 
  
    uint64_t Y = elgamal.GenY(23);
    printf("Y 值 : %llu\n", Y); // Y:89614

    // 使用 +0.5 並轉成整數的特性來達成四捨五入
    int temperature = (int)((t * 100) + 0.5);
    int humidity = (int)((h * 100) + 0.5);
    printf("加密前溫度 * 100 : %d\n", temperature);
    printf("加密前濕度 * 100 : %d\n", humidity);

    uint64_t x = 5;
    uint64_t* cipher_temp_array = elgamal.Encrypt(temperature, Y, x);
    uint64_t cipher_temp_K = cipher_temp_array[0]; // K:33904 
    uint64_t cipher_temp_c = cipher_temp_array[1]; // c:11882 

    uint64_t* cipher_humid_array = elgamal.Encrypt(humidity, Y, x);
    uint64_t cipher_humid_K = cipher_humid_array[0]; // K:33904
    uint64_t cipher_humid_c = cipher_humid_array[1]; // c:89394 

    // 釋放記憶體
    free(cipher_temp_array);
    free(cipher_humid_array);

    // K 值都是一樣的
    printf("加密後溫度的 K 值 : %llu， c 值 : %llu\n", cipher_temp_K, cipher_temp_c); 
    printf("加密後濕度的 K 值 : %llu， c 值 : %llu\n", cipher_humid_K, cipher_humid_c);


    int decrypt_humid = elgamal.Decrypt(cipher_humid_c, cipher_humid_K);
    int decrypt_temp = elgamal.Decrypt(cipher_temp_c, cipher_temp_K);
    printf("解密後溫度 * 100 : %d\n", decrypt_temp);
    printf("解密後濕度 * 100 : %d\n", decrypt_humid);
     
    printf("解密後溫度 : %f\n", double(decrypt_temp / 100.0));
    printf("解密後濕度 : %f\n", double(decrypt_humid / 100.0));
}

void loop() 
{ 
}
 
