using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;

using OnVifDiscovery;
//using OnVifServicesRunTime;
namespace OnVifDiscoveryApp
{
    class Program
    {
     


        static void Main(string[] args)
        {
            Stream debugFile = File.Create("Debugging.txt");
            Trace.Listeners.Add(new TextWriterTraceListener(debugFile));
            ConsoleTraceListener listener = new ConsoleTraceListener();
            Trace.Listeners.Add(listener);
            Trace.AutoFlush = true;

     //       Trace.Indent();

          

            CancellationTokenSource cts = new CancellationTokenSource();
    //        OnVifServicesRunTime.DiscoveryClients discoveryClients = new OnVifServicesRunTime.DiscoveryClients();
            DiscoveryServer discoveryServer = new OnVifDiscovery.DiscoveryServer();
            Thread workerThread = new Thread(new ParameterizedThreadStart(discoveryServer.ThreadStart));
            workerThread.IsBackground = true;
            workerThread.Start(cts);

            System.Diagnostics.Trace.WriteLine("Start Discovery App...");
            System.Diagnostics.Trace.WriteLine("Press Key C to stopp App");
            while (true)
            {
               ConsoleKeyInfo key = System.Console.ReadKey();
   
                if (key.KeyChar.ToString().ToUpperInvariant() == "C") // Cancel worker thread
                {
                    cts.Cancel();
                    break;
                }

            }
            workerThread.Join();


            Trace.Flush();
            debugFile.Close();
        }

    }
}
