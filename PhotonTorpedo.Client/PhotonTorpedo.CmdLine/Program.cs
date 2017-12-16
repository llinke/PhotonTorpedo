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
            var initStrip = new initStripFunc(32);
            var startStrip = new startStripFunc();
            var stopStrip = new stopStripFunc();

            var addGroup1 = new addGroupFunc("Group1", 0, 12);
            var addGroup2 = new addGroupFunc("Group2", 12, 20);
            var startGroup0 = new startGroupFunc(0);
            var startGroup1 = new startGroupFunc(1);
            var startGroup2 = new startGroupFunc(2);
            var stopGroup0 = new stopGroupFunc(0);
            var stopGroup1 = new stopGroupFunc(1, true);
            var stopGroup2 = new stopGroupFunc(2, true);

            var setEffect0 = new setEffectFunc(0, "fade");
            var setEffect1 = new setEffectFunc(1, "rainbow");
            var setEffect2 = new setEffectFunc(2, "confetti");
            var setColors0 = new setColorsFunc(0, "Christmas2"); // "Pop");
            var setColors1 = new setColorsFunc(1, "Unicorn1");
            var setColors2 = new setColorsFunc(2, "NightAndDay1");

            var client = new PhotonClient();
            Execute(client, initStrip);

            //Execute(client, stopStrip);
            setEffect0.glt = 64;
            setEffect0.fps = 50;
            Execute(client, setEffect0);
            Execute(client, setColors0);
            Execute(client, startStrip);
            Execute(client, startGroup0);
            Execute(client, stopGroup0);

            //Execute(client, addGroup1);
            //Execute(client, addGroup2);

            //Execute(client, setEffect1);
            //Execute(client, setColors1);
            //Execute(client, setEffect2);
            //Execute(client, setColors2);

            //Execute(client, startGroup1);
            //Execute(client, startGroup2);
            //Execute(client, startGroup0);
            //Execute(client, stopGroup0);
            //Execute(client, stopGroup1);
            //Execute(client, stopGroup2);

            Execute(client, stopStrip);
        }

        private static string Execute(PhotonClient client, IApiFunction func, bool waitForKey = true)
        {
            Console.WriteLine("".PadRight(75,'-'));
            Console.WriteLine($"Executing '{func.FunctionName}' ({func.GetType().Name})...");
            Console.WriteLine($"  Data: {func.GetJsonString()}");
            if (waitForKey)
            {
                Console.WriteLine("ENTER to continue...");
                Console.ReadLine();
            }
            var task = client.PostDataAsync(func);
            task.Wait();
            Console.WriteLine($"Finished, result: \r\n  {task.Result}");
            if (waitForKey)
            {
                Console.WriteLine("ENTER to continue...");
                Console.ReadLine();
            }
            return task.Result;
        }
    }
}
