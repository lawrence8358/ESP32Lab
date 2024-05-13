using Newtonsoft.Json;
using System;
using System.Numerics;
using System.Text;

namespace MqttReceived
{
    internal class Program
    {
        #region Members

        static int MQTT_Port = 1883;
        static string MQTT_Broker = "broker.emqx.io";
        static string MQTT_Topic = "primeeagle/esp32";

        static BigInteger Elgamal_P = 99999;
        static BigInteger Elgamal_G = 19;
        static BigInteger Elgamal_y = 23;

        #endregion

        #region Entry Point

        static async Task Main(string[] args)
        {
            Console.WriteLine("******* Hello, MqttReceived *******");
             
            Elgamal elgamal = new Elgamal(Elgamal_P, Elgamal_G);
            BigInteger Y = elgamal.GenY(Elgamal_y);

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
                mqttController.Publish($"g={Elgamal_G},p={Elgamal_P},Y={Y}");
                await Task.Delay(1000);
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
            if (dht11 != null && dht11.Temperature.HasValue && dht11.Humidity.HasValue && dht11.K.HasValue)
            {
                int k = dht11.K.Value;
                var decrypt_humid = (double)elgamal.Decrypt(dht11.Temperature.Value, k);
                var decrypt_temp = (double)elgamal.Decrypt(dht11.Humidity.Value, k);

                Console.WriteLine($"{DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss")}，收到 DHT11 的訊號");
                Console.WriteLine($"\t解密後溫度 : {(double)decrypt_temp / 100.0}");
                Console.WriteLine($"\t解密後濕度 : {(double)decrypt_humid / 100.0}");
            }
        }

        private static void ElgamalExample()
        {
            BigInteger Elgamal_P = 99999;
            BigInteger Elgamal_G = 19;
            BigInteger y = 23;
            BigInteger x = 5;

            Elgamal elgamal = new Elgamal(Elgamal_P, Elgamal_G);

            double t = 25.31, h = 75.12;
            Console.WriteLine($"加密前溫度: {t}");
            Console.WriteLine($"加密前濕度: {h}");

            BigInteger Y = elgamal.GenY(y);
            Console.WriteLine($"Y: {Y}");

            int temperature = (int)Math.Round(t * 100, MidpointRounding.AwayFromZero);
            int humidity = (int)Math.Round(h * 100, MidpointRounding.AwayFromZero);
            Console.WriteLine($"加密前溫度 * 100: {temperature}");
            Console.WriteLine($"加密前濕度 * 100: {humidity}");

            BigInteger[] cipher_temp_array = elgamal.Encrypt(temperature, Y, x);
            BigInteger cipher_temp_K = cipher_temp_array[0]; // K:33904
            BigInteger cipher_temp_c = cipher_temp_array[1]; // c:11882

            BigInteger[] cipher_humid_array = elgamal.Encrypt(humidity, Y, x);
            BigInteger cipher_humid_K = cipher_humid_array[0]; // K:33904
            BigInteger cipher_humid_c = cipher_humid_array[1]; // c:89394

            Console.WriteLine($"加密後溫度的 K 值 : {cipher_temp_K}， c 值 : {cipher_temp_c}");
            Console.WriteLine($"加密後濕度的 K 值 : {cipher_humid_K}， c 值 : {cipher_humid_c}");


            BigInteger decrypt_humid = elgamal.Decrypt(cipher_humid_c, cipher_humid_K);
            BigInteger decrypt_temp = elgamal.Decrypt(cipher_temp_c, cipher_temp_K);
            Console.WriteLine($"解密後溫度 * 100 : {decrypt_temp}");
            Console.WriteLine($"解密後濕度 * 100 : {decrypt_humid}");

            Console.WriteLine($"解密後溫度 : {(double)decrypt_temp / 100.0}");
            Console.WriteLine($"解密後濕度 : {(double)decrypt_humid / 100.0}");
        }

        #endregion
    }
}
