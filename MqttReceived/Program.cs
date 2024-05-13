using Newtonsoft.Json;
using System;
using System.Numerics;
using System.Text;

namespace MqttReceived
{
    internal class Program
    {
        #region Members

        static string MQTT_Broker = "broker.emqx.io";
        static int MQTT_Port = 1883;
        static string MQTT_Topic = "primeeagle/esp32";

        static BigInteger Elgamal_G = 1983; // 公開選定的數(任意)
        static BigInteger Elgamal_P = 2147483647; // 大質數
        static BigInteger Elgama_x = 9527;  // 接收端自選秘鑰(KEY) 

        #endregion

        #region Entry Point

        static async Task Main(string[] args)
        {
            // ElgamalExample(); 

            Console.WriteLine("******* Hello, MqttReceived *******");

            Elgamal elgamal = new Elgamal(Elgamal_P, Elgamal_G);
            BigInteger Y = elgamal.GenY(Elgama_x);

            var mqttController = new MQTTController(MQTT_Broker, MQTT_Port, MQTT_Topic);
            await mqttController.InitAsync(e =>
                {
                    var message = Encoding.UTF8.GetString(e.ApplicationMessage.PayloadSegment);
                    if (message.StartsWith("g=")) return Task.CompletedTask;

                    ParseDHT11(elgamal, message);

                    return Task.CompletedTask;
                });

            while (true)
            {
                // 發送公開金鑰
                string publicKey = $"g={Elgamal_G},p={Elgamal_P},Y={Y}";
                mqttController.Publish(publicKey);
                Console.WriteLine($"{DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss")}，發送公開金鑰 : {publicKey}");
                await Task.Delay(3000);
            }
        }

        #endregion

        #region Private Methods

        /// <summary>
        /// 解析溫溼度資料
        /// </summary>
        private static void ParseDHT11(Elgamal elgamal, string message)
        {
            var dht11 = JsonConvert.DeserializeObject<ReceiveDHT11Model>(message);
            if (dht11 != null && dht11.Temperature.HasValue && dht11.Humidity.HasValue && dht11.X.HasValue)
            {
                int k = dht11.X.Value;
                var decrypt_temp = (double)elgamal.Decrypt(dht11.Temperature.Value, k);
                var decrypt_humid = (double)elgamal.Decrypt(dht11.Humidity.Value, k);

                string printMessage = $"{DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss")}，收到 DHT11 的訊號";
                printMessage += $"，解密後溫度 : {decrypt_temp / 100.0}";
                printMessage += $"，解密後濕度 : {decrypt_humid / 100.0}"; 
                Console.WriteLine(printMessage);
            }
        }

        private static void ElgamalExample()
        {
            BigInteger Elgamal_G = 1983; // 公開選定的數(任意)
            BigInteger Elgamal_P = 2147483647; // 大質數
            BigInteger x = 9527;  // 接收端自選秘鑰(KEY)
            int message = 2531; // 要發送的訊息

            Elgamal elgamal = new Elgamal(Elgamal_P, Elgamal_G);

            Console.WriteLine("------------Step1. 接收端(解密方)--------------");
            BigInteger Y = elgamal.GenY(x); // 1860867
            Console.WriteLine($"公開金鑰(g, p, Y) : {Elgamal_G},{Elgamal_P},{Y}\n");


            Console.WriteLine("------------Step2. 發送端(加密方)--------------");
            BigInteger r = new Random().Next(1, 100); // 每次加密隨機產生的數，這邊的 r 要小於 P 的整數  
            BigInteger[] cipher_message_array = elgamal.Encrypt(message, Y, r);
            BigInteger cipher_temp_X = cipher_message_array[0]; // K:1993633645
            BigInteger cipher_temp_c = cipher_message_array[1]; // c:1447668692
            Console.WriteLine($"加密訊息 : {message}，隨機數 : {r}，加密的(c, X) : {cipher_temp_c},{cipher_temp_X}\n");


            Console.WriteLine("------------Step3. 接收端(解密方)--------------");
            BigInteger decrypt_message = elgamal.Decrypt(cipher_temp_c, cipher_temp_X);
            Console.WriteLine($"解密後的訊息 : {decrypt_message}");
        }

        #endregion
    }
}
