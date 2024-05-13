using Microsoft.Extensions.Configuration;
using Newtonsoft.Json;
using System;
using System.Numerics;
using System.Text;

namespace MqttReceived
{
    internal class Program
    {
        #region Members

        static string MQTT_Broker = "";
        static int MQTT_Port = 0;
        static string MQTT_Topic = "";

        static BigInteger Elgamal_G = 0; // 公開選定的數(任意)
        static BigInteger Elgamal_P = 0; // 大質數
        static BigInteger Elgama_x = 0;  // 接收端自選秘鑰(KEY) 
        static int SendPublicKeyInterval = 0;    

        #endregion

        #region Entry Point

        static void LoadConfig()
        {
            IConfigurationRoot configuration = new ConfigurationBuilder()
                .SetBasePath(AppDomain.CurrentDomain.BaseDirectory)
                .AddJsonFile("appsettings.json")
                .Build();

            MQTT_Broker = configuration["MQTT:Broker"]!;
            MQTT_Port = int.Parse(configuration["MQTT:Port"]!);
            MQTT_Topic = configuration["MQTT:Topic"]!;
            Elgamal_G = BigInteger.Parse(configuration["Elgamal:G"]!);
            Elgamal_P = BigInteger.Parse(configuration["Elgamal:P"]!);
            Elgama_x = BigInteger.Parse(configuration["Elgamal:X"]!);
            SendPublicKeyInterval = int.Parse(configuration["SendPublicKeyInterval"]!);

            Console.WriteLine("------------初始化參數--------------");
            Console.WriteLine($"MQTT.Broker: {MQTT_Broker}");
            Console.WriteLine($"MQTT.Port: {MQTT_Port}");
            Console.WriteLine($"MQTT.Topic: {MQTT_Topic}");
            Console.WriteLine($"Elgamal.G: {Elgamal_G}");
            Console.WriteLine($"Elgamal.P: {Elgamal_P}");
            Console.WriteLine($"Elgamal.X: {Elgama_x}");
            Console.WriteLine($"SendPublicKeyInterval: {SendPublicKeyInterval}");
            Console.WriteLine("------------初始化參數完成----------\n");
        }

        static async Task Main(string[] args)
        {
            LoadConfig();

            // ElgamalExample(); 

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
                await Task.Delay(SendPublicKeyInterval);
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
