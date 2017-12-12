using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;

namespace PhotonTorpedo.WebClient
{
    public class PhotonClient
    {
        public enum Pattern
        {
            NONE,
            RAINBOW_CYCLE,
            THEATER_CHASE,
            COLOR_WIPE,
            SCANNER,
            FADE
        };

        string accessToken = "e1f839239e589515e0c25ccd51e2b41cfa142325"; //This is your Particle Cloud Access Token
        string deviceId = "240025001047343438323536"; //This is your Particle Device Id
        string particleFunc = "changeMode"; //This is the name of your Particle Function
        string particleApiUrl = "https://api.particle.io/";

        public async Task SetPatternRainbow(byte interval)
        {
            await SendDataAsync(
                new Dictionary<string, string>
                {
                    { "pattern", Pattern.RAINBOW_CYCLE.ToString() },
                    { "interval", interval.ToString() }
                }
                );
        }

        protected Task<HttpResponseMessage> SendDataAsync(Dictionary<string, string> data = null)
        {
            HttpClient client = new HttpClient
            {
                BaseAddress = new Uri(particleApiUrl)
            };

            var apiParams = new List<KeyValuePair<string, string>>();
            apiParams.Add(new KeyValuePair<string, string>("access_token", accessToken));
            foreach (var dataVal in data)
            {
                apiParams.Add(new KeyValuePair<string, string>(dataVal.Key, dataVal.Value));
            }

            var content = new FormUrlEncodedContent(apiParams);
            return client.PostAsync("v1/devices/" + deviceId + "/" + particleFunc, content);
        }
    }
}
