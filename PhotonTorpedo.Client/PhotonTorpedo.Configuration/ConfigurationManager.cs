using System;
using System.Collections.Generic;
using System.Text;

namespace PhotonTorpedo.Configuration
{
    public static class ConfigurationManager
    {
        static ConfigurationManager()
        {
            AccessToken = "e1f839239e589515e0c25ccd51e2b41cfa142325"; //This is your Particle Cloud Access Token
            DeviceId = "240025001047343438323536"; //This is your Particle Device Id
            ParticleApiUrl = "https://api.particle.io";
        }

        public static string AccessToken { get; private set; }
        public static string DeviceId { get; private set; }
        public static string ParticleApiUrl { get; private set; }
    }
}
