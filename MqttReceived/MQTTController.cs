using System;
using System.Text;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Protocol;

namespace MqttReceived
{
    internal class MQTTController
    {
        #region Members

        IMqttClient? _mqttClient = null;

        #endregion

        #region Properties

        public int Port { get; } = 1883;

        public string Broker { get; } = "broker.emqx.io";

        public string Topic { get; } = "primeeagle/esp32";

        public string ClientId { get; } = Guid.NewGuid().ToString();

        #endregion

        #region Constructor

        public MQTTController(string broker, int port, string topic)
        {
            Broker = broker;
            Port = port;
            Topic = topic;
        }

        #endregion

        #region Public Methods

        public async Task InitAsync(Func<MqttApplicationMessageReceivedEventArgs, Task> receiveCallback)
        {
            MqttFactory factory = new MqttFactory();
            _mqttClient = factory.CreateMqttClient();

            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(Broker, Port)
                .WithClientId(ClientId)
                .WithCleanSession()
                .Build();

            var connectResult = await _mqttClient.ConnectAsync(options);

            if (connectResult.ResultCode == MqttClientConnectResultCode.Success)
            {
                Console.WriteLine("Connected to MQTT broker successfully.");

                await _mqttClient.SubscribeAsync(Topic);
                _mqttClient.ApplicationMessageReceivedAsync += receiveCallback;
            }
            else
            {
                Console.WriteLine($"Failed to connect to MQTT broker: {connectResult.ResultCode}");
            }
        }

        public void Publish(string message)
        {
            if (_mqttClient == null)
            {
                return;
            }

            var applicationMessage = new MqttApplicationMessageBuilder()
                .WithTopic(Topic)
                .WithPayload(Encoding.UTF8.GetBytes(message))
                .WithQualityOfServiceLevel(MqttQualityOfServiceLevel.AtLeastOnce)
                .WithRetainFlag()
                .Build();

            _mqttClient.PublishAsync(applicationMessage);
        }

        public async Task DisconnectAsync()
        {
            if (_mqttClient == null)
            {
                return;
            }

            await _mqttClient.UnsubscribeAsync(Topic);
            await _mqttClient.DisconnectAsync();
        }

        #endregion
    }
}
