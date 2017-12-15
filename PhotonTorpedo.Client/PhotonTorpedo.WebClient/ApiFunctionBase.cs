using System;
using System.Collections.Generic;
using System.Text;
using Newtonsoft.Json;

namespace PhotonTorpedo.WebClient
{
    public interface IApiFunction
    {
        string FunctionName { get; }
        string GetJsonString();
    }

    public abstract class FunctionDataBase : IApiFunction
    {
        [JsonIgnore]
        public string FunctionName => GetFunctionName();

        public abstract string GetFunctionName();

        public string GetJsonString()
        {
            return JsonConvert.SerializeObject(
                this,
                Formatting.None,
                new JsonSerializerSettings
                {
                    //NullValueHandling = NullValueHandling.Ignore,
                    StringEscapeHandling = StringEscapeHandling.Default,
                });
        }
    }
}
