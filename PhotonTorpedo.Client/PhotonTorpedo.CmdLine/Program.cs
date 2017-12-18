using PhotonTorpedo.WebClient;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PhotonTorpedo.CmdLine
{
    class Program
    {
        static void Main(string[] args)
        {
            var startStrip = new startStripFunc();
            var stopStrip = new stopStripFunc();

            var startGroup0 = new startGroupFunc(0);
            var stopGroup0 = new stopGroupFunc(0);

            var setColors1 = new setColorsFunc(0, "Christmas5"); // "Pop");
            var setColors2 = new setColorsFunc(0, "Unicorn1");

            var setEffect0 = new setEffectFunc(0, "wave");
            setEffect0.len = 48;
            setEffect0.glt = 48;
            setEffect0.fps = 25;
            setEffect0.mirr = 2;
            var setEffect1 = new setEffectFunc(0, "rainbow");
            setEffect1.len = 255;
            setEffect1.glt = 64;
            setEffect1.fps = 50;
            var setEffect2 = new setEffectFunc(0, "confetti");
            setEffect2.glt = 64;
            setEffect2.fps = 50;
            var setEffect3 = new setEffectFunc(0, "fade");
            setEffect3.glt = 64;
            setEffect3.fps = 20;

            var client = new PhotonClient();
            //Execute(client, startStrip);

            Execute(client, stopGroup0, false);

            Execute(client, setEffect0);
            Execute(client, startGroup0, false);

            Execute(client, setEffect1);
            Execute(client, startGroup0, false);

            Execute(client, setEffect2);
            Execute(client, startGroup0, false);

            Execute(client, setEffect3);
            Execute(client, startGroup0, false);

            Execute(client, setEffect0);
            Execute(client, startGroup0, false);

            //Execute(client, stopStrip);
        }

        private static string Execute(PhotonClient client, IApiFunction func, bool waitForKey = true)
        {
            Console.WriteLine("".PadRight(75,'-'));
            Console.WriteLine($"Executing '{func.FunctionName}' ({func.GetType().Name})...");
            Console.WriteLine($"  Data: {func.GetJsonString()}");
            if (waitForKey)
            {
                Console.WriteLine("ENTER to continue...");
                Console.ReadKey();
                Console.WriteLine();
            }
            var task = client.PostDataAsync(func);
            task.Wait();
            Console.WriteLine($"Finished, result: \r\n  {task.Result}");
            //if (waitForKey)
            //{
            //    Console.WriteLine("ENTER to continue...");
            //    Console.ReadKey();
            //    Console.WriteLine();
            //}
            return task.Result;
        }
    }
}
