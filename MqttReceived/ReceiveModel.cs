using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MqttReceived
{
    internal class ESP32ChipInfos
    {
        public int ChipId { get; set; }

        public string UserName { get; set; } = "";
    }

    internal class ReceiveBaseModel
    {
        public int Type { get; set; }

        public int ChipId { get; set; }

        public int? X { get; set; }
    }

    internal class ReceiveDHT11Model : ReceiveBaseModel
    {
        /// <summary>
        /// 溫度
        /// </summary>
        public int? Temperature { get; set; }

        /// <summary>
        /// 濕度
        /// </summary>
        public int? Humidity { get; set; }
    }
}
