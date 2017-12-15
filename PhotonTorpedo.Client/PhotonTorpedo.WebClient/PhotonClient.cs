#define UseSendAsync
#if !UseSendAsync
#define UseSendAsJson
#endif
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using PhotonTorpedo.Configuration;

namespace PhotonTorpedo.WebClient
{
    public class PhotonClient
    {
        private Uri CreateUriFromMethod(string functionName, bool withToken = false)
        {
            //https://api.particle.io/v1/devices/mydevice/wakeup?access_token=1234
            return withToken
                ? new Uri($"{ConfigurationManager.ParticleApiUrl}/v1/devices/{ConfigurationManager.DeviceId}/{functionName}?access_token={ConfigurationManager.AccessToken}")
                : new Uri($"{ConfigurationManager.ParticleApiUrl}/v1/devices/{ConfigurationManager.DeviceId}/{functionName}");
        }

        public async Task<string> PostDataAsync(IApiFunction data = null)
        {
#if UseSendAsync
            var request = new HttpRequestMessage(HttpMethod.Post, CreateUriFromMethod(data.FunctionName, false));
            if (data != null)
            {
                var apiParams = new List<KeyValuePair<string, string>>();
                apiParams.Add(new KeyValuePair<string, string>("access_token", ConfigurationManager.AccessToken));
                apiParams.Add(new KeyValuePair<string, string>("arg", data.GetJsonString()));
                request.Content = new FormUrlEncodedContent(apiParams);
            }
            return await SendAsync(request);
#else
#if UseSendAsJson
            return await PostDataAsyncAsJson(data);
#else
            using (HttpClient client = new HttpClient())
            {
                client.BaseAddress = new Uri(ConfigurationManager.ParticleApiUrl);
                client.DefaultRequestHeaders.CacheControl = new CacheControlHeaderValue() { NoCache = true };

                var apiParams = new List<KeyValuePair<string, string>>();
                apiParams.Add(new KeyValuePair<string, string>("access_token", ConfigurationManager.AccessToken));
                apiParams.Add(new KeyValuePair<string, string>("arg", data.GetJsonString()));
                var content = new FormUrlEncodedContent(apiParams);

                var response = await client.PostAsync("v1/devices/" + ConfigurationManager.DeviceId + "/" + data.FunctionName, content);
                return await response.Content.ReadAsStringAsync();
            }
#endif
#endif
        }

#if UseSendAsync
        private async Task<string> SendAsync(HttpRequestMessage request)
        {
            using (HttpClient client = new HttpClient())
            {
                //if (sendAuthHeader)
                //    client.DefaultRequestHeaders.Authorization = new AuthenticationHeaderValue("Basic", Convert.ToBase64String(Encoding.UTF8.GetBytes($"{OAuthClientId}:{OAuthClientSecret}")));

                client.DefaultRequestHeaders.CacheControl = new CacheControlHeaderValue() { NoCache = true };
                var response = await client.SendAsync(request);
                var responseContent = await response.Content.ReadAsStringAsync();

                switch (response.StatusCode)
                {
                    case HttpStatusCode.OK:
                    case HttpStatusCode.Created:
                        return responseContent;

                    case HttpStatusCode.Unauthorized:
                        throw new ParticleUnauthorizedException(responseContent);

                    case HttpStatusCode.NotFound:
                        throw new ParticleNotFoundException(responseContent);

                    case HttpStatusCode.BadRequest:
                        throw new ParticleRequestBadRequestException(responseContent);

                    default:
                        throw new Exception();
                }

            }
        }
#endif

#if UseSendAsJson
        private  async Task<string> PostDataAsyncAsJson(IApiFunction data = null)
        {
            using (HttpClient client = new HttpClient())
            {
                var response = await client.PostAsync(
                    CreateUriFromMethod(data.FunctionName),
                    new StringContent(
                        data.GetJsonString(),
                        Encoding.UTF8,
                        "application/json"));
                return await response.Content.ReadAsStringAsync();
            }
        }
#endif
    }
}
