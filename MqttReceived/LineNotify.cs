using System.Net.Http.Headers;
using System.Net;

namespace MqttReceived
{
    internal class LineNotify
    {

        #region Members

        private readonly HttpClient _httpClient;

        #endregion

        #region Constructor

        public LineNotify()
        {
            _httpClient = new HttpClient();
        }

        #endregion

        #region Public Methods

        public async Task SendAsync(string token, string message)
        {
            HttpResponseMessage? response = null;
            using var httpRequest = new HttpRequestMessage(HttpMethod.Post, string.Empty)
            {
                Headers = { Authorization = new AuthenticationHeaderValue("Bearer", token) }
            };
            httpRequest.RequestUri = new Uri("https://notify-api.line.me/api/notify");
            httpRequest.Content = new FormUrlEncodedContent(new Dictionary<string, string>
            {
                {"message", message}
            });

            response = await _httpClient.SendAsync(httpRequest);

            string resultMessage = string.Empty;
            if (response.Content != null)
                resultMessage = await response.Content.ReadAsStringAsync();

            if (response.StatusCode != HttpStatusCode.OK)
                Console.WriteLine($"發送 LINE Notify 失敗 : {resultMessage}");
            else
                Console.WriteLine($"發送 LINE Notify 成功 : {message}");
        }

        #endregion
    }
}
