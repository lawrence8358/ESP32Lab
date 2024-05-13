using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MqttReceived
{
    internal class ReceiveDHT11Model
    {
        /// <summary>
        /// 溫度
        /// </summary>
        public int? Temperature { get; set; }

        /// <summary>
        /// 濕度
        /// </summary>
        public int? Humidity { get; set; }

        public int? X { get; set; }
    }
}
