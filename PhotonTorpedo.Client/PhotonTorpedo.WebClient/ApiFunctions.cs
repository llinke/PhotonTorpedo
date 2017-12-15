using System;
using System.Collections.Generic;
using System.Text;
using Newtonsoft.Json;

namespace PhotonTorpedo.WebClient
{
    #region LED Strip
    public class initStripFunc : FunctionDataBase
    {
        public initStripFunc(int ledCount)
        {
            this.ledC = ledCount;
        }
        public initStripFunc()
        {
        }

        public override string GetFunctionName() => "initStrip";

        public int ledC { get; set; }
    }

    public class startStripFunc : FunctionDataBase
    {
        public startStripFunc()
        {
        }

        public override string GetFunctionName() => "startStrip";
    }

    public class stopStripFunc : FunctionDataBase
    {
        public stopStripFunc()
        {
        }

        public override string GetFunctionName() => "stopStrip";
    }
    #endregion

    #region Groups
    public class addGroupFunc : FunctionDataBase
    {
        public addGroupFunc()
        {
        }
        public addGroupFunc(string grp, int ledFirst, int ledCount)
        {
            this.grp = grp;
            this.ledF = ledFirst;
            this.ledC = ledCount;
        }

        public override string GetFunctionName() => "addGroup";

        public string grp { get; set; }
        public int ledF { get; set; }
        public int ledC { get; set; }
    }

    public class startGroupFunc : FunctionDataBase
    {
        public startGroupFunc()
        {
        }
        public startGroupFunc(int grp)
        {
            this.grp = grp;
        }

        public override string GetFunctionName() => "startGroup";

        public int grp { get; set; }
    }

    public class stopGroupFunc : FunctionDataBase
    {
        public stopGroupFunc()
        {
        }
        public stopGroupFunc(int grp, bool? now = null)
        {
            this.grp = grp;
            this.now = now;
        }

        public override string GetFunctionName() => "stopGroup";

        public int grp { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public bool? now { get; set; }
    }
    #endregion

    #region Effects
    public class setEffectFunc : FunctionDataBase
    {
        public setEffectFunc(int grp, string fx)
        {
            this.grp = grp;
            this.fx = fx;
        }
        public setEffectFunc()
        {
        }

        public override string GetFunctionName() => "setEffect";

        public int grp { get; set; }

        public string fx { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public bool? glt { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public int? fps { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public string dir { get; set; }
    }

    public class setColorsFunc : FunctionDataBase
    {
        public setColorsFunc(int grp, string pal)
        {
            this.grp = grp;
            this.pal = pal;
        }
        public setColorsFunc(int grp, UInt32[] cols, bool? clr = null, bool? gen = null)
        {
            this.grp = grp;
            this.cols = cols;
            this.clr = clr;
            this.gen = gen;
        }
        public setColorsFunc()
        {
        }

        public override string GetFunctionName() => "setColors";

        public int grp { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public string pal { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public bool? clr { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public bool? gen { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public UInt32[] cols { get; set; }
    }
    #endregion
}
